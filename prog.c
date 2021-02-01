#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) return -1;
    char *filename = argv[1];
    int count = atoi(argv[2]);

    FILE *f;
    for (int i = 0; i <= count; i++) {
        f = fopen(filename, "w");
        fprintf(f, "Process ran %d out of %d secs\n", i, count);
        fclose(f);
        sleep(1);
    }
}

