#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

#include "board-mcu-hack.h"

struct nucleo_table_s {
    const char * const inBoard;
    const char * const outBoard;
    const char * const outMcu;
};

const struct nucleo_table_s nucleo_table[] = {
    { "f030", "NUCLEO-F030R8", "STM32F030R8Tx" },
    { "f070", "NUCLEO-F070RB", "STM32F070RBTx" },
    { "f072", "NUCLEO-F072RB", "STM32F072RBTx" },
    { "f091", "NUCLEO-F091RC", "STM32F091RCTx" },
    { "f103", "NUCLEO-F103RB", "STM32F103RBTx" },
    { "f302", "NUCLEO-F302R8", "STM32F302R8Tx" },
    { "f303", "NUCLEO-F303RE", "STM32F303RETx" },
    { "f334", "NUCLEO-F334R8", "STM32F334R8Tx" },
    { "f401", "NUCLEO-F401RE", "STM32F401RETx" },
    { "f410", "NUCLEO-F410RB", "STM32F410RBTx" },
    { "f411", "NUCLEO-F411RE", "STM32F411RETx" },
    { "f446", "NUCLEO-F446RE", "STM32F446RETx" },
    { "g070", "NUCLEO-G070RB", "STM32G070RBTx" },
    { "g071", "NUCLEO-G071RB", "STM32G071RBTx" },
    { "g431", "NUCLEO-G431RB", "STM32G431RBTx" },
    { "g474", "NUCLEO-G474RE", "STM32G474RETx" },
    { "l010", "NUCLEO-L010RB", "STM32L010RBTx" },
    { "l053", "NUCLEO-L053R8", "STM32L053R8Tx" },
    { "l073", "NUCLEO-L073RZ", "STM32L073RZTx" },
    { "l152", "NUCLEO-L152RE", "STM32L152RETx" },
    { "l452", "NUCLEO-L452RE", "STM32L452RETx" },
    { "l476", "NUCLEO-L476RG", "STM32L476RGTx" },
    { "l412", "NUCLEO-L412RB-P", "STM32L412RBTx" },
    { "l433", "NUCLEO-L433RC-P", "STM32L433RCTx" },
    { "l452", "NUCLEO-L452RE-P", "STM32L452RETx" },
    { NULL, NULL, NULL }
};

struct board_table_s {
    const char * const board;
    const char * const outMcu;
};

const struct board_table_s board_table[] = {
    { "SKiM980A",       "STM32L151CBUxA" },
    { "SKiM881AXL",     "STM32L081CZTx" },
    { "SKiM880B",       "STM32L151CBUxA" },
    { "NAMote72",       "STM32L152RCTx" },
    { "B-L072Z-LRWAN1", "STM32L072CZYx" },
    { "MSP-EXP432P401R", "MSP432P401R" },
    { NULL, NULL }

};

void string_to_lower(const char *in, char *out)
{
    unsigned x;
    for (x = 0; in[x] != 0; x++)
        out[x] = tolower(in[x]);

    out[x] = 0;
}

int try_json(const char *board_in, char *board_out, char *mcu_out)
{
    struct stat st;
    struct json_object *parsed_json;
    int ret = -1;
    char *buffer = NULL;
    char lower[64];
    const char *filename = "translate_board_mcu.json";

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        goto json_done;
    }

    if (stat(filename, &st) < 0) {
        goto json_done;
    }

    buffer = malloc(st.st_size);
#ifdef __WIN32__
    fread_(buffer, st.st_size, fp);
#else
    fread(buffer, st.st_size, 1, fp);
#endif

    parsed_json = json_tokener_parse(buffer);
    if (parsed_json == NULL) {
        printf("unable to parse json in %s\r\n", filename);
        goto json_done;
    }

    struct json_object *groups_jobj;
    if (!json_object_object_get_ex(parsed_json, "groups", &groups_jobj)) {
        printf("%s no groups\r\n", filename);
        goto json_done;
    }
    unsigned n_groups = json_object_array_length(groups_jobj);
    for (unsigned n = 0; n < n_groups; n++) {
        struct json_object *family_jobj, *members_jobj;
        struct json_object *group_jobj = json_object_array_get_idx(groups_jobj, n);
        if (!json_object_object_get_ex(group_jobj, "members", &members_jobj))
            continue;
        unsigned n_members = json_object_array_length(members_jobj);
        if (json_object_object_get_ex(group_jobj, "family", &family_jobj)) {
            const char *familyName = json_object_get_string(family_jobj);
            string_to_lower(familyName, lower);
            if (strstr(board_in, lower) == NULL) {
                continue;   // this family not for board, on to next group
            }
            for (unsigned i = 0; i < n_members; i++) {
                struct json_object *substring_jobj;
                struct json_object *realName_jobj;
                struct json_object *mcu_jobj;
                struct json_object *member_jobj = json_object_array_get_idx(members_jobj, i);
                json_object_object_get_ex(member_jobj, "substring", &substring_jobj);
                const char *substring = json_object_get_string(substring_jobj);
                string_to_lower(substring, lower);
                if (strstr(board_in, lower) != 0) {
                    ret = 0;
                    if (json_object_object_get_ex(member_jobj, "realName", &realName_jobj))
                        strcpy(board_out, json_object_get_string(realName_jobj));
                    if (json_object_object_get_ex(member_jobj, "mcu", &mcu_jobj))
                        strcpy(mcu_out, json_object_get_string(mcu_jobj));
                }
            }
        } else {
            for (unsigned i = 0; i < n_members; i++) {
                struct json_object *substring_jobj;
                struct json_object *realName_jobj;
                struct json_object *mcu_jobj;
                struct json_object *member_jobj = json_object_array_get_idx(members_jobj, i);
                json_object_object_get_ex(member_jobj, "substring", &substring_jobj);
                const char *substring = json_object_get_string(substring_jobj);
                string_to_lower(substring, lower);
                if (strstr(lower, board_in) != 0) {
                    if (json_object_object_get_ex(member_jobj, "realName", &realName_jobj))
                        strcpy(board_out, json_object_get_string(realName_jobj));
                    else
                        strcpy(board_out, substring);

                    if (json_object_object_get_ex(member_jobj, "mcu", &mcu_jobj))
                        strcpy(mcu_out, json_object_get_string(mcu_jobj));
                    else
                        mcu_out[0] = 0;
                }
            }
        }
    }

json_done:
    if (buffer)
        free(buffer);
    return ret;
}

void translate_board_mcu(const char *board_in, char *board_out, char *mcu_out)
{
    unsigned n;
    char in_lower[64];

    board_out[0] = 0;
    mcu_out[0] = 0;

    string_to_lower(board_in, in_lower);

    if (try_json(in_lower, board_out, mcu_out) == 0) {
        /* board translated from json, skip internal table */
        printf("board found from json\r\n");
        return;
    }

    if (strstr(in_lower, "nucleo") != NULL) {
        for (n = 0; nucleo_table[n].inBoard != NULL; n++) {
            if (strstr(in_lower, nucleo_table[n].inBoard) != NULL) {
                strcpy(board_out, nucleo_table[n].outBoard);
                strcpy(mcu_out, nucleo_table[n].outMcu);
                break;
            }
        }
    } else {
        char bl[64];
        /* case insensitive string compare */
        for (n = 0; board_table[n].board != NULL; n++) {
            char *ptr;
            strcpy(bl, board_table[n].board);
            for (ptr = bl; *ptr != 0; ptr++) {
                *ptr = tolower(*ptr);
            }
            *ptr = 0;

            if (strcmp(in_lower, bl) == 0) {
                strcpy(board_out, board_table[n].board);
                strcpy(mcu_out, board_table[n].outMcu);
            }
        }
    }
}

#ifdef __WIN32__
int fread_(char *buffer, unsigned len, FILE *fp)
{
    int ret, fd = fileno(fp);
    char *ptr = buffer;
    unsigned n;

    for (n = 0; n < len; ) {
        unsigned to_read = len - n;
        ret = read(fd, ptr, to_read);
        if (ret < 1)
            return n;
        n += ret;
        ptr += ret;
    }
    return n;
}
#endif /* __WIN32__ */

