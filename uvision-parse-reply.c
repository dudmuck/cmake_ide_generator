
#include <sys/stat.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>
#include <json-c/json.h>

#include "board-mcu-hack.h"
#include "uvision.h"
#include "xml.h"

/*
 * ARM-ADS: Arm Developer Studio
 */

const char * const mcu_families[] = {
    "STM32G0",
    "STM32L4",
    "STM32L0",
    "STM32L1",
    NULL
};

struct node_s {
    char *str;
    bool taken;
    struct node_s *next;
};

typedef struct {
    char artifactName[96];
    char project_name[64];
    char linkerScript[192];  // full path, absolute path
    struct node_s *specs_list;
} from_codemodel_t;

typedef struct {
    char board[64];
} from_cache_t;

typedef struct {
    unsigned flash_base;
    unsigned flash_length;
    unsigned ram_base;
    unsigned ram_length;
    char vector_symbol[96];
} from_linker_script_t;

typedef struct {
    uint32_t rom_start;
    uint32_t rom_size;
    uint32_t ram_start;
    uint32_t ram_size;
    char rom_algorithm[192];
    char packVersion[16];
    char packVendor[96];
    char packUrl[128];
    char packName[64];
    char processorCore[96];
    char svd[128];
    char header[192];
    char ff0[64];
    char flm_path[64];
    char vendor[64];
} from_pack_t;

from_pack_t from_pack;

const char * const reply_directory = ".cmake/api/v1/reply";

from_codemodel_t from_codemodel;
from_cache_t from_cache;
from_linker_script_t from_linker_script;
unsigned cpu_clock_hz;

struct node_s *defines_list;
struct node_s *includes_list;

char full_path_codemodel[256];

xmlTextWriterPtr uvprojx_writer;
xmlTextWriterPtr uvoptx_writer;

typedef int (*parse_target_file_t)(const char *, const char *);

typedef struct {
    uint8_t type;
    unsigned start_address;
    unsigned size;
} ocr_rvct_t;

ocr_rvct_t ocr_rvct[11];

char *startup_asm_filename;

char *keil_path;

int parse_codemodel(const char *jsonFileName, parse_target_file_t ptf)
{
    struct json_object *parsed_json;
    size_t n_configurations;
    unsigned n;
    int ret;
    char *buffer = NULL;
    struct stat st;
    FILE *fp = fopen(jsonFileName, "r");
    if (fp == NULL) {
        perror(jsonFileName);
        return -1;
    }

    if (stat(jsonFileName, &st) < 0) {
        perror("stat-codemodel");
        return -1;
    }
    buffer = malloc(st.st_size);
#ifdef __WIN32__
    ret = fread_(buffer, st.st_size, fp);
#else
    ret = fread(buffer, st.st_size, 1, fp);
#endif

    parsed_json = json_tokener_parse(buffer);
    struct json_object *configurations;
    json_object_object_get_ex(parsed_json, "configurations", &configurations);
    n_configurations = json_object_array_length(configurations);
    if (n_configurations > 1) {
        /* expecting only one configuration */
        for (n = 0; n < n_configurations; n++) {
            struct json_object *name_jobj, *cfg = json_object_array_get_idx(configurations, n);
            json_object_object_get_ex(cfg, "name", &name_jobj);
            printf("configuration%u : %s\r\n", n, json_object_get_string(name_jobj));
        }
    }

    unsigned n_projects, p;
    struct json_object *projects;
    struct json_object *configuration = json_object_array_get_idx(configurations, 0);
    json_object_object_get_ex(configuration, "projects", &projects);
    n_projects = json_object_array_length(projects);
    for (p = 0; p < n_projects; p++) {
        struct json_object *project = json_object_array_get_idx(projects, p);
        struct json_object *parentIndex;
        json_object_object_get_ex(project, "parentIndex", &parentIndex);
        if (parentIndex == NULL) {
            const char *name;
            struct json_object *name_jobj;
            json_object_object_get_ex(project, "name", &name_jobj);
            name = json_object_get_string(name_jobj);
            strncpy(from_codemodel.project_name, name, sizeof(from_codemodel.project_name));
        }
    }

    struct json_object *paths_jobj;
    const char *source_path = NULL;
    if (json_object_object_get_ex(parsed_json, "paths", &paths_jobj)) {
        struct json_object *jobj;
        if (json_object_object_get_ex(paths_jobj, "source", &jobj)) {
            source_path = json_object_get_string(jobj);
        }
    }

    struct json_object *targets;
    json_object_object_get_ex(configuration, "targets", &targets);
    unsigned n_targets = json_object_array_length(targets);
    for (unsigned t = 0; t < n_targets; t++) {
        struct json_object *target = json_object_array_get_idx(targets, t);
        struct json_object *name_jobj;
        if (!json_object_object_get_ex(target, "name", &name_jobj)) {
            printf("target no name: %s\r\n", json_object_get_string(target));
        }
        struct json_object *jsonFile_jobj;
        if (json_object_object_get_ex(target, "jsonFile", &jsonFile_jobj)) {
            ptf(source_path, json_object_get_string(jsonFile_jobj));
        }
    }

    free(buffer);
    fclose(fp);
    return ret;
}

unsigned GroupNumber;
unsigned FileNumber;

int parse_target_sources_uvoptx(const char *source_path, const char *jsonFileName)
{
    struct json_object *parsed_json;
    struct stat st;
    char *buffer = NULL;
    char full_path[256];
    int ret = -1;;

    strcpy(full_path, reply_directory);
    strcat(full_path, "/");
    strcat(full_path, jsonFileName);

    if (stat(full_path, &st) < 0) {
        strcat(full_path, " stat");
        perror(full_path);
        goto target_done;
    }

    FILE *fp = fopen(full_path, "r");
    if (fp == NULL) {
        perror(full_path);
        goto target_done;
    }

    buffer = malloc(st.st_size);
#ifdef __WIN32__
    ret = fread_(buffer, st.st_size, fp);
#else
    ret = fread(buffer, st.st_size, 1, fp);
#endif

    parsed_json = json_tokener_parse(buffer);
    if (parsed_json == NULL) {
        printf("cannot parse json in %s\r\n", full_path);
        ret = -1;
        goto target_done;
    }

    struct json_object *type_jobj;
    if (!json_object_object_get_ex(parsed_json, "type", &type_jobj)) {
        printf("no type in %s\r\n", full_path);
        goto target_done;
    }

    const char *targetType = json_object_get_string(type_jobj);
    if (strcmp("OBJECT_LIBRARY", targetType) != 0 && strcmp("EXECUTABLE", targetType) != 0) {
        goto target_done;
    }

    struct json_object *sources_jobj;
    if (!json_object_object_get_ex(parsed_json, "sources", &sources_jobj)) {
        printf("no sources in %s\r\n", full_path);
        goto target_done;
    }

    struct json_object *name_jobj;
    if (!json_object_object_get_ex(parsed_json, "name", &name_jobj)) {
        printf("no name in %s\r\n", full_path);
        goto target_done;
    }

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"Group");


    const char *name = json_object_get_string(name_jobj);
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"GroupName", (xmlChar*)name);

    unsigned n_sources = json_object_array_length(sources_jobj);
    for (unsigned n = 0; n < n_sources; n++) {
        char str[256];
        struct json_object *path_jobj, *jobj;
        struct json_object *source_jobj = json_object_array_get_idx(sources_jobj, n);
        if (json_object_object_get_ex(source_jobj, "isGenerated", &jobj)) {
            if (json_object_get_boolean(jobj))
                continue;
        }
        if (!json_object_object_get_ex(source_jobj, "path", &path_jobj)) {
            printf("NO PATH: %s\r\n", json_object_get_string(source_jobj));
            continue;
        }

        char *bn, *ptr;
        const char *path = json_object_get_string(path_jobj);
        ptr = malloc(strlen(path)+1);
        strcpy(ptr, path);
        bn = basename(ptr);

        xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"File");

        sprintf(str, "%u", GroupNumber);
        xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"GroupNumber", (xmlChar*)str);

        sprintf(str, "%u", FileNumber++);
        xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"FileNumber", (xmlChar*)str);

        if (strstr(bn, ".c")) {
            xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"FileType", (xmlChar*)"1");
            strcpy(str, source_path);
            strcat(str, "/");
            strcat(str, path);
            xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"PathWithFileName", (xmlChar*)str);
        } else if (strstr(bn, ".s")) {
            xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"FileType", (xmlChar*)"2");
            strcpy(str, "./");
            strcat(str, bn);
            xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"PathWithFileName", (xmlChar*)str);
        } else {
            printf("%s unknown file type\r\n", bn);
        }

        xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"FilenameWithoutPath", (xmlChar*)bn);

        xmlTextWriterEndElement(uvoptx_writer); // File
    } // ..for (unsigned n = 0; n < n_sources; n++)

    xmlTextWriterEndElement(uvoptx_writer); // Group
    xmlTextWriterWriteRaw(uvoptx_writer, (xmlChar*)"\n");

    GroupNumber++;

target_done:
    free(buffer);
    fclose(fp);
    return ret;
}

int parse_target_sources_uvprojx(const char *source_path, const char *jsonFileName)
{
    struct json_object *parsed_json;
    struct stat st;
    char *buffer = NULL;
    char full_path[256];
    int ret = -1;;

    strcpy(full_path, reply_directory);
    strcat(full_path, "/");
    strcat(full_path, jsonFileName);

    if (stat(full_path, &st) < 0) {
        strcat(full_path, " stat");
        perror(full_path);
        goto target_done;
    }

    FILE *fp = fopen(full_path, "r");
    if (fp == NULL) {
        perror(full_path);
        goto target_done;
    }

    buffer = malloc(st.st_size);
#ifdef __WIN32__
    ret = fread_(buffer, st.st_size, fp);
#else
    ret = fread(buffer, st.st_size, 1, fp);
#endif

    parsed_json = json_tokener_parse(buffer);
    if (parsed_json == NULL) {
        printf("cannot parse json in %s\r\n", full_path);
        ret = -1;
        goto target_done;
    }

    struct json_object *type_jobj;
    if (!json_object_object_get_ex(parsed_json, "type", &type_jobj)) {
        printf("no type in %s\r\n", full_path);
        goto target_done;
    }

    const char *targetType = json_object_get_string(type_jobj);
    if (strcmp("OBJECT_LIBRARY", targetType) != 0 && strcmp("EXECUTABLE", targetType) != 0) {
        goto target_done;
    }

    struct json_object *sources_jobj;
    if (!json_object_object_get_ex(parsed_json, "sources", &sources_jobj)) {
        printf("no sources in %s\r\n", full_path);
        goto target_done;
    }

    struct json_object *name_jobj;
    if (!json_object_object_get_ex(parsed_json, "name", &name_jobj)) {
        printf("no name in %s\r\n", full_path);
        goto target_done;
    }

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"Group");

    const char *name = json_object_get_string(name_jobj);
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"GroupName", (xmlChar*)name);

    unsigned n_sources = json_object_array_length(sources_jobj);
    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"Files");
    for (unsigned n = 0; n < n_sources; n++) {
        struct json_object *path_jobj, *jobj;
        struct json_object *source_jobj = json_object_array_get_idx(sources_jobj, n);
        if (json_object_object_get_ex(source_jobj, "isGenerated", &jobj)) {
            if (json_object_get_boolean(jobj))
                continue;
        }
        if (json_object_object_get_ex(source_jobj, "path", &path_jobj)) {
            char str[256];
            char *bn, *ptr;
            const char *path = json_object_get_string(path_jobj);
            ptr = malloc(strlen(path)+1);
            strcpy(ptr, path);
            xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"File");
            bn = basename(ptr);
            xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"FileName", (xmlChar*)bn);
            str[0] = 0;
            if (strstr(bn, ".c")) {
                xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"FileType", (xmlChar*)"1");
                strcat(str, source_path);
                strcat(str, "/");
                strcat(str, path);
            } else if (strstr(bn, ".s")) {
                xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"FileType", (xmlChar*)"2");
                if (startup_asm_filename == NULL) {
                    startup_asm_filename = malloc(strlen(source_path) + strlen(path) + 2);
                    strcpy(startup_asm_filename, source_path);
                    strcat(startup_asm_filename, "/");
                    strcat(startup_asm_filename, path);
                }
                strcat(str, "./");
                strcat(str, bn);
            } else {
                printf("%s unknown file type\r\n", bn);
            }

            xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"FilePath", (xmlChar*)str);
            xmlTextWriterEndElement(uvprojx_writer); // File
            free(ptr);
        }
    }
    xmlTextWriterEndElement(uvprojx_writer); // Files

    xmlTextWriterEndElement(uvprojx_writer); // Group

target_done:
    free(buffer);
    fclose(fp);
    return ret;
}

int uvoptx_start(bool force, const char *Mcu)
{
    char str[512];
    int ret = -1;;
    char xml_filename[96];

    strcpy(xml_filename, from_codemodel.project_name);
    strcat(xml_filename, ".uvoptx");
    strcat(xml_filename, "");

    uvoptx_writer = xmlNewTextWriterFilename(xml_filename, 0);
    if (uvoptx_writer == NULL) {
        printf("cannot create xml writer\r\n");
        return -1;
    }
    ret = xmlTextWriterStartDocument(uvoptx_writer, NULL, "UTF-8", "no");
    if (ret < 0) {
        printf("Error at xmlTextWriterStartDocument\n");
        return -1;
    }
    ret = xmlTextWriterSetIndent(uvoptx_writer, 1);
    if (ret < 0) {
        printf("Error at xmlTextWriterSetIndent\n");
        return -1;
    }
    xmlTextWriterSetIndentString(uvoptx_writer, (xmlChar*)"  ");

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"ProjectOpt");
    xmlTextWriterWriteAttribute(uvoptx_writer, (xmlChar*)"xmlns:xsi", (xmlChar*)"http://www.w3.org/2001/XMLSchema-instance");
    xmlTextWriterWriteAttribute(uvoptx_writer, (xmlChar*)"xsi:noNamespaceSchemaLocation", (xmlChar*)"project_optx.xsd");
    xmlTextWriterWriteRaw(uvoptx_writer, (xmlChar*)"\n\n");

    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"SchemaVersion", (xmlChar*)"1.0");
    xmlTextWriterWriteRaw(uvoptx_writer, (xmlChar*)"\n");

    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Header", (xmlChar*)"### uVision Project, (C) Keil Software");
    xmlTextWriterWriteRaw(uvoptx_writer, (xmlChar*)"\n");

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"Extensions");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"cExt", (xmlChar*)"*.c");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"aExt", (xmlChar*)"*.s*; *.src; *.a*");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"oExt", (xmlChar*)"*.obj; *.o");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"lExt", (xmlChar*)"*.lib");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tExt", (xmlChar*)"*.txt; *.h; *.inc");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"pExt", (xmlChar*)"*.plm");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"CppX", (xmlChar*)"*.cpp");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"nMigrate", (xmlChar*)"0");
    xmlTextWriterEndElement(uvoptx_writer); // Extensions
    xmlTextWriterWriteRaw(uvoptx_writer, (xmlChar*)"\n");

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"DaveTm");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"dwLowDateTime", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"dwHighDateTime", (xmlChar*)"0");
    xmlTextWriterEndElement(uvoptx_writer); // DaveTm
    xmlTextWriterWriteRaw(uvoptx_writer, (xmlChar*)"\n");

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"Target");

    strcpy(xml_filename, from_codemodel.project_name);
    strcat(xml_filename, "-");
    strcat(xml_filename, from_cache.board);
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"TargetName", (xmlChar*)xml_filename);
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"ToolsetNumber", (xmlChar*)"0x4");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"ToolsetName", (xmlChar*)"ARM-ADS");
    
    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"TargetOption");

    sprintf(str, "%u", cpu_clock_hz);
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"CLKADS", (xmlChar*)str);

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"OPTTT");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"gFlags", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"BeepAtEnd", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"RunSim", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"RunTarget", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"RunAbUc", (xmlChar*)"0");
    xmlTextWriterEndElement(uvoptx_writer); // OPTTT

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"OPTHX");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"HexSelection", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"FlashByte", (xmlChar*)"65535");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"HexRangeLowAddress", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"HexRangeHighAddress", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"HexOffset", (xmlChar*)"0");
    xmlTextWriterEndElement(uvoptx_writer); // OPTHX
    
    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"OPTLEX");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"PageWidth", (xmlChar*)"79");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"PageLength", (xmlChar*)"66");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"TabStop", (xmlChar*)"8");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"ListingPath", (xmlChar*)".\\");
    xmlTextWriterEndElement(uvoptx_writer); // OPTLEX

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"ListingPage");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"CreateCListing", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"CreateAListing", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"CreateLListing", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"CreateIListing", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"AsmCond", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"AsmSymb", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"AsmXref", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"CCond", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"CCode", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"CListInc", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"CSymb", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"LinkerCodeListing", (xmlChar*)"0");
    xmlTextWriterEndElement(uvoptx_writer); // ListingPage

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"OPTXL");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"LMap", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"LComments", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"LGenerateSymbols", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"LLibSym", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"LLines", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"LLocSym", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"LPubSym", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"LXref", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"LExpSel", (xmlChar*)"0");
    xmlTextWriterEndElement(uvoptx_writer); // OPTXL

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"OPTFL");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tvExp", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tvExpOptDlg", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"IsCurrentTarget", (xmlChar*)"1");
    xmlTextWriterEndElement(uvoptx_writer); // OPTFL

    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"CpuCode", (xmlChar*)"0");

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"DebugOpt");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"uSim", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"uTrg", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"sLdApp", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"sGomain", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"sRbreak", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"sRwatch", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"sRmem", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"sRfunc", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"sRbox", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tLdApp", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tGomain", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tRbreak", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tRwatch", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tRmem", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tRfunc", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tRbox", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tRtrace", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"sRSysVw", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tRSysVw", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"sRunDeb", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"sLrtime", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"bEvRecOn", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"bSchkAxf", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"bTchkAxf", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"nTsel", (xmlChar*)"6");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"sDll", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"sDllPa", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"sDlgDll", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"sDlgPa", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"sIfile", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tDll", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tDllPa", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tDlgDll", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tDlgPa", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"tIfile", (xmlChar*)"");

    /* ? shouldnt pMon come from pack file ? */
    const char *pMon;
    if (strncmp(Mcu, "STM32", 5) == 0)
        pMon = "STLink\\ST-LINKIII-KEIL_SWO.dll";
    else {
        pMon = "";
        printf("need pMon for %s\r\n", Mcu);
    }
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"pMon", (xmlChar*)pMon);
    xmlTextWriterEndElement(uvoptx_writer); // DebugOpt


    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"TargetDriverDllRegistry");

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"SetRegEntry");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Number", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Key", (xmlChar*)"UL2CM3");

    sprintf(str, "UL2CM3(-S0 -C0 -P0 )  -FN1 -FC1000 -FD%08x -FF0%s -FL%06x -FS%08x -FP0($$Device:STM32L073RZTx$%s)", from_linker_script.ram_base, from_pack.ff0, from_linker_script.flash_length, from_linker_script.flash_base, from_pack.flm_path);
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Name", (xmlChar*)str);

    xmlTextWriterEndElement(uvoptx_writer); // SetRegEntry

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"SetRegEntry");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Number", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Key", (xmlChar*)"ST-LINKIII-KEIL_SWO"); /* TODO: other vendors besides ST */
    ret = sprintf(str, "-U066EFF555054877567072516 -O10446 -SF4000 -C0 -A0 -I0 -HNlocalhost -HP7184 -P2 -N00(\"ARM CoreSight SW-DP\") -D00(0BC11477) -L00(0) -TO18 -TC10000000 -TP21 -TDS8007 -TDT0 -TDC1F -TIEFFFFFFFF -TIP8 -FO7 -FD%08x -FC1000 -FN1 -FF0%s -FS%08x -FL%06x -FP0($$Device:STM32L073RZTx$%s)", from_linker_script.ram_base, from_pack.ff0, from_linker_script.flash_base, from_linker_script.flash_length, from_pack.flm_path);
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Name", (xmlChar*)str);
    xmlTextWriterEndElement(uvoptx_writer); // SetRegEntry

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"SetRegEntry");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Number", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Key", (xmlChar*)"ARMRTXEVENTFLAGS");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Name", (xmlChar*)"-L70 -Z18 -C0 -M0 -T1");
    xmlTextWriterEndElement(uvoptx_writer); // SetRegEntry

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"SetRegEntry");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Number", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Key", (xmlChar*)"DLGTARM");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Name", (xmlChar*)"(1010=-1,-1,-1,-1,0)(1007=-1,-1,-1,-1,0)(1008=-1,-1,-1,-1,0)(1009=-1,-1,-1,-1,0)");
    xmlTextWriterEndElement(uvoptx_writer); // SetRegEntry

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"SetRegEntry");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Number", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Key", (xmlChar*)"ARMDBGFLAGS");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Name", (xmlChar*)"");
    xmlTextWriterEndElement(uvoptx_writer); // SetRegEntry

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"SetRegEntry");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Number", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Key", (xmlChar*)"DLGUARM");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Name", (xmlChar*)"(105=-1,-1,-1,-1,0)");
    xmlTextWriterEndElement(uvoptx_writer); // SetRegEntry

    xmlTextWriterEndElement(uvoptx_writer); // TargetDriverDllRegistry

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"Breakpoint");
    xmlTextWriterEndElement(uvoptx_writer); // Breakpoint

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"Tracepoint");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"THDelay", (xmlChar*)"0");
    xmlTextWriterEndElement(uvoptx_writer); // Tracepoint

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"DebugFlag");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"trace", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"periodic", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"aLwin", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"aCover", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"aSer1", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"aSer2", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"aPa", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"viewmode", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"vrSel", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"aSym", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"aTbox", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"AscS1", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"AscS2", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"AscS3", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"aSer3", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"eProf", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"aLa", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"aPa1", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"AscS4", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"aSer4", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"StkLoc", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"TrcWin", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"newCpu", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"uProt", (xmlChar*)"0");
    xmlTextWriterEndElement(uvoptx_writer); // DebugFlag

    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"LintExecutable", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"LintConfigFile", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"bLintAuto", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"bAutoGenD", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"LntExFlags", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"pMisraName", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"pszMrule", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"pSingCmds", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"pMultCmds", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"pMisraNamep", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"pszMrulep", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"pSingCmdsp", (xmlChar*)"");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"pMultCmdsp", (xmlChar*)"");

    xmlTextWriterStartElement(uvoptx_writer, (xmlChar*)"DebugDescription");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Enable", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"EnableFlashSeq", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"EnableLog", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"Protocol", (xmlChar*)"2");
    xmlTextWriterWriteElement(uvoptx_writer, (xmlChar*)"DbgClock", (xmlChar*)"10000000");
    xmlTextWriterEndElement(uvoptx_writer); // DebugDescription

    xmlTextWriterEndElement(uvoptx_writer); // TargetOption

    xmlTextWriterEndElement(uvoptx_writer); // Target
    xmlTextWriterWriteRaw(uvoptx_writer, (xmlChar*)"\n");

    GroupNumber = 1;
    FileNumber = 1;
    ret = parse_codemodel(full_path_codemodel, parse_target_sources_uvoptx);
    if (ret < 0) {
        printf("%d = parse_codemodel()\r\n", ret);
    }

    xmlTextWriterEndElement(uvoptx_writer); // ProjectOpt

    xmlFreeTextWriter(uvoptx_writer);
    uvoptx_writer = NULL;

    return ret;
}

typedef union {
    struct {
        uint8_t r      : 1; // 0
        uint8_t w      : 1; // 1
        uint8_t x      : 1; // 2
        uint8_t unused : 5; // 3,4,5,6,7
    } bits;
    uint8_t octet;
} perms_t;

void get_perms(const char *start)
{
    unsigned origin, length;
    perms_t perms;
    perms.octet = 0;
    for (const char *ptr = start; *ptr != ')'; ptr++) {
        if (*ptr == 'r')
            perms.bits.r = 1;
        else if (*ptr == 'w')
            perms.bits.w = 1;
        else if (*ptr == 'x')
            perms.bits.x = 1;
    }

    const char *origin_at = strstr(start, "ORIGIN");
    do {
        origin_at++;
    } while (*origin_at != '=');
    origin_at++;
    sscanf(origin_at, "%x", &origin);

    const char *length_at = strstr(start, "LENGTH");
    do {
        length_at++;
    } while (*length_at != '=');
    length_at++;
    char multChar;
    unsigned len;
    sscanf(length_at, "%u%c", &len, &multChar);
    unsigned mult = 0;
    switch (multChar) {
        case 'K': mult = 1024; break;   // kilobyte = 2^10 bytes
        default: printf("unknown mult '%c'\r\n", multChar); break;
    }
    length = len * mult;

    if (perms.bits.r && perms.bits.x && !perms.bits.w) {
        from_linker_script.flash_base = origin;
        from_linker_script.flash_length = length;
    } if (perms.bits.r && perms.bits.w) {
        from_linker_script.ram_base = origin;
        from_linker_script.ram_length = length;
    }

}

int parse_linker_script(const char *file_name)
{
    char *buffer = NULL;
    int ret;
    struct stat st;
    FILE *fp;

    from_linker_script.vector_symbol[0] = 0;

    ret = stat(file_name, &st);
    if (ret < 0) {
        perror(file_name);
        return ret;
    }

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        perror(file_name);
        return -1;
    }
    buffer = malloc(st.st_size+1);
#ifdef __WIN32__
    fread_(buffer, st.st_size, fp);
#else
    fread(buffer, st.st_size, 1, fp);
#endif
    buffer[st.st_size] = 0;

    /* bison parse would be preferred, but trying without */
    char *memStart, *memEnd;
    char *memory = strstr(buffer, "MEMORY");
    for (memStart = memory; *memStart != '{'; memStart++)
        ;
    for (memEnd = memStart; *memEnd != '}'; memEnd++)
        ;
    char *ps;
    for (ps = memStart; *ps != '('; ps++)
        ;

    get_perms(ps+1);


    for (ps++; *ps != '('; ps++)
        ;

    get_perms(ps+1);

    ret = -1;
    /* get first FLASH section for isr vector symbol name */
    const char *SECTIONS = strstr(buffer, "SECTIONS");
    if (!SECTIONS) {
        printf("no SECTIONS in linker script\r\n");
        goto linker_script_done;
    }
    const char *FLASH = strstr(SECTIONS, "FLASH");
    if (!FLASH) {
        printf("no FLASH section in linker script\r\n");
        goto linker_script_done;
    }
    const char *flash_section_start = NULL;
    for (const char *ptr = FLASH; ptr > SECTIONS; ptr--) {
        if (*ptr == '{') {
            flash_section_start = ptr;
            break;
        }
    }
    if (!flash_section_start ) {
        printf("no '{' flash section start in linker script\r\n");
        goto linker_script_done;
    }

    const char *KEEP = strstr(flash_section_start, "KEEP(*");
    if (!KEEP) {
        printf("no KEEP in flash section of linker script\r\n");
        goto linker_script_done;
    }
    const char *symbol_name_start = NULL;
    for (const char *ptr = KEEP; ptr < FLASH; ptr++) {
        if (*ptr == '(')
            symbol_name_start = ptr;
        else if (*ptr == ')')
            break;
    }
    if (!symbol_name_start) {
        printf("no KEEP(symbol) in linker script\r\n");
        goto linker_script_done;
    }

    char *out = from_linker_script.vector_symbol;
    for (const char *ptr = symbol_name_start+1; ptr < FLASH; ptr++) {
        if (*ptr == ')')
            break;
        else
            *out++ = *ptr;
    }
    *out = 0;

    ret = 0;
linker_script_done:
    fclose(fp);
    return ret;
}

typedef enum {
    PACK_STATE_NONE,
    PACK_STATE_PACKAGE,
    PACK_STATE_PACKAGE_VENDOR,
    PACK_STATE_PACKAGE_URL,
    PACK_STATE_PACKAGE_NAME,
    PACK_STATE_RELEASES,
    PACK_STATE_DEVICES,
    PACK_STATE_DEVICES_FAMILY,
    PACK_STATE_DEVICES_SUB_FAMILY,
    PACK_STATE_DEVICE,
} pack_state_e;

#define N_DEPTHS        5
int pack_parse(xmlTextReaderPtr reader, const char *mcu)
{
    int ret, n;
    uint32_t rom_start, rom_size;
    uint32_t ram_start, ram_size;
    char rom_algorithm[192];
    char svd[128];
    char header[192];
    pack_state_e pack_state[N_DEPTHS];

    for (n = 0; n < N_DEPTHS; n++) {
        pack_state[n] = PACK_STATE_NONE;
    }
    svd[0] = 0;
    header[0] = 0;

    from_pack.packVersion[0] = 0;
    from_pack.packVendor[0] = 0;
    from_pack.packName[0] = 0;
    from_pack.packUrl[0] = 0;
    from_pack.processorCore[0] = 0;
    from_pack.svd[0] = 0;
    from_pack.header[0] = 0;
    from_pack.ff0[0] = 0;
    from_pack.rom_algorithm[0] = 0;
    from_pack.vendor[0] = 0;

    while ((ret = xmlTextReaderRead(reader)) == 1) {
        int depth;
        const char *name;
        int nodeType = xmlTextReaderNodeType(reader);
        if (nodeType == XML_NODE_TYPE_WHITE_SPACE)
            continue;   // white space

        name = (char*)xmlTextReaderConstName(reader);
        depth = xmlTextReaderDepth(reader);
        if (nodeType == XML_NODE_TYPE_START_ELEMENT) {
            if (depth == 0) {
                if (strcmp("package", name) == 0) {
                    pack_state[depth] = PACK_STATE_PACKAGE;
                }
            } else if (depth == 1 && pack_state[0] == PACK_STATE_PACKAGE) {
                if (strcmp("releases", name) == 0) {
                    pack_state[depth] = PACK_STATE_RELEASES;
                } else if (strcmp("devices", name) == 0) {
                    pack_state[depth] = PACK_STATE_DEVICES;
                } else if (strcmp("vendor", name) == 0) {
                    pack_state[depth] = PACK_STATE_PACKAGE_VENDOR;
                } else if (strcmp("url", name) == 0) {
                    pack_state[depth] = PACK_STATE_PACKAGE_URL;
                } else if (strcmp("name", name) == 0) {
                    pack_state[depth] = PACK_STATE_PACKAGE_NAME;
                }
            } else if (depth == 2) {
                if (pack_state[1] == PACK_STATE_RELEASES) {
                    if (from_pack.packVersion[0] == 0 && strcmp("release", name) == 0) {
                        char *attrib = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"version");
                        strcpy(from_pack.packVersion, attrib);
                        free(attrib);
                    }
                } else if (pack_state[1] == PACK_STATE_DEVICES) {
                    if (strcmp("family", name) == 0) {
                        pack_state[depth] = PACK_STATE_DEVICES_FAMILY;
                        char *va = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"Dvendor");
                        n = 0;
                        for (char *ptr = va; *ptr != 0 && *ptr != ':'; ptr++)
                            from_pack.vendor[n++] = *ptr;
                        free(va);
                    }
                }
            } else if (depth == 3) {
                if (pack_state[2] == PACK_STATE_DEVICES_FAMILY) {
                    if (strcmp("processor", name) == 0) {
                        char *a = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"Dcore");
                        strcpy(from_pack.processorCore, a);
                        free(a);
                    } else if (strcmp("subFamily", name) == 0) {
                        pack_state[3] = PACK_STATE_DEVICES_SUB_FAMILY;
                    }
                }
            } else if (depth == 4) {
                if (pack_state[3] == PACK_STATE_DEVICES_SUB_FAMILY) {
                    if (strcmp("device", name) == 0) {
                        pack_state[depth] = PACK_STATE_DEVICE;
                    } else if (strcmp("debug", name) == 0) {
                        char *svdAttr = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"svd");
                        strcpy(svd, svdAttr);
                        free(svdAttr);
                    } else if (strcmp("compile", name) == 0) {
                        char *headerAttr = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"header");
                        strcpy(header, headerAttr);
                        free(headerAttr);
                    }
                }
            } else if (depth == 5) {
                if (pack_state[4] == PACK_STATE_DEVICE) {
                    if (strcmp("memory", name) == 0) {
                        char *id_attrib = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"id");
                        char *start_attrib = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"start");
                        char *size_attrib = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"size");
                        if (strcmp("IROM1", id_attrib) == 0) {
                            sscanf(start_attrib, "%x", &rom_start);
                            sscanf(size_attrib , "%x", &rom_size);
                        } else if (strcmp("IRAM1", id_attrib) == 0) {
                            sscanf(start_attrib, "%x", &ram_start);
                            sscanf(size_attrib , "%x", &ram_size);
                        }
                        free(id_attrib);
                        free(start_attrib);
                        free(size_attrib);
                    } else if (strcmp("algorithm", name) == 0) {
                        uint32_t start, size;
                        char *start_attrib = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"start");
                        char *size_attrib = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"size");
                        sscanf(start_attrib, "%x", &start);
                        sscanf(size_attrib , "%x", &size);
                        if (start == rom_start && size == rom_size) {
                            char *name_attrib = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"name");
                            strcpy(rom_algorithm, name_attrib);
                            free(name_attrib);
                        }
                        free(start_attrib);
                        free(size_attrib);
                    } else if (strcmp("variant", name) == 0) {
                        char *attrib = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"Dvariant");
                        if (strcmp(attrib, mcu) == 0) {
                            strcpy(from_pack.rom_algorithm, rom_algorithm);
                            from_pack.rom_start = rom_start;
                            from_pack.rom_size = rom_size;
                            from_pack.ram_start = ram_start;
                            from_pack.ram_size = ram_size;
                            strcpy(from_pack.svd, svd);
                            strcpy(from_pack.header, header);
                        }
                        free(attrib);
                    }
                }
            }
        } /* ...XML_NODE_TYPE_START_ELEMENT */ else if (nodeType == XML_NODE_TYPE_TEXT) {
            const char *value = (char*)xmlTextReaderConstValue(reader);
            switch (pack_state[1]) {
                case PACK_STATE_PACKAGE_VENDOR: strcpy(from_pack.packVendor, value); break;
                case PACK_STATE_PACKAGE_URL: strcpy(from_pack.packUrl, value); break;
                case PACK_STATE_PACKAGE_NAME: strcpy(from_pack.packName, value); break;
                default: break;
            }
        } else if (nodeType == XML_NODE_TYPE_END_OF_ELEMENT) {
            pack_state[depth] = PACK_STATE_NONE;
        }
    } // ..while ((ret = xmlTextReaderRead(reader)) == 1)


    if (from_pack.rom_algorithm[0] != 0) {
        char *dest;
        char *bn = basename(from_pack.rom_algorithm);
        const char *extptr = strstr(bn, ".FLM");
        n = 0;
        for (char *ptr = bn; ptr != extptr; ptr++) {
            from_pack.ff0[n++] = *ptr;
        }
        from_pack.ff0[n] = 0;

        dest = from_pack.flm_path;
        for (char *ptr = from_pack.rom_algorithm; *ptr != 0; ptr++) {
#ifdef __WIN32__
            if (*ptr == '/')
                *dest++ = '\\';
            else
#endif /* __WIN32__ */
                *dest++ = *ptr;
        }
        *dest = 0;
    }

    return ret;
}

int find_pack(const char *mcu)
{
    int ret;
    DIR *dp;
    unsigned n;
    struct dirent *ep;
    char path[256];
    const char *mcuFamily = NULL;
    xmlTextReaderPtr reader = NULL;

    for (n = 0; mcu_families[n] != NULL; n++) {
        if (strstr(mcu, mcu_families[n]) != NULL) {
            mcuFamily = mcu_families[n];
            break;
        }
    }
    if (mcuFamily == NULL) {
        printf("no family for %s\r\n", mcu);
        return -1;
    }

    strcpy(path, keil_path);
    strcat(path, "/ARM/PACK/.Web/");

    dp = opendir(path);
    if (dp == NULL) {
        perror(path);
        return -1;
    }

    while ((ep = readdir (dp))) {
        if (strstr(ep->d_name, mcuFamily) != NULL) {
            char full_path[384];
            strcpy(full_path, path);
            strcat(full_path, ep->d_name);
            reader = xmlReaderForFile(full_path, NULL, 0);
            ret = pack_parse(reader, mcu);
            if (ret != -1)
                break;
            else {
                xmlFreeTextReader(reader);
            }
        }
    }

    if (reader == NULL) {
        printf("no pack xml for %s, family %s\r\n", mcu, mcuFamily);
        return -1;
    }

    xmlFreeTextReader(reader);

    return 0;
}

int uvprojx_start(bool force, char *Mcu)
{
    char str[256];
    char d_core[104];
    char xml_filename[96];
    int ret = -1;
    
    parse_linker_script(from_codemodel.linkerScript);

    strcpy(xml_filename, from_codemodel.project_name);
    strcat(xml_filename, ".uvprojx");
    strcat(xml_filename, "");
    uvprojx_writer = xmlNewTextWriterFilename(xml_filename, 0);
    if (uvprojx_writer == NULL) {
        printf("cannot create xml writer\r\n");
        return -1;
    }
    ret = xmlTextWriterStartDocument(uvprojx_writer, NULL, "UTF-8", "no");
    if (ret < 0) {
        printf("Error at xmlTextWriterStartDocument\n");
        return -1;
    }
    ret = xmlTextWriterSetIndent(uvprojx_writer, 1);
    if (ret < 0) {
        printf("Error at xmlTextWriterSetIndent\n");
        return -1;
    }
    xmlTextWriterSetIndentString(uvprojx_writer, (xmlChar*)"  ");

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"Project");
    xmlTextWriterWriteAttribute(uvprojx_writer, (xmlChar*)"xmlns:xsi", (xmlChar*)"http://www.w3.org/2001/XMLSchema-instance");
    xmlTextWriterWriteAttribute(uvprojx_writer, (xmlChar*)"xsi:noNamespaceSchemaLocation", (xmlChar*)"project_projx.xsd");
    xmlTextWriterWriteRaw(uvprojx_writer, (xmlChar*)"\n\n");

    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"SchemaVersion", (xmlChar*)"2.1");
    xmlTextWriterWriteRaw(uvprojx_writer, (xmlChar*)"\n");

    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Header", (xmlChar*)"### uVision Project, (C) Keil Software");
    xmlTextWriterWriteRaw(uvprojx_writer, (xmlChar*)"\n");

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"Targets");

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"Target");

    strcpy(xml_filename, from_codemodel.project_name);
    strcat(xml_filename, "-");
    strcat(xml_filename, from_cache.board);
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"TargetName", (xmlChar*)xml_filename);

    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"ToolsetNumber", (xmlChar*)"0x4");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"ToolsetName", (xmlChar*)"ARM-ADS");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"uAC6", (xmlChar*)"0");

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"TargetOption");
    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"TargetCommonOption");
    {
        char Board[64];
        translate_board_mcu(from_cache.board, Board, Mcu);

        cpu_clock_hz = 12000000;    // TODO where to get from?  crystal mounted on board
        find_pack(Mcu);

        xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Device", (xmlChar*)Mcu);

        xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Vendor", (xmlChar*)from_pack.vendor);

        strcpy(str, from_pack.packVendor);
        strcat(str, ".");
        strcat(str, from_pack.packName);
        strcat(str, ".");
        strcat(str, from_pack.packVersion);
        xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"PackID", (xmlChar*)str);
        xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"PackURL", (xmlChar*)from_pack.packUrl);

        strcpy(d_core, "\"");
        strcat(d_core, from_pack.processorCore);
        strcat(d_core, "\"");
        sprintf(str, "IRAM(0x%08x,0x%08x) IROM(0x%08x,0x%08x) CPUTYPE(%s) CLOCK(%u) ELITTLE", from_linker_script.ram_base, from_linker_script.ram_length, from_linker_script.flash_base, from_linker_script.flash_length, d_core, cpu_clock_hz);
        xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Cpu", (xmlChar*)str);
    }
    
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"FlashUtilSpec", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"StartupFile", (xmlChar*)"");
    {

        sprintf(str, "UL2CM3(-S0 -C0 -P0 -FD%08x -FC1000 -FN1 -FF0%s -FS%08x -FL%06x -FP0($$Device:STM32L073RZTx$%s))", from_linker_script.ram_base, from_pack.ff0, from_linker_script.flash_base, from_linker_script.flash_length, from_pack.flm_path);
 
        xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"FlashDriverDll", (xmlChar*)str);
    }
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"DeviceId", (xmlChar*)"");

    strcpy(str, "$$Device:");
    strcat(str, Mcu);
    strcat(str, "$");
    strcat(str, from_pack.header);
#ifdef __WIN32__
    for (char *ptr = str; *ptr != 0; ptr++)
        if (*ptr == '/')
            *ptr = '\\';
#endif /* __WIN32__ */
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RegisterFile", (xmlChar*)str);

    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"MemoryEnv", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Cmp", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Asm", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Linker", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"OHString", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"InfinionOptionDll", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"SLE66CMisc", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"SLE66AMisc", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"SLE66LinkerMisc", (xmlChar*)"");

    strcpy(str, "$$Device:");
    strcat(str, Mcu);
    strcat(str, "$");
    strcat(str, from_pack.svd);
#ifdef __WIN32__
    for (char *ptr = str; *ptr != 0; ptr++) {
        if (*ptr == '/')
            *ptr = '\\';
    }
#endif /* __WIN32__ */
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"SFDFile", (xmlChar*)str);

    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"bCustSvd", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UseEnv", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"BinPath", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"IncludePath", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"LibPath", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RegisterFilePath", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"DBRegisterFilePath", (xmlChar*)"");

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"TargetStatus");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Error", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"ExitCodeStop", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"ButtonStop", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"NotGenerated", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"InvalidFlash", (xmlChar*)"1");
    xmlTextWriterEndElement(uvprojx_writer); // TargetStatus

    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"OutputDirectory", (xmlChar*)".\\");

    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"OutputName", (xmlChar*)from_codemodel.artifactName);

    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"CreateExecutable", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"CreateLib", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"CreateHexFile", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"DebugInformation", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"BrowseInformation", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"ListingPath", (xmlChar*)".\\");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"HexFormatSelection", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Merge32K", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"CreateBatchFile", (xmlChar*)"0");

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"BeforeCompile");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RunUserProg1", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RunUserProg2", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UserProg1Name", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UserProg2Name", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UserProg1Dos16Mode", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UserProg2Dos16Mode", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"nStopU1X", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"nStopU2X", (xmlChar*)"0");
    xmlTextWriterEndElement(uvprojx_writer); // BeforeCompile

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"BeforeMake");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RunUserProg1", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RunUserProg2", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UserProg1Name", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UserProg2Name", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UserProg1Dos16Mode", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UserProg2Dos16Mode", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"nStopB1X", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"nStopB2X", (xmlChar*)"0");
    xmlTextWriterEndElement(uvprojx_writer); // BeforeMake

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"AfterMake");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RunUserProg1", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RunUserProg2", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UserProg1Name", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UserProg2Name", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UserProg1Dos16Mode", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UserProg2Dos16Mode", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"nStopA1X", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"nStopA2X", (xmlChar*)"0");
    xmlTextWriterEndElement(uvprojx_writer); // AfterMake

    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"SelectedForBatchBuild", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"SVCSIdString", (xmlChar*)"");

    xmlTextWriterEndElement(uvprojx_writer); // TargetCommonOption

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"CommonProperty");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UseCPPCompiler", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RVCTCodeConst", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RVCTZI", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RVCTOtherData", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"ModuleSelection", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"IncludeInBuild", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"AlwaysBuild", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"GenerateAssemblyFile", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"AssembleAssemblyFile", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"PublicsOnly", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"StopOnExitCode", (xmlChar*)"3");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"CustomArgument", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"IncludeLibraryModules", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"ComprImg", (xmlChar*)"1");
    xmlTextWriterEndElement(uvprojx_writer); // CommonProperty

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"DllOption");
    /* located at c:/Keil_v5/ARM/BIN/SarmCM3.dll */
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"SimDllName", (xmlChar*)"SARMCM3.DLL");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"SimDllArguments", (xmlChar*)" -REMAP-MPU ");
    /* located at c:/Keil_v5/ARM/BIN/DARMCM1.dll */
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"SimDlgDll", (xmlChar*)"DARMCM1.DLL");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"SimDlgDllArguments", (xmlChar*)"-pCM0+");   // TODO from cpu type
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"TargetDllName", (xmlChar*)"SARMCM3.DLL");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"TargetDllArguments", (xmlChar*)"-MPU ");
    /* located at c:/Keil_v5/ARM/BIN/TARMCM1.dll */
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"TargetDlgDll", (xmlChar*)"TARMCM1.DLL");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"TargetDlgDllArguments", (xmlChar*)"-pCM0+");   // TODO from cpu type
    xmlTextWriterEndElement(uvprojx_writer); // DllOption

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"DebugOption");
    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"OPTHX");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"HexSelection", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"HexRangeLowAddress", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"HexRangeHighAddress", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"HexOffset", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Oh166RecLen", (xmlChar*)"16");
    xmlTextWriterEndElement(uvprojx_writer); // OPTHX
    xmlTextWriterEndElement(uvprojx_writer); // DebugOption

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"Utilities");

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"Flash1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UseTargetDll", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UseExternalTool", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RunIndependent", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"UpdateFlashBeforeDebugging", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Capability", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"DriverSelection", (xmlChar*)"4096");
    xmlTextWriterEndElement(uvprojx_writer); // Flash1

    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"bUseTDR", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Flash2", (xmlChar*)"BIN\\UL2CM3.DLL");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Flash3", (xmlChar*)"\"\" ()");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Flash4", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"pFcarmOut", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"pFcarmGrp", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"pFcArmRoot", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"FcArmLst", (xmlChar*)"0");
    xmlTextWriterEndElement(uvprojx_writer); // Utilities

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"TargetArmAds");

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"ArmAdsMisc");

    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"GenerateListings", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"asHll", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"asAsm", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"asMacX", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"asSyms", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"asFals", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"asDbgD", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"asForm", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"ldLst", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"ldmm", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"ldXref", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"BigEnd", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"AdsALst", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"AdsACrf", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"AdsANop", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"AdsANot", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"AdsLLst", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"AdsLmap", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"AdsLcgr", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"AdsLsym", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"AdsLszi", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"AdsLtoi", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"AdsLsun", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"AdsLven", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"AdsLsxf", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RvctClst", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"GenPPlst", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"AdsCpuType", (xmlChar*)d_core);
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RvctDeviceName", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"mOS", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"uocRom", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"uocRam", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"hadIROM", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"hadIRAM", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"hadXRAM", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"uocXRam", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RvdsVP", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RvdsMve", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"hadIRAM2", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"hadIROM2", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"StupSel", (xmlChar*)"8");  // Startup memory region
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"useUlib", (xmlChar*)"1");  // use MicroLIB or not
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"EndSel", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"uLtcg", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"nSecure", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RoSelD", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RwSelD", (xmlChar*)"5");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"CodeSel", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"OptFeed", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"NoZi1", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"NoZi2", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"NoZi3", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"NoZi4", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"NoZi5", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Ro1Chk", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Ro2Chk", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Ro3Chk", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Ir1Chk", (xmlChar*)"1");  // is IROM1 default or not
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Ir2Chk", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Ra1Chk", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Ra2Chk", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Ra3Chk", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Im1Chk", (xmlChar*)"1");  // is IRAM1 default or not
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Im2Chk", (xmlChar*)"0");

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"OnChipMemories");

    for (unsigned ocm = 1; ocm < 7; ocm++) {
        sprintf(str, "Ocm%u", ocm);
        xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)str);
        xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Type", (xmlChar*)"0");
        xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"StartAddress", (xmlChar*)"0x0");
        xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Size", (xmlChar*)"0x0");
        xmlTextWriterEndElement(uvprojx_writer); // Ocmx
    }

    ocr_rvct[9].type = 0;    /* IRAM */
    ocr_rvct[9].start_address = from_linker_script.ram_base;
    ocr_rvct[9].size = from_linker_script.ram_length;
    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"IRAM");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Type", (xmlChar*)"0");
    sprintf(str, "0x%x", from_linker_script.ram_base);
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"StartAddress", (xmlChar*)str);
    sprintf(str, "0x%x", from_linker_script.ram_length);
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Size", (xmlChar*)str);
    xmlTextWriterEndElement(uvprojx_writer); // IRAM

    ocr_rvct[4].type = 1;   /* IROM */
    ocr_rvct[4].start_address = from_linker_script.flash_base;
    ocr_rvct[4].size = from_linker_script.flash_length;
    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"IROM");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Type", (xmlChar*)"1");
    sprintf(str, "0x%x", from_linker_script.flash_base);
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"StartAddress", (xmlChar*)str);
    sprintf(str, "0x%x", from_linker_script.flash_length);
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Size", (xmlChar*)str);
    xmlTextWriterEndElement(uvprojx_writer); // IROM


    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"XRAM");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Type", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"StartAddress", (xmlChar*)"0x0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Size", (xmlChar*)"0x0");
    xmlTextWriterEndElement(uvprojx_writer); // XRAM

    for (unsigned n = 1; n < 11; n++) {
        sprintf(str, "OCR_RVCT%u", n);
        xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)str);
        sprintf(str, "%u", ocr_rvct[n].type);
        xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Type", (xmlChar*)str);
        sprintf(str, "0x%x", ocr_rvct[n].start_address);
        xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"StartAddress", (xmlChar*)str);
        sprintf(str, "0x%x", ocr_rvct[n].size);
        xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Size", (xmlChar*)str);
        xmlTextWriterEndElement(uvprojx_writer); // OCR_RVCTn
    }


    xmlTextWriterEndElement(uvprojx_writer); // OnChipMemories
    
    xmlTextWriterEndElement(uvprojx_writer); // ArmAdsMisc

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"Cads");

    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"interw", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Optim", (xmlChar*)"4"); // TODO from fragment gcc -O optimization
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"oTime", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"SplitLS", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"OneElfS", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Strict", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"EnumInt", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"PlainCh", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Ropi", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Rwpi", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"wLevel", (xmlChar*)"2");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"uThumb", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"uSurpInc", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"uC99", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"uGnu", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"useXO", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"v6Lang", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"v6LangP", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"vShortEn", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"vShortWch", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"v6Lto", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"v6WtE", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"v6Rtti", (xmlChar*)"0");

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"VariousControls");

    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"MiscControls", (xmlChar*)"--C99");
    {
        struct node_s *my_list;
        char *str = NULL;
        unsigned total_length = 0;
        for (my_list = defines_list; my_list != NULL; my_list = my_list->next) {
            total_length += strlen(my_list->str) + 2;
        }
        str = malloc(total_length+1);
        str[0] = 0;
        for (my_list = defines_list; my_list != NULL; my_list = my_list->next) {
            if (str[0] == 0)
                strcpy(str, my_list->str);
            else {
                strcat(str, ", ");
                strcat(str, my_list->str);
            }
        }
        xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Define", (xmlChar*)str);

        free(str);
    }

    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Undefine", (xmlChar*)"");

    {
        char *str = NULL;
        struct node_s *my_list;
        unsigned total_length = 0;
        for (my_list = includes_list; my_list != NULL; my_list = my_list->next) {
            total_length += strlen(my_list->str) + 1;
        }
        str = malloc(total_length+1);
        str[0] = 0;
        for (my_list = includes_list; my_list != NULL; my_list = my_list->next) {
            if (str[0] == 0)
                strcpy(str, my_list->str);
            else {
                strcat(str, ";");
                strcat(str, my_list->str);
            }
        }
        xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"IncludePath", (xmlChar*)str);

        free(str);
    }

    xmlTextWriterEndElement(uvprojx_writer); // VariousControls
    xmlTextWriterEndElement(uvprojx_writer); // Cads

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"Aads");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"interw", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Ropi", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Rwpi", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"thumb", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"SplitLS", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"SwStkChk", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"NoWarn", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"uSurpInc", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"useXO", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"uClangAs", (xmlChar*)"0");

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"VariousControls");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"MiscControls", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Define", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Undefine", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"IncludePath", (xmlChar*)"");
    xmlTextWriterEndElement(uvprojx_writer); // VariousControls

    xmlTextWriterEndElement(uvprojx_writer); // Aads

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"LDads");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"umfTarg", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Ropi", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Rwpi", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"noStLib", (xmlChar*)"0");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"RepFail", (xmlChar*)"1");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"useFile", (xmlChar*)"0");
    sprintf(str, "0x%08x", from_linker_script.flash_base);
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"TextAddressRange", (xmlChar*)str);
    sprintf(str, "0x%08x", from_linker_script.ram_base);
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"DataAddressRange", (xmlChar*)str);
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"pXoBase", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"ScatterFile", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"IncludeLibs", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"IncludeLibsPath", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"Misc", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"LinkerInputFile", (xmlChar*)"");
    xmlTextWriterWriteElement(uvprojx_writer, (xmlChar*)"DisabledWarnings", (xmlChar*)"");
    xmlTextWriterEndElement(uvprojx_writer); // LDads

    xmlTextWriterEndElement(uvprojx_writer); // TargetArmAds

    xmlTextWriterEndElement(uvprojx_writer); // TargetOption

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"Groups");
    ret = parse_codemodel(full_path_codemodel, parse_target_sources_uvprojx);
    if (ret < 0) {
        printf("%d = parse_codemodel()\r\n", ret);
    }
    xmlTextWriterEndElement(uvprojx_writer); // Groups

    xmlTextWriterEndElement(uvprojx_writer); // Target
    
    xmlTextWriterEndElement(uvprojx_writer); // Targets


    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"RTE");

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"apis");
    xmlTextWriterEndElement(uvprojx_writer); // apis

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"components");
    xmlTextWriterEndElement(uvprojx_writer); // components

    xmlTextWriterStartElement(uvprojx_writer, (xmlChar*)"files");
    xmlTextWriterEndElement(uvprojx_writer); // files

    xmlTextWriterEndElement(uvprojx_writer); //RTE


    xmlTextWriterEndElement(uvprojx_writer); // Project

    ret = xmlTextWriterEndDocument(uvprojx_writer);
    if (ret < 0) {
        printf ("testXmlwriterFilename: Error at xmlTextWriterEndDocument\n");
        return ret;
    }
    xmlFreeTextWriter(uvprojx_writer);
    uvprojx_writer = NULL;


    return ret;
}

int parse_index(const char *jsonFileName, char *cache_filename, char *codemodel_filename)
{
    struct stat st;
    char *buffer = NULL;
    struct json_object *parsed_json;
    FILE *fp;
    int ret = -1;

    codemodel_filename[0] = 0;
    cache_filename[0] = 0;

    ret = stat(jsonFileName, &st);
    if (ret < 0) {
        perror(jsonFileName);
        return ret;
    }

    fp = fopen(jsonFileName, "r");
    if (fp == NULL) {
        perror(jsonFileName);
        return -1;
    }

    buffer = malloc(st.st_size);

#ifdef __WIN32__
    fread_(buffer, st.st_size, fp);
#else
    fread(buffer, st.st_size, 1, fp);
#endif

    parsed_json = json_tokener_parse(buffer);

    struct json_object *reply_jobj;
    if (!json_object_object_get_ex(parsed_json, "reply", &reply_jobj)) {
        printf("index no reply\r\n");
        goto index_done;
    }

    struct json_object *client_jobj;
    if (!json_object_object_get_ex(reply_jobj, "client-sw4stm32", &client_jobj)) {
        printf("index no client\r\n");
        goto index_done;
    }

    struct json_object *cache_jobj;
    if (json_object_object_get_ex(client_jobj, "cache-v2", &cache_jobj)) {
        struct json_object *jobj;
        if (json_object_object_get_ex(cache_jobj, "jsonFile", &jobj)) {
            strcpy(cache_filename, json_object_get_string(jobj));
        }
    } else {
        printf("index no cache-v2\r\n");
        goto index_done;
    }

    struct json_object *codemodel_jobj;
    if (json_object_object_get_ex(client_jobj, "codemodel-v2", &codemodel_jobj)) {
        struct json_object *jobj;
        if (json_object_object_get_ex(codemodel_jobj, "jsonFile", &jobj)) {
            strcpy(codemodel_filename, json_object_get_string(jobj));
        }
    } else {
        printf("index no codemodel-v2\r\n");
        goto index_done;
    }

    ret = 0;

index_done:
    fclose(fp);
    if (buffer)
        free(buffer);
    return ret;
}

void path_copy(char *dest, const char *src, size_t dest_size)
{
    int dest_idx = 0;
    int src_idx = 0;
    bool in_quotes = false;

    while (dest_idx < dest_size) {
        if (src[src_idx] == '"') {
            in_quotes ^= true;
            src_idx++;
        }

        if (in_quotes) {
            dest[dest_idx++] = src[src_idx++];
        } else {
            if (src[src_idx] == '\\') {
                ++src_idx; // advance past quoting char
                dest[dest_idx++] = src[src_idx++];
            } else if (src[src_idx] == ' ') {
                dest[dest_idx] = 0;
                return;
            } else
                dest[dest_idx++] = src[src_idx++];
        }
    }
}

void parse_executable_target(struct json_object *parsed_json)
{
    struct json_object *link_jobj;
    if (json_object_object_get_ex(parsed_json, "link", &link_jobj)) {
        struct json_object *commandFragments_jobj;
        if (json_object_object_get_ex(link_jobj, "commandFragments", &commandFragments_jobj)) {
            unsigned n, n_commandFragments = json_object_array_length(commandFragments_jobj);
            for (n = 0; n < n_commandFragments ; n++) {
                struct json_object *role_jobj;
                struct json_object *commandFragment_jobj = json_object_array_get_idx(commandFragments_jobj, n);
                if (json_object_object_get_ex(commandFragment_jobj, "role", &role_jobj)) {
                    struct json_object *fragment_jobj;
                    const char *fragment, *role = json_object_get_string(role_jobj);
                    json_object_object_get_ex(commandFragment_jobj, "fragment", &fragment_jobj);
                    fragment = json_object_get_string(fragment_jobj);
                    if (strcmp(role, "flags") == 0) {
                        unsigned len = 0;
                        const char *ptr = strstr(fragment, "-T");
                        if (ptr != NULL) {
                            /* -T is given absolute path (full path) */
                            path_copy(from_codemodel.linkerScript, ptr + 2, sizeof(from_codemodel.linkerScript));
                        }

                        /* save --specs= */
                        for (ptr = fragment; (ptr = strstr(ptr, "--specs")) != NULL; ptr += 7) {
                            struct node_s **node;
                            /* count length until next space */
                            for (len = 0; ptr[len] != ' '; len++)
                                ;

                            node = &from_codemodel.specs_list;
                            if (from_codemodel.specs_list) {
                                while ((*node)->next) {
                                    node = &(*node)->next;
                                }
                                node = &(*node)->next;
                            }
                            (*node) = malloc(sizeof(struct node_s));
                            (*node)->next = NULL;
                            (*node)->str = malloc(len+1);
                            strncpy((*node)->str, ptr, len);
                            (*node)->str[len] = 0;
                        }
                    } else if (strcmp(role, "libraries") == 0) {
                        /* TODO add extra link libraries ? */
                    }
                }
            }
        }
    } // ..if (json_object_object_get_ex(parsed_json, "link", &link_jobj))
}

int parse_target_defines_includes(const char *source_path, const char *jsonFileName)
{
    struct stat st;
    char *buffer = NULL;
    struct json_object *parsed_json;
    char full_path[256];
    int ret = -1;;

    strcpy(full_path, reply_directory);
    strcat(full_path, "/");
    strcat(full_path, jsonFileName);

    if (stat(full_path, &st) < 0) {
        strcat(full_path, " stat");
        perror(full_path);
        goto target_done;
    }

    FILE *fp = fopen(full_path, "r");
    if (fp == NULL) {
        perror(full_path);
        goto target_done;
    }

    buffer = malloc(st.st_size);
#ifdef __WIN32__
    ret = fread_(buffer, st.st_size, fp);
#else
    ret = fread(buffer, st.st_size, 1, fp);
#endif

    parsed_json = json_tokener_parse(buffer);
    if (parsed_json == NULL) {
        printf("cannot parse json in %s\r\n", full_path);
        ret = -1;
        goto target_done;
    }

    struct json_object *compileGroups_jobj;
    if (!json_object_object_get_ex(parsed_json, "compileGroups", &compileGroups_jobj)) {
        goto target_done;
    }

    struct json_object *name_jobj;
    if (!json_object_object_get_ex(parsed_json, "name", &name_jobj)) {
        printf("no name in %s\r\n", jsonFileName);
    }

    struct json_object *type_jobj;
    if (json_object_object_get_ex(parsed_json, "type", &type_jobj)) {
        const char *targetType = json_object_get_string(type_jobj);
        if (strcmp("EXECUTABLE", targetType) == 0) {
            strncpy(from_codemodel.artifactName, json_object_get_string(name_jobj), sizeof(from_codemodel.artifactName));
            parse_executable_target(parsed_json);
        }
    } else
        printf("no type in %s\r\n", jsonFileName);

    struct json_object *compileGroup_jobj = json_object_array_get_idx(compileGroups_jobj, 0);
    struct json_object *defines_jobj;
    if (json_object_object_get_ex(compileGroup_jobj, "defines", &defines_jobj)) {
        unsigned n, n_defines = json_object_array_length(defines_jobj);
        for (n = 0; n < n_defines; n++) {
            struct json_object *jobj;
            struct json_object *define_jobj = json_object_array_get_idx(defines_jobj, n);
            if (json_object_object_get_ex(define_jobj, "define", &jobj)) {
                const char *define = json_object_get_string(jobj);
                int define_len = strlen(define);
                if (!defines_list) {
                    defines_list = malloc(sizeof(struct node_s));
                    defines_list->next = NULL;
                    defines_list->str = malloc(strlen(define)+1);
                    strcpy(defines_list->str, define);
                } else {
                    struct node_s *my_list;
                    bool already = false;
                    for (my_list = defines_list; my_list->next != NULL; my_list = my_list->next) {
                        if (strlen(my_list->str) == define_len) {
                            if (strcmp(my_list->str, define) == 0) {
                                /* already have this define */
                                already = true;
                            }
                        }
                    }
                    if (!already) {
                        my_list->next= malloc(sizeof(struct node_s));
                        my_list->next->next = NULL;
                        my_list->next->str = malloc(strlen(define)+1);
                        strcpy(my_list->next->str, define);
                    }
                }
            } // ..if (json_object_object_get_ex(define_jobj, "define", &jobj))
        } // ..for (n = 0; n < n_defines; n++)
    } // ..if (json_object_object_get_ex(compileGroup_jobj, "defines", &defines_jobj))

    struct json_object *includes_jobj;
    if (json_object_object_get_ex(compileGroup_jobj, "includes", &includes_jobj)) {
        unsigned n, n_includes = json_object_array_length(includes_jobj);
        for (n = 0; n < n_includes; n++) {
            struct json_object *path_jobj;
            struct json_object *include_jobj = json_object_array_get_idx(includes_jobj, n);
            if (json_object_object_get_ex(include_jobj, "path", &path_jobj)) {
                const char *path = json_object_get_string(path_jobj);
                int path_len = strlen(path);
                unsigned n_checked = 0;
                if (!includes_list) {
                    includes_list = malloc(sizeof(struct node_s));
                    includes_list->next = NULL;
                    includes_list->str = malloc(strlen(path)+1);
                    strcpy(includes_list->str, path);
                } else {
                    struct node_s *my_list;
                    bool already = false;
                    int ret = INT_MIN;
                    for (my_list = includes_list; my_list->next != NULL; my_list = my_list->next) {
                        n_checked++;
                        if (strlen(my_list->str) == path_len) {
                            ret = strcmp(my_list->str, path);
                            if (ret == 0) {
                                /* already have this include */
                                already = true;
                                break;
                            }
                        }
                    }
                    if (!already) {
                        my_list->next= malloc(sizeof(struct node_s));
                        my_list->next->next = NULL;
                        my_list->next->str = malloc(strlen(path)+1);
                        strcpy(my_list->next->str, path);
                    }
                }
            }
        }
    }

target_done:
    free(buffer);
    fclose(fp);
    return ret;
}


int parse_cache(const char *jsonFileName)
{
    struct stat st;
    struct json_object *parsed_json;
    char *buffer = NULL;
    int ret;
    FILE *fp = fopen(jsonFileName, "r");
    if (fp == NULL) {
        perror(jsonFileName);
        return -1;
    }

    if (stat(jsonFileName, &st) < 0) {
        char str[256];
        strcpy(str, "stat ");
        strcat(str, jsonFileName);
        perror(str);
        goto cache_done;
    }
    buffer = malloc(st.st_size);

#ifdef __WIN32__
    ret = fread_(buffer, st.st_size, fp);
#else
    ret = fread(buffer, st.st_size, 1, fp);
#endif

    parsed_json = json_tokener_parse(buffer);

    struct json_object *entries_jobj;
    if (!json_object_object_get_ex(parsed_json, "entries", &entries_jobj)) {
        printf("cache no entries\r\n");
        goto cache_done;
    }

    unsigned n_entries = json_object_array_length(entries_jobj);
    for (unsigned n = 0; n < n_entries; n++) {
        struct json_object *name_jobj;
        struct json_object *entry_jobj = json_object_array_get_idx(entries_jobj, n);
        if (json_object_object_get_ex(entry_jobj, "name", &name_jobj)) {
            if (strcmp(json_object_get_string(name_jobj), "BOARD") == 0) {
                struct json_object *value_jobj;
                if (json_object_object_get_ex(entry_jobj, "value", &value_jobj)) {
                    strcpy(from_cache.board, json_object_get_string(value_jobj));
                }
            }
        }
    }

cache_done:
    fclose(fp);
    free(buffer);
    return ret;
}

int json_read()
{
    DIR *dp;
    int ret;
    char codemodel_filename[128];
    char cache_filename[128];
    char full_path[256];
    const char *index_prefix = "index";
    struct dirent *ep;

    dp = opendir(reply_directory);
    if (dp == NULL) {
        perror(reply_directory);
        return -1;
    }

    while ((ep = readdir (dp))) {
        if (strncmp(ep->d_name, index_prefix, strlen(index_prefix)) == 0) {
            strcpy(full_path, reply_directory);
            strcat(full_path, "/");
            strcat(full_path, ep->d_name);
            ret = parse_index(full_path, cache_filename, codemodel_filename);
            break;
        }
    }

    strcpy(full_path_codemodel, reply_directory);
    strcat(full_path_codemodel, "/");
    strcat(full_path_codemodel, codemodel_filename);
    ret = parse_codemodel(full_path_codemodel, parse_target_defines_includes);
    if (ret < 0) {
        return ret;
    }

    strcpy(full_path, reply_directory);
    strcat(full_path, "/");
    strcat(full_path, cache_filename);
    ret = parse_cache(full_path);
    if (ret < 0) {
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    char Mcu[64];
    int ret = -1, opt;
    bool force = false;

    keil_path = "c:/Keil_v5";

    while ((opt = getopt(argc, argv, "f")) != -1) {
        switch (opt) {
            case 'f':
                force = true;
                break;
        }
    }

    includes_list = NULL;
    defines_list = NULL;

    if (json_read() < 0) {
        goto main_end;
    }

    LIBXML_TEST_VERSION

    if ((ret = uvprojx_start(force, Mcu)) < 0) {
        goto main_end;
    }

    if ((ret = uvoptx_start(force, Mcu)) < 0) {
        goto main_end;
    }

    if (startup_asm_filename) {
        generate_startup(startup_asm_filename, 0x600, 0x200, from_linker_script.vector_symbol);

    }

main_end:
    return ret;
}

