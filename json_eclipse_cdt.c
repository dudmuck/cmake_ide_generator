/* Read cmake json (cmake file api), and output eclipse CDT XML */
#include <dirent.h>
#include <time.h>

#include "json_eclipse_cdt.h"
#include "board-mcu-hack.h"

xmlTextWriterPtr project_writer;
xmlTextWriterPtr cproject_writer;

from_codemodel_t from_codemodel;
from_cache_t from_cache;

const char * const reply_directory = ".cmake/api/v1/reply";

struct node_s *defines_list;
struct node_s *includes_list;

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

void put_scannerConfiguration(const instance_t *debugInstance, const instance_t *releaseInstance, const char *is_problemReportingEnabled)
{
    xmlTextWriterStartElement(cproject_writer, autodiscovery);
    xmlTextWriterWriteAttribute(cproject_writer, enabled, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"problemReportingEnabled", (xmlChar*)is_problemReportingEnabled);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"selectedProfileId", (xmlChar*)"");
    xmlTextWriterEndElement(cproject_writer); // autodiscovery

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"scannerConfigBuildInfo");
    {
        unsigned len = 12;  // semicolons
        len += strlen(debugInstance->config_gnu_cross_exe);
        len += strlen(debugInstance->config_gnu_cross_exe);
        len += strlen(debugInstance->tool_gnu_cross_c_compiler);
        len += strlen(debugInstance->tool_gnu_cross_c_compiler_input);
        char *str = malloc(len);
        strcpy(str, debugInstance->config_gnu_cross_exe);
        strcat(str, ";");
        strcat(str, debugInstance->config_gnu_cross_exe);
        strcat(str, ".;");
        strcat(str, debugInstance->tool_gnu_cross_c_compiler);
        strcat(str, ";");
        strcat(str, debugInstance->tool_gnu_cross_c_compiler_input);
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"instanceId", (xmlChar*)str);
        free(str);
    }
    xmlTextWriterStartElement(cproject_writer, autodiscovery);
    xmlTextWriterWriteAttribute(cproject_writer, enabled, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"problemReportingEnabled", (xmlChar*)"");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"selectedProfileId", (xmlChar*)"");
    xmlTextWriterEndElement(cproject_writer); // autodiscovery
    xmlTextWriterEndElement(cproject_writer); // scannerConfigBuildInfo 

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"scannerConfigBuildInfo");
    {
        unsigned len = 12;  // semicolons
        len += strlen(releaseInstance->config_gnu_cross_exe);
        len += strlen(releaseInstance->config_gnu_cross_exe);
        len += strlen(releaseInstance->tool_gnu_cross_c_compiler);
        len += strlen(releaseInstance->tool_gnu_cross_c_compiler_input);
        char *str = malloc(len);
        strcpy(str, releaseInstance->config_gnu_cross_exe);
        strcat(str, ";");
        strcat(str, releaseInstance->config_gnu_cross_exe);
        strcat(str, ".;");
        strcat(str, releaseInstance->tool_gnu_cross_c_compiler);
        strcat(str, ";");
        strcat(str, releaseInstance->tool_gnu_cross_c_compiler_input);
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"instanceId", (xmlChar*)str);
        free(str);
    }

    xmlTextWriterStartElement(cproject_writer, autodiscovery);
    xmlTextWriterWriteAttribute(cproject_writer, enabled, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"problemReportingEnabled", (xmlChar*)"");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"selectedProfileId", (xmlChar*)"");
    xmlTextWriterEndElement(cproject_writer); // autodiscovery
    xmlTextWriterEndElement(cproject_writer); // scannerConfigBuildInfo 
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

static void save_flags(const char *fragment, struct node_s **dest)
{
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
        }

next_token:
        token = strtok(NULL, " ");
    }

    free(copy);
}

int parse_target_file_to_linked_resources(const char *source_path, const char *jsonFileName)
{
    //struct json_tokener *tok = NULL;
    struct stat st;
    struct json_object *parsed_json;
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

    if (strcmp("UTILITY", targetType) == 0) {
        /* TODO get post-build steps */
        /* add_custom_target( COMMAND <foobar> ) where to i get <foobar> ? */
    }

    if (strcmp("OBJECT_LIBRARY", targetType) != 0 && strcmp("EXECUTABLE", targetType) != 0)
        goto target_done;

    char target_name[256];
    struct json_object *sources_jobj;
    xmlTextWriterStartElement(project_writer, (xmlChar*)"link");
    if (json_object_object_get_ex(parsed_json, "name", &name_jobj)) {
        sprintf(target_name, "[%s]", json_object_get_string(name_jobj));
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
            char str[256];
            char copy[256];

            const char *path = json_object_get_string(path_jobj);
            if (json_object_object_get_ex(source_jobj, "compileGroupIndex", &compileGroupIndex_jobj)) {
                strncpy(copy, path, sizeof(copy));
                xmlTextWriterStartElement(project_writer, (xmlChar*)"link");
                strcpy(str, target_name);
                strcat(str, "/");
                strcat(str, basename(copy));
                xmlTextWriterWriteElement(project_writer, name, (xmlChar*)str);
                xmlTextWriterWriteElement(project_writer, (xmlChar*)"type", (xmlChar*)"1");
                strcpy(str, source_path);
                strcat(str, "/");
                strcat(str, path);
                xmlTextWriterWriteElement(project_writer, (xmlChar*)"location", (xmlChar*)str);
                xmlTextWriterEndElement(project_writer); // link
            } else {
                if ((ret = unbuilt_source(source_path, path) < 0)) {  // destination:from_codemodel.buildPath
                    printf("%d = unbuilt_source()\n", ret);
                    goto target_done;
                }
            }
        }
    }

    struct json_object *compileGroups_jobj;
    if (!json_object_object_get_ex(parsed_json, "compileGroups", &compileGroups_jobj)) {
        printf("missing compileGroups");
        goto target_done;
    }

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
                save_flags(fragment, &from_codemodel.compile_fragment_list);
            } else
                printf("exe no fragment\r\n");
        }
    } else
        printf("exe no compileCommandFragments\r\n");
    

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
                        save_flags(fragment, &from_codemodel.linker_fragment_list);
                    } else if (strcmp(role, "libraries") == 0) {
                        /* TODO add extra link libraries ? */
                    }
                }
            }
        }
    } // ..if (json_object_object_get_ex(parsed_json, "link", &link_jobj))

target_done:
    free(buffer);
    fclose(fp);
    return ret;
} // ..parse_target_file_to_linked_resources()

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

    parsed_json = json_tokener_parse(buffer);
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

    xmlTextWriterStartElement(project_writer, (xmlChar*)"natures");
    write_natures();
    xmlTextWriterEndElement(project_writer); // natures

    xmlTextWriterStartElement(project_writer, (xmlChar*)"linkedResources");

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
            ret = parse_target_file_to_linked_resources(source_path, json_object_get_string(jsonFile_jobj));
            if (ret < 0)
                break;
        }
    }
    xmlTextWriterEndElement(project_writer); // linkedResources

    xmlTextWriterEndElement(project_writer); // projectDescription

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

    includes_list = NULL;
    defines_list = NULL;
    from_codemodel.compile_fragment_list = NULL;
    from_codemodel.linker_fragment_list = NULL;

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

void put_cconfiguration(bool debugBuild, instance_t *instance)
{
    char cconfiguration_superClass[96];
    char str[256];
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

    if (get_cconfiguration_id(debugBuild, Mcu, str, cconfiguration_superClass)) {
        printf("get_cconfiguration_id() failed\n");
        return;
    }
    strcpy(instance->config_gnu_cross_exe, str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)str);

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"buildSystemId", (xmlChar*)"org.eclipse.cdt.managedbuilder.core.configurationDataProvider");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)instance->config_gnu_cross_exe);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"org.eclipse.cdt.core.settings");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)Build);
    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"externalSettings");
    xmlTextWriterEndElement(cproject_writer); // externalSettings
    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"extensions");

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"extension");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)BINARY_PARSER);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"point", (xmlChar*)"org.eclipse.cdt.core.BinaryParser");
    xmlTextWriterEndElement(cproject_writer); // extension

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
    if (_put_configuration(debugBuild, instance, cconfiguration_superClass, Board, Mcu) < 0) {
        printf("put_configuration() failed\n");
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
    instance_t debugInstance, releaseInstance;
    put_cconfiguration(true, &debugInstance);
    put_cconfiguration(false, &releaseInstance);

    xmlTextWriterEndElement(cproject_writer); // storageModule

    put_other_storageModules(&debugInstance, &releaseInstance);

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

