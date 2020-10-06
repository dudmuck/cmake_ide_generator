#include <string.h>
#include <stdbool.h>
#include <libxml/xmlreader.h>
#include "xml.h"

const char * const TITLE = "fr.ac6.managedbuild";
const char * const TITLE_ = "fr.ac6.managedbuid";    // only for gnu.c.compiler.option.misc.other
const char * const enumerated = "enumerated";
const char * const boolean = "boolean";
const char * const string = "string";
const char * const stringList = "stringList";
const char * const userObjs = "userObjs";
const char * const libs = "libs";
const char * const libPaths = "libPaths";
const char * const gnu_c_compiler_option = "gnu.c.compiler.option";
const char * const gnu_c_link_option = "gnu.c.link.option";
const char * const gnu_cpp_link_option = "gnu.cpp.link.option";
const char * const gnu_cpp_compiler_option = "gnu.cpp.compiler.option";
const char * const cdt_managedbuild_tool = "cdt.managedbuild.tool";
const char * const gnu_both_asm_option = "gnu.both.asm.option";
const char * const storageModule = "storageModule";
const char * const settings = "org.eclipse.cdt.core.settings";
const char * const LanguageSettingsProviders = "org.eclipse.cdt.core.LanguageSettingsProviders";
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
const char * const refreshScope = "refreshScope";
const char * const cconfiguration = "cconfiguration";
const char * const configuration = "configuration";
const char * const sourceEntries = "sourceEntries";
const char * const cconfiguration_id_prefix = "fr.ac6.managedbuild.config.gnu.cross.exe";
const char * const toolchain_gnu_cross_exe = "toolchain.gnu.cross.exe";
const char * const target_gnu_cross_exe = "target.gnu.cross.exe";
const char * const configurationDataProvider = "org.eclipse.cdt.managedbuilder.core.configurationDataProvider";
const char * const externalSettings = "externalSettings";
const char * const extensions = "extensions";
const char * const extension = "extension";
const char * const folderInfo = "folderInfo";
const char * const toolChain = "toolChain";
const char * const tool = "tool";
const char * const option = "option";
const char * const useByScannerDiscovery = "useByScannerDiscovery";
const char * const listOptionValue = "listOptionValue";
const char * const _additionalInput = "additionalInput";
const char * const targetPlatform = "targetPlatform";
const char * const builder = "builder";
const char * const gnu_cross_c_compiler_input_c = "gnu.cross.c.compiler.input.c";
const char * const gnu_cross_c_compiler_input_s = "gnu.cross.c.compiler.input.s";
const char * const tool_gnu_cross_c_compiler_ffunction = "tool.gnu.cross.c.compiler.ffunction";
const char * const tool_gnu_cross_c_compiler_fdata = "tool.gnu.cross.c.compiler.fdata";
const char * const tool_gnu_cross_c_compiler = "tool.gnu.cross.c.compiler";
const char * const tool_gnu_cross_cpp_compiler = "tool.gnu.cross.cpp.compiler";
const char * const tool_gnu_cross_c_linker = "tool.gnu.cross.c.linker";
const char * const tool_gnu_cross_cpp_linker = "tool.gnu.cross.cpp.linker";
const char * const tool_gnu_archiver = "tool.gnu.archiver";
const char * const tool_gnu_cross_assembler = "tool.gnu.cross.assembler";
const char * const gnu_both_asm_option_include_paths = "gnu.both.asm.option.include.paths";
const char * const option_gnu_cross_mcu = "option.gnu.cross.mcu";
const char * const option_gnu_cross_fpu = "option.gnu.cross.fpu";
const char * const option_gnu_cross_floatabi = "option.gnu.cross.floatabi";
const char * const option_gnu_cross_board = "option.gnu.cross.board";
const char * const gnu_c_compiler_option_preprocessor_preprocess = "gnu.c.compiler.option.preprocessor.preprocess";
const char * const gnu_c_compiler_option_preprocessor_nostdinc = "gnu.c.compiler.option.preprocessor.nostdinc";
const char * const gnu_c_compiler_option_dialect_std = "gnu.c.compiler.option.dialect.std";
const char * const gnu_c_compiler_option_include_paths = "gnu.c.compiler.option.include.paths";
const char * const gnu_c_compiler_option_debugging_level = "gnu.c.compiler.option.debugging.level";
const char * const gnu_c_compiler_option_debugging_prof = "gnu.c.compiler.option.debugging.prof";
const char * const gnu_c_compiler_option_preprocessor_def_symbols = "gnu.c.compiler.option.preprocessor.def.symbols";
const char * const gnu_c_compiler_option_optimization_level = "gnu.c.compiler.option.optimization.level";
const char * const gnu_c_compiler_option_preprocessor_undef_symbol = "gnu.c.compiler.option.preprocessor.undef.symbol";
const char * const gnu_cpp_compiler_option_debugging_level = "gnu.cpp.compiler.option.debugging.level";

const char * const cdt_managedbuild_tool_gnu_c_linker_input = "cdt.managedbuild.tool.gnu.c.linker.input";
const char * const cdt_managedbuild_tool_gnu_cpp_linker_input = "cdt.managedbuild.tool.gnu.cpp.linker.input";
const char * const tool_gnu_cross_c_linker_script = "tool.gnu.cross.c.linker.script";
const char * const tool_gnu_cross_cpp_linker_script = "tool.gnu.cross.cpp.linker.script";
const char * const cdt_managedbuild_tool_gnu_assembler_input = "cdt.managedbuild.tool.gnu.assembler.input";
const char * const tool_gnu_cross_assembler_input = "tool.gnu.cross.assembler.input";
const char * const inputType = "inputType";
const char * const entry = "entry";
const char * const gnu_cpp_compiler_option_optimization_level = "gnu.cpp.compiler.option.optimization.level";



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
        uint32_t refreshScope              : 1;    // 5
    } bits;
    uint32_t dword;
} cproject_has_t;

typedef union {
    struct {
        uint32_t configurationDataProvider      : 1;    // 0
        uint32_t settings                       : 1;    // 1
        uint32_t externalSettings               : 1;    // 2
        uint32_t core_externalSettings          : 1;    // 3  f
        uint32_t extensions                     : 1;    // 4
        uint32_t folderInfo                     : 1;    // 5
        uint32_t tool_gnu_assembler_input       : 1;    // 6
        uint32_t tool_gnu_cross_assembler_input : 1;    // 7  f
        uint32_t GASErrorParser                 : 1;    // 8
        uint32_t GmakeErrorParser               : 1;    // 9
        uint32_t GLDErrorParser                 : 1;    // 10
        uint32_t CWDLocator                     : 1;    // 11  f
        uint32_t GCCErrorParser                 : 1;    // 12
        uint32_t cross_cpp_compiler             : 1;    // 13
        uint32_t cpp_optimization_level         : 1;    // 14
        uint32_t cpp_debug_level                : 1;    // 15
        uint32_t targetPlatform                 : 1;    // 16
        uint32_t builder                        : 1;    // 17
    } bits;
    uint32_t dword;
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
    /*  8 */ CPROJECT_STATE_TOOLCHAIN_GNU_CROSS_EXE,
    /*  9 */ CPROJECT_STATE_TOOL_GNU_CROSS_C_COMPILER,
    /* 10 */ CPROJECT_STATE_TOOL_GNU_CROSS_CPP_COMPILER,
    /* 11 */ CPROJECT_STATE_TOOL_GNU_CROSS_C_LINKER,
    /* 12 */ CPROJECT_STATE_TOOL_GNU_CROSS_CPP_LINKER,
    /* 13 */ CPROJECT_STATE_TOOL_GNU_ARCHIVER,
    /* 14 */ CPROJECT_STATE_TOOL_GNU_CROSS_ASSEMBLER,
    /* 15 */ CPROJECT_STATE_SCANNERCONFIGBUILDINFO,
    /* 16 */ CPROJECT_STATE_SOURCEENTRIES,
    /* 17 */ CPROJECT_STATE_PREPROCESSOR_DEF_SYMBOLS_,
    /* 18 */ CPROJECT_STATE_TOOL_GNU_CROSS_LINKER_INPUT_,
    /* 19 */ CPROJECT_STATE_INCLUDE_PATHS_,
    /* 20 */ CPROJECT_STATE_LISTOPTIONVALUE,
    /* 21 */ CPROJECT_STATE_ADDITIONALINPUT,
} cproject_state_e;

#define CPROJECT_DEPTH       12
cproject_state_e cproject_state[CPROJECT_DEPTH];
char storageModuleId[CPROJECT_DEPTH][128];

void show_cconfiguration_missing(cconfiguration_has_t ch)
{
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
        bool has_title = false;
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
            if (!attr_useByScannerDiscovery) {
                if (strstr(attr_superClass, option_gnu_cross_mcu) == NULL &&
                    strstr(attr_superClass, option_gnu_cross_floatabi) == NULL &&
                    strstr(attr_superClass, option_gnu_cross_board) == NULL &&
                    strstr(attr_superClass, option_gnu_cross_fpu) == NULL)

                {
                    printf("\e[31mline %d option %s no %s\e[0m ", __LINE__, attr_superClass, useByScannerDiscovery);
                    ret = -1;
                }
            }
        }

        if (attr_valueType) {
            if (strcmp(attr_valueType, boolean) == 0) {
                if (strcmp(attr_value, "true") != 0 && strcmp(attr_value, "false") != 0) {
                    printf("\e[31mline %d boolean not true or false\e[0m ", __LINE__);
                    ret = -1;
                }
            } else if (strcmp(attr_valueType, string) != 0 &&
                       strcmp(attr_valueType, stringList) != 0 &&
                       strcmp(attr_valueType, userObjs) != 0 &&
                       strcmp(attr_valueType, "includePath") != 0 &&
                       strcmp(attr_valueType, "definedSymbols") != 0 &&
                       strcmp(attr_valueType, "undefDefinedSymbols") != 0 &&
                       strcmp(attr_valueType, libs) != 0 &&
                       strcmp(attr_valueType, libPaths) != 0 &&
                       strcmp(attr_valueType, enumerated) != 0)

            {
                printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                ret = -1;
            }
        }

        if (depth > 0) {
            if (cproject_state[depth-1] == CPROJECT_STATE_LISTOPTIONVALUE) {
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
            } else if (cproject_state[depth-1] == CPROJECT_STATE_ADDITIONALINPUT) {
                printf("line %d ADDITIONALINPUT-%s %s ", __LINE__, name, attr_superClass);
                if (strcmp(name, _additionalInput) == 0) {
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
            }
        } // ..if (depth > 0)
        
        if (attr_id && attr_superClass) {
            if (strncmp(attr_superClass, attr_id, strlen(attr_superClass)) != 0) {
                printf("\nline %d:\n%s\n%s\n\n", __LINE__, attr_id, attr_superClass);
                ret = -1;
            }
        }

        if (attr_superClass) {
            if (strncmp(attr_superClass, TITLE, strlen(TITLE)) != 0 &&
                strncmp(attr_superClass, TITLE_, strlen(TITLE_)) != 0)
            {
                if (strncmp(attr_superClass, gnu_c_compiler_option, strlen(gnu_c_compiler_option)) != 0 &&
                    strncmp(attr_superClass, gnu_cpp_compiler_option, strlen(gnu_cpp_compiler_option)) != 0 &&
                    strncmp(attr_superClass, gnu_c_link_option, strlen(gnu_c_link_option)) != 0 &&
                    strncmp(attr_superClass, gnu_cpp_link_option, strlen(gnu_cpp_link_option)) != 0 &&
                    strncmp(attr_superClass, gnu_both_asm_option , strlen(gnu_both_asm_option)) != 0 &&
                    strncmp(attr_superClass, cdt_managedbuild_tool, strlen(cdt_managedbuild_tool)) != 0)
                {
                    printf("\e[31mline %d superClass %s\e[0m ", __LINE__, attr_superClass);
                    ret = -1;
                }
            } else
                has_title = true;
        }

        if (strcmp(storageModule, name) == 0) {
            char *attr_moduleId = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"moduleId");
            char *attr_buildSystemId = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"buildSystemId");
            strcpy(storageModuleId[depth], attr_moduleId);
            printf(" \e[33mstart-%s\e[0m ", storageModuleId[depth]);

            if (strcmp(attr_moduleId, refreshScope) == 0) {
                if (depth == 1) /* only on depth1 */
                    cproject_has.bits.refreshScope = 1;
            } else if (strcmp(attr_moduleId, LanguageSettingsProviders) == 0) {
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
                    /*
artifactExtension="elf"
artifactName="LoRaMac-periodic-uplink-lpp"
buildArtefactType="org.eclipse.cdt.build.core.buildArtefactType.exe"
buildProperties="org.eclipse.cdt.build.core.buildArtefactType=org.eclipse.cdt.build.core.buildArtefactType.exe,org.eclipse.cdt.build.core.buildType=org.eclipse.cdt.build.core.buildType.debug"
cleanCommand="rm -rf"
description=""
id="fr.ac6.managedbuild.config.gnu.cross.exe.debug.1986075698"
name="Debug"
parent="fr.ac6.managedbuild.config.gnu.cross.exe.debug"
*/
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
                    if (!attr_artifactExtension) {
                        printf("\e[31mline %d no artifactExtension\e[0m ", __LINE__);
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
                    if (strstr(attr_superClass, toolchain_gnu_cross_exe) != NULL) {
                        cproject_state[depth] = CPROJECT_STATE_TOOLCHAIN_GNU_CROSS_EXE;
                    } else {
                        printf("\e[31mline %d toolChain %s\e[0m ", __LINE__, attr_superClass);
                        ret = -1;
                    }
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOLCHAIN_GNU_CROSS_EXE) {
                printf("line %d TOOLCHAIN_GNU_CROSS_EXE-%s %s ", __LINE__, name, attr_superClass);
                if (strcmp(name, tool) == 0) {
                    if (strstr(attr_superClass, tool_gnu_cross_c_compiler) != NULL) {
                        cproject_state[depth] = CPROJECT_STATE_TOOL_GNU_CROSS_C_COMPILER;
                        strcpy(current_instance->tool_gnu_cross_c_compiler, attr_id);
                        if (!attr_name) {
                            printf("\e[31mline %d cross-c-compiler no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_gnu_cross_cpp_compiler) != NULL) {
                        cproject_state[depth] = CPROJECT_STATE_TOOL_GNU_CROSS_CPP_COMPILER;
                        if (!attr_name) {
                            printf("\e[31mline %d cross-cpp-compiler no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        cconfiguration_has.bits.cross_cpp_compiler = 1;
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_gnu_cross_c_linker) != NULL) {
                        cproject_state[depth] = CPROJECT_STATE_TOOL_GNU_CROSS_C_LINKER;
                        if (!attr_name) {
                            printf("\e[31mline %d cross-c-linker no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_gnu_cross_cpp_linker) != NULL) {
                        cproject_state[depth] = CPROJECT_STATE_TOOL_GNU_CROSS_CPP_LINKER;
                        if (!attr_name) {
                            printf("\e[31mline %d cross-cpp-linker no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_gnu_archiver) != NULL) {
                        cproject_state[depth] = CPROJECT_STATE_TOOL_GNU_ARCHIVER;
                        if (!attr_name) {
                            printf("\e[31mline %d archiver no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_gnu_cross_assembler) != NULL) {
                        cproject_state[depth] = CPROJECT_STATE_TOOL_GNU_CROSS_ASSEMBLER;
                        if (!attr_name) {
                            printf("\e[31mline %d assembler no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else {
                        printf("\e[31mline %d tool %s\e[0m ", __LINE__, attr_superClass);
                        ret = -1;
                    }
                } else if (strcmp(name, option) == 0) {
                    if (strstr(attr_superClass, option_gnu_cross_mcu) != NULL) {
                        if (!attr_name) {
                            printf("\e[31mline %d option.mcu no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (!attr_value) {
                            printf("\e[31mline %d mcu no-value\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, string) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, option_gnu_cross_fpu) != NULL) {
                        if (!attr_name) {
                            printf("\e[31mline %d option.fpu no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strstr(attr_value, option_gnu_cross_fpu) == NULL) {
                            printf("\e[31mline %d option.fpu bad-value %s\e[0m ", __LINE__, attr_value);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, enumerated) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, option_gnu_cross_floatabi) != NULL) {
                        if (!attr_name) {
                            printf("\e[31mline %d option.floatabi no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strstr(attr_value, option_gnu_cross_floatabi) == NULL) {
                            printf("\e[31mline %d option.floatabi bad-value %s\e[0m ", __LINE__, attr_value);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, enumerated) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, option_gnu_cross_board) != NULL) {
                        if (strcmp(attr_valueType, string) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        if (!attr_value) {
                            printf("\e[31mline %d board-no-value\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (!attr_name) {
                            printf("\e[31mline %d option.board no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else {
                        printf("\e[31mline %d unknown superClass %s\e[0m ", __LINE__, attr_superClass);
                        ret = -1;
                    }
                } else if (strcmp(name, targetPlatform) == 0) {
                    char *attr_archList = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"archList");
                    char *attr_binaryParser = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"binaryParser");
                    char *attr_isAbstract = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"isAbstract");
                    char *attr_osList = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"osList");
                    cconfiguration_has.bits.targetPlatform = 1;
                    if (!attr_archList) {
                        printf("\e[31mline %d targetPlatform-no-archList\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_binaryParser) {
                        printf("\e[31mline %d targetPlatform-no-binaryParser\e[0m ", __LINE__);
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
                    free(attr_binaryParser);
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
                    if (!attr_keepEnvironmentInBuildfile) {
                        printf("\e[31mline %d builder-no-keepEnvironmentInBuildfile\e[0m ", __LINE__);
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
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_GNU_CROSS_C_COMPILER) {
                printf("line %d TOOL_GNU_CROSS_C_COMPILER-%s %s ", __LINE__, name, attr_superClass);
                if (strcmp(name, inputType) == 0) {
                    if (strstr(attr_superClass, gnu_cross_c_compiler_input_c) != NULL)
                        strcpy(current_instance->tool_gnu_cross_c_compiler_input, attr_id);
                    else if (strstr(attr_superClass, gnu_cross_c_compiler_input_s) == NULL) {
                        printf("\e[31mline %d cross-c-compiler inputType %s\e[0m ", __LINE__, attr_superClass);
                        ret = -1;
                    }
                    printf(" * ");
                } else if (strcmp(name, option) == 0) {
                    const char * const hack = "fr.ac6.managedbuid.gnu.c.compiler.option";   /* clusterfucks */
                    bool ok = false;
                    if (strncmp(attr_superClass, gnu_c_compiler_option, strlen(gnu_c_compiler_option)) == 0 ||
                        strncmp(attr_superClass, hack, strlen(hack)) == 0)
                    {
                        if (!attr_useByScannerDiscovery) {
                            printf("\e[31mline %d %s no %s\e[0m ", __LINE__, attr_superClass, useByScannerDiscovery);
                            ret = -1;
                        } else
                            ok = true;
                    }
                    if (strcmp(attr_superClass, gnu_c_compiler_option_include_paths) == 0) {
                        cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                        ok = true;
                        if (!attr_IS_BUILTIN_EMPTY) {
                            printf("\e[31mline %d no IS_BUILTIN_EMPTY\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (!attr_IS_VALUE_EMPTY) {
                            printf("\e[31mline %d no IS_VALUE_EMPTY\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, "includePath") != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                    } else if (strcmp(attr_superClass, gnu_c_compiler_option_debugging_level) == 0) {
                        if (!attr_value) {
                            char *attr_defaultValue = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"defaultValue");
                            if (!attr_defaultValue) {
                                printf("\e[31mline %d no defaultValue\e[0m ", __LINE__);
                                ret = -1;
                            }
                            free(attr_defaultValue);
                        }
                        if (strcmp(attr_valueType, enumerated) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strcmp(attr_superClass, gnu_c_compiler_option_preprocessor_def_symbols) == 0) {
                        if (!attr_IS_BUILTIN_EMPTY) {
                            printf("\e[31mline %d no IS_BUILTIN_EMPTY\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (!attr_IS_VALUE_EMPTY) {
                            printf("\e[31mline %d no IS_VALUE_EMPTY\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, "definedSymbols") != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                        printf(" * ");
                    } else if (strstr(attr_superClass, gnu_c_compiler_option_preprocessor_undef_symbol) != NULL) {
                        if (!attr_IS_BUILTIN_EMPTY) {
                            printf("\e[31mline %d no IS_BUILTIN_EMPTY\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (!attr_IS_VALUE_EMPTY) {
                            printf("\e[31mline %d no IS_VALUE_EMPTY\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, "undefDefinedSymbols") != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                        printf(" * ");
                    } else if (strstr(attr_superClass, gnu_c_compiler_option_optimization_level) != NULL) {
                        if (attr_value) {
                            if (strncmp(attr_value, TITLE, strlen(TITLE)) != 0) {
                                printf("\e[31mline %d %s not %s\e[0m ", __LINE__, attr_value, TITLE);
                                ret = -1;
                            }
                            if (strstr(attr_value, "gnu.c.optimization.level") == NULL) {
                                printf("\e[31mline %d %s\e[0m ", __LINE__, attr_value);
                                ret = -1;
                            }
                        }
                    } else if (strstr(attr_superClass, tool_gnu_cross_c_compiler_fdata) != NULL ||
                               strstr(attr_superClass, tool_gnu_cross_c_compiler_ffunction) != NULL ||
                               strstr(attr_superClass, gnu_c_compiler_option_preprocessor_preprocess) != NULL ||
                               strstr(attr_superClass, gnu_c_compiler_option_debugging_prof) != NULL ||
                               strstr(attr_superClass, gnu_c_compiler_option_preprocessor_nostdinc) != NULL)
                    {
                        if (!attr_value) {
                            printf("\e[31mline %d no-value\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, boolean) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                    } else if (strstr(attr_superClass, gnu_c_compiler_option_dialect_std) != NULL) {
                        const char * const gnu_c_compiler_dialect = "gnu.c.compiler.dialect";
                        if (strncmp(attr_value, gnu_c_compiler_dialect, strlen(gnu_c_compiler_dialect)) != 0) {
                            printf("\e[31mline %d value %s != %s\e[0m ", __LINE__, attr_value, gnu_c_compiler_dialect);
                            ret = -1;
                        }
                        if (strcmp(attr_valueType, enumerated) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                    } else if (/*strcmp(attr_superClass, gnu_c_compiler_option_include_paths) != 0*/ !ok) {
                        printf("\e[31mline %d c-compiler-option superClass %s\e[0m ", __LINE__, attr_superClass);
                        ret = -1;
                    }
                } else {
                    printf("\e[31mline %d unknown name %s\e[0m ", __LINE__, name);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_GNU_CROSS_CPP_COMPILER) {
                printf("line %d TOOL_GNU_CROSS_CPP_COMPILER-%s %s ", __LINE__, name, attr_superClass);
                if (strcmp(name, option) == 0) {
                    if (!attr_name) {
                        printf("\e[31mline %d option no-name\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (strstr(attr_superClass, gnu_cpp_compiler_option_optimization_level) != NULL) {
                        cconfiguration_has.bits.cpp_optimization_level = 1;
                    } else if (strcmp(attr_superClass, gnu_cpp_compiler_option_debugging_level) == 0) {
                        cconfiguration_has.bits.cpp_debug_level = 1;
                    } else {
                        printf("\e[31mline %d superClass %se[0m ", __LINE__, attr_superClass);
                        ret = -1;
                    }
                } else {
                    printf("\e[31mline %d unknown name %s\e[0m ", __LINE__, name);
                    ret = -1;
                }
                printf(" * ");
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_GNU_CROSS_C_LINKER) {
                printf("line %d TOOL_GNU_CROSS_C_LINKER-%s %s", __LINE__, name, attr_superClass);
                if (strcmp(name, inputType) == 0) {
                    if (strcmp(attr_superClass, cdt_managedbuild_tool_gnu_c_linker_input) == 0) {
                        cproject_state[depth] = CPROJECT_STATE_ADDITIONALINPUT;
                        printf(" * ");
                    } else {
                        printf("\e[31mline %d linker-input %s\e[0m ", __LINE__, attr_superClass);
                        ret = -1;
                    }
                } else if (strcmp(name, option) == 0) {
                    if (strncmp(attr_superClass, gnu_c_link_option, strlen(gnu_c_link_option)) == 0 ||
                        strncmp(attr_superClass, gnu_cpp_link_option, strlen(gnu_cpp_link_option)) == 0)
                    {
                        bool check_builtin_value = false;
                        const char * vt = boolean;
                        if (strstr(attr_superClass, "link.option.flags") != NULL)
                            vt = string;
                        else if (strstr(attr_superClass, "link.option.userobjs") != NULL) {
                            cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                            vt = userObjs;
                            check_builtin_value = true;
                        } else if (strstr(attr_superClass, "link.option.other") != NULL) {
                            cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                            vt = stringList;
                            check_builtin_value = true;
                        } else if (strstr(attr_superClass, "link.option.libs") != NULL) {
                            cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                            vt = libs;
                            check_builtin_value = true;
                        } else if (strstr(attr_superClass, "link.option.paths") != NULL) {
                            cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                            vt = libPaths;
                            check_builtin_value = true;
                        }

                        if (strcmp(attr_valueType, vt) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                        if (check_builtin_value) {
                            if (!attr_IS_BUILTIN_EMPTY) {
                                printf("\e[31mline %d no IS_BUILTIN_EMPTY\e[0m ", __LINE__);
                                ret = -1;
                            }
                            if (!attr_IS_VALUE_EMPTY) {
                                printf("\e[31mline %d no IS_VALUE_EMPTY\e[0m ", __LINE__);
                                ret = -1;
                            }
                        }
                        if (!attr_useByScannerDiscovery) {
                            printf("\e[31mline %d %s no %s\e[0m ", __LINE__, attr_superClass, useByScannerDiscovery);
                            ret = -1;
                        }
                    } else if (strstr(attr_superClass, tool_gnu_cross_c_linker_script) != NULL) {
                        if (!attr_name) {
                            printf("\e[31mline %d cross-c-compiler no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (!attr_value) {
                            printf("\e[31mline %d cross-c-compiler no-value\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, tool_gnu_cross_c_linker) != NULL) {
                        if (strcmp(attr_valueType, boolean) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                    } else {
                        printf("\e[31mline %d linker-option %s\e[0m ", __LINE__, attr_superClass);
                        ret = -1;
                    }
                } else {
                    printf("\e[31mline %d link name %s\e[0m ", __LINE__, name);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_GNU_CROSS_CPP_LINKER) {
                printf("line %d TOOL_GNU_CROSS_CPP_LINKER-%s %s", __LINE__, name, attr_superClass);
                if (strcmp(name, option) == 0) {
                    if (strncmp(attr_superClass, gnu_cpp_link_option, strlen(gnu_cpp_link_option)) == 0) {
                        if (!attr_useByScannerDiscovery) {
                            printf("\e[31mline %d %s no %s\e[0m ", __LINE__, attr_superClass, useByScannerDiscovery);
                            ret = -1;
                        }
                    }
                    if (strstr(attr_superClass, tool_gnu_cross_cpp_linker_script) != NULL) {
                        if (!attr_name) {
                            printf("\e[31mline %d no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        if (!attr_value) {
                            printf("\e[31mline %d no-value\e[0m ", __LINE__);
                            ret = -1;
                        }
                    }
                    printf(" * ");
                } else if (strcmp(name, inputType) == 0) {
                    if (strcmp(attr_superClass, cdt_managedbuild_tool_gnu_cpp_linker_input) == 0) {
                        cproject_state[depth] = CPROJECT_STATE_ADDITIONALINPUT;
                        printf(" * ");
                    } else {
                        printf("\e[31mline %d linker-input %s\e[0m ", __LINE__, attr_superClass);
                        ret = -1;
                    }
                } else {
                    printf("\e[31mline %d cpp-link name %s\e[0m ", __LINE__, name);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_GNU_ARCHIVER) {
                printf("\e[31mline %d TOOL_GNU_ARCHIVER-%s\e[0m ", __LINE__, name);
                ret = -1;   /* unknown archiver element */
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_GNU_CROSS_ASSEMBLER) {
                printf("line %d TOOL_GNU_CROSS_ASSEMBLER-%s %s ", __LINE__, name, attr_superClass);
                if (strcmp(name, inputType) == 0) {
                    if (strcmp(attr_superClass, cdt_managedbuild_tool_gnu_assembler_input) == 0) {
                        cconfiguration_has.bits.tool_gnu_assembler_input = 1;
                    } else if (has_title) {
                        if (strstr(attr_superClass, tool_gnu_cross_assembler_input) != NULL) {
                            cconfiguration_has.bits.tool_gnu_cross_assembler_input = 1;
                        }
                    }
                } else if (strcmp(name, option) == 0) {
                    if (strcmp(attr_superClass, gnu_both_asm_option_include_paths) == 0) {
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
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                    } else {
                        if (strcmp(attr_valueType, boolean) != 0) {
                            printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                            ret = -1;
                        }
                    }
                } else {
                    printf("\e[31mline %d name %s\e[0m ", __LINE__, name);
                    ret = -1;
                }
                printf(" * ");
            } else if (cproject_state[depth-1] == CPROJECT_STATE_SOURCEENTRIES) {
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
                    char *attr_projectType = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"projectType");
                    printf("line %d STORAGEMODULE_CDTBUILDSYSTEM-%s\e[0m ", __LINE__, name);
                    if (!attr_name) {
                        printf("\e[31mline %d project  no-name\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (strstr(attr_projectType, target_gnu_cross_exe) == NULL) {
                        printf("\e[31mline %d project no %s in %s\e[0m ", __LINE__, target_gnu_cross_exe, attr_projectType);
                        ret = -1;
                    }
                    free(attr_projectType);
                    printf(" * ");
                } else if (cproject_state[depth-1] == CPROJECT_STATE_STORAGEMODULE_SCANNERCONFIGURATION) {
                    printf("line %d STORAGEMODULE_SCANNERCONFIGURATION-%s ", __LINE__, name);
                    if (strcmp(name, scannerConfigBuildInfo) == 0) {
                        cproject_state[depth] = CPROJECT_STATE_SCANNERCONFIGBUILDINFO;
                        char *attr_instanceId = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"instanceId");
                        if (attr_instanceId) {
                            struct node_s *my_list;
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
                                /*else {
                                    printf("\nline %d:\n%s\n%s\n\n", __LINE__, str, attr_instanceId);
                                    ret = -1;
                                }*/

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
                        printf("\e[7mcconfiguration id %s ((prefix %s))\e[0m ", attr_id, cconfiguration_id_prefix);

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
            printf("\e[42mcconfiguration_has.dword : %x\e[0m ", cconfiguration_has.dword);
            in_cconfiguration = false;
            cconfiguration_name[0] = 0;
            current_instance = NULL;
            if (cconfiguration_has.dword != 0x3ffff) {
                show_cconfiguration_missing(cconfiguration_has);
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

