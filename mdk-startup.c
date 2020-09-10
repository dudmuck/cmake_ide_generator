#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#include "uvision.h"
#include "board-mcu-hack.h"

typedef enum {
    STATE_FIND_VECTOR_SECTION,
    STATE_FIND_WORD,
} state_e;

int generate_startup(const char *gnu_startup_asm_filename, unsigned stack_size, unsigned heap_size, const char *vector_symbol)
{
    state_e state = STATE_FIND_VECTOR_SECTION;
    char *buffer = NULL;
    int ret = 0;
    FILE *gnu;
    struct stat st;
    if (stat(gnu_startup_asm_filename, &st) < 0) {
        perror("stat-asm");
        return -1;
    }
    gnu = fopen(gnu_startup_asm_filename, "r");
    if (gnu == NULL) {
        perror(gnu_startup_asm_filename);
        return -1;
    }
    buffer = malloc(st.st_size+1);

#ifdef __WIN32__
    ret = fread_(buffer, st.st_size, gnu);
#else
    ret = fread(buffer, st.st_size, 1, gnu);
#endif
    buffer[st.st_size] = 0;

    const char *ptr = buffer;
    const char *end = buffer + st.st_size;
    const char *found;
    for (; ptr < end;) {
        int line_length;
        char line[128];
        const char *target;

        if (state == STATE_FIND_VECTOR_SECTION)
            target = ".section";
        else if (state == STATE_FIND_WORD)
            target = ".word";

        found = strstr(ptr, target);
        if (!found)
            break;
        const char *line_end_cr = strchr(found, '\r');
        const char *line_end_lf = strchr(found, '\n');
        const char *line_end = line_end_lf < line_end_cr  ? line_end_lf : line_end_cr;

        line_length = line_end - found;

        strncpy(line, found, line_length);
        line[line_length] = 0;

        ptr = found + line_length;

        if (state == STATE_FIND_VECTOR_SECTION) {
            if (strstr(line, vector_symbol) != 0) {
                printf("%s in line\r\n", vector_symbol);
                state = STATE_FIND_WORD;
            }
        } else if (state == STATE_FIND_WORD) {
            printf("WORD %s\r\n", line);
        }

    }

//asm_done:
    free(buffer);
    fclose(gnu);
    return ret;
}
