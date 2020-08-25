/* Read cmake json (cmake file api), and output eclipse CDT XML */
#include <dirent.h>
#include <time.h>

#include "json_eclipse_cdt.h"

xmlTextWriterPtr project_writer;
xmlTextWriterPtr cproject_writer;

from_codemodel_t from_codemodel;
from_cache_t from_cache;

const char * const reply_directory = ".cmake/api/v1/reply";

struct node_s *defines_list;
struct node_s *includes_list;

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

static void save_compile_args(const char *fragment)
{
    char str[256];
    char *token;

    strncpy(str, fragment, sizeof(str));
    token = strtok(str, " ");

    while (token != NULL) {
        int toklen = strlen(token);
        struct node_s **node = &from_codemodel.compile_fragment_list;
        if (*node) {
            while ((*node)->next) {
                node = &(*node)->next;
            }
            node = &(*node)->next;
        }
        (*node) = malloc(sizeof(struct node_s));
        (*node)->next = NULL;
        (*node)->str = malloc(toklen+1);
        strcpy((*node)->str, token);

        token = strtok(NULL, " ");
    }
}

void foobar(const char *buffer)
{
    const char *ptr;
    //unsigned at = 0;
    int level = 0;

    for (ptr = buffer; *ptr != 0; ptr++) {
        if (*ptr == '{') {
            level++;
            printf("level-in %d at %u\r\n", level, ptr - buffer);
        } else if (*ptr == '}') {
            level--;
            printf("level-out %d at %u\r\n", level, ptr - buffer);
        }
    }
    printf("done %d at %u\r\n", level, ptr - buffer);
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
            return ret;
        n += ret;
        ptr += ret;
    }
    return n;
}
#endif /* __WIN32__ */

int parse_target_file_to_linked_resources(const char *source_path, const char *jsonFileName)
{
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
    xmlTextWriterStartElement(project_writer, "link");
    if (json_object_object_get_ex(parsed_json, "name", &name_jobj)) {
        sprintf(target_name, "[%s]", json_object_get_string(name_jobj));
        xmlTextWriterWriteElement(project_writer, "name", target_name);
        if (strcmp("EXECUTABLE", targetType) == 0) {
            strncpy(from_codemodel.artifactName, json_object_get_string(name_jobj), sizeof(from_codemodel.artifactName));
        }
    }
    xmlTextWriterWriteElement(project_writer, "type", "2");
    xmlTextWriterWriteElement(project_writer, "locationURI", "virtual:/virtual");
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
            char str[256];
            char copy[256];

            const char *path = json_object_get_string(path_jobj);
            strncpy(copy, path, sizeof(copy));
            xmlTextWriterStartElement(project_writer, "link");
            strcpy(str, target_name);
            strcat(str, "/");
            strcat(str, basename(copy));
            xmlTextWriterWriteElement(project_writer, "name", str);
            xmlTextWriterWriteElement(project_writer, "type", "1");
            strcpy(str, source_path);
            strcat(str, "/");
            strcat(str, path);
            xmlTextWriterWriteElement(project_writer, "location", str);
            xmlTextWriterEndElement(project_writer); // link
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
                const char *ptr, *fragment = json_object_get_string(fragment_jobj);
                save_compile_args(fragment);
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
                        unsigned len = 0;
                        const char *ptr = strstr(fragment, "-T");
                        if (ptr != NULL)
                            path_copy(from_codemodel.linkerScript, ptr + 2, sizeof(from_codemodel.linkerScript));

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

target_done:
    free(buffer);
    fclose(fp);
    return ret;
}

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

    xmlTextWriterStartElement(project_writer, "projectDescription");

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
        const char *pstr;
        struct json_object *project = json_object_array_get_idx(projects, p);
        struct json_object *parentIndex;
        pstr = json_object_get_string(project);
        json_object_object_get_ex(project, "parentIndex", &parentIndex);
        if (parentIndex == NULL) {
            const char *name;
            struct json_object *name_jobj;
            json_object_object_get_ex(project, "name", &name_jobj);
            name = json_object_get_string(name_jobj);
            strncpy(from_codemodel.project_name, name, sizeof(from_codemodel.project_name));
            xmlTextWriterWriteElement(project_writer, "name", name);
        }
    }

    
    xmlTextWriterWriteElement(project_writer, "comment", "");
    xmlTextWriterWriteElement(project_writer, "projects", "");

    xmlTextWriterStartElement(project_writer, "buildSpec");
    xmlTextWriterStartElement(project_writer, "buildCommand");
    xmlTextWriterWriteElement(project_writer, "name", "org.eclipse.cdt.managedbuilder.core.genmakebuilder");
    xmlTextWriterWriteElement(project_writer, "triggers", "clean,full,incremental,");
    xmlTextWriterEndElement(project_writer); // buildCommand
    xmlTextWriterStartElement(project_writer, "buildCommand");
    xmlTextWriterWriteElement(project_writer, "name", "org.eclipse.cdt.managedbuilder.core.ScannerConfigBuilder");
    put_ScannerConfigBuilder_triggers();
    xmlTextWriterStartElement(project_writer, "arguments");
    xmlTextWriterEndElement(project_writer); // arguments
    xmlTextWriterEndElement(project_writer); // buildCommand
    xmlTextWriterEndElement(project_writer); // buildSpec

    xmlTextWriterStartElement(project_writer, "natures");
    write_natures();
    xmlTextWriterWriteElement(project_writer, "nature", "org.eclipse.cdt.managedbuilder.core.managedBuildNature");
    xmlTextWriterWriteElement(project_writer, "nature", "org.eclipse.cdt.managedbuilder.core.ScannerConfigNature");
    xmlTextWriterWriteElement(project_writer, "nature", "org.eclipse.cdt.core.ccnature");
    xmlTextWriterWriteElement(project_writer, "nature", "org.eclipse.cdt.core.cnature");
    xmlTextWriterEndElement(project_writer); // natures

    xmlTextWriterStartElement(project_writer, "linkedResources");

    struct json_object *targets;
    json_object_object_get_ex(configuration, "targets", &targets);
    unsigned n_targets = json_object_array_length(targets);
    for (unsigned t = 0; t < n_targets; t++) {
        const char *name;
        struct json_object *target = json_object_array_get_idx(targets, t);
        struct json_object *name_jobj;
        if (json_object_object_get_ex(target, "name", &name_jobj)) {
            name = json_object_get_string(name_jobj);
        } else {
            printf("target no name: %s\r\n", json_object_get_string(target));
        }
        struct json_object *jsonFile_jobj;
        if (json_object_object_get_ex(target, "jsonFile", &jsonFile_jobj)) {
            parse_target_file_to_linked_resources(source_path, json_object_get_string(jsonFile_jobj));
        }
    }
    xmlTextWriterEndElement(project_writer); // linkedResources

    xmlTextWriterEndElement(project_writer); // projectDescription

codemodel_done:
    free(buffer);
    fclose(fp);
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
    from_codemodel.specs_list = NULL;
    from_codemodel.compile_fragment_list = NULL;

    ret = -1;
    while (ep = readdir (dp)) {
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
    xmlTextWriterSetIndentString(project_writer, "\t");

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
    const char * const artifactExtension = "elf";
    char config_gnu_cross_exe_parent[96];
    char parent_str[96];
    char id_str[128];
    char str[256];
    char strb[16];
    char Board[64], Mcu[64];
    const char *build;
    const char *Build;
    if (debugBuild) {
        build = "debug";
        Build = "Debug";
    } else {
        build = "release";
        Build = "Release";
    }
    
    for (struct node_s *list = from_codemodel.compile_fragment_list; list != NULL; list = list->next) {
        list->taken = false;
    }

    xmlTextWriterStartElement(cproject_writer, "cconfiguration");

    sprintf(str, CONFIGURATION_EXE_SUPERCLASS, TITLE, build);
    strcpy(config_gnu_cross_exe_parent, str);
    strcat(str, ".");
    get_us(strb);
    strcat(str, strb);
    strcpy(instance->config_gnu_cross_exe, str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", str);

    xmlTextWriterStartElement(cproject_writer, "storageModule");
    xmlTextWriterWriteAttribute(cproject_writer, "buildSystemId", "org.eclipse.cdt.managedbuilder.core.configurationDataProvider");
    xmlTextWriterWriteAttribute(cproject_writer, "id", instance->config_gnu_cross_exe);
    xmlTextWriterWriteAttribute(cproject_writer, "moduleId", "org.eclipse.cdt.core.settings");
    xmlTextWriterWriteAttribute(cproject_writer, "name", Build);
    xmlTextWriterStartElement(cproject_writer, "externalSettings");
    xmlTextWriterEndElement(cproject_writer); // externalSettings
    xmlTextWriterStartElement(cproject_writer, "extensions");

    xmlTextWriterStartElement(cproject_writer, "extension");
    xmlTextWriterWriteAttribute(cproject_writer, "id", "org.eclipse.cdt.core.ELF");
    xmlTextWriterWriteAttribute(cproject_writer, "point", "org.eclipse.cdt.core.BinaryParser");
    xmlTextWriterEndElement(cproject_writer); // extension
    xmlTextWriterStartElement(cproject_writer, "extension");
    xmlTextWriterWriteAttribute(cproject_writer, "id", "org.eclipse.cdt.core.GASErrorParser");
    xmlTextWriterWriteAttribute(cproject_writer, "point", "org.eclipse.cdt.core.ErrorParser");
    xmlTextWriterEndElement(cproject_writer); // extension
    xmlTextWriterStartElement(cproject_writer, "extension");
    xmlTextWriterWriteAttribute(cproject_writer, "id", "org.eclipse.cdt.core.GmakeErrorParser");
    xmlTextWriterWriteAttribute(cproject_writer, "point", "org.eclipse.cdt.core.ErrorParser");
    xmlTextWriterEndElement(cproject_writer); // extension
    xmlTextWriterStartElement(cproject_writer, "extension");
    xmlTextWriterWriteAttribute(cproject_writer, "id", "org.eclipse.cdt.core.GLDErrorParser");
    xmlTextWriterWriteAttribute(cproject_writer, "point", "org.eclipse.cdt.core.ErrorParser");
    xmlTextWriterEndElement(cproject_writer); // extension
    xmlTextWriterStartElement(cproject_writer, "extension");
    xmlTextWriterWriteAttribute(cproject_writer, "id", "org.eclipse.cdt.core.CWDLocator");
    xmlTextWriterWriteAttribute(cproject_writer, "point", "org.eclipse.cdt.core.ErrorParser");
    xmlTextWriterEndElement(cproject_writer); // extension
    xmlTextWriterStartElement(cproject_writer, "extension");
    xmlTextWriterWriteAttribute(cproject_writer, "id", "org.eclipse.cdt.core.GCCErrorParser");
    xmlTextWriterWriteAttribute(cproject_writer, "point", "org.eclipse.cdt.core.ErrorParser");
    xmlTextWriterEndElement(cproject_writer); // extension
    xmlTextWriterEndElement(cproject_writer); // extensions
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, "storageModule");
    xmlTextWriterWriteAttribute(cproject_writer, "moduleId", "cdtBuildSystem");
    xmlTextWriterWriteAttribute(cproject_writer, "version", "4.0.0");

    xmlTextWriterStartElement(cproject_writer, "configuration");
    xmlTextWriterWriteAttribute(cproject_writer, "artifactExtension", artifactExtension);
    xmlTextWriterWriteAttribute(cproject_writer, "artifactName", from_codemodel.artifactName);
    xmlTextWriterWriteAttribute(cproject_writer, "buildArtefactType", "org.eclipse.cdt.build.core.buildArtefactType.exe" );
    strcpy(str, "org.eclipse.cdt.build.core.buildArtefactType=org.eclipse.cdt.build.core.buildArtefactType.exe,org.eclipse.cdt.build.core.buildType=org.eclipse.cdt.build.core.buildType.");
    strcat(str, build);
    xmlTextWriterWriteAttribute(cproject_writer, "buildProperties", str);
    xmlTextWriterWriteAttribute(cproject_writer, "cleanCommand", "rm -rf");  // on windows too?
    xmlTextWriterWriteAttribute(cproject_writer, "description", "");
    /* stm32cubeIDE doesnt like "errorParsers" */
    //xmlTextWriterWriteAttribute(cproject_writer, "errorParsers", "org.eclipse.cdt.core.GASErrorParser;org.eclipse.cdt.core.GmakeErrorParser;org.eclipse.cdt.core.GLDErrorParser;org.eclipse.cdt.core.CWDLocator;org.eclipse.cdt.core.GCCErrorParser");
    xmlTextWriterWriteAttribute(cproject_writer, "id", instance->config_gnu_cross_exe);
    xmlTextWriterWriteAttribute(cproject_writer, "name", Build);
    //xmlTextWriterWriteAttribute(cproject_writer, "optionalBuildProperties", "org.eclipse.cdt.docker.launcher.containerbuild.property.volumes=,org.eclipse.cdt.docker.launcher.containerbuild.property.selectedvolumes=");
    xmlTextWriterWriteAttribute(cproject_writer, "parent", config_gnu_cross_exe_parent);

    put_post_build_steps(artifactExtension);

    xmlTextWriterStartElement(cproject_writer, "folderInfo");
    strcpy(str, instance->config_gnu_cross_exe);
    strcat(str, ".");   // what is this trailing dot for?
    xmlTextWriterWriteAttribute(cproject_writer, "id", str);
    xmlTextWriterWriteAttribute(cproject_writer, "name", "/");
    xmlTextWriterWriteAttribute(cproject_writer, "resourcePath", "");

    xmlTextWriterStartElement(cproject_writer, "toolChain");
    sprintf(parent_str, TOOLCHAIN_SUPERCLASS, TITLE, build);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "name", TOOLCHAIN_NAME);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);

    put_toolchain_type();
    put_toolchain_version();

    translate_board_mcu(from_cache.board, Board, Mcu);

    if (Mcu[0] != 0) {
        xmlTextWriterStartElement(cproject_writer, "option");
        sprintf(parent_str, OPTION_MCU_SUPERCLASS, TITLE);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
        xmlTextWriterWriteAttribute(cproject_writer, "name", "Mcu");
        xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, "value", Mcu);
        xmlTextWriterWriteAttribute(cproject_writer, "valueType", "string");
        xmlTextWriterEndElement(cproject_writer); // option
    } else
        printf("Warning: no Mcu defined\r\n");

    put_target_id();

        if (Board[0] != 0) {
        xmlTextWriterStartElement(cproject_writer, "option");
        sprintf(parent_str, OPTION_BOARD_SUPERCLASS, TITLE);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
        xmlTextWriterWriteAttribute(cproject_writer, "name", "Board");
        xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, "value", Board);
        xmlTextWriterWriteAttribute(cproject_writer, "valueType", "string");
        xmlTextWriterEndElement(cproject_writer); // option
    } else
        printf("Warning: no Board defined\r\n");

    xmlTextWriterStartElement(cproject_writer, "targetPlatform");
    xmlTextWriterWriteAttribute(cproject_writer, "archList", "all");
    xmlTextWriterWriteAttribute(cproject_writer, "binaryParser", "org.eclipse.cdt.core.ELF");
    sprintf(parent_str, TARGET_PLATFORM_SUPERCLASS, TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "isAbstract", "false");
    xmlTextWriterWriteAttribute(cproject_writer, "osList", "all");
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
    xmlTextWriterEndElement(cproject_writer); // targetPlatform

    xmlTextWriterStartElement(cproject_writer, "builder");
    strcpy(str, from_codemodel.buildPath);
    strcat(str, "/");
    strcat(str, Build);
    xmlTextWriterWriteAttribute(cproject_writer, "buildPath", str);
    sprintf(parent_str, BUILDER_SUPERCLASS, TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "keepEnvironmentInBuildfile", "false");
    xmlTextWriterWriteAttribute(cproject_writer, "managedBuildOn", "true");
    get_bulder_name(str, Build);
    //xmlTextWriterWriteAttribute(cproject_writer, "name", "Gnu Make Builder");
    xmlTextWriterWriteAttribute(cproject_writer, "name", str);
    xmlTextWriterWriteAttribute(cproject_writer, "parallelBuildOn", "true");
    xmlTextWriterWriteAttribute(cproject_writer, "parallelizationNumber", "optimal");
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
    xmlTextWriterEndElement(cproject_writer); // builder

    xmlTextWriterStartElement(cproject_writer, "tool");
    sprintf(parent_str, TOOL_C_COMPILER_SUPERCLASS, TITLE);
    put_id(parent_str, instance->tool_gnu_cross_c_compiler);
    xmlTextWriterWriteAttribute(cproject_writer, "id", instance->tool_gnu_cross_c_compiler);
    xmlTextWriterWriteAttribute(cproject_writer, "name", "MCU GCC Compiler");
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);

    // -O
    char optimization_level = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp("-O", my_list->str, 2) == 0) {
            optimization_level = my_list->str[2];
            my_list->taken = true;
        }
    }

    if (optimization_level != 0) {
        xmlTextWriterStartElement(cproject_writer, "option");
        sprintf(parent_str, C_OPTIMIZATION_LEVEL_SUPERCLASS, TITLE);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
        xmlTextWriterWriteAttribute(cproject_writer, "name", "Optimization Level");
        xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, "useByScannerDiscovery", "false");

        get_c_optimization_value(optimization_level, str);
        if (str[0] != 0) {
            xmlTextWriterWriteAttribute(cproject_writer, "value", str);
            xmlTextWriterWriteAttribute(cproject_writer, "valueType", "enumerated");
        }

        xmlTextWriterEndElement(cproject_writer); // option 
    }

    // -g
    char debugging_level = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp("-g", my_list->str, 2) == 0) {
            debugging_level = my_list->str[2];
            my_list->taken = true;
        }
    }

    put_c_debug_level(debugging_level);

    xmlTextWriterStartElement(cproject_writer, "option");
    xmlTextWriterWriteAttribute(cproject_writer, "IS_BUILTIN_EMPTY", "false");
    xmlTextWriterWriteAttribute(cproject_writer, "IS_VALUE_EMPTY", "false");
    strcpy(parent_str, C_DEFINED_SYMBOLS_SUPERCLASS);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "name", "Defined symbols (-D)");
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, "useByScannerDiscovery", "false");
    xmlTextWriterWriteAttribute(cproject_writer, "valueType", "definedSymbols");
    if (defines_list) {
        unsigned cnt = 0;
        struct node_s *my_list;
        for (my_list = defines_list; my_list != NULL; my_list = my_list->next) {
            xmlTextWriterStartElement(cproject_writer, "listOptionValue");
            xmlTextWriterWriteAttribute(cproject_writer, "builtIn", "false");
            xmlTextWriterWriteAttribute(cproject_writer, "value", my_list->str);
            xmlTextWriterEndElement(cproject_writer); // listOptionValue 
        }
    }

    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, "option");
    xmlTextWriterWriteAttribute(cproject_writer, "IS_BUILTIN_EMPTY", "false");
    xmlTextWriterWriteAttribute(cproject_writer, "IS_VALUE_EMPTY", "false");
    strcpy(parent_str, C_INCLUDE_PATHS_SUPERCLASS);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, "useByScannerDiscovery", "false");
    xmlTextWriterWriteAttribute(cproject_writer, "valueType", "includePath");
    if (includes_list) {
        struct node_s *my_list;
        for (my_list = includes_list; my_list != NULL; my_list = my_list->next) {
            xmlTextWriterStartElement(cproject_writer, "listOptionValue");
            xmlTextWriterWriteAttribute(cproject_writer, "builtIn", "false");
            xmlTextWriterWriteAttribute(cproject_writer, "value", my_list->str);
            xmlTextWriterEndElement(cproject_writer); // listOptionValue 
        }
    }
    xmlTextWriterEndElement(cproject_writer); // option 

    // -fdata-sections
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-fdata-sections", my_list->str) == 0) {
            xmlTextWriterStartElement(cproject_writer, "option");
            sprintf(parent_str, FDATA_SECTIONS_SUPERCLASS, TITLE);
            put_id(parent_str, id_str);
            xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
            xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
            xmlTextWriterWriteAttribute(cproject_writer, "value", "true");
            xmlTextWriterWriteAttribute(cproject_writer, "valueType", "boolean");
            xmlTextWriterEndElement(cproject_writer); // option 
            my_list->taken = true;
        }
    }

    // -std=foo
    const char *dialect = NULL;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        /* go thru all args, last one takes effect */
        if (strncmp("-std=", my_list->str, 5) == 0) {
            get_c_dialect_value_from_fragment(my_list->str, &dialect);

            my_list->taken = true;
        }
    }

    if (dialect) {
        xmlTextWriterStartElement(cproject_writer, "option");
        strcpy(parent_str, C_DIALECT_SUPERCLASS);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
        xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, "useByScannerDiscovery", "true");
        strcpy(str, C_DIALECT_VALUE_PREFIX);
        strcat(str, dialect);
        xmlTextWriterWriteAttribute(cproject_writer, "value", str);
        xmlTextWriterWriteAttribute(cproject_writer, "valueType", "enumerated");
        xmlTextWriterEndElement(cproject_writer); // option 
    }

    // -pedantic
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-pedantic", my_list->str) == 0) {
            xmlTextWriterStartElement(cproject_writer, "option");
            strcpy(parent_str, C_PEDANTIC_SUPERCLASS);
            put_id(parent_str, id_str);
            xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
            xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
            xmlTextWriterWriteAttribute(cproject_writer, "value", "true");
            xmlTextWriterWriteAttribute(cproject_writer, "valueType", "boolean");
            xmlTextWriterEndElement(cproject_writer); // option 
            my_list->taken = true;
        }
    }

    // -Wextra
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-Wextra", my_list->str) == 0) {
            xmlTextWriterStartElement(cproject_writer, "option");
            strcpy(parent_str, C_WARN_EXTRA_SUPERCLASS);
            put_id(parent_str, id_str);
            xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
            xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
            xmlTextWriterWriteAttribute(cproject_writer, "value", "true");
            xmlTextWriterWriteAttribute(cproject_writer, "valueType", "boolean");
            xmlTextWriterEndElement(cproject_writer); // option 
            my_list->taken = true;
        }
    }

    // any -W -f -m not already added
    put_c_other_flags();

    xmlTextWriterStartElement(cproject_writer, "inputType");
    sprintf(parent_str, C_INPUT_C_SUPERCLASS, TITLE);
    put_id(parent_str, instance->tool_gnu_cross_c_compiler_input);
    xmlTextWriterWriteAttribute(cproject_writer, "id", instance->tool_gnu_cross_c_compiler_input);
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
    xmlTextWriterEndElement(cproject_writer); // inputType

    if (C_INPUT_S_SUPERCLASS != NULL) {
        xmlTextWriterStartElement(cproject_writer, "inputType");
        sprintf(parent_str, C_INPUT_S_SUPERCLASS, TITLE);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
        xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
        xmlTextWriterEndElement(cproject_writer); // inputType
    }

    xmlTextWriterEndElement(cproject_writer); // tool  tool.gnu.cross.c.compiler

    xmlTextWriterStartElement(cproject_writer, "tool");
    sprintf(parent_str, TOOL_CPP_COMPILER_SUPERCLASS, TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "name", "MCU G++ Compiler");
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);

    if (optimization_level != 0) {
        xmlTextWriterStartElement(cproject_writer, "option");
        sprintf(parent_str, CPP_OPTIMIZATION_LEVEL_SUPERCLASS, TITLE);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
        xmlTextWriterWriteAttribute(cproject_writer, "name", "Optimization Level");
        xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, "useByScannerDiscovery", "false");
        get_cpp_optimization_value(optimization_level, str);
        xmlTextWriterWriteAttribute(cproject_writer, "value", str);
        xmlTextWriterWriteAttribute(cproject_writer, "valueType", "enumerated");
        xmlTextWriterEndElement(cproject_writer); // option 
    }

    xmlTextWriterStartElement(cproject_writer, "option");
    put_cpp_debugging_default_value();
    get_cpp_debugging_level_superclass(parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "name", "Debug Level");
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, "useByScannerDiscovery", "false");
    xmlTextWriterWriteAttribute(cproject_writer, "valueType", "enumerated");
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterEndElement(cproject_writer); // tool

    xmlTextWriterStartElement(cproject_writer, "tool");
    sprintf(parent_str, C_LINKER_SUPERCLASS, TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "name", "MCU GCC Linker");
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);

    xmlTextWriterStartElement(cproject_writer, "option");
    sprintf(parent_str, C_LINKER_SCRIPT_SUPERCLASS, TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "name", "Linker Script (-T)");
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, "useByScannerDiscovery", "false");
    xmlTextWriterWriteAttribute(cproject_writer, "value", from_codemodel.linkerScript);
    xmlTextWriterWriteAttribute(cproject_writer, "valueType", "string");
    xmlTextWriterEndElement(cproject_writer); // option 

    put_c_link_option_flags();

    xmlTextWriterStartElement(cproject_writer, "inputType");
    strcpy(parent_str, C_LINKER_INPUT_SUPERCLASS);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
    xmlTextWriterStartElement(cproject_writer, "additionalInput");
    xmlTextWriterWriteAttribute(cproject_writer, "kind", "additionalinputdependency");
    xmlTextWriterWriteAttribute(cproject_writer, "paths", "$(USER_OBJS)");
    xmlTextWriterEndElement(cproject_writer); // additionalInput 

    xmlTextWriterStartElement(cproject_writer, "additionalInput");
    xmlTextWriterWriteAttribute(cproject_writer, "kind", "additionalinput");
    xmlTextWriterWriteAttribute(cproject_writer, "paths", "$(LIBS)");
    xmlTextWriterEndElement(cproject_writer); // additionalInput 
    xmlTextWriterEndElement(cproject_writer); // inputType

    xmlTextWriterEndElement(cproject_writer); // tool    tool.gnu.cross.c.linker

    xmlTextWriterStartElement(cproject_writer, "tool");
    sprintf(parent_str, CPP_LINKER_SUPERCLASS, TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "name", "MCU G++ Linker");
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);

    xmlTextWriterStartElement(cproject_writer, "option");
    sprintf(parent_str, CPP_LINKER_SCRIPT_SUPERCLASS, TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "name", "Linker Script (-T)");
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, "useByScannerDiscovery", "false");
    xmlTextWriterWriteAttribute(cproject_writer, "value", from_codemodel.linkerScript);
    xmlTextWriterWriteAttribute(cproject_writer, "valueType", "string");
    xmlTextWriterEndElement(cproject_writer); // option

    put_cpp_link_option_flags();

    xmlTextWriterStartElement(cproject_writer, "inputType");
    strcpy(parent_str, CPP_LINKER_INPUT_SUPERCLASS);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);

    xmlTextWriterStartElement(cproject_writer, "additionalInput");
    xmlTextWriterWriteAttribute(cproject_writer, "kind", "additionalinputdependency");
    xmlTextWriterWriteAttribute(cproject_writer, "paths", "$(USER_OBJS)");
    xmlTextWriterEndElement(cproject_writer); // additionalInput 

    xmlTextWriterStartElement(cproject_writer, "additionalInput");
    xmlTextWriterWriteAttribute(cproject_writer, "kind", "additionalinput");
    xmlTextWriterWriteAttribute(cproject_writer, "paths", "$(LIBS)");
    xmlTextWriterEndElement(cproject_writer); // additionalInput 

    xmlTextWriterEndElement(cproject_writer); // inputType
    xmlTextWriterEndElement(cproject_writer); // tool      tool.gnu.cross.cpp.linker

    xmlTextWriterStartElement(cproject_writer, "tool");
    sprintf(parent_str, ARCHIVER_SUPERCLASS, TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "name", "MCU GCC Archiver");
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
    xmlTextWriterEndElement(cproject_writer); // tool

    xmlTextWriterStartElement(cproject_writer, "tool");
    if (ASSEMBLER_SUPERCLASS)
        sprintf(parent_str, ASSEMBLER_SUPERCLASS, TITLE);
    else
        get_assembler_superclass(debugBuild, parent_str);

    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "name", "MCU GCC Assembler");
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);

    put_assembler_options(debugging_level);

    xmlTextWriterStartElement(cproject_writer, "inputType");
    sprintf(parent_str, ASSEMBLER_INPUT_SUPERCLASS, TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
    xmlTextWriterEndElement(cproject_writer); // inputType

    xmlTextWriterEndElement(cproject_writer); // tool   tool.gnu.cross.assembler.exe.*

    xmlTextWriterEndElement(cproject_writer); // toolChain
    xmlTextWriterEndElement(cproject_writer); // folderInfo

    xmlTextWriterStartElement(cproject_writer, "sourceEntries");
    xmlTextWriterStartElement(cproject_writer, "entry");
    xmlTextWriterWriteAttribute(cproject_writer, "excluding", "CMakeFiles");
    xmlTextWriterWriteAttribute(cproject_writer, "flags", "VALUE_WORKSPACE_PATH|RESOLVED");
    xmlTextWriterWriteAttribute(cproject_writer, "kind", "sourcePath");
    xmlTextWriterWriteAttribute(cproject_writer, "name", "");
    xmlTextWriterEndElement(cproject_writer); // entry 
    xmlTextWriterEndElement(cproject_writer); // sourceEntries

    xmlTextWriterEndElement(cproject_writer); // configuration
    xmlTextWriterEndElement(cproject_writer); // storageModule for moduleId=cdtBuildSystem

    xmlTextWriterStartElement(cproject_writer, "storageModule");
    xmlTextWriterWriteAttribute(cproject_writer, "moduleId", "org.eclipse.cdt.core.externalSettings");
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterEndElement(cproject_writer); // cconfiguration
}

int cproject_start(bool force)
{
    struct stat st;
    const char *xml_filename = ".cproject";
    int ret;

    if (!force && stat(xml_filename, &st) == 0) {
        printf("%s already exists ", xml_filename);
        return -EEXIST;
    }

    cproject_writer = xmlNewTextWriterFilename(xml_filename, 0);
    if (cproject_writer == NULL) {
        printf("cannot create xml writer\r\n");
        return -1;
    }
    ret = xmlTextWriterStartDocument(cproject_writer, NULL, "UTF-8", NULL);
    if (ret < 0) {
        printf("Error at xmlTextWriterStartDocument\n");
        return ret;
    }
    ret = xmlTextWriterSetIndent(cproject_writer, 1);
    if (ret < 0) {
        printf("Error at xmlTextWriterSetIndent\n");
        return ret;
    }
    xmlTextWriterSetIndentString(cproject_writer, "\t");

    xmlTextWriterWritePI(cproject_writer, "fileVersion", "4.0.0");
    xmlTextWriterStartElement(cproject_writer, "cproject");
    xmlTextWriterWriteAttribute(cproject_writer, "storage_type_id", "org.eclipse.cdt.core.XmlProjectDescriptionStorage");

    xmlTextWriterStartElement(cproject_writer, "storageModule");
    xmlTextWriterWriteAttribute(cproject_writer, "moduleId", "org.eclipse.cdt.core.settings");
    instance_t debugInstance, releaseInstance;
    put_cconfiguration(true, &debugInstance);
    put_cconfiguration(false, &releaseInstance);

    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, "storageModule");
    xmlTextWriterWriteAttribute(cproject_writer, "moduleId", "cdtBuildSystem");
    xmlTextWriterWriteAttribute(cproject_writer, "version", "4.0.0");
    xmlTextWriterStartElement(cproject_writer, "project");
    put_cdt_project();
    xmlTextWriterEndElement(cproject_writer); // project
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, "storageModule");
    xmlTextWriterWriteAttribute(cproject_writer, "moduleId", "scannerConfiguration");
    xmlTextWriterStartElement(cproject_writer, "autodiscovery");
    xmlTextWriterWriteAttribute(cproject_writer, "enabled", "true");
    xmlTextWriterWriteAttribute(cproject_writer, "problemReportingEnabled", "");
    xmlTextWriterWriteAttribute(cproject_writer, "selectedProfileId", "");
    xmlTextWriterEndElement(cproject_writer); // autodiscovery

    xmlTextWriterStartElement(cproject_writer, "scannerConfigBuildInfo");
    {
        unsigned len = 12;  // semicolons
        len += strlen(debugInstance.config_gnu_cross_exe);
        len += strlen(debugInstance.config_gnu_cross_exe);
        len += strlen(debugInstance.tool_gnu_cross_c_compiler);
        len += strlen(debugInstance.tool_gnu_cross_c_compiler_input);
        char *str = malloc(len);
        strcpy(str, debugInstance.config_gnu_cross_exe);
        strcat(str, ";");
        strcat(str, debugInstance.config_gnu_cross_exe);
        strcat(str, ".;");
        strcat(str, debugInstance.tool_gnu_cross_c_compiler);
        strcat(str, ";");
        strcat(str, debugInstance.tool_gnu_cross_c_compiler_input);
        xmlTextWriterWriteAttribute(cproject_writer, "instanceId", str);
        free(str);
    }
    xmlTextWriterStartElement(cproject_writer, "autodiscovery");
    xmlTextWriterWriteAttribute(cproject_writer, "enabled", "true");
    xmlTextWriterWriteAttribute(cproject_writer, "problemReportingEnabled", "");
    xmlTextWriterWriteAttribute(cproject_writer, "selectedProfileId", "");
    xmlTextWriterEndElement(cproject_writer); // autodiscovery
    xmlTextWriterEndElement(cproject_writer); // scannerConfigBuildInfo 

    xmlTextWriterStartElement(cproject_writer, "scannerConfigBuildInfo");
    {
        unsigned len = 12;  // semicolons
        len += strlen(releaseInstance.config_gnu_cross_exe);
        len += strlen(releaseInstance.config_gnu_cross_exe);
        len += strlen(releaseInstance.tool_gnu_cross_c_compiler);
        len += strlen(releaseInstance.tool_gnu_cross_c_compiler_input);
        char *str = malloc(len);
        strcpy(str, releaseInstance.config_gnu_cross_exe);
        strcat(str, ";");
        strcat(str, releaseInstance.config_gnu_cross_exe);
        strcat(str, ".;");
        strcat(str, releaseInstance.tool_gnu_cross_c_compiler);
        strcat(str, ";");
        strcat(str, releaseInstance.tool_gnu_cross_c_compiler_input);
        xmlTextWriterWriteAttribute(cproject_writer, "instanceId", str);
        free(str);
    }

    xmlTextWriterStartElement(cproject_writer, "autodiscovery");
    xmlTextWriterWriteAttribute(cproject_writer, "enabled", "true");
    xmlTextWriterWriteAttribute(cproject_writer, "problemReportingEnabled", "");
    xmlTextWriterWriteAttribute(cproject_writer, "selectedProfileId", "");
    xmlTextWriterEndElement(cproject_writer); // autodiscovery
    xmlTextWriterEndElement(cproject_writer); // scannerConfigBuildInfo 

    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, "storageModule");
    xmlTextWriterWriteAttribute(cproject_writer, "moduleId", "org.eclipse.cdt.core.LanguageSettingsProviders");
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, "storageModule");
    xmlTextWriterWriteAttribute(cproject_writer, "moduleId", "refreshScope");
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, "storageModule");
    xmlTextWriterWriteAttribute(cproject_writer, "moduleId", "org.eclipse.cdt.make.core.buildtargets");
    xmlTextWriterEndElement(cproject_writer); // storageModule

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

