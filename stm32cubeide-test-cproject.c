#include <string.h>
#include <stdbool.h>
#include <libxml/xmlreader.h>
#include "xml.h"

const char * const TITLE = "com.st.stm32cube.ide.mcu.gnu.managedbuild";
const char * const TITLE_OPTION = "com.st.stm32cube.ide.mcu.option";    // only for toolchain.version?
const char * const settings = "org.eclipse.cdt.core.settings";
const char * const LanguageSettingsProviders = "org.eclipse.cdt.core.LanguageSettingsProviders";
const char * const string = "string";
const char * const stringList = "stringList";
const char * const boolean = "boolean";
const char * const enumerated = "enumerated";
const char * const extensions = "extensions";
const char * const extension = "extension";
const char * const folderInfo = "folderInfo";
const char * const toolChain = "toolChain";
const char * const refreshScope = "refreshScope";
const char * const storageModule = "storageModule";
const char * const tool = "tool";
const char * const useByScannerDiscovery = "useByScannerDiscovery";
const char * const targetPlatform = "targetPlatform";
const char * const builder = "builder";
const char * const entry = "entry";
const char * const option = "option";
const char * const inputType = "inputType";
const char * const listOptionValue = "listOptionValue";
const char * const additionalInput = "additionalInput";
const char * const toolchain_exe = "toolchain.exe"; // was toolchain_gnu_cross_exe 
const char * const internal_toolchain_type = "internal.toolchain.type";
const char * const internal_toolchain_version = "internal.toolchain.version";
const char * const option_target_mcu = "option.target_mcu";
const char * const option_target_cpuid = "option.target_cpuid";
const char * const option_target_coreid = "option.target_coreid";
const char * const option_fpu = "option.fpu";
const char * const option_floatabi = "option.floatabi";
const char * const option_target_board = "option.target_board";
const char * const option_defaults = "option.defaults";
const char * const tool_assembler = "tool.assembler";
const char * const tool_assembler_option_debuglevel = "tool.assembler.option.debuglevel";
const char * const tool_assembler_option_suppresswarnings = "tool.assembler.option.suppresswarnings";
const char * const tool_assembler_option_preprocessassembler = "tool.assembler.option.preprocessassembler";
const char * const tool_assembler_option_nostdinc = "tool.assembler.option.nostdinc";
const char * const tool_assembler_option_preprocessonly = "tool.assembler.option.preprocessonly";
const char * const tool_assembler_option_includepaths = "tool.assembler.option.includepaths";
const char * const tool_assembler_option_includefiles = "tool.assembler.option.includefiles";
const char * const tool_assembler_option_otherflags = "tool.assembler.option.otherflags";
const char * const tool_assembler_input = "tool.assembler.input";
const char * const tool_c_compiler = "tool.c.compiler";

const char * const tool_c_compiler_option = "tool.c.compiler.option";
const char * const tool_c_compiler_input_c= "tool.c.compiler.input.c";
const char * const tool_cpp_compiler = "tool.cpp.compiler";
const char * const tool_cpp_compiler_option_debuglevel = "tool.cpp.compiler.option.debuglevel";
const char * const tool_cpp_compiler_option_optimization_level = "tool.cpp.compiler.option.optimization.level";
const char * const tool_c_linker = "tool.c.linker";
const char * const tool_c_linker_option = "tool.c.linker.option";
const char * const tool_c_linker_input = "tool.c.linker.input";
const char * const tool_cpp_linker = "tool.cpp.linker";
const char * const tool_cpp_linker_option_script = "tool.cpp.linker.option.script";
const char * const tool_cpp_linker_input = "tool.cpp.linker.input";
const char * const tool_archiver = "tool.archiver";
const char * const tool_size = "tool.size";
const char * const tool_objdump_listfile = "tool.objdump.listfile";
const char * const tool_objcopy_hex = "tool.objcopy.hex";
const char * const tool_objcopy_binary = "tool.objcopy.binary";
const char * const tool_objcopy_verilog = "tool.objcopy.verilog";
const char * const tool_objcopy_srec = "tool.objcopy.srec";
const char * const tool_objcopy_symbolsrec = "tool.objcopy.symbolsrec";
const char * const core_externalSettings = "org.eclipse.cdt.core.externalSettings";
const char * const org_eclipse_cdt_core_BinaryParser = "org.eclipse.cdt.core.BinaryParser";
const char * const org_eclipse_cdt_core_ErrorParser = "org.eclipse.cdt.core.ErrorParser";
const char * const org_eclipse_cdt_core_ELF = "org.eclipse.cdt.core.ELF";
const char * const org_eclipse_cdt_core_GASErrorParser = "org.eclipse.cdt.core.GASErrorParser";
const char * const org_eclipse_cdt_core_GmakeErrorParser = "org.eclipse.cdt.core.GmakeErrorParser";
const char * const org_eclipse_cdt_core_GLDErrorParser = "org.eclipse.cdt.core.GLDErrorParser";
const char * const org_eclipse_cdt_core_CWDLocator = "org.eclipse.cdt.core.CWDLocator";
const char * const org_eclipse_cdt_core_GCCErrorParser = "org.eclipse.cdt.core.GCCErrorParser";
const char * const buildtargets = "org.eclipse.cdt.make.core.buildtargets";
const char * const cdtBuildSystem = "cdtBuildSystem";
const char * const scannerConfiguration = "scannerConfiguration";
const char * const scannerConfigBuildInfo = "scannerConfigBuildInfo";
const char * const autodiscovery = "autodiscovery";
const char * const cconfiguration = "cconfiguration";
const char * const configuration = "configuration";
const char * const sourceEntries = "sourceEntries";
const char * const configurationDataProvider = "org.eclipse.cdt.managedbuilder.core.configurationDataProvider";
const char * const externalSettings = "externalSettings";


struct node_s {
    void *ptr;
    struct node_s *next;
};

typedef struct {
    char config_gnu_cross_exe[96];
    char config_gnu_cross_exe_folderInfo[96];
    char tool_gnu_cross_c_compiler[96];
    char tool_gnu_cross_c_compiler_input[96];
    bool scannerInfo;
} instance_t;
struct node_s *instance_list;

typedef union {
    struct {
        uint32_t buildtargets              : 1;    // 0
        uint32_t settings                  : 1;    // 1
        uint32_t cdtBuildSystem            : 1;    // 2
        uint32_t scannerConfiguration      : 1;    // 3
        uint32_t LanguageSettingsProviders : 1;    // 4
    } bits;
    uint32_t dword;
} cproject_has_t;

typedef union {
    struct {
        uint64_t configurationDataProvider                   : 1;    // 0
        uint64_t settings                                    : 1;    // 1
        uint64_t externalSettings                            : 1;    // 2
        uint64_t core_externalSettings                       : 1;    // 3
        uint64_t extensions                                  : 1;    // 4
        uint64_t folderInfo                                  : 1;    // 5
        uint64_t GASErrorParser                              : 1;    // 6
        uint64_t GmakeErrorParser                            : 1;    // 7
        uint64_t CWDLocator                                  : 1;    // 8
        uint64_t GCCErrorParser                              : 1;    // 9
        uint64_t GLDErrorParser                              : 1;    // 10
        uint64_t targetPlatform                              : 1;    // 11
        uint64_t builder                                     : 1;    // 12
        uint64_t internal_toolchain_type                     : 1;    // 13
        uint64_t internal_toolchain_version                  : 1;    // 14
        uint64_t option_target_mcu                           : 1;    // 15
        uint64_t option_target_cpuid                         : 1;    // 16
        uint64_t option_target_coreid                        : 1;    // 17
        uint64_t option_target_board                         : 1;    // 18
        uint64_t tool_assembler                              : 1;    // 19
        uint64_t tool_assembler_option_debuglevel            : 1;    // 20
        uint64_t tool_assembler_input                        : 1;    // 21
        uint64_t tool_c_compiler                             : 1;    // 22
        uint64_t tool_c_compiler_option_debuglevel           : 1;    // 23
        uint64_t tool_c_compiler_option_optimization_level   : 1;    // 24
        uint64_t tool_c_compiler_option_definedsymbols       : 1;    // 25
        uint64_t tool_c_compiler_option_includepaths         : 1;    // 26
        uint64_t tool_c_compiler_input_c                     : 1;    // 27
        uint64_t tool_cpp_compiler                           : 1;    // 28
        uint64_t tool_cpp_compiler_option_debuglevel         : 1;    // 29
        uint64_t tool_cpp_compiler_option_optimization_level : 1;    // 30
        uint64_t tool_c_linker                               : 1;    // 31
        uint64_t tool_c_linker_option_script                 : 1;    // 32
        uint64_t tool_c_linker_input                         : 1;    // 33
        uint64_t tool_cpp_linker                             : 1;    // 34
        uint64_t tool_cpp_linker_option_script               : 1;    // 35
        uint64_t tool_archiver                               : 1;    // 36
    } bits;
    uint64_t dword;
} cconfiguration_has_t;

cproject_has_t cproject_has;

typedef enum {
    /*  0 */ CPROJECT_STATE_NONE = 0,
    /*  1 */ CPROJECT_STATE_CPROJECT,
    /*  2 */ CPROJECT_STATE_STORAGEMODULE_SETTINGS,
    /*  3 */ CPROJECT_STATE_STORAGEMODULE_CDTBUILDSYSTEM,
    /*  4 */ CPROJECT_STATE_STORAGEMODULE_SCANNERCONFIGURATION,
    /*  5 */ CPROJECT_STATE_EXTENSIONS,
    /*  6 */ CPROJECT_STATE_CONFIGURATION,
    /*  7 */ CPROJECT_STATE_FOLDERINFO,
    /*  8 */ CPROJECT_STATE_TOOLCHAIN_EXE,
    /*  9 */ CPROJECT_STATE_SOURCEENTRIES,
    /*  9 */ CPROJECT_STATE_TOOL_ASSEMBLER,
    /* 10 */ CPROJECT_STATE_TOOL_ASSEMBLER_OPTION_DEBUGLEVEL,
    /* 11 */ CPROJECT_STATE_TOOL_ASSEMBLER_INPUT,
    /* 12 */ CPROJECT_STATE_TOOL_C_COMPILER,
    /* 13 */ CPROJECT_STATE_TOOL_C_COMPILER_OPTION_DEBUGLEVEL,
    /* 14 */ CPROJECT_STATE_TOOL_C_COMPILER_OPTION_OPTIMIZATION_LEVEL,
    /* 19 */ CPROJECT_STATE_TOOL_ASSEMBLER_OPTION_OTHERFLAGS_,
    /* 20 */ CPROJECT_STATE_TOOL_C_COMPILER_INPUT_C,
    /* 21 */ CPROJECT_STATE_TOOL_CPP_COMPILER,
    /* 22 */ CPROJECT_STATE_TOOL_CPP_COMPILER_OPTION_DEBUGLEVEL,
    /* 23 */ CPROJECT_STATE_TOOL_CPP_COMPILER_OPTION_OPTIMIZATION_LEVEL,
    /* 24 */ CPROJECT_STATE_TOOL_C_LINKER,
    /* 25 */ CPROJECT_STATE_TOOL_C_LINKER_OPTION_SCRIPT,
    /* 26 */ CPROJECT_STATE_TOOL_LINKER_INPUT,  /* C and CPP */
    /* 27 */ CPROJECT_STATE_TOOL_CPP_LINKER,
    /* 28 */ CPROJECT_STATE_TOOL_CPP_LINKER_OPTION_SCRIPT,
    /* 29 */ CPROJECT_STATE_TOOL_ARCHIVER,
    /* 30 */ CPROJECT_STATE_TOOL_SIZE,
    /* 31 */ CPROJECT_STATE_TOOL_OBJDUMP_LISTFILE,
    /* 32 */ CPROJECT_STATE_TOOL_OBJCOPY_HEX,
    /* 33 */ CPROJECT_STATE_TOOL_OBJCOPY_BINARY,
    /* 34 */ CPROJECT_STATE_TOOL_OBJCOPY_VERILOG,
    /* 35 */ CPROJECT_STATE_TOOL_OBJCOPY_SREC,
    /* 36 */ CPROJECT_STATE_TOOL_OBJCOPY_SYMBOLSREC,
    /* 37 */ CPROJECT_STATE_SCANNERCONFIGBUILDINFO,
    /* 38 */ CPROJECT_STATE_LISTOPTIONVALUE,
} cproject_state_e;

#define CPROJECT_DEPTH       12
cproject_state_e cproject_state[CPROJECT_DEPTH];
char storageModuleId[CPROJECT_DEPTH][128];

void show_cconfiguration_missing(cconfiguration_has_t ch)
{
    printf("missing ");
    if (!ch.bits.configurationDataProvider)
        printf("configurationDataProvider ");
    if (!ch.bits.settings)
        printf("settings ");
    if (!ch.bits.externalSettings)
        printf("externalSettings ");
    if (!ch.bits.core_externalSettings)
        printf("core_externalSettings ");
    if (!ch.bits.extensions)
        printf("extensions ");
    if (!ch.bits.folderInfo)
        printf("folderInfo ");
    if (!ch.bits.GASErrorParser)
        printf("GASErrorParser ");
    if (!ch.bits.GmakeErrorParser)
        printf("GmakeErrorParser ");
    if (!ch.bits.CWDLocator)
        printf("CWDLocator ");
    if (!ch.bits.GCCErrorParser)
        printf("GCCErrorParser ");
    if (!ch.bits.GLDErrorParser)
        printf("GLDErrorParser ");
    if (!ch.bits.targetPlatform)
        printf("targetPlatform ");
    if (!ch.bits.builder)
        printf("builder ");
    if (!ch.bits.internal_toolchain_type)
        printf("internal_toolchain_type ");
    if (!ch.bits.internal_toolchain_version)
        printf("internal_toolchain_version ");
    if (!ch.bits.option_target_mcu)
        printf("option_target_mcu ");
    if (!ch.bits.option_target_cpuid)
        printf("option_target_cpuid ");
    if (!ch.bits.option_target_coreid)
        printf("option_target_coreid ");
    if (!ch.bits.option_target_board)
        printf("option_target_board ");
    if (!ch.bits.tool_assembler)
        printf("tool_assembler ");
    if (!ch.bits.tool_assembler_option_debuglevel)
        printf("tool_assembler_option_debuglevel ");
    if (!ch.bits.tool_assembler_input)
        printf("tool_assembler_input ");
    if (!ch.bits.tool_c_compiler)
        printf("tool_c_compiler ");
    if (!ch.bits.tool_c_compiler_option_debuglevel)
        printf("tool_c_compiler_option_debuglevel ");
    if (!ch.bits.tool_c_compiler_option_optimization_level)
        printf("tool_c_compiler_option_optimization_level ");
    if (!ch.bits.tool_c_compiler_option_definedsymbols)
        printf("tool_c_compiler_option_definedsymbols ");
    if (!ch.bits.tool_c_compiler_option_includepaths)
        printf("tool_c_compiler_option_includepaths ");
    if (!ch.bits.tool_c_compiler_input_c)
        printf("tool_c_compiler_input_c ");
    if (!ch.bits.tool_cpp_compiler)
        printf("tool_cpp_compiler ");
    if (!ch.bits.tool_cpp_compiler_option_debuglevel)
        printf("tool_cpp_compiler_option_debuglevel ");
    if (!ch.bits.tool_cpp_compiler_option_optimization_level)
        printf("tool_cpp_compiler_option_optimization_level ");
    if (!ch.bits.tool_c_linker)
        printf("tool_c_linker ");
    if (!ch.bits.tool_c_linker_option_script)
        printf("tool_c_linker_option_script ");
    if (!ch.bits.tool_c_linker_input)
        printf("tool_c_linker_input ");
    if (!ch.bits.tool_cpp_linker)
        printf("tool_cpp_linker ");
    if (!ch.bits.tool_cpp_linker_option_script)
        printf("tool_cpp_linker_option_script ");
    if (!ch.bits.tool_archiver)
        printf("tool_archiver ");

    printf("\n");
}

static int processNode(xmlTextReaderPtr reader)
{
    static instance_t *current_instance = NULL;
    static cconfiguration_has_t cconfiguration_has;
    static char cconfiguration_name[32];
    static bool in_cconfiguration = false;
    int ret = 0;
    const char *name, *value = NULL;
    int depth, nodeType = xmlTextReaderNodeType(reader);
    if (nodeType == XML_NODE_TYPE_WHITE_SPACE)
        return 0;   // white space

    name = (char*)xmlTextReaderConstName(reader);
    depth = xmlTextReaderDepth(reader);

    printf("depth%d:%d", depth, cproject_state[depth]);
    if (depth > 0)
        printf(":%d ", cproject_state[depth-1]);

    printf(" %s \"%s\" %s %s", 
	    nodeTypeToString(nodeType),
	    name,
	    xmlTextReaderIsEmptyElement(reader) ? "empty" : " - ",
	    xmlTextReaderHasValue(reader) ? "value" : " - "
    );

    if (xmlTextReaderHasValue(reader)) {
        value = (char*)xmlTextReaderConstValue(reader);
        printf(" %s ", value);
    }

    if (nodeType == XML_NODE_TYPE_START_ELEMENT) {
        char *attr_id = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"id");
        char *attr_superClass = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"superClass");
        char *attr_name = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"name");
        char *attr_valueType = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"valueType");
        char *attr_value = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"value");
        char *attr_useByScannerDiscovery = NULL;
        char *attr_IS_BUILTIN_EMPTY = NULL;
        char *attr_IS_VALUE_EMPTY = NULL;

        if (strcmp(name, option) == 0) {
            attr_useByScannerDiscovery = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)useByScannerDiscovery);
            attr_IS_BUILTIN_EMPTY = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"IS_BUILTIN_EMPTY");
            attr_IS_VALUE_EMPTY = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"IS_VALUE_EMPTY");
        }

        if (attr_valueType) {
            if (strcmp(attr_valueType, boolean) == 0) {
                if (strcmp(attr_value, "true") != 0 && strcmp(attr_value, "false") != 0) {
                    printf("\e[31mline %d boolean not true or false\e[0m ", __LINE__);
                    ret = -1;
                }
            } else if (strcmp(attr_valueType, string) != 0 &&
                       strcmp(attr_valueType, stringList) != 0 &&
                       strcmp(attr_valueType, "userObjs") != 0 &&
                       strcmp(attr_valueType, "includePath") != 0 &&
                       strcmp(attr_valueType, "includeFiles") != 0 &&
                       strcmp(attr_valueType, "definedSymbols") != 0 &&
                       strcmp(attr_valueType, "undefDefinedSymbols") != 0 &&
                       strcmp(attr_valueType, "libs") != 0 &&
                       strcmp(attr_valueType, "libPaths") != 0 &&
                       strcmp(attr_valueType, enumerated) != 0)

            {
                printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                ret = -1;
            }
        }

        if (depth > 0 && cproject_state[depth-1] == CPROJECT_STATE_LISTOPTIONVALUE) {
            printf("line %d LISTOPTIONVALUE-%s %s ", __LINE__, name, attr_superClass);
            if (strcmp(name, listOptionValue) == 0) {
                char *attr_builtIn = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"builtIn");
                if (!attr_builtIn ) {
                    printf("\e[31mline %d listOptionValue-no-builtIn\e[0m ", __LINE__);
                    ret = -1;
                }
                if (!attr_value) {
                    printf("\e[31mline %d listOptionValue-no-value\e[0m ", __LINE__);
                    ret = -1;
                }
                printf(" * ");
                free(attr_builtIn);
            } else {
                printf("\e[31mline %d unknown name %s\e[0m ", __LINE__, name);
                ret = -1;
            }
        }

        if (attr_id && attr_superClass) {
            if (strncmp(attr_superClass, attr_id, strlen(attr_superClass)) != 0) {
                printf("\nline %d superClass:\n%s\n%s\n\n", __LINE__, attr_id, attr_superClass);
                ret = -1;
            }
        }

        if (attr_superClass) {
            if (strncmp(attr_superClass, TITLE, strlen(TITLE)) != 0 &&
                strncmp(attr_superClass, TITLE_OPTION, strlen(TITLE_OPTION)) != 0) {
                printf("\e[31mline %d superClass\n%s\n%s\e[0m\n ", __LINE__, attr_superClass, TITLE);
                ret = -1;
            }
        }

        if (strcmp(storageModule, name) == 0) {
            char *attr_moduleId = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"moduleId");
            char *attr_buildSystemId = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"buildSystemId");
            strcpy(storageModuleId[depth], attr_moduleId);
            printf(" \e[33mstart-%s\e[0m ", storageModuleId[depth]);

            if (strcmp(attr_moduleId, LanguageSettingsProviders) == 0) {
                if (depth == 1) /* only on depth1 */
                    cproject_has.bits.LanguageSettingsProviders = 1;
            } else if (strcmp(attr_moduleId, scannerConfiguration) == 0) {
                cproject_state[depth] = CPROJECT_STATE_STORAGEMODULE_SCANNERCONFIGURATION;
                if (depth == 1) /* only on depth1 */
                    cproject_has.bits.scannerConfiguration = 1;
            } else if (strcmp(attr_moduleId, buildtargets) == 0) {
                if (depth == 1) /* only on depth1 */
                    cproject_has.bits.buildtargets = 1;
            } else if (strcmp(attr_moduleId, settings) == 0) {
                cproject_state[depth] = CPROJECT_STATE_STORAGEMODULE_SETTINGS;
                if (depth == 1) /* depth1 recording outside of cconfiguration */
                    cproject_has.bits.settings = 1;
                if (in_cconfiguration)
                    cconfiguration_has.bits.settings = 1;
            } else if (strcmp(attr_moduleId, core_externalSettings) == 0) {
                cconfiguration_has.bits.core_externalSettings = 1;
            } else if (strcmp(attr_moduleId, cdtBuildSystem) == 0) {
                char *version = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"version");
                cproject_state[depth] = CPROJECT_STATE_STORAGEMODULE_CDTBUILDSYSTEM;
                if (strcmp(version, "4.0.0") != 0) {
                    printf("\e[31mline %d wrong cdtBuildSystem version %s\e[0m ", __LINE__, version);
                    ret = -1;
                }

                free(version);
                if (depth == 1)
                    cproject_has.bits.cdtBuildSystem = 1;   /* cdtBuildSystem outside of cconfiguration */
            } else if (strcmp(attr_moduleId, refreshScope) == 0) {
                /* empty */
            } else {
                printf("\e[31mline %d moduleId %s\e[0m ", __LINE__, attr_moduleId);
                ret = -1;
            }

            if (attr_id) {
                if (strncmp(attr_id, TITLE, strlen(TITLE)) != 0) {
                    printf("\e[31mline %d %s not %s\e[0m ", __LINE__, attr_id, TITLE);
                    ret = -1;
                } 
            }

            if (attr_buildSystemId) {
                printf(" (buildSystemId %s) ", attr_buildSystemId);
                if (strcmp(attr_buildSystemId, configurationDataProvider) == 0) {
                    cconfiguration_has.bits.configurationDataProvider = 1;
                }
                if (strcmp(attr_id, current_instance->config_gnu_cross_exe) != 0) {
                    printf("\nline %d:\n%s\n%s\n\n", __LINE__, attr_id, current_instance->config_gnu_cross_exe);
                    ret = -1;
                }
            }
            if (attr_name) {
                printf(" (name %s) ", attr_name);
                strcpy(cconfiguration_name, attr_name);
            }

            if (depth > 0) {
                if (cproject_state[depth-1] == CPROJECT_STATE_STORAGEMODULE_SETTINGS) {
                }
            }

            free(attr_moduleId);
            free(attr_buildSystemId);
        } // ..if (strcmp(storageModule, name) == 0)
        else if (strcmp(extension, name) == 0) {
            if (cproject_state[depth-1] == CPROJECT_STATE_EXTENSIONS) {
                char *attr_point = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"point");
                printf("line %d {{{%s %s}}} ", __LINE__, attr_id, attr_point);
                if (strcmp(attr_point, org_eclipse_cdt_core_BinaryParser) == 0) {
                    if (strcmp(attr_id, org_eclipse_cdt_core_ELF) != 0) {
                        printf("\e[31mline %d %s not %s\e[0m ", __LINE__, org_eclipse_cdt_core_BinaryParser, org_eclipse_cdt_core_ELF);
                        ret = -1;
                    }
                } else if (strcmp(attr_point, org_eclipse_cdt_core_ErrorParser) == 0) {
                    if (strcmp(attr_id, org_eclipse_cdt_core_GASErrorParser) == 0)
                        cconfiguration_has.bits.GASErrorParser = 1;
                    else if (strcmp(attr_id, org_eclipse_cdt_core_GmakeErrorParser) == 0)
                        cconfiguration_has.bits.GmakeErrorParser = 1;
                    else if (strcmp(attr_id, org_eclipse_cdt_core_GLDErrorParser) == 0)
                        cconfiguration_has.bits.GLDErrorParser = 1;
                    else if (strcmp(attr_id, org_eclipse_cdt_core_CWDLocator) == 0)
                        cconfiguration_has.bits.CWDLocator = 1;
                    else if (strcmp(attr_id, org_eclipse_cdt_core_GCCErrorParser) == 0)
                        cconfiguration_has.bits.GCCErrorParser = 1;
                    else {
                        printf("\e[31mline %d point=%s id=%s\e[0m ", __LINE__, attr_point, attr_id);
                        ret = -1;
                    }
                } else {
                    printf("\e[31mline %d point=%s id=%s\e[0m ", __LINE__, attr_point, attr_id);
                    ret = -1;
                }
                free(attr_point);
                printf(" *  ");
            }
        } else if (strcmp(autodiscovery, name) == 0) {
            char *attr_enabled = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"enabled");
            char *attr_problemReportingEnabled = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"problemReportingEnabled");
            char *attr_selectedProfileId = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"selectedProfileId");
            if (!attr_enabled) {
                printf("\e[31mline %d autodiscovery no-enabled\e[0m ", __LINE__);
                ret = -1;
            }
            if (!attr_problemReportingEnabled) {
                printf("\e[31mline %d autodiscovery no-problemReportingEnabled\e[0m ", __LINE__);
                ret = -1;
            }
            if (!attr_selectedProfileId) {
                printf("\e[31mline %d autodiscovery no-selectedProfileId\e[0m ", __LINE__);
                ret = -1;
            }
            free(attr_enabled);
            free(attr_problemReportingEnabled);
            free(attr_selectedProfileId);
            printf(" * ");
        }

        if (depth > 0) {
            if (cproject_state[depth-1] == CPROJECT_STATE_STORAGEMODULE_SETTINGS) {
                if (in_cconfiguration) {
                    if (strcmp(name, externalSettings) == 0) {
                        cconfiguration_has.bits.externalSettings = 1;
                    } else if (strcmp(name, extensions) == 0) {
                        cconfiguration_has.bits.extensions = 1;
                        cproject_state[depth] = CPROJECT_STATE_EXTENSIONS;
                    }
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_STORAGEMODULE_CDTBUILDSYSTEM) {
                if (strcmp(name, configuration) == 0) {
                    char *attr_artifactExtension = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"artifactExtension");
                    char *attr_artifactName = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"artifactName");
                    char *attr_buildArtefactType = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"buildArtefactType");
                    char *attr_buildProperties = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"buildProperties");
                    char *attr_cleanCommand = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"cleanCommand");
                    char *attr_description = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"description");
                    char *attr_parent = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"parent");
                    if (strstr(attr_id, attr_parent) == NULL) {
                        printf("\e[31mline %d parent not in id\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_description) {
                        printf("\e[31mline %d no description\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_cleanCommand) {
                        printf("\e[31mline %d no cleanCommand\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_buildArtefactType) {
                        printf("\e[31mline %d no buildArtefactType\e[0m ", __LINE__);
                        ret = -1;
                    } else if (strcmp(attr_buildArtefactType, "org.eclipse.cdt.build.core.buildArtefactType.exe") != 0) {
                        printf("\e[31mline %d wrong buildArtefactType\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_buildProperties) {
                        printf("\e[31mline %d no buildProperties\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_artifactName) {
                        printf("\e[31mline %d no artifactName\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (strcmp(attr_name, cconfiguration_name) != 0) {
                        printf("\e[31mline %d cconf 'name' %s vs '%s'\e[0m ", __LINE__, attr_name, cconfiguration_name);
                        ret = -1;
                    }
                    if (strcmp(attr_id, current_instance->config_gnu_cross_exe) != 0) {
                        printf("\nline %d:\n%s\n%s\n\n", __LINE__, attr_id, current_instance->config_gnu_cross_exe);
                        ret = -1;
                    }
                    if (strncmp(attr_parent, attr_id, strlen(attr_parent)) != 0) {
                        printf("\nline %d:\t(parent)\n%s\n%s\n\n", __LINE__, attr_parent, attr_id);
                        ret = -1;
                    }

                    free(attr_artifactExtension);
                    free(attr_artifactName);
                    free(attr_buildArtefactType);
                    free(attr_buildProperties);
                    free(attr_cleanCommand);
                    free(attr_description);
                    free(attr_parent);
                    cproject_state[depth] = CPROJECT_STATE_CONFIGURATION;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_CONFIGURATION) {
                if (strcmp(name, folderInfo) == 0) {
                    char *attr_resourcePath = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"resourcePath");

                    if (strncmp(attr_id, current_instance->config_gnu_cross_exe, strlen(current_instance->config_gnu_cross_exe)) != 0) {  /* trailing '.' */
                        printf("\nline %d:\n\n%s\n%s\n\n", __LINE__, attr_id, current_instance->config_gnu_cross_exe);
                        ret = -1;
                    }
                    if (strcmp(attr_name, "/") != 0) {
                        printf("\nline %d:\n\n%s\n%s\n\n", __LINE__, attr_name, "/");
                        ret = -1;
                    }

                    strcpy(current_instance->config_gnu_cross_exe_folderInfo, attr_id);
                    cconfiguration_has.bits.folderInfo = 1;
                    free(attr_resourcePath);
                    cproject_state[depth] = CPROJECT_STATE_FOLDERINFO;
                } else if (strcmp(name, sourceEntries) == 0) {
                    cproject_state[depth] = CPROJECT_STATE_SOURCEENTRIES;
                } else {
                    printf("\e[31mline %d configuration %s\e[0m ", __LINE__, name);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_FOLDERINFO) {
                if (strcmp(name, toolChain) == 0) {
                    if (strncmp(attr_id, TITLE, strlen(TITLE)) != 0) {
                        printf("\e[31mline %d %s not %s\e[0m ", __LINE__, attr_id, TITLE);
                        ret = -1;
                    }
                    if (strstr(attr_superClass, toolchain_exe) != NULL) {
                        cproject_state[depth] = CPROJECT_STATE_TOOLCHAIN_EXE;
                    } else {
                        printf("\e[31mline %d toolChain %s\e[0m ", __LINE__, attr_superClass);
                        ret = -1;
                    }
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOLCHAIN_EXE) {
                printf("line %d TOOLCHAIN_EXE-%s %s ", __LINE__, name, attr_superClass);
                if (strcmp(name, tool) == 0) {
                    if (strstr(attr_superClass, tool_assembler) != NULL) {
                        cconfiguration_has.bits.tool_assembler = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_ASSEMBLER;
                        if (!attr_name) {
                            printf("\e[31mline %d no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_size) != NULL) {
                        //cconfiguration_has.bits.tool_size = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_SIZE;
                        if (!attr_name) {
                            printf("\e[31mline %d no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_archiver) != NULL) {
                        cconfiguration_has.bits.tool_archiver = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_ARCHIVER;
                        if (!attr_name) {
                            printf("\e[31mline %d no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_c_compiler) != NULL) {
                        cconfiguration_has.bits.tool_c_compiler = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_C_COMPILER;
                        strcpy(current_instance->tool_gnu_cross_c_compiler, attr_id);
                        /* ? attr_name required ? */
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_cpp_linker) != NULL) {
                        cconfiguration_has.bits.tool_cpp_linker = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_CPP_LINKER;
                        if (!attr_name) {
                            printf("\e[31mline %d cross-c-compiler no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_c_linker) != NULL) {
                        cconfiguration_has.bits.tool_c_linker = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_C_LINKER;
                        if (!attr_name) {
                            printf("\e[31mline %d no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_cpp_compiler) != NULL) {
                        cconfiguration_has.bits.tool_cpp_compiler = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_CPP_COMPILER;
                        if (!attr_name) {
                            printf("\e[31mline %d no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_objdump_listfile) != NULL) {
                        //cconfiguration_has.bits.tool_objdump_listfile = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_OBJDUMP_LISTFILE;
                        if (!attr_name) {
                            printf("\e[31mline %d no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_objcopy_hex) != NULL) {
                        //cconfiguration_has.bits.tool_objcopy_hex = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_OBJCOPY_HEX;
                        if (!attr_name) {
                            printf("\e[31mline %d no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_objcopy_binary) != NULL) {
                        //cconfiguration_has.bits.tool_objcopy_binary = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_OBJCOPY_BINARY;
                        if (!attr_name) {
                            printf("\e[31mline %d no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_objcopy_verilog) != NULL) {
                        //cconfiguration_has.bits.tool_objcopy_verilog = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_OBJCOPY_VERILOG;
                        if (!attr_name) {
                            printf("\e[31mline %d no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_objcopy_srec) != NULL) {
                        //cconfiguration_has.bits.tool_objcopy_srec = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_OBJCOPY_SREC;
                        if (!attr_name) {
                            printf("\e[31mline %d no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_objcopy_symbolsrec) != NULL) {
                        //cconfiguration_has.bits.tool_objcopy_symbolsrec = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_OBJCOPY_SYMBOLSREC;
                        if (!attr_name) {
                            printf("\e[31mline %d no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else {
                        printf("\e[31mline %d tool %s\e[0m ", __LINE__, attr_superClass);
                        ret = -1;
                    }
                } else if (strcmp(name, option) == 0) {
                    if (strstr(attr_superClass, internal_toolchain_type) != NULL) {
                        cconfiguration_has.bits.internal_toolchain_type = 1;
                        if (!attr_value) {
                            printf("\e[31mline %d no-value\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, string) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, internal_toolchain_version) != NULL) {
                        cconfiguration_has.bits.internal_toolchain_version = 1;
                        if (!attr_value) {
                            printf("\e[31mline %d no-value\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, string) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        printf(" * " );
                    } else if (strstr(attr_superClass, option_target_mcu) != NULL) {
                        cconfiguration_has.bits.option_target_mcu = 1;
                        if (!attr_value) {
                            printf("\e[31mline %d no-value\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, string) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, option_target_cpuid) != NULL) {
                        cconfiguration_has.bits.option_target_cpuid = 1;
                        if (!attr_value) {
                            printf("\e[31mline %d no-value\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, string) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, option_target_coreid) != NULL) {
                        cconfiguration_has.bits.option_target_coreid = 1;
                        if (!attr_value) {
                            printf("\e[31mline %d no-value\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, string) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, option_fpu) != NULL) {
                        //cconfiguration_has.bits.option_fpu = 1;
                        if (!attr_value) {
                            printf("\e[31mline %d no-value\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, enumerated) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, option_floatabi) != NULL) {
                        //cconfiguration_has.bits.option_floatabi = 1;
                        if (!attr_value) {
                            printf("\e[31mline %d no-value\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, enumerated) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, option_target_board) != NULL) {
                        cconfiguration_has.bits.option_target_board = 1;
                        if (!attr_value) {
                            printf("\e[31mline %d no-value\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, string) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, option_defaults) != NULL) {
                        // optional cconfiguration_has.bits.option_defaults = 1;
                        if (!attr_value) {
                            printf("\e[31mline %d no-value\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, string) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        printf(" * ");
                    } else {
                        printf("\e[31mline %d unknown superClass %s\e[0m ", __LINE__, attr_superClass);
                        ret = -1;
                    }
                } else if (strcmp(name, targetPlatform) == 0) {
                    char *attr_archList = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"archList");
                    char *attr_isAbstract = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"isAbstract");
                    char *attr_osList = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"osList");
                    cconfiguration_has.bits.targetPlatform = 1;
                    if (!attr_archList) {
                        printf("\e[31mline %d targetPlatform-no-archList\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_isAbstract) {
                        printf("\e[31mline %d targetPlatform-no-isAbstract\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_osList) {
                        printf("\e[31mline %d targetPlatform-no-osList\e[0m ", __LINE__);
                        ret = -1;
                    }
                    free(attr_archList);
                    free(attr_isAbstract);
                    free(attr_osList);
                    printf(" * ");
                } else if (strcmp(name, builder) == 0) {
                    char *attr_buildPath = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"buildPath");
                    char *attr_keepEnvironmentInBuildfile = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"keepEnvironmentInBuildfile");
                    char *attr_managedBuildOn = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"managedBuildOn");
                    char *attr_parallelBuildOn = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"parallelBuildOn");
                    char *attr_parallelizationNumber = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"parallelizationNumber");
                    cconfiguration_has.bits.builder = 1;
                    if (!attr_name) {
                        printf("\e[31mline %d builder-no-name\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_buildPath) {
                        printf("\e[31mline %d builder-no-buildPath\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_managedBuildOn) {
                        printf("\e[31mline %d builder-no-managedBuildOn\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_parallelBuildOn) {
                        printf("\e[31mline %d builder-no-parallelBuildOn\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_parallelizationNumber) {
                        printf("\e[31mline %d builder-no-parallelizationNumber\e[0m ", __LINE__);
                        ret = -1;
                    }
                    free(attr_buildPath);
                    free(attr_keepEnvironmentInBuildfile);
                    free(attr_managedBuildOn);
                    free(attr_parallelBuildOn);
                    free(attr_parallelizationNumber);
                    printf(" * ");
                } else {
                    printf("\e[31mline %d unknown name %s\e[0m ", __LINE__, name);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_ASSEMBLER_OPTION_DEBUGLEVEL) {
                printf("line %d TOOL_ASSEMBLER_OPTION_DEBUGLEVEL-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_ASSEMBLER_INPUT) {
                printf("line %d TOOL_ASSEMBLER_INPUT-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;

            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_ASSEMBLER) {
                printf("line %d TOOL_ASSEMBLER-%s %s ", __LINE__, name, attr_superClass);
                if (strcmp(name, option) == 0) {
                    if (strstr(attr_superClass, tool_assembler_option_debuglevel) != NULL) {
                        cconfiguration_has.bits.tool_assembler_option_debuglevel = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_ASSEMBLER_OPTION_DEBUGLEVEL;
                        if (!attr_value) {
                            printf("\e[31mline %d no-value\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, enumerated) != 0) {
                            printf("\e[31mline %d valueType\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_assembler_option_includepaths) != NULL) {
                        cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                        if (!attr_IS_BUILTIN_EMPTY) {
                            printf("\e[31mline %d no IS_BUILTIN_EMPTY\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (!attr_IS_VALUE_EMPTY) {
                            printf("\e[31mline %d no IS_VALUE_EMPTY\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, "includePath") != 0) {
                            printf("\e[31mline %d valueType\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (!attr_useByScannerDiscovery) {
                            printf("\e[31mline %d %s no %s\e[0m ", __LINE__, attr_superClass, useByScannerDiscovery);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_assembler_option_includefiles) != NULL) {
                        cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                        if (!attr_IS_BUILTIN_EMPTY) {
                            printf("\e[31mline %d no IS_BUILTIN_EMPTY\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (!attr_IS_VALUE_EMPTY) {
                            printf("\e[31mline %d no IS_VALUE_EMPTY\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, "includeFiles") != 0) {
                            printf("\e[31mline %d valueType\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (!attr_useByScannerDiscovery) {
                            printf("\e[31mline %d %s no %s\e[0m ", __LINE__, attr_superClass, useByScannerDiscovery);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_assembler_option_otherflags) != NULL) {
                        cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                        if (!attr_IS_BUILTIN_EMPTY) {
                            printf("\e[31mline %d no IS_BUILTIN_EMPTY\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (!attr_IS_VALUE_EMPTY) {
                            printf("\e[31mline %d no IS_VALUE_EMPTY\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, stringList) != 0) {
                            printf("\e[31mline %d valueType\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (!attr_useByScannerDiscovery) {
                            printf("\e[31mline %d %s no %s\e[0m ", __LINE__, attr_superClass, useByScannerDiscovery);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_assembler_option_suppresswarnings) != NULL ||
                               strstr(attr_superClass, tool_assembler_option_preprocessassembler) != NULL ||
                               strstr(attr_superClass, tool_assembler_option_preprocessonly) != NULL ||
                               strstr(attr_superClass, tool_assembler_option_nostdinc) != NULL)
                    {
                        if (strcmp(attr_valueType, boolean) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        if (!attr_useByScannerDiscovery) {
                            printf("\e[31mline %d %s no %s\e[0m ", __LINE__, attr_superClass, useByScannerDiscovery);
                            ret = -1;
                        }
                        printf(" * ");
                    } else
                        ret = -1;

                } else if (strcmp(name, inputType) == 0) {
                    if (strstr(attr_superClass, tool_assembler_input) != NULL) {
                        cconfiguration_has.bits.tool_assembler_input = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_ASSEMBLER_INPUT;
                        printf(" * ");
                    } else
                        ret = -1;
                } else {
                    printf("\e[31mline %d unknown name %s\e[0m ", __LINE__, name);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_SIZE) {
                printf("line %d TOOL_SIZE-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_ARCHIVER) {
                printf("line %d TOOL_ARCHIVER-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_C_COMPILER_OPTION_DEBUGLEVEL) {
                printf("line %d TOOL_C_COMPILER_OPTION_DEBUGLEVEL-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_C_COMPILER_OPTION_OPTIMIZATION_LEVEL) {
                printf("line %d TOOL_C_COMPILER_OPTION_OPTIMIZATION_LEVEL-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_C_COMPILER_INPUT_C) {
                printf("line %d TOOL_C_COMPILER_INPUT_C-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_CPP_COMPILER_OPTION_DEBUGLEVEL) {
                printf("line %d TOOL_CPP_COMPILER_OPTION_DEBUGLEVEL-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_CPP_COMPILER_OPTION_OPTIMIZATION_LEVEL) {
                printf("line %d TOOL_CPP_COMPILER_OPTION_OPTIMIZATION_LEVEL-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_C_COMPILER) {
                printf("line %d TOOL_C_COMPILER-%s %s ", __LINE__, name, attr_superClass);
                if (strcmp(name, option) == 0) {
                    if (strstr(attr_superClass, tool_c_compiler_option) != NULL) {
                        if (strstr(attr_superClass, "definedsymbols") != NULL) {
                            cconfiguration_has.bits.tool_c_compiler_option_definedsymbols = 1;
                            cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                            if (strcmp(attr_valueType, "definedSymbols") != 0) {
                                printf("\e[31mline %d valueType\e[0m ", __LINE__);
                                ret = -1;
                            }
                        } else if (strstr(attr_superClass, "debuglevel") != NULL) {
                            cconfiguration_has.bits.tool_c_compiler_option_debuglevel = 1;
                            if (strcmp(attr_valueType, enumerated) != 0) {
                                printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                                ret = -1;
                            }
                        } else if (strstr(attr_superClass, "includepaths") != NULL) {
                            cconfiguration_has.bits.tool_c_compiler_option_includepaths = 1;
                            cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                            if (!attr_IS_BUILTIN_EMPTY) {
                                printf("\e[31mline %d no IS_BUILTIN_EMPTY\e[0m ", __LINE__);
                                ret = -1;
                            }
                            if (!attr_IS_VALUE_EMPTY) {
                                printf("\e[31mline %d no IS_VALUE_EMPTY\e[0m ", __LINE__);
                                ret = -1;
                            }
                            if (strcmp(attr_valueType, "includePath") != 0) {
                                printf("\e[31mline %d valueType\e[0m ", __LINE__);
                                ret = -1;
                            }
                        } else if (strstr(attr_superClass, "optimization.level") != NULL) {
                            cconfiguration_has.bits.tool_c_compiler_option_optimization_level  = 1;
                            if (strcmp(attr_valueType, enumerated) != 0) {
                                printf("\e[31mline %d valueType\e[0m ", __LINE__);
                                ret = -1;
                            }
                        } else if (strstr(attr_superClass, "languagestandard") != NULL) {
                            if (strcmp(attr_valueType, enumerated) != 0) {
                                printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                                ret = -1;
                            }
                        } else if (strstr(attr_superClass, "otherflags") != NULL) {
                            if (strcmp(attr_valueType, stringList) != 0) {
                                printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                                ret = -1;
                            }
                        } else {
                            if (strcmp(attr_valueType, boolean) != 0) {
                                printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                                ret = -1;
                            }
                            printf(" ### ");
                        }
                        if (!attr_valueType) {
                            printf("\e[31mline %d no-valueType\e[0m ", __LINE__);
                            ret = -1;
                        }
                        /*if (!attr_useByScannerDiscovery) {
                            printf("\e[31mline %d %s no %s\e[0m ", __LINE__, attr_superClass, useByScannerDiscovery);
                            ret = -1;
                        }*/
                    } else {
                        printf("\e[31mline %d not-%s\e[0m ", __LINE__, tool_c_compiler_option);
                        ret = -1;
                    }

                } else if (strcmp(name, inputType) == 0) {
                    if (strstr(attr_superClass, tool_c_compiler_input_c) != NULL) {
                        cconfiguration_has.bits.tool_c_compiler_input_c = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_C_COMPILER_INPUT_C;
                        strcpy(current_instance->tool_gnu_cross_c_compiler_input, attr_id);
                        printf(" * ");
                    } else {
                        printf("\e[31mline %d\e[0m ", __LINE__);
                        ret = -1;
                    }
                } else {
                    printf("\e[31mline %d unknown name %s\e[0m ", __LINE__, name);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_LINKER_INPUT) {
                printf("line %d TOOL_LINKER_INPUT-%s %s ", __LINE__, name, attr_superClass);
                if (strcmp(name, additionalInput) == 0) {
                    char *attr_kind = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"kind");
                    char *attr_paths = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"paths");
                    if (!attr_kind) {
                        printf("\e[31mline %d linker-input no kind\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_paths) {
                        printf("\e[31mline %d linker-input no paths\e[0m ", __LINE__);
                        ret = -1;
                    }
                    free(attr_kind);
                    free(attr_paths);
                } else {
                    printf("\e[31mline %d name\e[0m ", __LINE__);
                    ret = -1;
                }
                printf(" * ");
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_C_LINKER) {
                printf("line %d TOOL_C_LINKER-%s %s ", __LINE__, name, attr_superClass);
                if (strcmp(name, option) == 0) {
                    if (strstr(attr_superClass, tool_c_linker_option) != NULL) {
                        if (strstr(attr_superClass, "script") != NULL) {
                            cconfiguration_has.bits.tool_c_linker_option_script = 1;
                            cproject_state[depth] = CPROJECT_STATE_TOOL_C_LINKER_OPTION_SCRIPT;
                            if (!attr_value) {
                                printf("\e[31mline %d no-value\e[0m ", __LINE__);
                                ret = -1;
                            }
                            if (strcmp(attr_valueType, string) != 0) {
                                printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                                ret = -1;
                            }
                            printf(" * ");
                        } else if (strstr(attr_superClass, "libraries") != NULL) {
                            cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                            if (strcmp(attr_valueType, "libs") != 0) {
                                printf("\e[31mline %d valueType\e[0m ", __LINE__);
                                ret = -1;
                            }
                            printf(" * ");
                        } else if (strstr(attr_superClass, "directories") != NULL) {
                            cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                            if (strcmp(attr_valueType, "libPaths") != 0) {
                                printf("\e[31mline %d valueType\e[0m ", __LINE__);
                                ret = -1;
                            }
                            printf(" * ");
                        } else if (strstr(attr_superClass, "otherflag") != NULL) {  /* c.linker */
                            cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                            if (strcmp(attr_valueType, stringList) != 0) {
                                printf("\e[31mline %d valueType\e[0m ", __LINE__);
                                ret = -1;
                            }
                            printf(" * ");
                        } else if (strstr(attr_superClass, "additionalobjs") != NULL) {
                            cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                            if (strcmp(attr_valueType, "userObjs") != 0) {
                                printf("\e[31mline %d valueType\e[0m ", __LINE__);
                                ret = -1;
                            }
                            printf(" * ");
                        } else {
                            if (strcmp(attr_valueType, boolean) != 0) {
                                printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                                ret = -1;
                            }
                            if (!attr_useByScannerDiscovery) {
                                printf("\e[31mline %d %s no %s\e[0m ", __LINE__, attr_superClass, useByScannerDiscovery);
                                ret = -1;
                            }
                        }
                    }
                } else if (strcmp(name, inputType) == 0) {
                    if (strstr(attr_superClass, tool_c_linker_input) != NULL) {
                        cconfiguration_has.bits.tool_c_linker_input = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_LINKER_INPUT;
                        printf(" * ");
                    } else {
                        printf("\e[31mline %d linker-input %s\e[0m ", __LINE__, attr_superClass);
                        ret = -1;
                    }
                } else {
                    printf("\e[31mline %d unknown name %s\e[0m ", __LINE__, name);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_CPP_LINKER) {
                printf("line %d TOOL_CPP_LINKER-%s %s ", __LINE__, name, attr_superClass);
                if (strcmp(name, option) == 0) {
                    if (strstr(attr_superClass, tool_cpp_linker_option_script) != NULL) {
                        cconfiguration_has.bits.tool_cpp_linker_option_script = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_CPP_LINKER_OPTION_SCRIPT;
                        if (!attr_value) {
                            printf("\e[31mline %d no-value\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, string) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        printf(" * ");
                    } else
                        ret = -1;
                } else if (strcmp(name, inputType) == 0) {
                    if (strstr(attr_superClass, tool_cpp_linker_input) != NULL) {
                        cproject_state[depth] = CPROJECT_STATE_TOOL_LINKER_INPUT;
                        printf(" * ");
                    } else
                        ret = -1;
                } else {
                    printf("\e[31mline %d unknown name %s\e[0m ", __LINE__, name);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_OBJDUMP_LISTFILE) {
                printf("line %d TOOL_OBJDUMP_LISTFILE-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_OBJCOPY_HEX) {
                printf("line %d TOOL_OBJCOPY_HEX-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_OBJCOPY_BINARY) {
                printf("line %d TOOL_OBJCOPY_BINARY-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_OBJCOPY_VERILOG) {
                printf("line %d TOOL_OBJCOPY_VERILOG-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_OBJCOPY_SREC) {
                printf("line %d TOOL_OBJCOPY_SREC-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_OBJCOPY_SYMBOLSREC) {
                printf("line %d TOOL_OBJCOPY_SYMBOLSREC-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_C_LINKER_OPTION_SCRIPT) {
                printf("line %d TOOL_C_LINKER_OPTION_SCRIPT-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_CPP_LINKER_OPTION_SCRIPT) {
                printf("line %d TOOL_CPP_LINKER_OPTION_SCRIPT-%s %s ", __LINE__, name, attr_superClass);
                ret = -1;
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_CPP_COMPILER) {
                printf("line %d TOOL_CPP_COMPILER-%s %s ", __LINE__, name, attr_superClass);
                if (strcmp(name, option) == 0) {
                    if (strstr(attr_superClass, tool_cpp_compiler_option_debuglevel) != NULL) {
                        cconfiguration_has.bits.tool_cpp_compiler_option_debuglevel = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_CPP_COMPILER_OPTION_DEBUGLEVEL;
                        if (strcmp(attr_valueType, enumerated) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_cpp_compiler_option_optimization_level) != NULL) {
                        cconfiguration_has.bits.tool_cpp_compiler_option_optimization_level = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_CPP_COMPILER_OPTION_OPTIMIZATION_LEVEL;
                        if (!attr_useByScannerDiscovery) {
                            printf("\e[31mline %d %s no %s\e[0m ", __LINE__, attr_superClass, useByScannerDiscovery);
                            ret = -1;
                        }
                        printf(" * ");
                    } else
                        ret = -1;
                } else {
                    printf("\e[31mline %d unknown name %s\e[0m ", __LINE__, name);
                    ret = -1;
                }
            }
            else if (cproject_state[depth-1] == CPROJECT_STATE_SOURCEENTRIES) {
                printf("line %d SOURCEENTRIES-%s ", __LINE__, name);
                if (strcmp(name, entry) == 0) {
                    char *attr_flags = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"flags");
                    char *attr_kind = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"kind");
                    if (attr_flags == NULL || attr_kind == NULL) {
                        printf("\e[31mline %d flags==NULL || kind==NULL\e[0m ", __LINE__);
                        ret = -1;
                    }
                    free(attr_flags);
                    free(attr_kind);
                    printf(" * ");
                }
            }
        } // ..if (depth > 0)

        switch (depth) {
            case 0:
                if (strcmp(name, "cproject") != 0) {
                    fprintf(stderr, ".cproject not cproject ");
                    ret = -1;
                } else
                    cproject_state[0] = CPROJECT_STATE_CPROJECT;
                break;
            case 1:
                if (cproject_state[0] != CPROJECT_STATE_CPROJECT) {
                    fprintf(stderr, ".cproject depth1: depth0 not cproject");
                    ret = -1;
                    break;
                }
                if (strcmp(storageModule, name) != 0) {
                    printf("\e[31m.cproject line %d unknown element %s\e[0m ", __LINE__, name);
                    ret = -1;
                }
                break;
            case 2:
                if (cproject_state[depth-1] == CPROJECT_STATE_STORAGEMODULE_CDTBUILDSYSTEM) {
                    printf("line %d STORAGEMODULE_CDTBUILDSYSTEM-%s\e[0m ", __LINE__, name);
                    if (!attr_name) {
                        printf("\e[31mline %d project no-name\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_id) {
                        printf("\e[31mline %d project no-id\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (attr_id && attr_name) {
                        if (strncmp(attr_name, attr_id, strlen(attr_name)) != 0) {
                            printf("\nline %d:\n%s\n%s\n\n", __LINE__, attr_id, attr_name);
                            ret = -1;
                        }
                    }
                    printf(" * ");
                } else if (cproject_state[depth-1] == CPROJECT_STATE_STORAGEMODULE_SCANNERCONFIGURATION) {
                    printf("line %d STORAGEMODULE_SCANNERCONFIGURATION-%s ", __LINE__, name);
                    if (strcmp(name, scannerConfigBuildInfo) == 0) {
                        cproject_state[depth] = CPROJECT_STATE_SCANNERCONFIGBUILDINFO;
                        char *attr_instanceId = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"instanceId");
                        if (attr_instanceId) {
                            struct node_s *my_list;
                            //printf("\n\n%s\n\n", attr_instanceId);
                            for (my_list = instance_list; my_list != NULL; my_list = my_list->next) {
                                instance_t *instance = my_list->ptr;
                                char *str;
                                int len;
                                if (instance->scannerInfo)
                                    continue;

                                len = strlen(instance->config_gnu_cross_exe) + strlen(instance->config_gnu_cross_exe_folderInfo) + strlen(instance->tool_gnu_cross_c_compiler) + strlen(instance->tool_gnu_cross_c_compiler_input) + 8;
                                str = malloc(len);
                                strcpy(str, instance->config_gnu_cross_exe);
                                strcat(str, ";");
                                strcat(str, instance->config_gnu_cross_exe_folderInfo);
                                strcat(str, ";");
                                strcat(str, instance->tool_gnu_cross_c_compiler);
                                strcat(str, ";");
                                strcat(str, instance->tool_gnu_cross_c_compiler_input);
                                if (strcmp(str, attr_instanceId) == 0)
                                    instance->scannerInfo = true;

                                free(str);
                            }
                        } else {
                            printf("\e[31mline %d no instanceId\e[0m ", __LINE__);
                            ret = -1;
                        }
                        free(attr_instanceId);
                        printf(" * ");
                    } else if (strcmp(name, autodiscovery) == 0) {
                    } else {
                        printf("\e[31mline %d\e[0m ", __LINE__);
                        ret = -1;
                    }
                } else if (cproject_state[depth-1] == CPROJECT_STATE_STORAGEMODULE_SETTINGS) {
                    if (strcmp(cconfiguration, name) == 0) {
                        in_cconfiguration = true;
                        cconfiguration_name[0] = 0;
                        cconfiguration_has.dword = 0;
                        printf("\e[7mcconfiguration id %s\e[0m ", attr_id);

                        struct node_s **node = &instance_list;
                        if (*node) {
                            while ((*node)->next) {
                                node = &(*node)->next;
                            }
                            node = &(*node)->next;
                        }
                        (*node) = malloc(sizeof(struct node_s));
                        (*node)->next = NULL;
                        (*node)->ptr = malloc(sizeof(instance_t));
                        memset((*node)->ptr, 0, sizeof(instance_t)); 
                        current_instance = (*node)->ptr;

                        strcpy(current_instance->config_gnu_cross_exe, attr_id);
                    } else {
                        printf("\e[31mline %d CPROJECT_STATE_STORAGEMODULE_SETTINGS-%s ", __LINE__, name);
                        ret = -1;
                    }
                } else {
                    printf("\e[31m.cproject line %d unknown cproject_state[1]=%d\e[0m ", __LINE__, cproject_state[1]);
                    ret = -1;
                }
                break;
            case 3:
                if (cproject_state[depth-1] == CPROJECT_STATE_SCANNERCONFIGBUILDINFO) {
                    printf("line %d SCANNERCONFIGBUILDINFO-%s ", __LINE__, name);
                    if (strcmp(name, autodiscovery) != 0) {
                        printf("\e[31mline %d\e[0m ", __LINE__);
                        ret = -1;
                    }
                }
                break;
        } // ..switch (depth)

        free(attr_superClass);
        free(attr_id);
        free(attr_name);
        free(attr_valueType);
        free(attr_useByScannerDiscovery);
        free(attr_IS_BUILTIN_EMPTY);
        free(attr_IS_VALUE_EMPTY);
        free(attr_value);
    } // ..if (nodeType == XML_NODE_TYPE_START_ELEMENT)
    else if (nodeType == XML_NODE_TYPE_END_OF_ELEMENT) {
        cproject_state[depth] = CPROJECT_STATE_NONE;
        if (strcmp(storageModule, name) == 0) {
            printf(" \e[33mend-%s\e[0m ", storageModuleId[depth]);
            if (strcmp(scannerConfiguration, storageModuleId[depth]) == 0) {
                struct node_s *my_list;
                for (my_list = instance_list; my_list != NULL; my_list = my_list->next) {
                    instance_t *instance = my_list->ptr;
                    if (!instance->scannerInfo) {
                        printf("\e[31mline %d no %s for %s\e[0m ", __LINE__, scannerConfiguration, instance->config_gnu_cross_exe);
                        ret = -1;
                    } else
                        printf("%s-ok-%s ", scannerConfiguration, instance->config_gnu_cross_exe);
                }
            }
            storageModuleId[depth][0] = 0;
        } else if (strcmp(cconfiguration, name) == 0) {
            printf("\e[42mcconfiguration_has.dword : %lx\e[0m ", cconfiguration_has.dword);
            in_cconfiguration = false;
            cconfiguration_name[0] = 0;
            current_instance = NULL;
            if (cconfiguration_has.dword != 0x1fffffffff) {
                show_cconfiguration_missing(cconfiguration_has);
                printf("\e[31mline %d cconfiguration_has.dword : %lx\e[0m ", __LINE__, cconfiguration_has.dword);
                ret = -1;
            }
        }
    }

    printf("\n");

    return ret;
}

int test_cproject()
{
    int ret;
    const char * const cproject_file_name = ".cproject";
    xmlTextReaderPtr cproject_reader;

    cproject_reader = xmlReaderForFile(cproject_file_name, NULL, 0);
    if (!cproject_reader) {
        fprintf(stderr, "Unable to open %s\n", cproject_file_name);
        return -1;
    }
    cproject_has.dword = 0;
    for (unsigned n = 0; n < CPROJECT_DEPTH; n++)
        cproject_state[n] = CPROJECT_STATE_NONE;

    ret = xmlTextReaderRead(cproject_reader);
    while (ret == 1) {
        if (processNode(cproject_reader) < 0)
            break;
        ret = xmlTextReaderRead(cproject_reader);
    }

    xmlFreeTextReader(cproject_reader);

    printf("cproject_has:%x\e[0m\r\n", cproject_has.dword);

    return ret;
}

