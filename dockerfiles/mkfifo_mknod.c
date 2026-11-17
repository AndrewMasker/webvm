/*
 * mkfifo replacement that creates FIFOs with the mknod system call.
 *
 * Bookworm's glibc implements mkfifo() on top of the mknodat system call, which
 * WebVM's CheerpX doesn't implement (mkfifo fails with "Function not implemented").
 * The older mknod system call works there; it's what Buster's mkfifo used.
 *
 * Installed as /usr/local/bin/mkfifo, which comes before /usr/bin in PATH.
 * Usage: mkfifo [-m MODE] [--] NAME...
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int i = 1, status = 0, have_mode = 0;
	mode_t mode = 0666;

	while (i < argc && argv[i][0] == '-') {
		if (strcmp(argv[i], "--") == 0) {
			i++;
			break;
		} else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
			char *end;
			mode = (mode_t)strtol(argv[i + 1], &end, 8);
			if (*end != '\0') {
				fprintf(stderr, "mkfifo: invalid mode '%s' (octal only)\n", argv[i + 1]);
				return 1;
			}
			have_mode = 1;
			i += 2;
		} else {
			fprintf(stderr, "mkfifo: unsupported option '%s'\n", argv[i]);
			return 1;
		}
	}
	if (i >= argc) {
		fprintf(stderr, "mkfifo: missing operand\n");
		return 1;
	}

	for (; i < argc; i++) {
		if (syscall(SYS_mknod, argv[i], S_IFIFO | mode, 0) != 0) {
			fprintf(stderr, "mkfifo: cannot create fifo '%s': %s\n", argv[i], strerror(errno));
			status = 1;
		} else if (have_mode && chmod(argv[i], mode) != 0) {
			fprintf(stderr, "mkfifo: cannot set permissions of '%s': %s\n", argv[i], strerror(errno));
			status = 1;
		}
	}
	return status;
}
