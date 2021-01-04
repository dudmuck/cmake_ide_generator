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

#define INSTANCE_MAX_STRLEN     384

struct node_s {
    char *str;
    const void *arg;
    bool taken;
    struct node_s *next;
};

typedef struct {
    char *board;
    struct node_s *c_flags_debug;
    struct node_s *c_flags_release;
    struct node_s *cxx_flags_debug;
    struct node_s *cxx_flags_release;
    struct node_s *asm_flags_debug;
    struct node_s *asm_flags_release;
} from_cache_t;

typedef struct {
    char project_name[64];
    char artifactName[96];
    char buildPath[96];
    struct node_s *compile_fragment_list;
    struct node_s *linker_fragment_list;
} from_codemodel_t;


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
extern const char * const BINARY_PARSERS[];
extern const char * const ERROR_PARSERS[];

extern bool cpp_input; // false=c-project, true=cpp-project
extern bool cpp_linker; // true=linking done by c++ compiler
extern char linker_script[];
extern const char *build;
extern const char *Build;
extern struct node_s *c_flags;  // from CMAKE_C_FLAGS_<build>
extern struct node_s *cpp_flags;    // from CMAKE_CXX_FLAGS_<build>

int project_start(bool);
int cproject_start(bool);
void free_lists(void);
void get_us(char *dest);
void put_id(const char *in, char *out);

/* callbacks: */
void write_natures_(void);
int get_cconfiguration_id(bool, const char*, char*, char*);
struct node_s *add_instance(const char *ccfg_id);
int _put_configuration(bool, const char *, const char *, const char *, const char *, struct node_s *);
void put_listOptionValue(xmlTextWriterPtr, bool, const char *);
void put_additionalInput(xmlTextWriterPtr, const char*, const char *);
void put_other_storageModules(void);
int cproject_init(void);
void put_project_other_builders(void);
void put_scannerConfiguration(const char *is_problemReportingEnabled);
void put_other_cconfiguration_storageModules(bool);
int unbuilt_source(const char *, const char *);
void cat_additional_exclude_directories(char *);


