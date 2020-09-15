#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>

#include "uvision.h"
#include "board-mcu-hack.h"

typedef enum {
    STATE_FIND_VECTOR_SECTION,
    STATE_FIND_WORD,
} state_e;

struct node_s {
    char *isr;
    char *comment;
    struct node_s *next;
};

int generate_startup(const char *gnu_startup_asm_filename, unsigned stack_size, unsigned heap_size, const char *vector_symbol)
{
    const char * const EXPORT = "EXPORT  ";
    const char * const WEAK = "[WEAK]";
    const char *__Vectors = "__Vectors";
    const char *__initial_sp = "__initial_sp";
    char out_str[128];
    unsigned vect_num, out_str_idx;
    FILE *keil_asm = NULL;
    char *bn, *out_filename = NULL;
    state_e state = STATE_FIND_VECTOR_SECTION;
    char *buffer = NULL;
    int ret = 0;
    FILE *gnu;
    struct stat st;
    struct node_s *vector_list = NULL;

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

    out_filename = malloc(strlen(gnu_startup_asm_filename));
    strcpy(out_filename, gnu_startup_asm_filename);
    bn = basename(out_filename);

    keil_asm = fopen(bn, "w");
    if (!keil_asm) {
        perror(bn);
        goto asm_done;
    }

    fprintf(keil_asm, "; Amount of memory (in bytes) allocated for Stack\n");
    fprintf(keil_asm, "; Tailor this value to your application needs\n");
    fprintf(keil_asm, "; <h> Stack Configuration\n");
    fprintf(keil_asm, ";   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>\n");
    fprintf(keil_asm, "; </h>\n");
    fprintf(keil_asm, "Stack_Size      EQU     0x%08x\n\n", stack_size);
    fprintf(keil_asm, "                AREA    STACK, NOINIT, READWRITE, ALIGN=3\n");
    fprintf(keil_asm, "Stack_Mem       SPACE   Stack_Size\n");
    fprintf(keil_asm, "__initial_sp\n\n\n");
    fprintf(keil_asm, "; <h> Heap Configuration\n");
    fprintf(keil_asm, ";   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>\n");
    fprintf(keil_asm, "; </h>\n");
    fprintf(keil_asm, "Heap_Size       EQU     0x%08x\n\n", heap_size);
    fprintf(keil_asm, "                AREA    HEAP, NOINIT, READWRITE, ALIGN=3\n");
    fprintf(keil_asm, "__heap_base\n");
    fprintf(keil_asm, "Heap_Mem        SPACE   Heap_Size\n");
    fprintf(keil_asm, "__heap_limit\n\n");
    fprintf(keil_asm, "                PRESERVE8\n");
    fprintf(keil_asm, "                THUMB\n\n\n");
    fprintf(keil_asm, "; Vector Table Mapped to Address 0 at Reset\n");
    fprintf(keil_asm, "                AREA    RESET, DATA, READONLY\n");
    fprintf(keil_asm, "                EXPORT  __Vectors\n");
    fprintf(keil_asm, "                EXPORT  __Vectors_End\n");
    fprintf(keil_asm, "                EXPORT  __Vectors_Size\n\n");


    const char *ptr = buffer;
    const char *end = buffer + st.st_size;
    const char *found;
    unsigned isr_max_len = 0;
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
                state = STATE_FIND_WORD;
            }
        } else if (state == STATE_FIND_WORD) {
            unsigned n, isr_len, comment_len = 0;
            const char *lp, *endPtr;
            char isr[64];
            char comment[96];

            for (lp = strchr(line, ' '); *lp == ' '; lp++)
                ;
            endPtr = strchr(lp, ' ');
            if (!endPtr)
                endPtr = strchr(lp, '\r');
            if (!endPtr)
                endPtr = strchr(lp, '\n');
            if (!endPtr)
                endPtr = strchr(lp, 0);
            for (n = 0; lp < endPtr; lp++)
                isr[n++] = *lp;
            isr[n] = 0;
            isr_len = n;

            lp = strstr(lp, "/*");
            if (lp) {
                n = 0;
                lp += 2;
                for (endPtr = strstr(lp, "*/"); lp < endPtr; lp++)
                    comment[n++] = *lp;
                comment[n] = 0;
                comment_len = n;
            }

            struct node_s **node;
            node = &vector_list;
            if (vector_list) {
                while ((*node)->next) {
                    node = &(*node)->next;
                }
                node = &(*node)->next;
            }
            (*node) = malloc(sizeof(struct node_s));
            (*node)->next = NULL;

            if (isr_len > 0) {
                (*node)->isr = malloc(isr_len+1);
                strncpy((*node)->isr, isr, isr_len);
                (*node)->isr[isr_len] = 0;
                if (isr_len > isr_max_len)
                    isr_max_len = isr_len;
            } else
                (*node)->isr = NULL;

            if (comment_len > 0) {
                (*node)->comment = malloc(comment_len+1);
                strncpy((*node)->comment, comment, comment_len);
                (*node)->comment[comment_len] = 0;
            } else
                (*node)->comment = NULL;
        } // ..if (state == STATE_FIND_WORD)

    }

    struct node_s *my_list;
    vect_num = 0;
    for (my_list = vector_list; my_list != NULL; my_list = my_list->next) {
        unsigned isr_column = 25;
        out_str_idx = 0;

        if (vect_num == 0) {
            strcpy(out_str, __Vectors);
            out_str_idx += strlen(__Vectors);
        }

        while (out_str_idx < 17)
            out_str[out_str_idx++] = ' ';

        out_str[out_str_idx++] = 'D';
        out_str[out_str_idx++] = 'C';
        out_str[out_str_idx++] = 'D';

        while (out_str_idx < isr_column)
            out_str[out_str_idx++] = ' ';

        if (vect_num == 0) {
            strcpy(out_str+out_str_idx, __initial_sp);
            out_str_idx += strlen(__initial_sp);
        } else {
            strcpy(out_str+out_str_idx, my_list->isr);
            out_str_idx += strlen(my_list->isr);
        }

        if (my_list->comment) {
            unsigned column = isr_max_len + isr_column + 1;
            while (out_str_idx < column)
                out_str[out_str_idx++] = ' ';
            out_str[out_str_idx++] = ';';
            strcpy(out_str+out_str_idx, my_list->comment);
            out_str_idx += strlen(my_list->comment);
        }

        out_str[out_str_idx] = 0;
        fprintf(keil_asm, "%s\n", out_str);

        vect_num++;
    }

    fprintf(keil_asm, "__Vectors_End\n\n");
    fprintf(keil_asm, "__Vectors_Size  EQU  __Vectors_End - __Vectors\n\n");
    fprintf(keil_asm, "                AREA    |.text|, CODE, READONLY\n\n");
    fprintf(keil_asm, "; Reset handler routine\n");
    fprintf(keil_asm, "Reset_Handler    PROC\n");
    fprintf(keil_asm, "                 EXPORT  Reset_Handler                 [WEAK]\n");
    fprintf(keil_asm, "        IMPORT  __main\n");
    fprintf(keil_asm, "        IMPORT  SystemInit  \n");
    fprintf(keil_asm, "                 LDR     R0, =SystemInit\n");
    fprintf(keil_asm, "                 BLX     R0\n");
    fprintf(keil_asm, "                 LDR     R0, =__main\n");
    fprintf(keil_asm, "                 BX      R0\n");
    fprintf(keil_asm, "                 ENDP\n\n");
    fprintf(keil_asm, "; Dummy Exception Handlers (infinite loops which can be modified)\n");

    for (my_list = vector_list, vect_num = 0; my_list != NULL && vect_num < 16; my_list = my_list->next, vect_num++) {
        if (vect_num > 1 && my_list->isr[0] != '0') {
            unsigned base_col;
            strcpy(out_str, my_list->isr);
            out_str_idx = strlen(my_list->isr);

            while (out_str_idx < (isr_max_len+1))
                out_str[out_str_idx++] = ' ';

            out_str[out_str_idx++] = 'P';
            out_str[out_str_idx++] = 'R';
            out_str[out_str_idx++] = 'O';
            out_str[out_str_idx++] = 'C';

            out_str[out_str_idx] = 0;
            fprintf(keil_asm, "%s\n", out_str);

            out_str_idx = 0;
            while (out_str_idx < (isr_max_len+1))
                out_str[out_str_idx++] = ' ';

            strcpy(out_str+out_str_idx, EXPORT);
            out_str_idx += strlen(EXPORT);

            base_col = out_str_idx;
            strcpy(out_str+out_str_idx, my_list->isr);
            out_str_idx += strlen(my_list->isr);

            while (out_str_idx < (base_col+isr_max_len+1))
                out_str[out_str_idx++] = ' ';

            strcpy(out_str+out_str_idx, WEAK);
            out_str_idx += strlen(WEAK);

            out_str[out_str_idx] = 0;
            fprintf(keil_asm, "%s\n", out_str);

            out_str_idx = 0;

            while (out_str_idx < (isr_max_len+1))
                out_str[out_str_idx++] = ' ';

            out_str[out_str_idx++] = 'B';

            while (out_str_idx < base_col)
                out_str[out_str_idx++] = ' ';

            out_str[out_str_idx++] = '.';

            out_str[out_str_idx] = 0;
            fprintf(keil_asm, "%s\n", out_str);

            out_str_idx = 0;

            while (out_str_idx < (isr_max_len+1))
                out_str[out_str_idx++] = ' ';

            out_str[out_str_idx++] = 'E';
            out_str[out_str_idx++] = 'N';
            out_str[out_str_idx++] = 'D';
            out_str[out_str_idx++] = 'P';

            out_str[out_str_idx] = 0;
            fprintf(keil_asm, "%s\n", out_str);
        }
    }

    fprintf(keil_asm, "\nDefault_Handler PROC\n\n");
    for (my_list = vector_list, vect_num = 0; my_list != NULL; my_list = my_list->next, vect_num++) {
        if (vect_num > 15 && my_list->isr[0] != '0') {
            unsigned base_col;
            out_str_idx = 0;
            while (out_str_idx < 17)
                out_str[out_str_idx++] = ' ';

            strcpy(out_str+out_str_idx, EXPORT);
            out_str_idx += strlen(EXPORT);

            base_col = out_str_idx;
            strcpy(out_str+out_str_idx, my_list->isr);
            out_str_idx += strlen(my_list->isr);

            while (out_str_idx < (base_col+isr_max_len+1))
                out_str[out_str_idx++] = ' ';

            strcpy(out_str+out_str_idx, WEAK);
            out_str_idx += strlen(WEAK);

            out_str[out_str_idx] = 0;
            fprintf(keil_asm, "%s\n", out_str);
        }
    }

    fprintf(keil_asm, "\n\n");
    for (my_list = vector_list, vect_num = 0; my_list != NULL; my_list = my_list->next, vect_num++) {
        if (vect_num > 15 && my_list->isr[0] != '0') {
            fprintf(keil_asm, "%s\n", my_list->isr);
        }
    }

    fprintf(keil_asm, "\n                B       .\n\n");
    fprintf(keil_asm, "                ENDP\n\n");
    fprintf(keil_asm, "                ALIGN\n\n");

    fprintf(keil_asm, "; User Stack and Heap initialization\n");
    fprintf(keil_asm, "                 IF      :DEF:__MICROLIB\n\n");
    fprintf(keil_asm, "                 EXPORT  __initial_sp\n");
    fprintf(keil_asm, "                 EXPORT  __heap_base\n");
    fprintf(keil_asm, "                 EXPORT  __heap_limit\n\n");
    fprintf(keil_asm, "                 ELSE\n\n");
    fprintf(keil_asm, "                 IMPORT  __use_two_region_memory\n");
    fprintf(keil_asm, "                 EXPORT  __user_initial_stackheap\n\n");
    fprintf(keil_asm, "__user_initial_stackheap\n\n");
    fprintf(keil_asm, "                 LDR     R0, =  Heap_Mem\n");
    fprintf(keil_asm, "                 LDR     R1, =(Stack_Mem + Stack_Size)\n");
    fprintf(keil_asm, "                 LDR     R2, = (Heap_Mem +  Heap_Size)\n");
    fprintf(keil_asm, "                 LDR     R3, = Stack_Mem\n");
    fprintf(keil_asm, "                 BX      LR\n\n");
    fprintf(keil_asm, "                 ALIGN\n\n");
    fprintf(keil_asm, "                 ENDIF\n\n");
    fprintf(keil_asm, "                 END\n");

asm_done:
    free(out_filename);
    fclose(keil_asm);
    free(buffer);
    fclose(gnu);
    return ret;
}
