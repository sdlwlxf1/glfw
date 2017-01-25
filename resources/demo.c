#include <stdio.h>
#include "syscalls.h"

main() {
    char buf[BUFFSIZ];
    int n;
    
    while((n = read(0, buf, BUFSIZ)) > 0)
        write(1, buf, n);
    return 0;
}

int getchar(void)
{
    char c;
    return (read(0, &c, 1) == 1) ? (unsigned char) c : EOF;
}

int getchar(void)
{
    static char buf[BUFSIZ];
    static char *bufp = buf;
    static int n = 0;

    if (n == 0) {
        n = read(0, buf, sizeof(buf));
        bufp = buf;
    }
    return (--n >= 0) ? (unsigned char) *bufp++ : EOF;
}

void main(int argc, char *argv[]) {
    FILE *fp;
    void fileCopy(FILE *dest, FILE *src);

    if(argc == 1)
        filecopy(stdout, stdin);
    else
        while(--argc > 0)
            if((fp = fopen(*++argv, "r")) == NULL) {
                printf("cat: can't open %s\n", *argv);
                return 1;
            } else {
                filecopy(stdout, fp);
                fclose(fp);
            }
    return 0;
}

void fileCopy(FILE *dest, FILE *src)
{
    char c;
    while((c = getc(src)) != EOF)
        putc(c, dest);
}
