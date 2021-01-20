/* Read cmake json (cmake file api), and output eclipse CDT XML */
#include <dirent.h>
#include <time.h>

#include "json_eclipse_cdt.h"
#include "board-mcu-hack.h"
//#define VERBOSE     1

xmlTextWriterPtr project_writer;
xmlTextWriterPtr cproject_writer;

from_codemodel_t from_codemodel;
from_cache_t from_cache;

const char * const reply_directory = ".cmake/api/v1/reply";

struct node_s *defines_list;
struct node_s *includes_list;
struct node_s *instance_list;
struct node_s *link_libs;   // library names EXECUTABLE links in precompiled (not built)
struct node_s *required_targets;    // library names EXECUTABLE target links in, built from source
static struct node_s *source_files;
static struct node_s *language_list;
static struct node_s *static_libraries;

bool cpp_input; // false=c-project, true=cpp-project
bool cpp_linker; // true=linking done by c++ compiler
char linker_script[128];
const char *build;
const char *Build;
struct node_s *c_flags;
struct node_s *cpp_flags;

void put_listOptionValue(xmlTextWriterPtr w, bool built_in, const char *vv)
{
    xmlTextWriterStartElement(w, (xmlChar*)"listOptionValue");
    xmlTextWriterWriteAttribute(w, (xmlChar*)"builtIn", built_in ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(w, value, (xmlChar*)vv);
    xmlTextWriterEndElement(w); // listOptionValue 
}

void put_additionalInput(xmlTextWriterPtr w, const char *kind, const char *paths)
{
    xmlTextWriterStartElement(w, (xmlChar*)"additionalInput");
    xmlTextWriterWriteAttribute(w, (xmlChar*)"kind", (xmlChar*)kind);
    xmlTextWriterWriteAttribute(w, (xmlChar*)"paths", (xmlChar*)paths);
    xmlTextWriterEndElement(w); // additionalInput 
}

void put_scannerConfiguration(const char *is_problemReportingEnabled)
{
    xmlTextWriterStartElement(cproject_writer, autodiscovery);
    xmlTextWriterWriteAttribute(cproject_writer, enabled, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"problemReportingEnabled", (xmlChar*)is_problemReportingEnabled);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"selectedProfileId", (xmlChar*)"");
    xmlTextWriterEndElement(cproject_writer); // autodiscovery

    for (struct node_s *my_list = instance_list; my_list != NULL; my_list = my_list->next) {
        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"scannerConfigBuildInfo");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"instanceId", (xmlChar*)my_list->str);

        xmlTextWriterStartElement(cproject_writer, autodiscovery);
        xmlTextWriterWriteAttribute(cproject_writer, enabled, (xmlChar*)"true");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"problemReportingEnabled", (xmlChar*)is_problemReportingEnabled);
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"selectedProfileId", (xmlChar*)"");
        xmlTextWriterEndElement(cproject_writer); // autodiscovery

        xmlTextWriterEndElement(cproject_writer); // scannerConfigBuildInfo 
    }
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

static struct node_s *save_to_list(const char *fragment, struct node_s **dest, const void *arg)
{
    static struct node_s *ret = NULL;
    char *copy;
    char *token;

    copy = malloc(strlen(fragment)+1);
    strcpy(copy, fragment);
    token = strtok(copy, " ");

    while (token != NULL) {
        bool ok_for_xml = true;
        int n, toklen = strlen(token);
        struct node_s **node = dest;
        for (n = 0; n < toklen; n++) {
            if (token[n] < ' ') {
                /* strtok() could be broken in windows */
                ok_for_xml = false;
                break;
            }
        }
        if (ok_for_xml) {
            unsigned cnt = 0;
            if (*node) {
                while ((*node)->next) {
                    if (strcmp(token, (*node)->str) == 0) {
                        goto next_token;
                    }
                    cnt++;
                    node = &(*node)->next;
                }
                node = &(*node)->next;
            }
            (*node) = malloc(sizeof(struct node_s));
            (*node)->next = NULL;
            (*node)->str = malloc(toklen+1);
            strcpy((*node)->str, token);
            (*node)->arg = arg;
            ret = *node;
        }

next_token:
        token = strtok(NULL, " ");
    }

    free(copy);
    return ret;
}

int preparse_targets(const char *source_path, const char *jsonFileName)
{
    FILE *fp;
    int ret = -1;;
    struct stat st;
    struct json_object *type_jobj, *parsed_json, *jobj;
    char *buffer = NULL;
    char full_path[256];
    const char *targetType;

    strcpy(full_path, reply_directory);
    strcat(full_path, "/");
    strcat(full_path, jsonFileName);

    fp = fopen(full_path, "r");
    if (fp == NULL) {
        perror(full_path);
        return -1;
    }

    if (stat(full_path, &st) < 0) {
        strcat(full_path, " stat");
        perror(full_path);
        goto preparse_target_done;
    }

    buffer = malloc(st.st_size * 2); // x2: json_tokern_parse is operating beyond buffer
#ifdef __WIN32__
    ret = fread_(buffer, st.st_size, fp);
#else
    ret = fread(buffer, st.st_size, 1, fp);
#endif

    /* ? trailing garbage ? */
    for (unsigned n = st.st_size; n > 0; n--) {
        if (buffer[n] == '}')
            break;
        else
            buffer[n] = 0;
    }

    parsed_json = json_tokener_parse(buffer);
    if (parsed_json == NULL) {
        printf("%s\n", buffer);
        printf("cannot parse json in %s (%d bytes) st_size:%ld\r\n", full_path, (int)strlen(buffer), st.st_size);
        ret = -1;
        goto preparse_target_done;
    }

    if (!json_object_object_get_ex(parsed_json, "type", &type_jobj)) {
        printf("no type in %s\r\n", full_path);
        goto preparse_target_done;
    }
    targetType = json_object_get_string(type_jobj);

    if (strcmp("STATIC_LIBRARY", targetType) == 0 && json_object_object_get_ex(parsed_json, "artifacts", &jobj)) {
        struct json_object *path_jobj;
        unsigned n_artifacts = json_object_array_length(jobj);
        for (unsigned n = 0; n < n_artifacts; n++) {
            struct json_object *artifact_jobj = json_object_array_get_idx(jobj, n);
            if (json_object_object_get_ex(artifact_jobj, "path", &path_jobj)) {
                bool already_have = false;
                const char *path = json_object_get_string(path_jobj);
                for (struct node_s *my_list = static_libraries; my_list != NULL; my_list = my_list->next) {
                    if (strcmp(my_list->str, path) == 0) {
                        already_have = true;
                        break;
                    }
                }
                if (!already_have)
                    save_to_list(path, &static_libraries, NULL);
            }
        }
    }

preparse_target_done:
    free(buffer);
    fclose(fp);
    return ret;
}

int parse_executable_target(const char *source_path, const char *jsonFileName)
{
    struct json_object *link_jobj, *parsed_json;
    struct stat st;
    char *buffer = NULL;
    char full_path[256];
    int ret = -1;;
    strcpy(full_path, reply_directory);
    strcat(full_path, "/");
    strcat(full_path, jsonFileName);

    FILE *fp = fopen(full_path, "r");
    if (fp == NULL) {
        perror(full_path);
        return -1;
    }
    if (stat(full_path, &st) < 0) {
        strcat(full_path, " stat");
        perror(full_path);
        goto exe_target_done;
    }

    buffer = malloc(st.st_size * 2); // x2: json_tokern_parse is operating beyond buffer
#ifdef __WIN32__
    ret = fread_(buffer, st.st_size, fp);
#else
    ret = fread(buffer, st.st_size, 1, fp);
#endif

    /* ? trailing garbage ? */
    for (unsigned n = st.st_size; n > 0; n--) {
        if (buffer[n] == '}')
            break;
        else
            buffer[n] = 0;
    }

    parsed_json = json_tokener_parse(buffer);
    if (parsed_json == NULL) {
        printf("%s\n", buffer);
        printf("cannot parse json in %s (%d bytes) st_size:%ld\r\n", full_path, (int)strlen(buffer), st.st_size);
        ret = -1;
        goto exe_target_done;
    }

    struct json_object *type_jobj;
    struct json_object *name_jobj;
    if (!json_object_object_get_ex(parsed_json, "type", &type_jobj)) {
        printf("no type in %s\r\n", full_path);
        goto exe_target_done;
    }
    json_object_object_get_ex(parsed_json, "name", &name_jobj);
#ifdef VERBOSE
    const char *targetType = json_object_get_string(type_jobj);
#endif

    if (json_object_object_get_ex(parsed_json, "link", &link_jobj)) {
        const char *langPtr = NULL;
        struct json_object *commandFragments_jobj, *jobj;
        if (json_object_object_get_ex(link_jobj, "language", &jobj)) {
            const char *language = json_object_get_string(jobj);
            for (struct node_s *my_list = language_list; my_list != NULL; my_list = my_list->next) {
                if (strcmp(language, my_list->str) == 0) {
                    langPtr = my_list->str;
                    break;
                }
            }
            if (!langPtr) {
#ifdef VERBOSE
                printf("line %d new language \e[7m%s\e[0m\n", __LINE__, language);
#endif
                struct node_s *node = save_to_list(language, &language_list, NULL);
                langPtr = node->str;
            }
        } else {
            printf("\e[31mline %d: link %s no language\e[0m\n", __LINE__, json_object_get_string(name_jobj));
            ret = -1;
            goto exe_target_done;
        }
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
                    if (strcmp(role, "libraries") == 0) {
                        bool static_library = false;
                        if (linker_script[0] == 0 && strstr(fragment, ".cmd") != NULL) {
                            /* texas instruments linker script */
                            strcpy(linker_script, fragment);
                        }
                        for (struct node_s *my_list = static_libraries; my_list != NULL; my_list = my_list->next) {
                            char tmp[256];
                            const char *fragment_ptr = fragment;
                            if (strchr(my_list->str, '/') != NULL && strchr(fragment, '\\') != NULL) {
                                /* path conversion, windows fuck-nuttery */
                                unsigned len = strlen(fragment);
                                strcpy(tmp, fragment);
                                for (unsigned n = 0; n < len; n++) {
                                    if (tmp[n] == '\\')
                                        tmp[n] = '/';
                                }
                                fragment_ptr = tmp;
                            }
                            if (strcmp(my_list->str, fragment_ptr) == 0)
                                static_library = true;  /* built by this project */
                        }
                        if (!static_library) {
                            /* static_library: only save libs which arent built by project */
                            bool already_have = false;
                            for (struct node_s *my_list = link_libs; my_list != NULL; my_list = my_list->next) {
                                if (strcmp(my_list->str, fragment) == 0) {
                                    already_have = true;
                                    break;
                                }
                            }
                            if (!already_have) {
                                save_to_list(fragment, &link_libs, langPtr);
#ifdef VERBOSE
                                printf("%d \e[7m%s\e[0m exe link library %s\n", __LINE__, langPtr, fragment);
#endif
                            }
                        } else {
                            /* static_library true */
#ifdef VERBOSE
                            printf("\e[34m%d %s \e[7m%s\e[0m \e[34mexe link %s\e[0m\n", __LINE__, targetType, langPtr, fragment);
#endif
                            save_to_list(fragment, &required_targets, langPtr);
                        }
                    }
                }
            }
        }
    } // ..if (json_object_object_get_ex(parsed_json, "link", &link_jobj))

    struct json_object *compileGroups_jobj;
    if (json_object_object_get_ex(parsed_json, "compileGroups", &compileGroups_jobj)) {
        for (unsigned idx = 0; idx < json_object_array_length(compileGroups_jobj); idx++) {
            const char *langPtr = NULL;
            struct json_object *jobj, *compileGroup_jobj = json_object_array_get_idx(compileGroups_jobj, idx);
            if (json_object_object_get_ex(compileGroup_jobj, "language", &jobj)) {
                const char *language = json_object_get_string(jobj);
                for (struct node_s *my_list = language_list; my_list != NULL; my_list = my_list->next) {
                    if (strcmp(language, my_list->str) == 0) {
                        langPtr = my_list->str;
                        break;
                    }
                }
                if (langPtr == NULL) {
#ifdef VERBOSE
                    printf("line %d %s new language \e[7m%s\e[0m\n", __LINE__, targetType, language);
#endif
                    save_to_list(language, &language_list, NULL);
                }
            }
        }
    } // ..compileGroups

exe_target_done:
    free(buffer);
    fclose(fp);
    return ret;
}

void header_in_define(const char **arg)
{
    bool quoted;
    const char *in;
    unsigned in_idx, out_idx, arg_len;;
    char *str, *a = strchr(*arg, '\"');
    if (a == NULL)
        return;
    if (strstr(a, ".h\"") == NULL)
        return;
    arg_len = strlen(*arg);
    str = malloc(arg_len + 3); // TODO: free later?

    quoted = false;
    in = *arg;
    for (in_idx = 0, out_idx = 0; in_idx < arg_len; ) {
        bool quote = in[in_idx] == '\"';
        if (quoted) {
            if (quote) {
                str[out_idx++] = '>';
                quoted = false;
            }
            str[out_idx++] = in[in_idx++];
        } else {
            str[out_idx++] = in[in_idx++];
            if (quote) {
                str[out_idx++] = '<';
                quoted = true;
            }
        }
    }
    str[out_idx] = 0;
    *arg = str;
}

int parse_target_file_to_linked_resources(const char *source_path, const char *jsonFileName)
{
    struct json_object *link_jobj/*, *nod_jobj*/, *parsed_json, *jobj;
    struct stat st;
    char *buffer = NULL;
    char full_path[256];
    int ret = -1;;
    strcpy(full_path, reply_directory);
    strcat(full_path, "/");
    strcat(full_path, jsonFileName);

    //printf("%d PTFTLR %s %s\n", __LINE__, source_path, jsonFileName);
    FILE *fp = fopen(full_path, "r");
    if (fp == NULL) {
        perror(full_path);
        return -1;
    }
    if (stat(full_path, &st) < 0) {
        strcat(full_path, " stat");
        perror(full_path);
        goto target_done;
    }

    buffer = malloc(st.st_size * 2); // x2: json_tokern_parse is operating beyond buffer
#ifdef __WIN32__
    ret = fread_(buffer, st.st_size, fp);
#else
    ret = fread(buffer, st.st_size, 1, fp);
#endif

    /* ? trailing garbage ? */
    for (unsigned n = st.st_size; n > 0; n--) {
        if (buffer[n] == '}')
            break;
        else
            buffer[n] = 0;
    }

    parsed_json = json_tokener_parse(buffer);
    if (parsed_json == NULL) {
        printf("%s\n", buffer);
        printf("cannot parse json in %s (%d bytes) st_size:%ld\r\n", full_path, (int)strlen(buffer), st.st_size);
        ret = -1;
        goto target_done;
    }

    struct json_object *type_jobj;
    struct json_object *name_jobj;
    if (!json_object_object_get_ex(parsed_json, "type", &type_jobj)) {
        printf("no type in %s\r\n", full_path);
        goto target_done;
    }
    json_object_object_get_ex(parsed_json, "name", &name_jobj);
    const char *targetType = json_object_get_string(type_jobj);

#ifdef VERBOSE
    printf("line %d %s %s ", __LINE__, json_object_get_string(name_jobj), targetType);
#endif

    const char *artifact_path = NULL;
    /* UTILITY doesnt have "artifacts" */

    if (strcmp("OBJECT_LIBRARY", targetType) != 0 && json_object_object_get_ex(parsed_json, "artifacts", &jobj)) {
        unsigned n_artifacts = json_object_array_length(jobj);
        for (unsigned n = 0; n < n_artifacts; n++) {
            struct json_object *path_jobj;
            struct json_object *artifact_jobj = json_object_array_get_idx(jobj, n);
            if (n > 1)
                printf("extra artifact %s\n", json_object_get_string(artifact_jobj));
            if (json_object_object_get_ex(artifact_jobj, "path", &path_jobj))
                 artifact_path = json_object_get_string(path_jobj);
        }
    }

    if (strcmp("UTILITY", targetType) == 0) {
        /* TODO get post-build steps */
        /* add_custom_target( COMMAND <foobar> ) where to i get <foobar> ? */
    }

    /* target types to get sources from */
    if (strcmp("OBJECT_LIBRARY", targetType) != 0 &&
        strcmp("EXECUTABLE", targetType) != 0 &&
        strcmp("STATIC_LIBRARY", targetType) != 0)
    {
#ifdef VERBOSE
        printf("\e[33mignore\e[0m");
#endif
        goto target_done;
    }

    /* only build targets required by EXECUTABLE target */
    if (artifact_path && strcmp("STATIC_LIBRARY", targetType) == 0) {
        struct node_s *my_list;
        bool needed = false;
        for (my_list = required_targets; my_list != NULL; my_list = my_list->next) {
            char tmp[256], *ptr;
            ptr = my_list->str;
            if (strchr(my_list->str, '\\') != NULL && strchr(artifact_path, '/') != NULL) {
                unsigned len = strlen(my_list->str);
                strcpy(tmp, my_list->str);
                for (unsigned n = 0; n < len; n++)
                    if (tmp[n] == '\\')
                        tmp[n] = '/';
                ptr = tmp;
            }
            if (strcmp(artifact_path, ptr) == 0) {
                needed = true;
                break;
            }
        }
        if (!needed)
            goto target_done;
    }

    char target_name[256];
    struct json_object *sources_jobj;
    xmlTextWriterStartElement(project_writer, (xmlChar*)"link");
    if (json_object_object_get_ex(parsed_json, "name", &name_jobj)) {
        sprintf(target_name, "[%s]", json_object_get_string(name_jobj));
#ifdef VERBOSE
        printf(" %s ", target_name);
#endif
        xmlTextWriterWriteElement(project_writer, name, (xmlChar*)target_name);
        if (strcmp("EXECUTABLE", targetType) == 0) {
            strncpy(from_codemodel.artifactName, json_object_get_string(name_jobj), sizeof(from_codemodel.artifactName));
        }
    }
    xmlTextWriterWriteElement(project_writer, (xmlChar*)"type", (xmlChar*)"2");
    xmlTextWriterWriteElement(project_writer, (xmlChar*)"locationURI", (xmlChar*)"virtual:/virtual");
    xmlTextWriterEndElement(project_writer); // link

    if (!json_object_object_get_ex(parsed_json, "sources", &sources_jobj)) {
        printf("no sources in %s\r\n", full_path);
        goto target_done;
    }

    unsigned n_sources = json_object_array_length(sources_jobj);
    for (unsigned n = 0; n < n_sources; n++) {
        struct json_object *path_jobj, *jobj;
        struct json_object *source_jobj = json_object_array_get_idx(sources_jobj, n);
        if (json_object_object_get_ex(source_jobj, "isGenerated", &jobj)) {
            if (json_object_get_boolean(jobj))
                continue;
        }
        if (json_object_object_get_ex(source_jobj, "path", &path_jobj)) {
            struct json_object *compileGroupIndex_jobj;
            bool alreadyHave = false;
            char str[256];
            char location_str[256];
            char copy[256];

            const char *path = json_object_get_string(path_jobj);
            strcpy(location_str, source_path);
            strcat(location_str, "/");
            strcat(location_str, path);
            for (struct node_s *my_list = source_files; my_list != NULL; my_list = my_list->next) {
                if (strcmp(location_str, my_list->str) == 0) {
                    alreadyHave = true;
                    break;
                }
            }
            if (!alreadyHave) {
                if (json_object_object_get_ex(source_jobj, "compileGroupIndex", &compileGroupIndex_jobj)) {
                    strncpy(copy, path, sizeof(copy));
                    xmlTextWriterStartElement(project_writer, (xmlChar*)"link");
                    strcpy(str, target_name);
                    strcat(str, "/");
                    strcat(str, basename(copy));
                    xmlTextWriterWriteElement(project_writer, name, (xmlChar*)str);
                    xmlTextWriterWriteElement(project_writer, (xmlChar*)"type", (xmlChar*)"1");
                    xmlTextWriterWriteElement(project_writer, (xmlChar*)"location", (xmlChar*)location_str);
                    xmlTextWriterEndElement(project_writer); // link
                    save_to_list(location_str, &source_files, NULL);
                } else {
                    if ((ret = unbuilt_source(source_path, path) < 0)) {  // destination:from_codemodel.buildPath
                        printf("%d = unbuilt_source()\n", ret);
                        goto target_done;
                    }
                }
            } // ..if (!alreadyHave)
        } // ..."path"
    } // ..."sources" array

    struct json_object *compileGroups_jobj;
    if (json_object_object_get_ex(parsed_json, "compileGroups", &compileGroups_jobj)) {
        for (unsigned idx = 0; idx < json_object_array_length(compileGroups_jobj); idx++) {
            struct json_object *compileGroup_jobj = json_object_array_get_idx(compileGroups_jobj, idx);
            const char *langPtr = NULL;
            if (json_object_object_get_ex(compileGroup_jobj, "language", &jobj)) {
                const char *language = json_object_get_string(jobj);
                for (struct node_s *my_list = language_list; my_list != NULL; my_list = my_list->next) {
                    if (strcmp(language, my_list->str) == 0) {
                        langPtr = my_list->str;
                        break;
                    }
                }
                if (langPtr == NULL) {
                    printf("line %d %s new language \e[7m%s\e[0m\n", __LINE__, targetType, language);
                    struct node_s *node = save_to_list(language, &language_list, NULL);
                    langPtr = node->str;
                }
            } else {
                printf("\e[31mline %d: %s no language\e[0m\n", __LINE__, json_object_get_string(name_jobj));
                ret = -1;
                goto target_done;
            }

            struct json_object *defines_jobj;
            if (json_object_object_get_ex(compileGroup_jobj, "defines", &defines_jobj)) {
                unsigned n, n_defines = json_object_array_length(defines_jobj);
                for (n = 0; n < n_defines; n++) {
                    struct json_object *jobj;
                    struct json_object *define_jobj = json_object_array_get_idx(defines_jobj, n);
                    if (json_object_object_get_ex(define_jobj, "define", &jobj)) {
                        const char *define = json_object_get_string(jobj);
                        bool newDefine = true;
                        for (struct node_s *my_list = defines_list; my_list != NULL; my_list = my_list->next) {
                            if (strcmp(define , my_list->str) == 0) {
                                newDefine = false;
                                break;
                            }
                        }
                        if (newDefine) {
#ifdef VERBOSE
                            printf("\e[33mline %d: %s %s \e[7m%s\e[0m \e[33mdefine %s\e[0m\n", __LINE__, targetType, json_object_get_string(name_jobj), langPtr, define);
#endif
                            save_to_list(define, &defines_list, langPtr);
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
                        bool newPath = true;
                        for (struct node_s *my_list = includes_list; my_list != NULL; my_list = my_list->next) {
                            if (strcmp(path, my_list->str) == 0) {
                                newPath = false;
                                break;
                            }
                        }
                        if (newPath) {
#ifdef VERBOSE
                            printf("line %d: %s %s \e[7m%s\e[0m incPath %s\n", __LINE__, targetType, json_object_get_string(name_jobj), langPtr, path);
#endif
                            save_to_list(path, &includes_list, langPtr);
                        }
                    }
                }
            }

            if (strcmp("EXECUTABLE", targetType) != 0) {
                /* the remainder of this function is only for executable target */
                goto target_done;
            }

            struct json_object *compileCommandFragments_jobj;
            if (json_object_object_get_ex(compileGroup_jobj, "compileCommandFragments", &compileCommandFragments_jobj)) {
                unsigned n, n_compileCommandFragments = json_object_array_length(compileCommandFragments_jobj);
                for (n = 0; n < n_compileCommandFragments; n++) {
                    struct json_object *fragment_jobj;
                    struct json_object *compileCommandFragment_jobj = json_object_array_get_idx(compileCommandFragments_jobj, n);
                    if (json_object_object_get_ex(compileCommandFragment_jobj , "fragment", &fragment_jobj)) {
                        const char *fragment = json_object_get_string(fragment_jobj);
#ifdef VERBOSE
                        printf("line %d: %s %s \e[7m%s\e[0m compile-fragment %s\n", __LINE__, targetType, json_object_get_string(name_jobj), langPtr, fragment);
#endif
                        save_to_list(fragment, &from_codemodel.compile_fragment_list, langPtr);
                    } else
                        printf("exe no fragment\r\n");
                }
            } else
                printf("exe no compileCommandFragments\r\n");
        }
    } // ..compileGroups
    
    if (json_object_object_get_ex(parsed_json, "link", &link_jobj)) {
        struct json_object *commandFragments_jobj;
        const char *langPtr = NULL;
        if (json_object_object_get_ex(link_jobj, "language", &jobj)) {
            const char *language = json_object_get_string(jobj);
            for (struct node_s *my_list = language_list; my_list != NULL; my_list = my_list->next) {
                if (strcmp(language, my_list->str) == 0) {
                    langPtr = my_list->str;
                    break;
                }
            }
            if (!langPtr) {
                printf("\e[31mline %d: link %s language %s not found\e[0m\n", __LINE__, json_object_get_string(name_jobj), language);
                ret = -1;

                goto target_done;
            }
        } else {
            printf("\e[31mline %d: link %s no language\e[0m\n", __LINE__, json_object_get_string(name_jobj));
            ret = -1;
            goto target_done;
        }

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
#ifdef VERBOSE
                        printf("line %d: %s %s \e[7m%s\e[0m link-fragment %s\n", __LINE__, targetType, json_object_get_string(name_jobj), langPtr, fragment);
#endif
                        save_to_list(fragment, &from_codemodel.linker_fragment_list, langPtr);
                    } else if (strcmp(role, "libraries") == 0) {
                        if (fragment[0] == '-') /* linker script could be in target_link_libraries() */ {
#ifdef VERBOSE
                            printf("line %d: %s %s \e[7m%s\e[0m link-fragment %s\n", __LINE__, targetType, json_object_get_string(name_jobj), langPtr, fragment);
#endif
                            save_to_list(fragment, &from_codemodel.linker_fragment_list, langPtr);
                        }
                    }
                }
            }
        }
    } // ..if (json_object_object_get_ex(parsed_json, "link", &link_jobj))

target_done:
#ifdef VERBOSE
    printf("\n");
#endif
    free(buffer);
    fclose(fp);
    return ret;
} // ..parse_target_file_to_linked_resources()

extern volatile char wbr;
int parse_codemodel_to_eclipse_project(const char *jsonFileName)
{
    struct stat st;
    struct json_object *parsed_json;
    struct json_object *configurations;
    size_t n_configurations;
    unsigned n;
    int ret;
    char *buffer = NULL;
    FILE *fp = fopen(jsonFileName, "r");
    if (fp == NULL) {
        perror(jsonFileName);
        return -1;
    }

    xmlTextWriterStartElement(project_writer, (xmlChar*)"projectDescription");

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

    wbr = 1;
    parsed_json = json_tokener_parse(buffer);
    wbr = 0;
    if (parsed_json == NULL) {
        printf("%s\n", buffer);
        printf("cannot parse json in %s (%d bytes) st_size:%ld ret:%d\r\n", jsonFileName, (int)strlen(buffer), st.st_size, ret);
        ret = -1;
        goto codemodel_done;
    }
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

    struct json_object *paths_jobj;
    const char *source_path = NULL;
    if (json_object_object_get_ex(parsed_json, "paths", &paths_jobj)) {
        struct json_object *jobj;
        if (json_object_object_get_ex(paths_jobj, "source", &jobj)) {
            source_path = json_object_get_string(jobj);
        }
        if (json_object_object_get_ex(paths_jobj, "build", &jobj)) {
            strncpy(from_codemodel.buildPath, json_object_get_string(jobj), sizeof(from_codemodel.buildPath));
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
            const char *json_name;
            struct json_object *name_jobj;
            json_object_object_get_ex(project, "name", &name_jobj);
            json_name = json_object_get_string(name_jobj);
            strncpy(from_codemodel.project_name, json_name, sizeof(from_codemodel.project_name));
            xmlTextWriterWriteElement(project_writer, name, (xmlChar*)json_name);
        }
    }

    xmlTextWriterWriteElement(project_writer, (xmlChar*)"comment", (xmlChar*)"");
    xmlTextWriterWriteElement(project_writer, (xmlChar*)"projects", (xmlChar*)"");

    xmlTextWriterStartElement(project_writer, (xmlChar*)"buildSpec");
    xmlTextWriterStartElement(project_writer, (xmlChar*)"buildCommand");
    xmlTextWriterWriteElement(project_writer, name, (xmlChar*)"org.eclipse.cdt.managedbuilder.core.genmakebuilder");
    if (GENMAKEBUILDER_TRIGGERS)
        xmlTextWriterWriteElement(project_writer, (xmlChar*)"triggers", (xmlChar*)GENMAKEBUILDER_TRIGGERS);
    if (GENMAKEBUILDER_ARGUMENTS) {
        xmlTextWriterWriteElement(project_writer, (xmlChar*)"arguments", (xmlChar*)GENMAKEBUILDER_ARGUMENTS);
    }
    xmlTextWriterEndElement(project_writer); // buildCommand
    xmlTextWriterStartElement(project_writer, (xmlChar*)"buildCommand");
    xmlTextWriterWriteElement(project_writer, name, (xmlChar*)"org.eclipse.cdt.managedbuilder.core.ScannerConfigBuilder");
    if (SCANNERCONFIGBUILDER_TRIGGERS) {
        xmlTextWriterWriteElement(project_writer, (xmlChar*)"triggers", (xmlChar*)SCANNERCONFIGBUILDER_TRIGGERS);
    }
    if (SCANNERCONFIGBUILDER_ARGUMENTS) {
        xmlTextWriterWriteElement(project_writer, (xmlChar*)"arguments", (xmlChar*)SCANNERCONFIGBUILDER_ARGUMENTS);
    }
    xmlTextWriterEndElement(project_writer); // buildCommand
    put_project_other_builders();
    xmlTextWriterEndElement(project_writer); // buildSpec

    struct json_object *targets;
    json_object_object_get_ex(configuration, "targets", &targets);
    unsigned n_targets = json_object_array_length(targets);

    static_libraries = NULL;

    for (unsigned t = 0; t < n_targets; t++) {
        struct json_object *target = json_object_array_get_idx(targets, t);
        struct json_object *name_jobj;
        if (!json_object_object_get_ex(target, "name", &name_jobj)) {
            printf("target no name: %s\r\n", json_object_get_string(target));
        }
        struct json_object *jsonFile_jobj;
        if (json_object_object_get_ex(target, "jsonFile", &jsonFile_jobj)) {
            ret = preparse_targets(source_path, json_object_get_string(jsonFile_jobj));
            if (ret < 0)
                break;
        }
    }

#ifdef VERBOSE
    printf("------------------------ ..preparse ---------------------\n");
#endif
    for (unsigned t = 0; t < n_targets; t++) {
        struct json_object *target = json_object_array_get_idx(targets, t);
        struct json_object *name_jobj;
        if (!json_object_object_get_ex(target, "name", &name_jobj)) {
            printf("target no name: %s\r\n", json_object_get_string(target));
        }
        struct json_object *jsonFile_jobj;
        if (json_object_object_get_ex(target, "jsonFile", &jsonFile_jobj)) {
            ret = parse_executable_target(source_path, json_object_get_string(jsonFile_jobj));
            if (ret < 0)
                break;
        }
    }
#ifdef VERBOSE
    printf("------------------------ ..executable-target ---------------------\n");
#endif

    xmlTextWriterStartElement(project_writer, (xmlChar*)"natures");
    for (struct node_s *my_list = language_list; my_list != NULL; my_list = my_list->next) {
        if (strcmp("C", my_list->str) == 0) {
            xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"org.eclipse.cdt.core.cnature");
        } else if (strcmp("CXX", my_list->str) == 0) {
            xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"org.eclipse.cdt.core.ccnature");
        }
    }
    write_natures();
    xmlTextWriterEndElement(project_writer); // natures

    xmlTextWriterStartElement(project_writer, (xmlChar*)"linkedResources");

    for (unsigned t = 0; t < n_targets; t++) {
        struct json_object *target = json_object_array_get_idx(targets, t);
        struct json_object *name_jobj;
        if (!json_object_object_get_ex(target, "name", &name_jobj)) {
            printf("target no name: %s\r\n", json_object_get_string(target));
        }
        struct json_object *jsonFile_jobj;
        if (json_object_object_get_ex(target, "jsonFile", &jsonFile_jobj)) {
            ret = parse_target_file_to_linked_resources(source_path, json_object_get_string(jsonFile_jobj));
            if (ret < 0)
                break;
        }
    }
    xmlTextWriterEndElement(project_writer); // linkedResources

    xmlTextWriterEndElement(project_writer); // projectDescription

codemodel_done:
    free(buffer);
    fclose(fp);
    return ret;
} // ..parse_codemodel_to_eclipse_project()

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
        struct json_object *name_jobj, *value_jobj;
        struct json_object *entry_jobj = json_object_array_get_idx(entries_jobj, n);
        if (json_object_object_get_ex(entry_jobj, "name", &name_jobj) &&
            json_object_object_get_ex(entry_jobj, "value", &value_jobj))
        {
            if (strcmp(json_object_get_string(name_jobj), "BOARD") == 0 ||
                strcmp(json_object_get_string(name_jobj), "AFR_BOARD_NAME") == 0)
            {
                const char *vstr = json_object_get_string(value_jobj);
                from_cache.board = malloc(strlen(vstr)+1);
                strcpy(from_cache.board, vstr);
            } else if (strcmp(json_object_get_string(name_jobj), "CMAKE_CXX_FLAGS_DEBUG") == 0) {
                save_to_list(json_object_get_string(value_jobj), &from_cache.cxx_flags_debug, NULL);
            } else if (strcmp(json_object_get_string(name_jobj), "CMAKE_CXX_FLAGS_RELEASE") == 0) {
                save_to_list(json_object_get_string(value_jobj), &from_cache.cxx_flags_release, NULL);
            } else if (strcmp(json_object_get_string(name_jobj), "CMAKE_C_FLAGS_RELEASE") == 0) {
                save_to_list(json_object_get_string(value_jobj), &from_cache.c_flags_release, NULL);
            } else if (strcmp(json_object_get_string(name_jobj), "CMAKE_C_FLAGS_DEBUG") == 0) {
                save_to_list(json_object_get_string(value_jobj), &from_cache.c_flags_debug, NULL);
            } else if (strcmp(json_object_get_string(name_jobj), "CMAKE_ASM_FLAGS_DEBUG") == 0) {
                save_to_list(json_object_get_string(value_jobj), &from_cache.asm_flags_debug, NULL);
            } else if (strcmp(json_object_get_string(name_jobj), "CMAKE_ASM_FLAGS_RELEASE") == 0) {
                save_to_list(json_object_get_string(value_jobj), &from_cache.asm_flags_release, NULL);
            }
        }
    }

cache_done:
    fclose(fp);
    free(buffer);
    return ret;
}

int project_start(bool force)
{
    struct stat st;
    char full_path[256];
    DIR *dp;
    int ret;
    struct dirent *ep;
    const char *index_prefix = "index";
    char cache_filename[128];
    char codemodel_filename[128];
    const char *xml_filename = ".project";

    LIBXML_TEST_VERSION
    if (!force && stat(xml_filename, &st) == 0) {
        printf("%s already exists ", xml_filename);
        return -EEXIST;
    }

    dp = opendir(reply_directory);
    if (dp == NULL) {
        perror(reply_directory);
        return -1;
    }

    linker_script[0] = 0;
    includes_list = NULL;
    defines_list = NULL;
    from_codemodel.compile_fragment_list = NULL;
    from_codemodel.linker_fragment_list = NULL;
    memset(&from_cache, 0, sizeof(from_cache_t));

    ret = -1;
    while ((ep = readdir (dp))) {
        if (strncmp(ep->d_name, index_prefix, strlen(index_prefix)) == 0) {
            strcpy(full_path, reply_directory);
            strcat(full_path, "/");
            strcat(full_path, ep->d_name);
            ret = parse_index(full_path, cache_filename, codemodel_filename);
            break;
        }
    }

    (void) closedir (dp);

    if (ret < 0) {
        printf("failed to find cmake json index file\r\n");
        return ret;
    }

    project_writer = xmlNewTextWriterFilename(xml_filename, 0);
    if (project_writer == NULL) {
        printf("cannot create xml writer\r\n");
        return -1;
    }
    ret = xmlTextWriterStartDocument(project_writer, NULL, "UTF-8", NULL);
    if (ret < 0) {
        printf("Error at xmlTextWriterStartDocument\n");
        return -1;
    }
    ret = xmlTextWriterSetIndent(project_writer, 1);
    if (ret < 0) {
        printf("Error at xmlTextWriterSetIndent\n");
        return -1;
    }
    xmlTextWriterSetIndentString(project_writer, (xmlChar*)"\t");

    strcpy(full_path, reply_directory);
    strcat(full_path, "/");
    strcat(full_path, cache_filename);
    ret = parse_cache(full_path);
    if (ret < 0) {
        return -1;
    }

    strcpy(full_path, reply_directory);
    strcat(full_path, "/");
    strcat(full_path, codemodel_filename);
    ret = parse_codemodel_to_eclipse_project(full_path);
    if (ret < 0) {
        return ret;
    }

    ret = xmlTextWriterEndDocument(project_writer);
    if (ret < 0) {
        printf ("testXmlwriterFilename: Error at xmlTextWriterEndDocument\n");
        return ret;
    }
    xmlFreeTextWriter(project_writer);

    return ret;
}

void get_us(char *dest)
{
    unsigned us;
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    us = (tp.tv_sec % 1000) * 1e9;
    us += tp.tv_nsec / 1e3;
    sprintf(dest, "%u", us);
}

void put_id(const char *in, char *out)
{
    char us_str[16];
    strcpy(out, in);
    strcat(out, ".");
    get_us(us_str);
    strcat(out, us_str);
}

struct node_s *add_instance(const char *ccfg_id)
{
    struct node_s **node = &instance_list;
    if (*node) {
        while ((*node)->next)
            node = &(*node)->next;
        node = &(*node)->next;
    }
    (*node) = malloc(sizeof(struct node_s));
    (*node)->next = NULL;
    (*node)->str = malloc(INSTANCE_MAX_STRLEN);
    strcpy((*node)->str, ccfg_id);
    strcat((*node)->str, ";");
    strcat((*node)->str, ccfg_id);
    strcat((*node)->str, ".");

    return *node;
}

int put_cconfiguration(bool debugBuild)
{
    char cconfiguration_superClass[96];
    char str[256];
    char ccfg_id[256];
    char Board[64], Mcu[64];
    const char *Build;
    if (debugBuild) {
        Build = "Debug";
    } else {
        Build = "Release";
    }

    for (struct node_s *list = from_codemodel.compile_fragment_list; list != NULL; list = list->next) {
        list->taken = false;
    }
    for (struct node_s *list = from_codemodel.linker_fragment_list; list != NULL; list = list->next) {
        list->taken = false;
    }

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"cconfiguration");

    translate_board_mcu(from_cache.board, Board, Mcu);
    if (Mcu[0] == 0) {
        printf("no Mcu for %s\n", from_cache.board);
        return -1;
    }

    if (get_cconfiguration_id(debugBuild, Mcu, ccfg_id, cconfiguration_superClass)) {
        printf("get_cconfiguration_id() failed\n");
        return -1;
    }

    struct node_s *node = add_instance(ccfg_id);

    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)ccfg_id);

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"buildSystemId", (xmlChar*)"org.eclipse.cdt.managedbuilder.core.configurationDataProvider");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)ccfg_id);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"org.eclipse.cdt.core.settings");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)Build);
    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"externalSettings");
    xmlTextWriterEndElement(cproject_writer); // externalSettings
    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"extensions");

    for (unsigned n = 0; BINARY_PARSERS[n] != NULL; n++) {
        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"extension");
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)BINARY_PARSERS[n]);
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"point", (xmlChar*)"org.eclipse.cdt.core.BinaryParser");
        xmlTextWriterEndElement(cproject_writer); // extension
    }

    for (unsigned n = 0; ERROR_PARSERS[n] != NULL; n++) {
        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"extension");
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)ERROR_PARSERS[n]);
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"point", (xmlChar*)"org.eclipse.cdt.core.ErrorParser");
        xmlTextWriterEndElement(cproject_writer); // extension
    }

    xmlTextWriterEndElement(cproject_writer); // extensions
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"cdtBuildSystem");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"version", (xmlChar*)"4.0.0");

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"configuration");
    if (_put_configuration(debugBuild, ccfg_id, cconfiguration_superClass, Board, Mcu, node) < 0) {
        printf("_put_configuration() failed\n");
    }
    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"sourceEntries");
    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"entry");
    strcpy(str, "CMakeFiles");
    cat_additional_exclude_directories(str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"excluding", (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"flags", (xmlChar*)"VALUE_WORKSPACE_PATH|RESOLVED");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"kind", (xmlChar*)"sourcePath");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"");
    xmlTextWriterEndElement(cproject_writer); // entry 
    xmlTextWriterEndElement(cproject_writer); // sourceEntries

    xmlTextWriterEndElement(cproject_writer); // configuration

    xmlTextWriterEndElement(cproject_writer); // storageModule for moduleId=cdtBuildSystem

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"org.eclipse.cdt.core.externalSettings");
    xmlTextWriterEndElement(cproject_writer); // storageModule

    put_other_cconfiguration_storageModules(debugBuild);

    xmlTextWriterEndElement(cproject_writer); // cconfiguration
    return 0;
}

int cproject_start(bool force)
{
    struct stat st;
    const char *xml_filename = ".cproject";
    int ret;

    if (cproject_init() < 0) {
        printf("cproject_init() failed\n");
        return -1;
    }

    if (!force && stat(xml_filename, &st) == 0) {
        printf("%s already exists ", xml_filename);
        return -EEXIST;
    }

    cproject_writer = xmlNewTextWriterFilename(xml_filename, 0);
    if (cproject_writer == NULL) {
        printf("cannot create xml writer\r\n");
        return -1;
    }
    ret = xmlTextWriterStartDocument(cproject_writer, NULL, "UTF-8", "no");
    if (ret < 0) {
        printf("Error at xmlTextWriterStartDocument\n");
        return ret;
    }

    ret = xmlTextWriterSetIndent(cproject_writer, 1);
    if (ret < 0) {
        printf("Error at xmlTextWriterSetIndent\n");
        return ret;
    }
    xmlTextWriterSetIndentString(cproject_writer, (xmlChar*)"\t");  // tabs instead of spaces

    xmlTextWriterWritePI(cproject_writer, (xmlChar*)"fileVersion", (xmlChar*)"4.0.0");

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"cproject");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"storage_type_id", (xmlChar*)"org.eclipse.cdt.core.XmlProjectDescriptionStorage");

    xmlTextWriterStartElement(cproject_writer, storageModule);
    if (CDT_CORE_SETTINGS_CONFIGRELATIONS)
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"configRelations", (xmlChar*)CDT_CORE_SETTINGS_CONFIGRELATIONS);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"org.eclipse.cdt.core.settings");

    cpp_input = false;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (strcmp("CXX", (char*)my_list->arg) == 0) {
            cpp_input = true;
            break;
        }
    }

    cpp_linker = false;
    for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (strcmp("CXX", (char*)my_list->arg) == 0)
            cpp_linker = true;
        if (linker_script[0] == 0 && strncmp(my_list->str, "-T", 2) == 0)
            strcpy(linker_script, my_list->str+2);
    }

    instance_list = NULL;

    build = "debug";
    Build = "Debug";
    c_flags = from_cache.c_flags_debug;
    cpp_flags = from_cache.cxx_flags_debug;
    if (put_cconfiguration(true) < 0)
        return -1;

    build = "release";
    Build = "Release";
    c_flags = from_cache.c_flags_release;
    cpp_flags = from_cache.cxx_flags_release;
    if (put_cconfiguration(false) < 0)
        return -1;

    xmlTextWriterEndElement(cproject_writer); // storageModule

    put_other_storageModules();

    xmlTextWriterEndElement(cproject_writer); // cproject

    ret = xmlTextWriterEndDocument(cproject_writer);
    if (ret < 0) {
        printf ("testXmlwriterFilename: Error at xmlTextWriterEndDocument\n");
        return ret;
    }
    xmlFreeTextWriter(cproject_writer);

    return 0;
}

void free_lists()
{
    struct node_s *my_list;

    for (my_list = defines_list; my_list != NULL; ) {
        void *next = my_list->next;
        free(my_list->str);
        free(my_list);
        my_list = next;
    }

    for (my_list = includes_list; my_list != NULL; ) {
        void *next = my_list->next;
        free(my_list->str);
        free(my_list);
        my_list = next;
    }
}

