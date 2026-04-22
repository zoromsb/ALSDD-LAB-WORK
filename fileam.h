/*
 * fileam.h — File Abstract Machine
 *
 * Models the POSIX file as a state machine:
 *
 *   State = { fd, mode, offset, user_buf[BUF_CAPACITY], buf_len, dirty }
 *
 *   Transitions
 *     fam_open   (path, mode)      CLOSED → OPEN, loads existing data into buf
 *     fam_write  (buf, n)          copies n bytes into user_buf (NOT on disk yet)
 *     fam_read   (buf, n)          copies n bytes from user_buf at cursor
 *     fam_seek   (offset, whence)  moves cursor; no I/O
 *     fam_flush  ()                write-back: user_buf → disk  (dirty → 0)
 *     fam_close  ()                implicit flush if dirty, then releases fd
 *
 * Buffering model
 *   Writes go to user_buf first (cheap, in-memory).
 *   dirty=1 means user_buf has data not yet on disk.
 *   Only fam_flush / fam_close perform the actual write(2) syscall.
 *
 * Usage
 *   FileAM f = { .fd = -1 };
 *   fam_open(&f, "out.txt", MODE_WRITE);
 *   fam_write(&f, "hello", 5);
 *   fam_close(&f);           // implicit flush
 */

#ifndef FILEAM_H
#define FILEAM_H

#include <stddef.h>    /* size_t  */
#include <sys/types.h> /* ssize_t */

/* ── constants ──────────────────────────────────────────────────────── */

/** Maximum bytes held in the user-space buffer. */
#define BUF_CAPACITY 256

/* ── types ──────────────────────────────────────────────────────────── */

/**
 * FileMode — open-mode flags mirroring fopen() semantics.
 *
 *   MODE_READ    O_RDONLY              — read only, file must exist
 *   MODE_WRITE   O_WRONLY|CREAT|TRUNC — write only, truncates on open
 *   MODE_APPEND  O_WRONLY|CREAT|APPEND — append only
 *   MODE_RW      O_RDWR|CREAT          — read + write
 */
typedef enum {
    MODE_READ,
    MODE_WRITE,
    MODE_APPEND,
    MODE_RW
} FileMode;

/**
 * FileAM — the complete abstract machine state.
 *
 * Do not modify fields directly; use the fam_* functions below.
 *
 *   fd        kernel file descriptor (-1 = closed)
 *   mode      how the file was opened
 *   path      null-terminated path string (copy, max 255 chars)
 *   dirty     1 if user_buf has data not yet written to disk
 *   user_buf  user-space I/O buffer
 *   buf_len   number of valid bytes in user_buf
 *   offset    current read/write cursor position within user_buf
 */
typedef struct {
    int           fd;
    FileMode      mode;
    char          path[256];
    int           dirty;
    unsigned char user_buf[BUF_CAPACITY];
    size_t        buf_len;
    size_t        offset;
} FileAM;

/* ── lifecycle ──────────────────────────────────────────────────────── */

/**
 * fam_open — open path in the given mode.
 * For MODE_READ and MODE_RW, loads existing file contents into user_buf.
 * Returns the kernel fd on success, -1 on error (errno set).
 *
 * Precondition: f->fd == -1  (file not already open).
 */
int fam_open(FileAM *f, const char *path, FileMode mode);

/**
 * fam_close — flush if dirty, then release the file descriptor.
 * After this call f->fd == -1.
 * Returns 0 on success, -1 on error.
 */
int fam_close(FileAM *f);

/* ── I/O operations ─────────────────────────────────────────────────── */

/**
 * fam_write — copy n bytes from buf into user_buf at the current offset.
 * Advances offset by the number of bytes written.
 * Sets dirty=1.  Data is NOT on disk until fam_flush or fam_close.
 * Returns bytes written, or -1 if the file is not open.
 */
ssize_t fam_write(FileAM *f, const void *buf, size_t n);

/**
 * fam_read — copy up to n bytes from user_buf at the current offset into buf.
 * Advances offset by the number of bytes read.
 * Returns bytes read (0 = EOF), or -1 if the file is not open.
 */
ssize_t fam_read(FileAM *f, void *buf, size_t n);

/**
 * fam_seek — reposition the cursor without performing any I/O.
 * @offset   byte offset relative to whence.
 * @whence   SEEK_SET, SEEK_CUR, or SEEK_END  (from <unistd.h>).
 * Returns 0 on success, -1 on error (out-of-range or bad whence).
 */
int fam_seek(FileAM *f, long offset, int whence);

/* ── persistence ────────────────────────────────────────────────────── */

/**
 * fam_flush — commit user_buf to disk via write(2).
 * Rewinds the kernel fd to offset 0, overwrites with the full buffer.
 * Sets dirty=0 on success.
 * Returns 0 on success, -1 on error.
 */
int fam_flush(FileAM *f);

/* ── diagnostics ────────────────────────────────────────────────────── */

/**
 * fam_state_dump — print a formatted state summary to stdout showing
 * fd, path, mode, offset, buf_len, dirty flag, and buffer contents.
 */
void fam_state_dump(const FileAM *f);

#endif /* FILEAM_H */
