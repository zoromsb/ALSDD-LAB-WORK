/*
 * fileam.c — File Abstract Machine  (implementation)
 * See fileam.h for the full API documentation.
 */

#include "fileam.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

/* ── helpers ────────────────────────────────────────────────────────── */

static const char *mode_str(FileMode m) {
    switch (m) {
        case MODE_READ:   return "r";
        case MODE_WRITE:  return "w";
        case MODE_APPEND: return "a";
        case MODE_RW:     return "r+w";
        default:          return "?";
    }
}

void fam_state_dump(const FileAM *f) {
    printf("  ┌─ FileAM state ─────────────────────────────\n");
    printf("  │  fd       = %d\n",    f->fd);
    printf("  │  path     = \"%s\"\n", f->path);
    printf("  │  mode     = %s\n",    mode_str(f->mode));
    printf("  │  offset   = %zu\n",   f->offset);
    printf("  │  buf_len  = %zu / %d\n", f->buf_len, BUF_CAPACITY);
    printf("  │  dirty    = %s\n",    f->dirty ? "yes (unflushed)" : "no");

    /* print buffer contents as ASCII, dots for non-printable */
    printf("  │  buf      = [");
    for (size_t i = 0; i < f->buf_len && i < 64; i++) {
        unsigned char c = f->user_buf[i];
        printf("%c", (c >= 32 && c < 127) ? c : '.');
    }
    if (f->buf_len > 64) printf("...");
    printf("]\n");
    printf("  └────────────────────────────────────────────\n");
}

/* ── abstract machine operations ────────────────────────────────────── */

/*
 * open — transition from CLOSED to OPEN.
 * Allocates a kernel file descriptor and, for readable modes,
 * loads existing file contents into user_buf.
 */
int fam_open(FileAM *f, const char *path, FileMode mode) {
    int flags;
    switch (mode) {
        case MODE_READ:   flags = O_RDONLY; break;
        case MODE_WRITE:  flags = O_WRONLY | O_CREAT | O_TRUNC;  break;
        case MODE_APPEND: flags = O_WRONLY | O_CREAT | O_APPEND; break;
        case MODE_RW:     flags = O_RDWR   | O_CREAT;             break;
        default: return -1;
    }

    f->fd = open(path, flags, 0644);
    if (f->fd < 0) { perror("fam_open"); return -1; }

    strncpy(f->path, path, sizeof(f->path) - 1);
    f->mode    = mode;
    f->dirty   = 0;
    f->buf_len = 0;
    f->offset  = 0;

    /* pre-load existing content for readable modes */
    if (mode == MODE_READ || mode == MODE_RW) {
        ssize_t n = read(f->fd, f->user_buf, BUF_CAPACITY);
        if (n < 0) { perror("fam_open: read"); close(f->fd); return -1; }
        f->buf_len = (size_t)n;
        /* reset to start for r+w */
        if (mode == MODE_RW) lseek(f->fd, 0, SEEK_SET);
    }

    printf("open(\"%s\", \"%s\") → fd = %d\n", path, mode_str(mode), f->fd);
    return f->fd;
}

/*
 * write — copy bytes into user_buf at current offset (buffered).
 * Data is NOT on disk until fam_flush / fam_close.
 */
ssize_t fam_write(FileAM *f, const void *buf, size_t n) {
    if (f->fd < 0) { fprintf(stderr, "fam_write: not open\n"); return -1; }

    size_t space = BUF_CAPACITY - f->offset;
    size_t to_write = n < space ? n : space;

    memcpy(f->user_buf + f->offset, buf, to_write);
    f->offset += to_write;
    if (f->offset > f->buf_len) f->buf_len = f->offset;
    f->dirty = 1;

    printf("write(%zu bytes) → buffered at offset %zu–%zu  [NOT on disk yet]\n",
           to_write, f->offset - to_write, f->offset - 1);
    return (ssize_t)to_write;
}

/*
 * read — copy bytes from user_buf at current offset.
 */
ssize_t fam_read(FileAM *f, void *buf, size_t n) {
    if (f->fd < 0) { fprintf(stderr, "fam_read: not open\n"); return -1; }

    size_t avail   = f->buf_len - f->offset;
    size_t to_read = n < avail ? n : avail;

    if (to_read == 0) { printf("read(%zu) → EOF\n", n); return 0; }

    memcpy(buf, f->user_buf + f->offset, to_read);
    f->offset += to_read;

    printf("read(%zu bytes) → \"%.*s\"  cursor now at %zu\n",
           to_read, (int)to_read, (char *)buf, f->offset);
    return (ssize_t)to_read;
}

/*
 * seek — reposition cursor without I/O.
 * whence: SEEK_SET, SEEK_CUR, SEEK_END  (from <unistd.h>)
 */
int fam_seek(FileAM *f, long offset, int whence) {
    if (f->fd < 0) { fprintf(stderr, "fam_seek: not open\n"); return -1; }

    long new_off;
    switch (whence) {
        case SEEK_SET: new_off = offset;                          break;
        case SEEK_CUR: new_off = (long)f->offset + offset;       break;
        case SEEK_END: new_off = (long)f->buf_len + offset;      break;
        default: fprintf(stderr, "fam_seek: bad whence\n"); return -1;
    }

    if (new_off < 0 || (size_t)new_off > f->buf_len) {
        fprintf(stderr, "fam_seek: offset %ld out of range [0, %zu]\n",
                new_off, f->buf_len);
        return -1;
    }

    f->offset = (size_t)new_off;
    printf("seek(%ld, %s) → cursor = %zu\n",
           offset,
           whence == SEEK_SET ? "SEEK_SET" :
           whence == SEEK_CUR ? "SEEK_CUR" : "SEEK_END",
           f->offset);
    return 0;
}

/*
 * flush — write user_buf to disk (write-back).
 * After this call dirty=0 and the kernel page cache holds the data.
 */
int fam_flush(FileAM *f) {
    if (f->fd < 0) { fprintf(stderr, "fam_flush: not open\n"); return -1; }
    if (!f->dirty) { printf("flush() → nothing to flush\n"); return 0; }

    /* rewind fd, write full buffer */
    if (lseek(f->fd, 0, SEEK_SET) < 0) { perror("fam_flush: lseek"); return -1; }
    ssize_t written = write(f->fd, f->user_buf, f->buf_len);
    if (written < 0) { perror("fam_flush: write"); return -1; }

    f->dirty = 0;
    printf("flush() → %zd bytes written to disk  [dirty=0]\n", written);
    return 0;
}

/*
 * close — implicit flush if dirty, then release fd.
 */
int fam_close(FileAM *f) {
    if (f->fd < 0) { fprintf(stderr, "fam_close: not open\n"); return -1; }

    if (f->dirty) {
        printf("close() → dirty buffer, flushing first…\n");
        if (fam_flush(f) < 0) return -1;
    }

    close(f->fd);
    printf("close() → fd %d released\n", f->fd);

    f->fd      = -1;
    f->buf_len = 0;
    f->offset  = 0;
    return 0;
}

/* ── demo ───────────────────────────────────────────────────────────── */

