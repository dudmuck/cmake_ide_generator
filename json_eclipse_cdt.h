#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <json-c/json.h>
#include <libgen.h>
#include <stdbool.h>

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
    char linkerScript[192];  // full path, absolute path
    struct node_s *specs_list;
    struct node_s *compile_fragment_list;
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

extern const char * const TITLE;
extern const char * const CONFIGURATION_EXE_SUPERCLASS;
extern const char * const TOOLCHAIN_SUPERCLASS; // appended with build (i.e. release / debug)
extern const char * const OPTION_MCU_SUPERCLASS;
extern const char * const OPTION_BOARD_SUPERCLASS;
extern const char * const TARGET_PLATFORM_SUPERCLASS;
extern const char * const BUILDER_SUPERCLASS;
extern const char * const TOOL_C_COMPILER_SUPERCLASS;
extern const char * const TOOL_CPP_COMPILER_SUPERCLASS;
extern const char * const C_OPTIMIZATION_LEVEL_SUPERCLASS;
extern const char * const CPP_OPTIMIZATION_LEVEL_SUPERCLASS;
extern const char * const FDATA_SECTIONS_SUPERCLASS;
extern const char * const C_INPUT_C_SUPERCLASS;
extern const char * const C_INPUT_S_SUPERCLASS;
extern const char * const C_LINKER_SUPERCLASS;
extern const char * const CPP_LINKER_SUPERCLASS;
extern const char * const C_LINKER_SCRIPT_SUPERCLASS;
extern const char * const CPP_LINKER_SCRIPT_SUPERCLASS;
extern const char * const ARCHIVER_SUPERCLASS;
extern const char * const ASSEMBLER_SUPERCLASS;
extern const char * const ASSEMBLER_INPUT_SUPERCLASS;
extern const char * const TOOLCHAIN_NAME;
extern const char * const C_DEFINED_SYMBOLS_SUPERCLASS;
extern const char * const C_DIALECT_SUPERCLASS;
extern const char * const C_DIALECT_VALUE_PREFIX;
extern const char * const C_PEDANTIC_SUPERCLASS;
extern const char * const C_WARN_EXTRA_SUPERCLASS;
extern const char * const C_INCLUDE_PATHS_SUPERCLASS;
extern const char * const C_LINKER_INPUT_SUPERCLASS;
extern const char * const CPP_LINKER_INPUT_SUPERCLASS;

int parse_codemodel_to_eclipse_project(const char *jsonFileName);
int project_start(bool);
int cproject_start(bool);
void free_lists(void);
void get_us(char *dest);
void put_id(const char *in, char *out);

void translate_board_mcu(const char *board_in, char *board_out, char *mcu_out);

/* callbacks: */
void write_natures(void );
void put_ScannerConfigBuilder_triggers(void);
void translate_board_mcu(const char*, char*, char*);
void get_c_optimization_value(char, char *);
void get_cpp_optimization_value(char, char *);
void get_assembler_superclass(bool, char *);
void put_cdt_project(void);
void get_bulder_name(char *, const char *);
void get_cpp_debugging_level_superclass(char *);
void put_post_build_steps(const char *);
void put_assembler_options(void);
void put_c_debug_level(char);
void put_c_other_flags(void);
void get_c_dialect_value_from_fragment(const char *in, const char **out);
void put_cpp_debugging_default_value(void);
void put_cpp_link_option_flags(void);
void put_c_link_option_flags(void);
void put_toolchain_type(void);
void put_toolchain_version(void);
void put_target_id(void);

