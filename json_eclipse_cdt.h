#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <libxml/xmlwriter.h>
#include <json-c/json.h>
#include <libgen.h>
#include <stdbool.h>

#include "xml.h"

typedef struct {
    char config_gnu_cross_exe[96];
    char tool_gnu_cross_c_compiler[96];
    char tool_gnu_cross_c_compiler_input[96];
} instance_t;

typedef struct {
    char board[64];
} from_cache_t;

typedef struct {
    char project_name[64];
    char artifactName[96];
    char buildPath[96];
    struct node_s *compile_fragment_list;
    struct node_s *linker_fragment_list;
} from_codemodel_t;

struct node_s {
    char *str;
    bool taken;
    struct node_s *next;
};

extern from_codemodel_t from_codemodel;
extern struct node_s *defines_list;
extern struct node_s *includes_list;
extern from_cache_t from_cache;

extern xmlTextWriterPtr project_writer;
extern xmlTextWriterPtr cproject_writer;

extern const char * const GENMAKEBUILDER_TRIGGERS;
extern const char * const GENMAKEBUILDER_ARGUMENTS;
extern const char * const SCANNERCONFIGBUILDER_TRIGGERS;
extern const char * const SCANNERCONFIGBUILDER_ARGUMENTS;
extern const char * const CDT_CORE_SETTINGS_CONFIGRELATIONS;
extern const char * const BINARY_PARSER;
extern const char * const ERROR_PARSERS[];

int project_start(bool);
int cproject_start(bool);
void free_lists(void);
void get_us(char *dest);
void put_id(const char *in, char *out);

/* callbacks: */
void write_natures(void);
int get_cconfiguration_id(bool, const char*, char*, char*);
int put_configuration(bool, instance_t*, const char *, const char *, const char *);
void put_listOptionValue(xmlTextWriterPtr, bool, const char *);
void put_other_storageModules(const instance_t *, const instance_t *);
int cproject_init(void);



