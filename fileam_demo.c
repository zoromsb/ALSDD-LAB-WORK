/*
 * fileam_demo.c — demo for the file abstract machine.
 * Compile: gcc -Wall -o fileam_demo fileam_demo.c fileam.c
 */

#include "fileam.c"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    printf("=== File Abstract Machine — string I/O demo ===\n\n");

    FileAM f = { .fd = -1 };
    const char *path = "/tmp/fileam_demo.txt";

    /* ── write multiple strings ─────────────────────────── */
    printf("-- writing strings to file\n");
    fam_open(&f, path, MODE_WRITE);

    const char *lines[] = {
        "first line\n",
        "second line\n",
        "third line\n"
    };
    for (int i = 0; i < 3; i++)
        fam_write(&f, lines[i], strlen(lines[i]));

    fam_state_dump(&f);
    fam_close(&f);   /* implicit flush */

    /* ── read back line by line ─────────────────────────── */
    printf("\n-- reading file back\n");
    fam_open(&f, path, MODE_READ);

    char buf[64];
    ssize_t n;
    while ((n = fam_read(&f, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("  read %zd bytes: [%s]\n", n, buf);
        break;   /* single buffered read gets the whole file */
    }

    fam_close(&f);

    /* ── seek + overwrite a word in place ───────────────── */
    printf("\n-- seek to byte 6, overwrite 'line' with 'LINE'\n");
    fam_open(&f, path, MODE_RW);
    fam_seek(&f, 6, SEEK_SET);
    fam_write(&f, "LINE", 4);
    fam_flush(&f);
    fam_state_dump(&f);
    fam_close(&f);

    /* ── verify final content ───────────────────────────── */
    printf("\n-- final file content:\n");
    fam_open(&f, path, MODE_READ);
    char result[BUF_CAPACITY] = {0};
    fam_read(&f, result, f.buf_len);
    printf("%s", result);
    fam_close(&f);

    unlink(path);
    return 0;
}
