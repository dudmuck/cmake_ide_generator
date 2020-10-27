#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <libxml/xmlreader.h>
#include "xml.h"

/*
 * https://www.ti.com/tool/CCSTUDIO
 */

const char * const TITLE = "com.ti.ccstudio";
const char * const TITLE_buildDefinitions = "com.ti.ccstudio.buildDefinitions";
const char * const settings = "org.eclipse.cdt.core.settings";
const char * const builder = "builder";
const char * const tool = "tool";
const char * const project = "project";
const char * const sourceEntries = "sourceEntries";
const char * const entry = "entry";
const char * const XmlProjectDescriptionStorage = "org.eclipse.cdt.core.XmlProjectDescriptionStorage";
const char * const targetPlatform = "targetPlatform";
const char * const listOptionValue = "listOptionValue";
const char * const inputType = "inputType";
//const char * const enumerated = "enumerated";
const char * const includePath = "includePath";
const char * const includeFiles = "includeFiles";
const char * const libPaths = "libPaths";
const char * const libs = "libs";
const char * const definedSymbols = "definedSymbols";
const char * const boolean = "boolean";
const char * const stringList = "stringList";
const char * const string = "string";
//const char * const storageModule = "storageModule";
const char * const cconfiguration = "cconfiguration";
const char * const configuration = "configuration";
const char * const externalSettings = "externalSettings";
const char * const extensions = "extensions";
const char * const extension = "extension";
const char * const cdtBuildSystem = "cdtBuildSystem";
const char * const folderInfo = "folderInfo";
const char * const toolChain = "toolChain";
//const char * const option = "option";
const char * const org_eclipse_cdt_core = "org.eclipse.cdt.core";
const char * const org_eclipse_cdt_core_BinaryParser = "org.eclipse.cdt.core.BinaryParser";
const char * const org_eclipse_cdt_core_ErrorParser = "org.eclipse.cdt.core.ErrorParser";
const char * const com_ti_ccstudio_binaryparser_CoffParser = "com.ti.ccstudio.binaryparser.CoffParser";
const char * const org_eclipse_cdt_core_GmakeErrorParser = "org.eclipse.cdt.core.GmakeErrorParser";
const char * const com_ti_ccstudio_errorparser_CoffErrorParser = "com.ti.ccstudio.errorparser.CoffErrorParser";
const char * const com_ti_ccstudio_errorparser_AsmErrorParser = "com.ti.ccstudio.errorparser.AsmErrorParser";
const char * const com_ti_ccstudio_errorparser_LinkErrorParser = "com.ti.ccstudio.errorparser.LinkErrorParser";
const char * const configurationDataProvider = "org.eclipse.cdt.managedbuilder.core.configurationDataProvider";

const char * const fpMode[] = {
    ".relaxed",
    ".strict",
    NULL,
};

const char * const optForSpeed[] = {
    ".0",
    ".1",
    ".2",
    ".3",
    ".4",
    ".5",
    NULL,
};

const char * const optLevels[] = {
    ".0",
    ".1",
    ".2",
    ".3",
    ".4",
    NULL,
};

const char * const floatSupports[] = {
    ".VFPv2",
    ".VFPv3",
    ".VFPv3D16",
    ".FPv4SPD16",
    NULL,
};

const char * const ABIs[] = {
    ".eabi",
    NULL,
};

const char * const codeStates[] = {
    "16",
    "32",
    NULL,
};

const char * const siliconVersionsMSP430[] = {
    "mspx",
    NULL,
};

const char * const siliconVersionsARM[] = {
    ".4",
    ".5e",
    ".6",
    ".6M0",
    ".7A8",
    ".7M3",
    ".7M4",
    ".7R4",
    ".7R5",
    NULL,
};

typedef union {
    struct {
        uint32_t settings                  : 1;    // 0
        uint32_t cdtBuildSystem            : 1;    // 1
    } bits;
    uint32_t dword;
} cproject_has_t;

typedef enum {
    /*  0 */ CPROJECT_STATE_NONE = 0,
    /*  1 */ CPROJECT_STATE_CDT_SETTINGS,
    /*  2 */ CPROJECT_STATE_CCONFIGURATION,
    /*  3 */ CPROJECT_STATE_CCONFIGURATION_SETTINGS,
    /*  4 */ CPROJECT_STATE_EXTENSIONS,
    /*  5 */ CPROJECT_STATE_STORAGEMODULE_CDTBUILDSYSTEM,
    /*  6 */ CPROJECT_STATE_CONFIGURATION,
    /*  7 */ CPROJECT_STATE_FOLDERINFO,
    /*  8 */ CPROJECT_STATE_TOOLCHAIN,
    /*  9 */ CPROJECT_STATE_LISTOPTIONVALUE,
    /* 10 */ CPROJECT_STATE_TOOL_COMPILER,
    /* 11 */ CPROJECT_STATE_TOOL_LINKER,
    /* 12 */ CPROJECT_STATE_TOOL_HEX,
    /* 13 */ CPROJECT_STATE_SOURCEENTRIES,
} cproject_state_e;


typedef union {
    struct {
        uint32_t settings                  : 1; // 0
        uint32_t externalSettings          : 1; // 1
        uint32_t CoffParser                : 1; // 2
        uint32_t GmakeErrorParser          : 1; // 3
        uint32_t CoffErrorParser           : 1; // 4
        uint32_t AsmErrorParser            : 1; // 5
        uint32_t LinkErrorParser           : 1; // 6
        uint32_t configuration             : 1; // 7
        uint32_t folderInfo                : 1; // 8
        uint32_t toolChain                 : 1; // 9
        uint32_t targetTool                : 1; // 10
        uint32_t OPT_TAGS                  : 1; // 11
        uint32_t OPT_CODEGEN_VERSION       : 1; // 12
        uint32_t targetPlatform            : 1; // 13
        uint32_t configurationDataProvider : 1; // 14
        uint32_t builder                   : 1; // 15
        uint32_t tool_compiler             : 1; // 16
        uint32_t SILICON_VERSION           : 1; // 17
        uint32_t CODE_STATE                : 1; // 18
        uint32_t ABI                       : 1; // 19
        uint32_t DEFINE                    : 1; // 20
        uint32_t INCLUDE_PATH              : 1; // 21
        uint32_t DEBUGGING_MODEL           : 1; // 22
        uint32_t tool_linker               : 1; // 23
        uint32_t tool_hex                  : 1; // 24
        uint32_t inputType__C_SRCS         : 1; // 25
        uint32_t inputType__CPP_SRCS       : 1; // 26
        uint32_t inputType__ASM_SRCS       : 1; // 27
        uint32_t inputType__ASM2_SRCS      : 1; // 28
        uint32_t inputType__CMD_SRCS       : 1; // 29
        uint32_t inputType__CMD2_SRCS      : 1; // 30
        uint32_t inputType__GEN_CMDS       : 1; // 31
    } bits;
    uint32_t dword;
} cconfiguration_has_t;

const char * const MSP432 = "MSP432";
const char * const MSP430 = "MSP430";
typedef enum {
    ARCH_NONE = 0,
    ARCH_MSP430,
    ARCH_ARM,
} arch_t;

cproject_has_t cproject_has;
#define CPROJECT_DEPTH       12
cproject_state_e cproject_state[CPROJECT_DEPTH];
char storageModuleId[CPROJECT_DEPTH][128];
char cconfiguration_id[128];
char cconfiguration_targetTool[128];
char cconfiguration_name[32];

void print_cconfiguration_missing(cconfiguration_has_t _cch, uint32_t expected)
{
    cconfiguration_has_t failed;

    failed.dword = _cch.dword ^ expected;

    printf("fail: ");
    if (failed.bits.settings)
        printf("settings ");
    if (failed.bits.externalSettings)
        printf("externalSettings ");
    if (failed.bits.CoffParser)
        printf("CoffParser ");
    if (failed.bits.GmakeErrorParser)
        printf("GmakeErrorParser ");
    if (failed.bits.CoffErrorParser)
        printf("CoffErrorParser ");
    if (failed.bits.AsmErrorParser)
        printf("AsmErrorParser ");
    if (failed.bits.LinkErrorParser)
        printf("LinkErrorParser ");
    if (failed.bits.configuration)
        printf("configuration ");
    if (failed.bits.folderInfo)
        printf("folderInfo ");
    if (failed.bits.toolChain)
        printf("toolChain ");
    if (failed.bits.targetTool)
        printf("targetTool ");
    if (failed.bits.OPT_TAGS)
        printf("OPT_TAGS ");
    if (failed.bits.OPT_CODEGEN_VERSION)
        printf("OPT_CODEGEN_VERSION ");
    if (failed.bits.targetPlatform)
        printf("targetPlatform ");
    if (failed.bits.configurationDataProvider)
        printf("configurationDataProvider ");
    if (failed.bits.builder)
        printf("builder ");
    if (failed.bits.tool_compiler)
        printf("tool_compiler ");
    if (failed.bits.SILICON_VERSION)
        printf("SILICON_VERSION ");
    if (failed.bits.CODE_STATE)
        printf("CODE_STATE ");
    if (failed.bits.ABI)
        printf("ABI ");
    if (failed.bits.DEFINE)
        printf("DEFINE ");
    if (failed.bits.INCLUDE_PATH)
        printf("INCLUDE_PATH ");
    if (failed.bits.DEBUGGING_MODEL)
        printf("DEBUGGING_MODEL ");
    if (failed.bits.tool_linker)
        printf("tool_linker ");
    if (failed.bits.tool_hex)
        printf("tool_hex ");
    if (failed.bits.inputType__C_SRCS)
        printf("inputType__C_SRCS ");
    if (failed.bits.inputType__CPP_SRCS)
        printf("inputType__CPP_SRCS ");
    if (failed.bits.inputType__ASM_SRCS)
        printf("inputType__ASM_SRCS ");
    if (failed.bits.inputType__ASM2_SRCS)
        printf("inputType__ASM2_SRCS ");
    if (failed.bits.inputType__CMD_SRCS)
        printf("inputType__CMD_SRCS ");
    if (failed.bits.inputType__CMD2_SRCS)
        printf("inputType__CMD2_SRCS ");
    if (failed.bits.inputType__GEN_CMDS)
        printf("inputType__GEN_CMDS ");

    printf("\r\n");
}

static int processNode(xmlTextReaderPtr reader)
{
    static char title_toolchain[96];
    static const char *mcu_family = NULL;
    static cconfiguration_has_t cconfiguration_has;
    const char *name_in, *value_in = NULL;
    int ret = 0;
    int depth, nodeType = xmlTextReaderNodeType(reader);
    static arch_t arch = ARCH_NONE;
    if (nodeType == XML_NODE_TYPE_WHITE_SPACE)
        return 0;   // white space

    name_in = (char*)xmlTextReaderConstName(reader);
    depth = xmlTextReaderDepth(reader);

    printf("depth%d state:%d ", depth, cproject_state[depth]);
    if (depth > 0)
        printf(" ps:%d ", cproject_state[depth-1]);

    printf(" %s \"%s\" %s %s", 
	    nodeTypeToString(nodeType),
	    name_in,
	    xmlTextReaderIsEmptyElement(reader) ? "empty" : " - ",
	    xmlTextReaderHasValue(reader) ? "value" : " - "
    );

    if (xmlTextReaderHasValue(reader)) {
        value_in = (char*)xmlTextReaderConstValue(reader);
        printf(" %s ", value_in);
    }

    if (nodeType == XML_NODE_TYPE_START_ELEMENT) {
        if (depth == 0) {
            if (strcmp(name_in, "cproject") == 0) {
                char *attr_storage_type_id = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"storage_type_id");
                if (strcmp(XmlProjectDescriptionStorage, attr_storage_type_id) != 0) {
                    printf("\e[31mline %d storage_type_id\e[0m ", __LINE__);
                    ret = -1;
                }
                free(attr_storage_type_id);
            } else {
                printf("\e[31mline %d depth==0\e[0m ", __LINE__);
                ret = -1;
            }
            title_toolchain[0] = 0;
        } else {    /* depth > 0: */
            bool check_is_empty = false;
            const char *check_valueType = NULL;
            char *attr_superClass = (char*)xmlTextReaderGetAttribute(reader, superClass);
            char *attr_name = (char*)xmlTextReaderGetAttribute(reader, name);
            char *attr_id = (char*)xmlTextReaderGetAttribute(reader, id);
            char *attr_valueType = (char*)xmlTextReaderGetAttribute(reader, valueType);
            char *attr_value = (char*)xmlTextReaderGetAttribute(reader, value);
            char *attr_useByScannerDiscovery = NULL;
            char *attr_IS_BUILTIN_EMPTY = NULL;
            char *attr_IS_VALUE_EMPTY = NULL;

            if (attr_id) {
                if (title_toolchain[0] != 0) {
                    const char *title = title_toolchain;
                    if (strstr(attr_id, ".core.OPT_TAGS") != NULL ||
                        strstr(attr_id, ".core.OPT_CODEGEN_VERSION") != NULL)
                    {
                        /* id's in toolChain without mcuFamily_toolVersion */
                        title = TITLE_buildDefinitions;
                    }

                    if (strncmp(attr_id, title, strlen(title)) != 0) {
                        printf("\e[31mline %d id title_toolchain\e[0m ", __LINE__);
                        ret = -1;
                    }
                } else {
                    const char *title = TITLE_buildDefinitions;
                    if (depth > 0 && cproject_state[depth-1] == CPROJECT_STATE_EXTENSIONS)
                        title = TITLE;

                    if (strcmp(name_in, project) != 0 &&
                        strncmp(attr_id, title, strlen(title)) != 0 &&
                        strncmp(attr_id, org_eclipse_cdt_core, strlen(org_eclipse_cdt_core)) != 0)
                    {
                        printf("\e[31mline %d id\e[0m ", __LINE__);
                        ret = -1;
                    }
                }
            } else if (strcmp(name_in, (char*)option) == 0 || strcmp(name_in, project) == 0) {
                printf("\e[31mline %d no id\e[0m ", __LINE__);
                ret = -1;
                return ret; /* would crash */
            }

            if (attr_superClass) {
                if (!attr_id || strncmp(attr_superClass, attr_id, strlen(attr_superClass)) != 0) {
                    printf("\nline %d superClass:\n%s\n%s\n\n", __LINE__, attr_id, attr_superClass);
                    ret = -1;
                }
            }

            if (strcmp(name_in, (char*)option) == 0) {
                attr_useByScannerDiscovery = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)useByScannerDiscovery);
                attr_IS_BUILTIN_EMPTY = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"IS_BUILTIN_EMPTY");
                attr_IS_VALUE_EMPTY = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"IS_VALUE_EMPTY");
                if (!attr_superClass) {
                    printf("\e[31mline %d id without superClass %s\e[0m ", __LINE__, attr_id);
                    ret = -1;
                    return ret; /* would crash */
                }
            }

            if (attr_valueType) {
                if (strcmp(attr_valueType, boolean) == 0) {
                    if (strcmp(attr_value, "true") != 0 && strcmp(attr_value, "false") != 0) {
                        printf("\e[31mline %d boolean not true or false\e[0m ", __LINE__);
                        ret = -1;
                    }
                } else if (strcmp(attr_valueType, (char*)enumerated) == 0) {
                    if (strncmp(attr_value, attr_superClass, strlen(attr_superClass)) != 0) {
                        printf("\e[31mline %d bad enumerated value\e[0m ", __LINE__);
                        ret = -1;
                    }
                } else if (strcmp(attr_valueType, stringList) == 0 ||
                           strcmp(attr_valueType, definedSymbols) == 0 ||
                           strcmp(attr_valueType, libPaths) == 0 ||
                           strcmp(attr_valueType, libs) == 0 ||
                           strcmp(attr_valueType, includeFiles) == 0 ||
                           strcmp(attr_valueType, includePath) == 0)
                {
                    cproject_state[depth] = CPROJECT_STATE_LISTOPTIONVALUE;
                } else if (strcmp(attr_valueType, string) == 0) {
                    if (!attr_value) {
                        printf("\e[31mline %d string-no-value\e[0m ", __LINE__);
                        ret = -1;
                    }
                } else {
                    printf("\e[31mline %d valueType %s\e[0m ", __LINE__, attr_valueType);
                    ret = -1;
                }
            }

            if (strcmp((char*)storageModule, name_in) == 0) {
                char *attr_moduleId = (char*)xmlTextReaderGetAttribute(reader, moduleId);
                strcpy(storageModuleId[depth], attr_moduleId);
                printf(" \e[33mstart-%s\e[0m ", storageModuleId[depth]);
                if (strcmp(attr_moduleId, settings) == 0) {
                    if (cproject_state[depth-1] == CPROJECT_STATE_CCONFIGURATION) {
                        char *attr_buildSystemId = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"buildSystemId");
                        if (strcmp(attr_buildSystemId, configurationDataProvider) == 0)
                            cconfiguration_has.bits.configurationDataProvider = 1;
                        if (strstr(attr_id, attr_name) == NULL) {
                            printf("\e[31mline %d %s not in %s\e[0m ", __LINE__, attr_name, attr_id);
                            ret = -1;
                        }
                        strcpy(cconfiguration_name, attr_name);
                        cconfiguration_has.bits.settings = 1;
                        cproject_state[depth] = CPROJECT_STATE_CCONFIGURATION_SETTINGS;
                        free(attr_buildSystemId);
                    } else
                        cproject_state[depth] = CPROJECT_STATE_CDT_SETTINGS;
                } else if (strcmp(attr_moduleId, cdtBuildSystem) == 0) {
                    char *version = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"version");
                    cproject_state[depth] = CPROJECT_STATE_STORAGEMODULE_CDTBUILDSYSTEM;
                    if (strcmp(version, "4.0.0") != 0) {
                        printf("\e[31mline %d wrong cdtBuildSystem version %s\e[0m ", __LINE__, version);
                        ret = -1;
                    }
                    if (depth == 1)
                        cproject_has.bits.cdtBuildSystem = 1;   /* cdtBuildSystem outside of cconfiguration */
                    free(version);
                }

                free(attr_moduleId);
            } else if (cproject_state[depth-1] == CPROJECT_STATE_CDT_SETTINGS && strcmp(cconfiguration, name_in) == 0) {
                cproject_state[depth] = CPROJECT_STATE_CCONFIGURATION;
                cconfiguration_has.dword = 0;
                cconfiguration_targetTool[0] = 0;
                cconfiguration_name[0] = 0;
                strcpy(cconfiguration_id, attr_id);
                if (strstr(attr_id, MSP432) != NULL) {
                    arch = ARCH_ARM;
                    mcu_family = MSP432;
                } else if (strstr(attr_id, MSP430) != NULL) {
                    arch = ARCH_MSP430;
                    mcu_family = MSP430;
                } else {
                    mcu_family = NULL;
                    printf("\e[31mline %d cannot find mcu arch %s\e[0m ", __LINE__, attr_id);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_CCONFIGURATION_SETTINGS) {
                if (strcmp(externalSettings, name_in) == 0)
                    cconfiguration_has.bits.externalSettings = 1;
                else if (strcmp(extensions, name_in) == 0) {
                    cproject_state[depth] = CPROJECT_STATE_EXTENSIONS;
                } else {
                    printf("\e[31mline %d %s\e[0m ", __LINE__, name_in);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_EXTENSIONS) {
                if (strcmp(extension, name_in) == 0) {
                    char *attr_point = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"point");
                    printf( " id = %s ", attr_id);
                    if (strcmp(attr_point, org_eclipse_cdt_core_BinaryParser) == 0) {
                        if (strcmp(attr_id, com_ti_ccstudio_binaryparser_CoffParser) == 0)
                            cconfiguration_has.bits.CoffParser = 1;
                        else {
                            printf("\e[31mline %d %s\e[0m ", __LINE__, name_in);
                            ret = -1;
                        }
                    } else if (strcmp(attr_point, org_eclipse_cdt_core_ErrorParser) == 0) {
                        if (strcmp(attr_id, org_eclipse_cdt_core_GmakeErrorParser) == 0)
                            cconfiguration_has.bits.GmakeErrorParser = 1;
                        else if (strcmp(attr_id, com_ti_ccstudio_errorparser_CoffErrorParser) == 0)
                            cconfiguration_has.bits.CoffErrorParser = 1;
                        else if (strcmp(attr_id, com_ti_ccstudio_errorparser_AsmErrorParser) == 0)
                            cconfiguration_has.bits.AsmErrorParser = 1;
                        else if (strcmp(attr_id, com_ti_ccstudio_errorparser_LinkErrorParser) == 0)
                            cconfiguration_has.bits.LinkErrorParser = 1;
                        else {
                            printf("\e[31mline %d id=%s point=%s\e[0m ", __LINE__, attr_id, attr_point);
                            ret = -1;
                        }
                    } else {
                        printf("\e[31mline %d %s\e[0m ", __LINE__, name_in);
                        ret = -1;
                    }
                    free(attr_point);
                } else {
                    printf("\e[31mline %d %s\e[0m ", __LINE__, name_in);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_CONFIGURATION) {
                if (strcmp(name_in, folderInfo) == 0) {
                    char id[192];
                    char *attr_resourcePath = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"resourcePath");
                    strcpy(id, cconfiguration_id);
                    strcat(id, ".");
                    if (!attr_resourcePath) {
                        printf("\e[31mline %d no resourcePath\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_name) {
                        printf("\e[31mline %d no name\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (strcmp(id, attr_id) != 0) {
                        printf("\e[31mline %d id\n%s\n%s\n\e[0m ", __LINE__, id, attr_id);
                        ret = -1;
                    }
                    if (ret != -1)
                        cconfiguration_has.bits.folderInfo = 1;

                    free(attr_resourcePath);
                    cproject_state[depth] = CPROJECT_STATE_FOLDERINFO;
                    title_toolchain[0] = 0;
                    printf(" * ");
                } else if (strcmp(name_in, sourceEntries) == 0) {
                    cproject_state[depth] = CPROJECT_STATE_SOURCEENTRIES;
                } else {
                    printf("\e[31mline %d name %s\e[0m ", __LINE__, name_in);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_SOURCEENTRIES) {
                printf("line %d SOURCEENTRIES-%s\e[0m ", __LINE__, name_in);
                if (strcmp(name_in, entry) == 0) {
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
            } else if (cproject_state[depth-1] == CPROJECT_STATE_STORAGEMODULE_CDTBUILDSYSTEM) {
                printf("line %d STORAGEMODULE_CDTBUILDSYSTEM-%s\e[0m ", __LINE__, name_in);
                if (strcmp(name_in, configuration) == 0) {
                    char *attr_artifactExtension = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"artifactExtension");
                    char *attr_artifactName = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"artifactName");
                    char *attr_buildProperties = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"buildProperties");
                    char *attr_cleanCommand = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"cleanCommand");
                    char *attr_description = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"description");
                    char *attr_parent = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"parent");
                    if (!attr_artifactExtension) {
                        printf("\e[31mline %d no \e[0m artifactExtension", __LINE__);
                        ret = -1;
                    }
                    if (!attr_artifactName) {
                        printf("\e[31mline %d no artifactName\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_buildProperties) {
                        printf("\e[31mline %d no buildProperties\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_cleanCommand) {
                        printf("\e[31mline %d no cleanCommand\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_description) {
                        printf("\e[31mline %d no description\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_name || strcmp(attr_name, cconfiguration_name) != 0) {
                        printf("\e[31mline %d  name not %s\e[0m ", __LINE__, cconfiguration_name);
                        ret = -1;
                    }
                    if (!attr_parent) {
                        printf("\e[31mline %d no parent\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_id || strcmp(cconfiguration_id, attr_id) != 0) {
                        printf("\e[31mline %d id\n%s\n%s\n\e[0m ", __LINE__, cconfiguration_id, attr_id);
                        ret = -1;
                    }
                    if (ret != -1)
                        cconfiguration_has.bits.configuration = 1;

                    free(attr_artifactExtension);
                    free(attr_artifactName);
                    free(attr_buildProperties);
                    free(attr_cleanCommand);
                    free(attr_description);
                    free(attr_parent);
                    cproject_state[depth] = CPROJECT_STATE_CONFIGURATION;
                    printf(" * ");
                } else if (strcmp(name_in, project) == 0) {
                    char *attr_projectType = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"projectType");
                    if (strstr(attr_id, attr_projectType) == NULL) {
                        printf("\nline %d projectType:\n%s\n%s\n\n", __LINE__, attr_id, attr_projectType);
                        ret = -1;
                    }
                    free(attr_projectType);
                    printf(" * ");
                } else {
                    printf("\e[31mline %d name %s\e[0m ", __LINE__, name_in);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_FOLDERINFO) {
                printf(" line %d FOLDERINFO-%s %s ", __LINE__, name_in, attr_superClass);
                if (strcmp(name_in, toolChain) == 0) {
                    char *attr_targetTool = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"targetTool");
                    cconfiguration_has.bits.toolChain = 1;
                    if (!attr_name) {
                        printf("\e[31mline %d no name\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (strstr(attr_superClass, mcu_family) == NULL) {
                        printf("\e[31mline %d superClass %s not in %s\e[0m ", __LINE__, mcu_family, attr_superClass);
                        ret = -1;
                    }
                    if (strstr(attr_superClass, cconfiguration_name) == NULL) { // i.e. contains Debug or Release
                        printf("\e[31mline %d superClass %s not in %s\e[0m ", __LINE__, cconfiguration_name, attr_superClass);
                        ret = -1;
                    }
                    if (attr_targetTool == NULL || strstr(attr_targetTool, cconfiguration_name) == NULL) {
                        printf("\e[31mline %d targetTool %s not in %s\e[0m ", __LINE__, cconfiguration_name, attr_targetTool);
                        ret = -1;
                    }

                    if (ret != -1) {
                        const char *endPtr = strstr(attr_superClass, ".exe.");
                        if (endPtr == NULL) {
                            printf("\e[31mline %d no .exe. %s\e[0m ", __LINE__, attr_superClass);
                            ret = -1;
                        } else {
                            unsigned length = (endPtr - attr_superClass) + 1;   // +1 for trailing dot
                            strncpy(title_toolchain, attr_superClass, length);
                            title_toolchain[length] = 0;
                            printf("title_toolchain \"%s\" ", title_toolchain);
                        }
                    }
                    if (attr_targetTool) {
                        cconfiguration_has.bits.targetTool = 1;
                        strcpy(cconfiguration_targetTool, attr_targetTool);
                        free(attr_targetTool);
                    }
                    cproject_state[depth] = CPROJECT_STATE_TOOLCHAIN;
                    printf(" * ");
                } else {
                    printf("\e[31mline %d name %s\e[0m ", __LINE__, name_in);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_LISTOPTIONVALUE) {
                printf("line %d LISTOPTIONVALUE-%s %s ", __LINE__, name_in, attr_superClass);
                if (strcmp(name_in, listOptionValue) == 0) {
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
                    printf("\e[31mline %d unknown name %s\e[0m ", __LINE__, name_in);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOLCHAIN) {
                printf(" line %d TOOLCHAIN-%s %s ", __LINE__, name_in, attr_superClass);
                if (strcmp(name_in, (char*)option) == 0) {
                    if (strstr(attr_id, "OPT_TAGS") != NULL) {
                        cconfiguration_has.bits.OPT_TAGS = 1;
                        check_is_empty = true;
                        check_valueType = stringList;
                    } else if (strstr(attr_id, "OPT_CODEGEN_VERSION") != NULL) {
                        cconfiguration_has.bits.OPT_CODEGEN_VERSION = 1;
                        check_valueType = string;
                        printf(" * ");
                    } else {
                        printf("\e[31mline %d toolchain-option\e[0m ", __LINE__);
                        ret = -1;
                    }
                } else if (strcmp(name_in, targetPlatform) == 0) {
                    if (!attr_name) {
                        printf("\e[31mline %d no-name\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (strstr(attr_superClass, cconfiguration_name) == NULL) {
                        printf("\e[31mline %d %s not in %s\e[0m ", __LINE__, cconfiguration_name, attr_superClass);
                        ret = -1;
                    }
                    cconfiguration_has.bits.targetPlatform = 1;
                    printf(" * ");
                } else if (strcmp(name_in, builder) == 0) {
                    char *attr_buildPath = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"buildPath");
                    char *attr_keepEnvironmentInBuildfile = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"keepEnvironmentInBuildfile");
                    char *attr_parallelBuildOn = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"parallelBuildOn");
                    char *attr_parallelizationNumber = (char*)xmlTextReaderGetAttribute(reader, (xmlChar*)"parallelizationNumber");
                    if (strstr(attr_superClass, cconfiguration_name) == NULL) {
                        printf("\e[31mline %d %s not in %s\e[0m ", __LINE__, cconfiguration_name, attr_superClass);
                        ret = -1;
                    }
                    if (!attr_buildPath) {
                        printf("\e[31mline %d no buildPath\e[0m ", __LINE__);
                        ret = -1;
                    }
                    /* only in Debug : if (!attr_keepEnvironmentInBuildfile) {
                        printf("\e[31mline %d no keepEnvironmentInBuildfile\e[0m ", __LINE__);
                        ret = -1;
                    }*/
                    if (!attr_parallelBuildOn) {
                        printf("\e[31mline %d no parallelBuildOn\e[0m ", __LINE__);
                        ret = -1;
                    }
                    if (!attr_parallelizationNumber) {
                        printf("\e[31mline %d no parallelizationNumber\e[0m ", __LINE__);
                        ret = -1;
                    }
                    cconfiguration_has.bits.builder = 1;
                    free(attr_buildPath);
                    free(attr_keepEnvironmentInBuildfile);
                    free(attr_parallelBuildOn);
                    free(attr_parallelizationNumber);
                    printf(" * ");
                } else if (strcmp(name_in, tool) == 0) {
                    char str_compiler[64];
                    char str_linker[64];
                    strcpy(str_compiler, "compiler");
                    strcat(str_compiler, cconfiguration_name);

                    strcpy(str_linker, "linker");
                    strcat(str_linker, cconfiguration_name);
                    if (strstr(attr_superClass, str_compiler) != NULL) {
                        cconfiguration_has.bits.tool_compiler = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_COMPILER;
                        if (!attr_name) {
                            printf("\e[31mline %d no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, str_linker) != NULL) {
                        cconfiguration_has.bits.tool_linker = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_LINKER;
                        if (!attr_name) {
                            printf("\e[31mline %d no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else if (strstr(attr_superClass, "hex") != NULL) {
                        cconfiguration_has.bits.tool_hex = 1;
                        cproject_state[depth] = CPROJECT_STATE_TOOL_HEX;
                        if (!attr_name) {
                            printf("\e[31mline %d no-name\e[0m ", __LINE__);
                            ret = -1;
                        }
                        printf(" * ");
                    } else {
                        printf("\e[31mline %d tool\e[0m ", __LINE__);
                        ret = -1;
                    }
                } else {
                    printf("\e[31mline %d name %s\e[0m ", __LINE__, name_in);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_HEX) {
                printf(" line %d TOOL_HEX-%s %s ", __LINE__, name_in, attr_superClass);
                if (strcmp(name_in, (char*)option) == 0) {
                    if (strstr(attr_superClass, "hex") != NULL) {
                        if (strstr(attr_superClass, "ROMWIDTH") != NULL) {
                            check_valueType = string;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "MEMWIDTH") != NULL) {
                            check_valueType = string;
                            printf(" * ");
                        } else {
                            printf("\e[31mline %d hex?\e[0m ", __LINE__);
                            ret = -1;
                        }
                    } else {
                        printf("\e[31mline %d not hex\e[0m ", __LINE__);
                        ret = -1;
                    }
                } else {
                    printf("\e[31mline %d name %s\e[0m ", __LINE__, name_in);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_LINKER) {
                printf(" line %d TOOL_LINKER-%s %s ", __LINE__, name_in, attr_superClass);
                if (strcmp(name_in, (char*)option) == 0) {
                    if (strstr(attr_superClass, "linkerID") != NULL) {
                        if (strstr(attr_superClass, "MAP_FILE") != NULL) {
                            check_valueType = string;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "STACK_SIZE") != NULL) {
                            check_valueType = string;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "HEAP_SIZE") != NULL) {
                            check_valueType = string;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "OUTPUT_FILE") != NULL) {
                            check_valueType = string;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "XML_LINK_INFO") != NULL) {
                            check_valueType = string;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "DISPLAY_ERROR_NUMBER") != NULL) {
                            check_valueType = boolean;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "DIAG_WRAP") != NULL) {
                            check_valueType = (char*)enumerated;
                            printf(" *D* ");
                        } else if (strstr(attr_superClass, "SEARCH_PATH") != NULL) {
                            check_valueType = libPaths;
                            check_is_empty = true;
                            printf(" *S* ");
                        } else if (strstr(attr_superClass, "LIBRARY") != NULL) {
                            check_valueType = libs;
                            check_is_empty = true;
                            printf(" *L* ");
                        } else if (strstr(attr_superClass, "DEFINE") != NULL) { // only for msp430?
                            check_valueType = definedSymbols;
                            check_is_empty = true;
                        } else if (strstr(attr_superClass, "USE_HW_MPY") != NULL) { // only for msp430?
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "CINIT_HOLD_WDT") != NULL) { // only for msp430?
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "PRIORITY") != NULL) { // only for msp430?
                            check_valueType = boolean;
                        } else {
                            printf("\e[31mline %d linkerID ?\e[0m ", __LINE__);
                            ret = -1;
                        }
                    } else {
                        printf("\e[31mline %d not linkerID\e[0m ", __LINE__);
                        ret = -1;
                    }
                } else if (strcmp(name_in, inputType) == 0) {
                    if (strstr(attr_superClass, "exeLinker") != NULL) {
                        if (strstr(attr_superClass, "inputType__CMD_SRCS") != NULL) {
                            cconfiguration_has.bits.inputType__CMD_SRCS = 1;
                            printf(" *I1* ");
                        } else if (strstr(attr_superClass, "inputType__CMD2_SRCS") != NULL) {
                            cconfiguration_has.bits.inputType__CMD2_SRCS = 1;
                            printf(" *I2* ");
                        } else if (strstr(attr_superClass, "inputType__GEN_CMDS") != NULL) {
                            cconfiguration_has.bits.inputType__GEN_CMDS = 1;
                            printf(" *I3* ");
                        } else {
                            printf("\e[31mline %d exeLinker-superClass\e[0m ", __LINE__);
                            ret = -1;
                        }
                    } else {
                        printf("\e[31mline %d exeLinker\e[0m ", __LINE__);
                        ret = -1;
                    }
                } else {
                    printf("\e[31mline %d name %s\e[0m ", __LINE__, name_in);
                    ret = -1;
                }
            } else if (cproject_state[depth-1] == CPROJECT_STATE_TOOL_COMPILER) {
                printf(" line %d TOOL_COMPILER-%s %s ", __LINE__, name_in, attr_superClass);
                if (strcmp(name_in, (char*)option) == 0) {
                    if (strstr(attr_superClass, "compilerID") != NULL) {
                        if (strstr(attr_superClass, "GCC") != NULL) {
                            check_valueType = boolean;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "SILICON_VERSION") != NULL) {
                            const char * const * svs;
                            int len = strlen(attr_value);
                            bool value_found = false;
                            if (arch == ARCH_MSP430)
                                svs = siliconVersionsMSP430;
                            else
                                svs = siliconVersionsARM;

                            for (unsigned n = 0; svs[n] != NULL; n++) {
                                if (strcmp(svs[n], attr_value + (len - strlen(svs[n]))) == 0) {
                                    value_found = true;
                                }
                            }
                            if (!value_found) {
                                printf("\e[31mline %d value-not-found\e[0m ", __LINE__);
                                ret = -1;
                            }
                            check_valueType = (char*)enumerated;
                            cconfiguration_has.bits.SILICON_VERSION = 1;
                            printf(" *SV* ");
                        } else if (strstr(attr_superClass, "CODE_STATE") != NULL) {
                            int len = strlen(attr_value);
                            bool value_found = false;
                            for (unsigned n = 0; codeStates[n] != NULL; n++) {
                                if (strcmp(codeStates[n], attr_value + (len - strlen(codeStates[n]))) == 0) {
                                    value_found = true;
                                }
                            }
                            if (!value_found) {
                                printf("\e[31mline %d value-not-found\e[0m ", __LINE__);
                                ret = -1;
                            }
                            check_valueType = (char*)enumerated;
                            cconfiguration_has.bits.CODE_STATE = 1;
                            printf(" *CS* ");
                        } else if (strstr(attr_superClass, "ABI") != NULL) {
                            int len = strlen(attr_value);
                            bool value_found = false;
                            for (unsigned n = 0; ABIs[n] != NULL; n++) {
                                if (strcmp(ABIs[n], attr_value + (len - strlen(ABIs[n]))) == 0) {
                                    value_found = true;
                                }
                            }
                            if (!value_found) {
                                printf("\e[31mline %d value-not-found\e[0m ", __LINE__);
                                ret = -1;
                            }
                            cconfiguration_has.bits.ABI = 1;
                            check_valueType = (char*)enumerated;
                            printf(" *ABI* ");
                        } else if (strstr(attr_superClass, "FLOAT_SUPPORT") != NULL) {
                            int len = strlen(attr_value);
                            bool value_found = false;
                            for (unsigned n = 0; floatSupports[n] != NULL; n++) {
                                if (strcmp(floatSupports[n], attr_value + (len - strlen(floatSupports[n]))) == 0) {
                                    value_found = true;
                                }
                            }
                            if (!value_found) {
                                printf("\e[31mline %d value-not-found\e[0m ", __LINE__);
                                ret = -1;
                            }
                            check_valueType = (char*)enumerated;
                            printf(" *FS* ");
                        } else if (strstr(attr_superClass, ".UNDEFINE") != NULL) {
                            check_valueType = stringList;
                            check_is_empty = true;
                            printf(" *UD* ");
                        } else if (strstr(attr_superClass, ".DEFINE") != NULL) {
                            cconfiguration_has.bits.DEFINE = 1;
                            check_valueType = definedSymbols;
                            check_is_empty = true;
                            printf(" *D* ");
                        } else if (strstr(attr_superClass, "INCLUDE_PATH") != NULL) {
                            cconfiguration_has.bits.INCLUDE_PATH = 1;
                            check_valueType = includePath;
                            check_is_empty = true;
                            printf(" *IP* ");
                        } else if (strstr(attr_superClass, "DEBUGGING_MODEL") != NULL) {
                            cconfiguration_has.bits.DEBUGGING_MODEL = 1;
                            check_valueType = (char*)enumerated;
                            printf(" *DM* ");
                        } else if (strstr(attr_superClass, "DIAG_WARNING") != NULL) {
                            check_valueType = stringList;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "DISPLAY_ERROR_NUMBER") != NULL) {
                            check_valueType = boolean;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "DIAG_WRAP") != NULL) {
                            check_valueType = (char*)enumerated;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "ADVICE__POWER") != NULL) {
                            check_valueType = string;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "LITTLE_ENDIAN") != NULL) {
                            check_valueType = boolean;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "OPT_LEVEL") != NULL) {
                            int len = strlen(attr_value);
                            bool value_found = false;
                            for (unsigned n = 0; optLevels[n] != NULL; n++) {
                                if (strcmp(optLevels[n], attr_value + (len - strlen(optLevels[n]))) == 0) {
                                    value_found = true;
                                }
                            }
                            if (!value_found) {
                                printf("\e[31mline %d value-not-found\e[0m ", __LINE__);
                                ret = -1;
                            }
                            check_valueType = (char*)enumerated;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "OPT_FOR_SPEED") != NULL) {
                            int len = strlen(attr_value);
                            bool value_found = false;
                            for (unsigned n = 0; optForSpeed[n] != NULL; n++) {
                                if (strcmp(optForSpeed[n], attr_value + (len - strlen(optForSpeed[n]))) == 0) {
                                    value_found = true;
                                }
                            }
                            if (!value_found) {
                                printf("\e[31mline %d value-not-found\e[0m ", __LINE__);
                                ret = -1;
                            }
                            check_valueType = (char*)enumerated;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "FP_MODE") != NULL) {
                            int len = strlen(attr_value);
                            bool value_found = false;
                            for (unsigned n = 0; fpMode[n] != NULL; n++) {
                                if (strcmp(fpMode[n], attr_value + (len - strlen(fpMode[n]))) == 0) {
                                    value_found = true;
                                }
                            }
                            if (!value_found) {
                                printf("\e[31mline %d value-not-found\e[0m ", __LINE__);
                                ret = -1;
                            }
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "PREINCLUDE") != NULL) {
                            check_is_empty = true;
                            check_valueType = includeFiles;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "SYMDEBUG__DWARF_VERSION") != NULL) {
                            /* TODO check value */
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "C_DIALECT") != NULL) {
                            /* TODO check value */
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "C___DIALECT") != NULL) {
                            /* TODO check value */
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "LANGUAGE_MODE") != NULL) {
                            /* TODO check value */
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "EXCEPTIONS") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "RTTI") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "CPP_DEFAULT") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "EXTERN_C_CAN_THROW") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "FLOAT_OPERATIONS_ALLOWED") != NULL) {
                            /* TODO check value */
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "PLAIN_CHAR") != NULL) {
                            /* TODO check value */
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "PENDING_INSTANTIATIONS") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "PRINTF_SUPPORT") != NULL) {
                            /* TODO check value */
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "PREPROC_MODE") != NULL) {
                            /* TODO check value */
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "GEN_CROSS_REFERENCE_LISTING") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "GEN_FUNC_INFO_LISTING") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "GEN_PREPROCESSOR_LISTING") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "PARALLEL") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "CHECK_MISRA") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "CMD_FILE") != NULL) {
                            check_valueType = stringList;
                        } else if (strstr(attr_superClass, "ASM_EXTENSION") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, ".C_EXTENSION") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, ".CPP_EXTENSION") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "OBJ_EXTENSION") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "LISTING_EXTENSION") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "ASM_DIRECTORY") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "LIST_DIRECTORY") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "TEMP_DIRECTORY") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "PP_DIRECTORY") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "OUTPUT_FILE") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "ASM_FILE") != NULL) {
                            check_valueType = stringList;
                        } else if (strstr(attr_superClass, ".C_FILE") != NULL) {
                            check_valueType = stringList;
                        } else if (strstr(attr_superClass, ".CPP_FILE") != NULL) {
                            check_valueType = stringList;
                        } else if (strstr(attr_superClass, ".OBJ_FILE") != NULL) {
                            check_valueType = stringList;
                        } else if (strstr(attr_superClass, "KEEP_ASM") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "ASM_LISTING") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "SOURCE_INTERLIST") != NULL) {
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "ASM_DEFINE") != NULL) {
                            check_valueType = stringList;
                        } else if (strstr(attr_superClass, "ASM_UNDEFINE") != NULL) {
                            check_valueType = stringList;
                        } else if (strstr(attr_superClass, "ASM_CROSS_REFERENCE_LISTING") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "ASM_INCLUDES") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "ASM_DEPENDENCY") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "GEN_PROFILE_INFO") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, ".ANALYZE_ONLY") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, ".ANALYZE") != NULL) {
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "USE_PROFILE_INFO") != NULL) {
                            check_valueType = stringList;
                        } else if (strstr(attr_superClass, "ENTRY_HOOK") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "EXIT_HOOK") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "ENTRY_PARM") != NULL) {
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "EXIT_PARM") != NULL) {
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "REMOVE_HOOKS_WHEN_INLINING") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "FP_REASSOC") != NULL) {
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "SAT_REASSOC") != NULL) {
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "PROGRAM_LEVEL_COMPILE") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "CALL_ASSUMPTIONS") != NULL) {
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "DISABLE_INLINING") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "ALIASED_VARIABLES") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "AUTO_INLINE") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "GEN_FUNC_SUBSECTIONS") != NULL) {
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "GEN_DATA_SUBSECTIONS") != NULL) {
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "EMBEDDED_CONSTANTS") != NULL) {
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "UNALIGNED_ACCESS") != NULL) {
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "ENUM_TYPE") != NULL) {
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "WCHAR_T") != NULL) {
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, ".COMMON") != NULL) {
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "DIAG_ERROR") != NULL) {
                            check_valueType = stringList;
                        } else if (strstr(attr_superClass, "DIAG_REMARK") != NULL) {
                            check_valueType = stringList;
                        } else if (strstr(attr_superClass, "DIAG_SUPPRESS") != NULL) {
                            check_valueType = stringList;
                        } else if (strstr(attr_superClass, "EMIT_WARNINGS_AS_ERRORS") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "NO_WARNINGS") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "ISSUE_REMARKS") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "VERBOSE_DIAGNOSTICS") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "SET_ERROR_LIMIT") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "QUIET_LEVEL") != NULL) {
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "PREPROC_ONLY") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "PREPROC_WITH_COMMENT") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "PREPROC_WITH_COMPILE") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "PREPROC_WITH_LINE") != NULL) {
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "PREPROC_DEPENDENCY") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "PREPROC_INCLUDES") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "PREPROC_MACROS") != NULL) {
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "DATA_MODEL") != NULL) { // msp430
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "ADVICE__HW_CONFIG") != NULL) {  // msp430
                            check_valueType = string;
                        } else if (strstr(attr_superClass, "USE_HW_MPY") != NULL) { // msp430
                            check_valueType = (char*)enumerated;
                        } else if (strstr(attr_superClass, "SILICON_ERRATA.CPU21") != NULL) {   // msp430
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "SILICON_ERRATA.CPU22") != NULL) {   // msp430
                            check_valueType = boolean;
                        } else if (strstr(attr_superClass, "SILICON_ERRATA.CPU40") != NULL) {   // msp430
                            check_valueType = boolean;
                        } else {
                            printf("\e[31mline %d compilerID ?\e[0m ", __LINE__);
                            ret = -1;
                        }
                    } else {
                        printf("\e[31mline %d not compilerID\e[0m ", __LINE__);
                        ret = -1;
                    }
                } else if (strcmp(name_in, inputType) == 0) {
                    if (strstr(attr_superClass, "compiler") != NULL) {
                        if (strstr(attr_superClass, "inputType__C_SRCS") != NULL) {
                            cconfiguration_has.bits.inputType__C_SRCS = 1;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "inputType__CPP_SRCS") != NULL) {
                            cconfiguration_has.bits.inputType__CPP_SRCS = 1;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "inputType__ASM_SRCS") != NULL) {
                            cconfiguration_has.bits.inputType__ASM_SRCS = 1;
                            printf(" * ");
                        } else if (strstr(attr_superClass, "inputType__ASM2_SRCS") != NULL) {
                            cconfiguration_has.bits.inputType__ASM2_SRCS = 1;
                            printf(" * ");
                        } else {
                            printf("\e[31mline %d compilerID ?\e[0m ", __LINE__);
                            ret = -1;
                        }
                    } else {
                        printf("\e[31mline %d not compiler\e[0m ", __LINE__);
                        ret = -1;
                    }
                } else {
                    printf("\e[31mline %d name %s\e[0m ", __LINE__, name_in);
                    ret = -1;
                }
            } else {
                printf("\e[31mline %d state\e[0m ", __LINE__);
                ret = -1;
            }

            if (check_valueType) {
                if (!attr_valueType || strcmp(attr_valueType, check_valueType) != 0) {
                    printf("\e[31mline %d valueType want %s, have %s\e[0m ", __LINE__, check_valueType, attr_valueType);
                    ret = -1;
                }
            }

            if (check_is_empty) {
                if (!attr_IS_BUILTIN_EMPTY) {
                    printf("\e[31mline %d no IS_BUILTIN_EMPTY\e[0m ", __LINE__);
                    ret = -1;
                }
                if (!attr_IS_VALUE_EMPTY) {
                    printf("\e[31mline %d no IS_VALUE_EMPTY\e[0m ", __LINE__);
                    ret = -1;
                }
            }

            free(attr_id);
            free(attr_name);
            free(attr_superClass);
            free(attr_useByScannerDiscovery);
            free(attr_IS_BUILTIN_EMPTY);
            free(attr_IS_VALUE_EMPTY);
            free(attr_valueType);
            free(attr_value);
        } // ..if (depth < 0)
    } // ..if (nodeType == XML_NODE_TYPE_START_ELEMENT)
    else if (nodeType == XML_NODE_TYPE_END_OF_ELEMENT) {
        cproject_state[depth] = CPROJECT_STATE_NONE;
        if (strcmp((char*)storageModule, name_in) == 0) {
            printf(" \e[33mend-%s\e[0m ", storageModuleId[depth]);
            storageModuleId[depth][0] = 0;
        } else if (strcmp(cconfiguration, name_in) == 0) {
            uint32_t expected;
            if (strcmp(cconfiguration_name, "Debug") == 0)
                expected = 0xffffffff;
            else if (strcmp(cconfiguration_name, "Release") == 0)
                expected = 0xffbfffff;
            else {
                printf("line %d need cconfiguration_has for \"%s\" ", __LINE__, cconfiguration_name);
                expected = 0;
            }
            printf("\e[42m%s cconfiguration_has.dword : %x\e[0m ", cconfiguration_name, cconfiguration_has.dword);
            if (cconfiguration_has.dword != expected) {
                print_cconfiguration_missing(cconfiguration_has, expected);
                ret = -1;
            }
            cconfiguration_id[0] = 0;
            cconfiguration_name[0] = 0;
            cconfiguration_targetTool[0] = 0;
        } else if (strcmp(toolChain, name_in) == 0) {
            title_toolchain[0] = 0;
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

