#include "json_eclipse_cdt.h"
#include "board-mcu-hack.h"

/*
 * https://www.ti.com/tool/CCSTUDIO
 */

const char * const CODEGEN_VERSION = "20.2.2.LTS";
const char * const VERSION_MSP432 = "20.2";
const char * const VERSION_TMS470 = "16.9";

const char * const GENMAKEBUILDER_TRIGGERS = NULL;
const char * const GENMAKEBUILDER_ARGUMENTS = "";
const char * const SCANNERCONFIGBUILDER_TRIGGERS = "full,incremental,";
const char * const SCANNERCONFIGBUILDER_ARGUMENTS = "";
const char * const CDT_CORE_SETTINGS_CONFIGRELATIONS = "2";
const char * const BINARY_PARSERS[] = {
    "com.ti.ccstudio.binaryparser.CoffParser",
    NULL
};
const char * const ERROR_PARSERS[] = {
    "org.eclipse.cdt.core.GmakeErrorParser",
    "com.ti.ccstudio.errorparser.CoffErrorParser",
    "com.ti.ccstudio.errorparser.AsmErrorParser",
    "com.ti.ccstudio.errorparser.LinkErrorParser",
    NULL
};
const char * const MSP432 = "MSP432";
const char * const TMS470 = "TMS470";
const char * mcu_family = NULL;
const char * const TITLE_buildDefinitions = "com.ti.ccstudio.buildDefinitions";
const char * const big = "big";
const char * const little = "little";
const char * const RUNTIME_SUPPORT_LIBRARY = "libc.a";

char family_version[64];
char deviceVariant[32];
bool little_endian;
bool isElfFormat;

void put_other_storageModules()
{
    char parent_str[128];
    char str[128];
    char us_str[16];

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"org.eclipse.cdt.core.LanguageSettingsProviders");
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"cdtBuildSystem");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"version", (xmlChar*)"4.0.0");
    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"project");

    strcpy(parent_str, TITLE_buildDefinitions);
    if (mcu_family) {
        strcat(parent_str, ".");
        strcat(parent_str, mcu_family);
    } else
        printf("no mcu_family\n");
    strcat(parent_str, ".ProjectType");

    strcpy(str, from_codemodel.project_name);
    strcat(str, ".");
    strcat(str, parent_str);
    get_us(us_str);
    strcat(str, ".");
    strcat(str, us_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)mcu_family);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"projectType", (xmlChar*)parent_str);

    xmlTextWriterEndElement(cproject_writer); // project
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"scannerConfiguration");
    xmlTextWriterEndElement(cproject_writer); // storageModule

    if (mcu_family == TMS470) {
        xmlTextWriterStartElement(cproject_writer, storageModule);
        xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"org.eclipse.cdt.core.language.mapping");
        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"project-mappings");

        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"content-type-mapping");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"configuration", (xmlChar*)"");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"content-type", (xmlChar*)"org.eclipse.cdt.core.asmSource");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"language", (xmlChar*)"com.ti.ccstudio.core.TIASMLanguage");
        xmlTextWriterEndElement(cproject_writer); // content-type-mapping

        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"content-type-mapping");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"configuration", (xmlChar*)"");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"content-type", (xmlChar*)"org.eclipse.cdt.core.cHeader");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"language", (xmlChar*)"com.ti.ccstudio.core.TIGCCLanguage");
        xmlTextWriterEndElement(cproject_writer); // content-type-mapping

        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"content-type-mapping");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"configuration", (xmlChar*)"");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"content-type", (xmlChar*)"org.eclipse.cdt.core.cSource");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"language", (xmlChar*)"com.ti.ccstudio.core.TIGCCLanguage");
        xmlTextWriterEndElement(cproject_writer); // content-type-mapping

        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"content-type-mapping");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"configuration", (xmlChar*)"");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"content-type", (xmlChar*)"org.eclipse.cdt.core.cxxHeader");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"language", (xmlChar*)"com.ti.ccstudio.core.TIGPPLanguage");
        xmlTextWriterEndElement(cproject_writer); // content-type-mapping

        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"content-type-mapping");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"configuration", (xmlChar*)"");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"content-type", (xmlChar*)"org.eclipse.cdt.core.cxxSource");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"language", (xmlChar*)"com.ti.ccstudio.core.TIGPPLanguage");

        xmlTextWriterEndElement(cproject_writer); // content-type-mapping
        xmlTextWriterEndElement(cproject_writer); // project-mappings
        xmlTextWriterEndElement(cproject_writer); // storageModule

        xmlTextWriterStartElement(cproject_writer, storageModule);
        xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"refreshScope");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"versionNumber", (xmlChar*)"2");
        xmlTextWriterEndElement(cproject_writer); // storageModule
    } // ..if (mcu_family == TMS470)
}

int get_cconfiguration_id(bool debugBuild, const char *mcu, char *out_id, char *out_superClass)
{
    char strb[16];

    strcpy(out_superClass, TITLE_buildDefinitions);
    strcat(out_superClass, ".");
    if (strstr(mcu, MSP432) != NULL) {
        mcu_family = MSP432;
        little_endian = true;
    } else if (strstr(mcu, "CC3220sf") != NULL) {
        mcu_family = TMS470;
        little_endian = true;
    } else {
        printf("line %d unknown mcu \"%s\"\n", __LINE__, mcu);
        return -1;
    }

    strcat(out_superClass, mcu_family);
    strcat(out_superClass, ".");
    strcat(out_superClass, debugBuild ? "Debug" : "Release");

    strcpy(out_id, out_superClass);
    strcat(out_id, ".");
    get_us(strb);
    strcat(out_id, strb);

    return 0;
}

void write_natures()
{
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"com.ti.ccstudio.core.ccsNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"org.eclipse.cdt.managedbuilder.core.managedBuildNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"org.eclipse.cdt.managedbuilder.core.ScannerConfigNature");
}


const char * get_silicon_version(const char *Mcu, const char **codeState)
{
    if (strcmp("MSP432P401R", Mcu) == 0) {
        *codeState = "16";
        sprintf(family_version, "%s_%s", MSP432, VERSION_MSP432);
        return "7M4";
    } else if (strcmp("CC3220sf", Mcu) == 0) {
        *codeState = "16";
        sprintf(family_version, "%s_%s", TMS470, VERSION_TMS470);
        return "7M4";
    } else
        return NULL;
}

int cproject_init()
{
    /* TI CCS doesnt accept full path to linker command file: must exist in current directory (project directory) */
    int ret;
    FILE *fp_in = NULL;
    FILE *fp_out;
    struct stat st;
    char *buffer;
    char *withoutPath;

    if (linker_script[0] == 0) {
        for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
            if (strstr(my_list->str, ".cmd") != NULL) {
                fp_in = fopen(my_list->str, "r");
                if (fp_in == NULL) {
                    perror(my_list->str);
                    return -1;
                }
                if (stat(my_list->str, &st) < 0) {
                    perror("stat-codemodel");
                    return -1;
                }
                withoutPath = basename(my_list->str);
                break;
            }
        }

        if (fp_in == NULL) {
            printf("missing linker command file\n");
            return -1;
        }

        buffer = malloc(st.st_size);
#ifdef __WIN32__
        ret = fread_(buffer, st.st_size, fp_in);
#else
        ret = fread(buffer, st.st_size, 1, fp_in);
#endif
        if (ret == 0)
            return -1;

        fp_out = fopen(withoutPath, "w");
        ret = fwrite(buffer, st.st_size, 1, fp_out);
        fclose(fp_out);

        free(buffer);
        fclose(fp_in);

        strcpy(linker_script, withoutPath);
    } else
        ret = 0;

    return ret;
}

int _put_configuration(bool debugBuild, const char *ccfg_id, const char *cconfiguration_superClass, const char *Board, const char *Mcu, struct node_s *instance_node)
{
    int ret = 0;
    char parent_str[96];
    char buildDef_parent_str[72];
    char linker_parent_str[96];
    char id_str[128];
    char linker_id_str[128];
    char str[256];
    const char * const artifactExtension = "out";
    const char *silicon_version = NULL;
    const char *code_state = NULL;
    const char *family = NULL;

    if (strstr(cconfiguration_superClass, MSP432) != NULL)
        family = MSP432;
    else if (strstr(cconfiguration_superClass, TMS470) != NULL)
        family = TMS470;
    else {
        printf("line %d cconfiguration_superClass: \"%s\"\n", __LINE__, cconfiguration_superClass);
        return -1;
    }

    strcpy(deviceVariant, Mcu);

    silicon_version = get_silicon_version(Mcu, &code_state);
    if (silicon_version == NULL) {
        printf("silicon version of %s\n", Mcu);
        return -1;
    }

    strcpy(buildDef_parent_str, TITLE_buildDefinitions);
    strcat(buildDef_parent_str, ".");
    strcat(buildDef_parent_str, family_version);

    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".exe.linker");
    strcat(parent_str, Build);
    put_id(parent_str, linker_id_str);  // targetTool

    strcpy(linker_parent_str, parent_str);


    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"artifactExtension", (xmlChar*)artifactExtension);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"artifactName", (xmlChar*)from_codemodel.artifactName);
    //xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"artifactName", (xmlChar*) "${ProjName}");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"buildProperties", (xmlChar*)"");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"cleanCommand", (xmlChar*)"${CG_CLEAN_CMD}");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"description", (xmlChar*)"");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)ccfg_id);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)Build);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"parent", (xmlChar*)cconfiguration_superClass);
    if (family == TMS470) {
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"postbuildStep", (xmlChar*)"${CCS_INSTALL_ROOT}/utils/tiobj2bin/tiobj2bin ${BuildArtifactFileName} ${BuildArtifactFileBaseName}.bin ${CG_TOOL_ROOT}/bin/armofd ${CG_TOOL_ROOT}/bin/armhex ${CCS_INSTALL_ROOT}/utils/tiobj2bin/mkhex4bin;");
    }

    strcpy(str, ccfg_id);
    strcat(str, ".");

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"folderInfo");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"/");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"resourcePath", (xmlChar*)"");


    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"toolChain");
    strcpy(parent_str, TITLE_buildDefinitions);
    strcat(parent_str, ".");
    strcat(parent_str, family_version);
    strcat(parent_str, ".exe.");
    strcat(parent_str, Build);
    strcat(parent_str, "Toolchain");

    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"TI Build Tools");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"targetTool", (xmlChar*)linker_id_str);    // linker
    
    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    strcpy(parent_str, TITLE_buildDefinitions);
    strcat(parent_str, ".core.OPT_TAGS");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"stringList");

    strcpy(str, "DEVICE_CONFIGURATION_ID=");
    strcat(str, Mcu);
    put_listOptionValue(cproject_writer, false, str);

    strcpy(str, "DEVICE_ENDIANNESS=");
    strcat(str, little_endian ? little : big);
    put_listOptionValue(cproject_writer, false, str);

    strcpy(str, "OUTPUT_FORMAT=");
    strcat(str, isElfFormat ? "ELF": "?outFormat?");
    put_listOptionValue(cproject_writer, false, str);
    put_listOptionValue(cproject_writer, false, "CCS_MBS_VERSION=6.1.3");

    strcpy(str, "LINKER_COMMAND_FILE=");
    strcat(str, linker_script);
    put_listOptionValue(cproject_writer, false, str);

    strcpy(str, "RUNTIME_SUPPORT_LIBRARY=");
    strcat(str, RUNTIME_SUPPORT_LIBRARY);
    put_listOptionValue(cproject_writer, false, str);
    put_listOptionValue(cproject_writer, false, "OUTPUT_TYPE=executable");
    put_listOptionValue(cproject_writer, false, "PRODUCTS=");
    put_listOptionValue(cproject_writer, false, "PRODUCT_MACRO_IMPORTS={}");
    xmlTextWriterEndElement(cproject_writer); // option


    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Compiler version");
    strcpy(parent_str, TITLE_buildDefinitions);
    strcat(parent_str, ".core.OPT_CODEGEN_VERSION");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)CODEGEN_VERSION);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"targetPlatform");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".exe.targetPlatform");
    strcat(parent_str, Build);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Platform");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); //targetPlatform

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"builder");
    strcpy(str, from_codemodel.buildPath);

    strcat(str, "/");
    strcat(str, Build);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"buildPath", (xmlChar*)str);

    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".exe.builder");
    strcat(parent_str, Build);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    strcpy(str, "GNU Make.");
    strcat(str, Build);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"parallelBuildOn", (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"parallelizationNumber", (xmlChar*)"optimal");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"keepEnvironmentInBuildfile", (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // builder

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".exe.compiler");    /* ? both C and C++ ? */
    strcat(parent_str, Build);
    put_id(parent_str, id_str);  // targetTool
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"ARM Compiler");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".compilerID.GCC");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Target processor version (--silicon_version, -mv)");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".compilerID.SILICON_VERSION");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(str, parent_str);
    strcat(str, ".");
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        //           123456789012345678
        if (strncmp("--silicon_version=", my_list->str, 18) == 0) {
            silicon_version = my_list->str + 18;
            my_list->taken = true;
        //                  123
        } else if (strncmp("-mv", my_list->str, 3) == 0) {
            silicon_version = my_list->str + 3;
            my_list->taken = true;
        }
    }
    strcat(str, silicon_version);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Designate code state, 16-bit (thumb) or 32-bit (--code_state)");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".compilerID.CODE_STATE");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(str, parent_str);
    strcat(str, ".");
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        //           1234567890123
        if (strncmp("--code_state=", my_list->str, 13) == 0) {
            code_state = my_list->str + 13;
            my_list->taken = true;
            break;
        }
    }
    strcat(str, code_state);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Application binary interface. (--abi)");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".compilerID.ABI");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    strcpy(str, parent_str);
    strcat(str, ".");
    strcat(str, "eabi");    // ? other values ?
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Specify floating point support (--float_support)");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".compilerID.FLOAT_SUPPORT");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    {
        const char *ptr = "FPv4SPD16";
        strcpy(str, parent_str);
        strcat(str, ".");
        for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
            //           1234567890123456
            if (strncmp("--float_support=", my_list->str, 16) == 0) {
                ptr = my_list->str + 16;
                my_list->taken = true;
                break;
            }
        }
        strcat(str, ptr);    // TODO get from cmake
    }
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Pre-define NAME (--define, -D)");
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".compilerID.DEFINE");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"definedSymbols");
    for (struct node_s *my_list = defines_list; my_list != NULL; my_list = my_list->next) {
        const char *definePtr = my_list->str;
        header_in_define(&definePtr);
        put_listOptionValue(cproject_writer, false, definePtr);
    }
    str[0] = 0;
    if (family == TMS470) {
        /* every define should already have been provided */
    } else if (family == MSP432) {
        put_listOptionValue(cproject_writer, false, "ccs");
        sprintf(str, "__%s__", Mcu);
    } else {
        printf("line %d definedSymbols for family %s\n", __LINE__, family);
    }
    if (str[0] != 0)
        put_listOptionValue(cproject_writer, false, str);

    xmlTextWriterEndElement(cproject_writer); // option

    if (includes_list) {
        struct node_s *my_list;
        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Add dir to #include search path (--include_path, -I)");
        xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
        strcpy(parent_str, buildDef_parent_str);
        strcat(parent_str, ".compilerID.INCLUDE_PATH");
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"includePath");
        for (my_list = includes_list; my_list != NULL; my_list = my_list->next)
            put_listOptionValue(cproject_writer, false, my_list->str);

        xmlTextWriterEndElement(cproject_writer); // option
    }

    if (debugBuild) {
        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Debugging model");
        strcpy(parent_str, buildDef_parent_str);
        strcat(parent_str, ".compilerID.DEBUGGING_MODEL");
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        strcpy(str, parent_str);
        strcat(str, ".");
        strcat(str, "SYMDEBUG__DWARF");
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
        xmlTextWriterEndElement(cproject_writer); // option
    } // ..if (debugBuild)

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Treat diagnostic <id> as warning (--diag_warning, -pdsw)");
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".compilerID.DIAG_WARNING");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"stringList");
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        //           123456789012345
        if (strncmp("--diag_warning=", my_list->str, 15) == 0) {
            put_listOptionValue(cproject_writer, false, my_list->str + 15);
            my_list->taken = true;
            break;
        }
    }
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Emit diagnostic identifier numbers (--display_error_number, -pden)");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".compilerID.DISPLAY_ERROR_NUMBER");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterEndElement(cproject_writer); // option

    if (family == TMS470) {
        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Level of printf/scanf support required (--printf_support)");
        strcpy(parent_str, buildDef_parent_str);
        strcat(parent_str, ".compilerID.PRINTF_SUPPORT");
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        strcpy(str, parent_str);
        strcat(str, ".");
        /* ? where to get this from json ? */
        strcat(str, "full");
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
        xmlTextWriterEndElement(cproject_writer); // option
    }

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Wrap diagnostic messages (--diag_wrap)");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".compilerID.DIAG_WRAP");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(str, parent_str);
    strcat(str, ".");
    strcat(str, "off");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".compilerID.ADVICE__POWER");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"all");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterEndElement(cproject_writer); // option

    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        //           123456789
        if (strncmp("--endian=", my_list->str, 9) == 0) {
            little_endian = strcmp(my_list->str+9, "big") == 0 ? false : true;
            my_list->taken = true;
        } else if (strcmp("--little_endian", my_list->str) == 0 ||
                   strcmp("-me", my_list->str) == 0)
        {
            little_endian = true;
            my_list->taken = true;
            break;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Little endian code [See 'General' page to edit] (--little_endian, -me)");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".compilerID.LITTLE_ENDIAN");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, value, little_endian ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterEndElement(cproject_writer); // option

    char c_dialect[16];
    strcpy(c_dialect, "C99");
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (strcmp("--c99", my_list->str) == 0) {
            my_list->taken = true;
        } else if (strcmp("--c89", my_list->str) == 0) {
            strcpy(c_dialect, "C89");
            my_list->taken = true;
        } else if (strcmp("--c11", my_list->str) == 0) {
            strcpy(c_dialect, "C11");
            my_list->taken = true;
        }
    }

    if (strcmp(c_dialect, "C99") != 0) {
        xmlTextWriterStartElement(cproject_writer, option);
        strcpy(parent_str, buildDef_parent_str);
        strcat(parent_str, ".compilerID.C_DIALECT");
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        strcpy(str, parent_str);
        strcat(str, ".");
        strcat(str, c_dialect);
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
        xmlTextWriterEndElement(cproject_writer); // option
    }

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".compiler.inputType__C_SRCS");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"C Sources");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // inputType 

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".compiler.inputType__CPP_SRCS");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"C++ Sources");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // inputType 

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".compiler.inputType__ASM_SRCS");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Assembly Sources");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // inputType 

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".compiler.inputType__ASM2_SRCS");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Assembly Sources");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // inputType 

    xmlTextWriterEndElement(cproject_writer); // tool   .exe.compiler

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".exe.linker");
    strcat(parent_str, Build);
    put_id(parent_str, id_str);  // targetTool
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"ARM Linker");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Link information (map) listed into <file> (--map_file, -m)");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".linkerID.MAP_FILE");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"${ProjName}.map");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Set C system stack size (--stack_size, -stack)");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".linkerID.STACK_SIZE");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    {
        const char *ptr = "512";
        for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
            //           1234567890123
            if (strncmp("--stack_size=", my_list->str, 13) == 0) {
                ptr = my_list->str + 13;
                my_list->taken = true;
            }
        }
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)ptr);
    }
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Heap size for C/C++ dynamic memory allocation (--heap_size, -heap)");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".linkerID.HEAP_SIZE");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    {
        const char *ptr = "1024";
        for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
            //           123456789012
            if (strncmp("--heap_size=", my_list->str, 12) == 0) {
                ptr = my_list->str + 12;
                my_list->taken = true;
            }
        }
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)ptr);
    }
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Specify output file name (--output_file, -o)");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".linkerID.OUTPUT_FILE");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"${ProjName}.out");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Detailed link information data-base into <file> (--xml_link_info, -xml_link_info)");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".linkerID.XML_LINK_INFO");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    {
        const char *ptr = "${ProjName}_linkInfo.xml";
        for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
            //           1234567890123456
            if (strncmp("--xml_link_info=", my_list->str, 16) == 0) {
                ptr = my_list->str + 16;
                my_list->taken = true;
            }
        }
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)ptr);
    }
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Emit diagnostic identifier numbers (--display_error_number, -pden)");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".linkerID.DISPLAY_ERROR_NUMBER");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Wrap diagnostic messages (--diag_wrap)");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".linkerID.DIAG_WRAP");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(str, parent_str);
    strcat(str, ".");
    strcat(str, "off");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Add <dir> to library search path (--search_path, -i)");
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".linkerID.SEARCH_PATH");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"libPaths");
    put_listOptionValue(cproject_writer, false, "${CCS_BASE_ROOT}/arm/include");
    put_listOptionValue(cproject_writer, false, "${CG_TOOL_ROOT}/lib");
    put_listOptionValue(cproject_writer, false, "${CG_TOOL_ROOT}/include");
    xmlTextWriterEndElement(cproject_writer); // option

    bool diag_suppress = false;
    for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
        //           123456789012345
        if (strncmp("--diag_suppress", my_list->str, 15) == 0) {
            diag_suppress = true;
            break;
        }
    }

    if (diag_suppress) {
        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Suppress diagnostic <id> (--diag_suppress)");
        strcpy(parent_str, buildDef_parent_str);
        strcat(parent_str, ".linkerID.DIAG_SUPPRESS");
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"stringList");
        for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
            //           123456789012345
            if (strncmp("--diag_suppress", my_list->str, 15) == 0) {
                my_list->taken = true;
                put_listOptionValue(cproject_writer, false, my_list->str + 16);
            }
        }
        xmlTextWriterEndElement(cproject_writer); // option
    }

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Include library file or command file as input (--library, -l)");
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".linkerID.LIBRARY");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"libs");
    bool libc = false;
    for (struct node_s *my_list = link_libs; my_list != NULL; my_list = my_list->next) {
        char tmp[256];
        const char *ptr;
        if (strncmp(my_list->str, "--", 2) == 0)
            continue;   /* some other linker argument */
        if (strstr(my_list->str, ".cmd") != NULL)
            continue;   /* TI linker script is in the linker library list */
        if (strncmp(my_list->str, "-l", 2) == 0)
            ptr = my_list->str + 2;
        else
            ptr = my_list->str;

        if (strncmp(my_list->str, "..", 2) == 0) {
            strcpy(tmp, "${PROJECT_LOC}");
            strcat(tmp, "/");
            strcat(tmp, ptr);
            ptr = tmp;
        }
        put_listOptionValue(cproject_writer, false, ptr);
        if (strcmp(ptr, "libc.a") == 0)
            libc = true;
    }
    if (!libc)
        put_listOptionValue(cproject_writer, false, "libc.a");
    xmlTextWriterEndElement(cproject_writer); // option

    for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (strcmp("-llibc.a", my_list->str) == 0)
            my_list->taken = true;
    }

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".exeLinker.inputType__CMD_SRCS");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Linker Command Files");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // inputType 

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".exeLinker.inputType__CMD2_SRCS");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Linker Command Files");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // inputType 

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".exeLinker.inputType__GEN_CMDS");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Generated Linker Command Files");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // inputType 

    xmlTextWriterEndElement(cproject_writer); // tool linker

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".hex");
    put_id(parent_str, id_str);  // targetTool
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"ARM Hex Utility");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".hex.ROMWIDTH");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"8");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, buildDef_parent_str);
    strcat(parent_str, ".hex.MEMWIDTH");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"8");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterEndElement(cproject_writer); // tool hex

    xmlTextWriterEndElement(cproject_writer); // toolChain

    xmlTextWriterEndElement(cproject_writer); // folderInfo

    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (!my_list->taken) {
            printf("\e[31mnotTaken compile fragment \e[7m%s\e[0m\n", my_list->str);
        }
    }

    for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (!my_list->taken) {
            printf("\e[31mnotTaken linker fragment \e[7m%s\e[0m\n", my_list->str);
        }
    }

    return ret;
} // ..put_configuration()

int ccsproject_start(bool force)
{
    xmlTextWriterPtr ccsproject_writer;
    struct stat st;
    const char *xml_filename = ".ccsproject";
    int ret;

    if (!force && stat(xml_filename, &st) == 0) {
        printf("%s already exists ", xml_filename);
        return -EEXIST;
    }

    ccsproject_writer = xmlNewTextWriterFilename(xml_filename, 0);
    if (ccsproject_writer == NULL) {
        printf("cannot create xml writer\r\n");
        return -1;
    }

    // <?xml version="1.0" encoding="UTF-8" ?>
    ret = xmlTextWriterStartDocument(ccsproject_writer, NULL, "UTF-8", NULL);
    if (ret < 0) {
        printf("Error at xmlTextWriterStartDocument\n");
        return ret;
    }
    ret = xmlTextWriterSetIndent(ccsproject_writer, 1);
    if (ret < 0) {
        printf("Error at xmlTextWriterSetIndent\n");
        return ret;
    }
    xmlTextWriterSetIndentString(ccsproject_writer, (xmlChar*)"\t");

    xmlTextWriterWritePI(ccsproject_writer, (xmlChar*)"ccsproject", (xmlChar*)"version=\"1.0\"");
    xmlTextWriterStartElement(ccsproject_writer, (xmlChar*)"projectOptions");

    xmlTextWriterStartElement(ccsproject_writer, (xmlChar*)"ccsVersion");
    xmlTextWriterWriteAttribute(ccsproject_writer, value, (xmlChar*)"10.1.0");
    xmlTextWriterEndElement(ccsproject_writer); // ccsVersion 

    xmlTextWriterStartElement(ccsproject_writer, (xmlChar*)"deviceVariant");
    xmlTextWriterWriteAttribute(ccsproject_writer, value, (xmlChar*)deviceVariant);
    xmlTextWriterEndElement(ccsproject_writer); //

    xmlTextWriterStartElement(ccsproject_writer, (xmlChar*)"deviceFamily");
    xmlTextWriterWriteAttribute(ccsproject_writer, value, (xmlChar*)mcu_family);
    xmlTextWriterEndElement(ccsproject_writer); //

    xmlTextWriterStartElement(ccsproject_writer, (xmlChar*)"deviceEndianness");
    xmlTextWriterWriteAttribute(ccsproject_writer, value, little_endian ? (xmlChar*)little : (xmlChar*)big);
    xmlTextWriterEndElement(ccsproject_writer); //

    xmlTextWriterStartElement(ccsproject_writer, (xmlChar*)"codegenToolVersion");
    xmlTextWriterWriteAttribute(ccsproject_writer, value, (xmlChar*)CODEGEN_VERSION);
    xmlTextWriterEndElement(ccsproject_writer); //

    xmlTextWriterStartElement(ccsproject_writer, (xmlChar*)"isElfFormat");
    xmlTextWriterWriteAttribute(ccsproject_writer, value, isElfFormat ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterEndElement(ccsproject_writer); //

    xmlTextWriterStartElement(ccsproject_writer, (xmlChar*)"connection");
    xmlTextWriterWriteAttribute(ccsproject_writer, value, (xmlChar*)"common/targetdb/connections/TIXDS110_Connection.xml");
    xmlTextWriterEndElement(ccsproject_writer); //

    xmlTextWriterStartElement(ccsproject_writer, (xmlChar*)"linkerCommandFile");
    xmlTextWriterWriteAttribute(ccsproject_writer, value, (xmlChar*)linker_script);
    xmlTextWriterEndElement(ccsproject_writer); //

    xmlTextWriterStartElement(ccsproject_writer, (xmlChar*)"rts");
    xmlTextWriterWriteAttribute(ccsproject_writer, value, (xmlChar*)RUNTIME_SUPPORT_LIBRARY);
    xmlTextWriterEndElement(ccsproject_writer); //

    xmlTextWriterStartElement(ccsproject_writer, (xmlChar*)"createSlaveProjects");
    xmlTextWriterWriteAttribute(ccsproject_writer, value, (xmlChar*)"");
    xmlTextWriterEndElement(ccsproject_writer); //

    xmlTextWriterStartElement(ccsproject_writer, (xmlChar*)"templateProperties");
    xmlTextWriterWriteAttribute(ccsproject_writer, value, (xmlChar*)"id=com.ti.common.project.core.emptyProjectWithMainTemplate_msp432");
    xmlTextWriterEndElement(ccsproject_writer); //

    xmlTextWriterStartElement(ccsproject_writer, (xmlChar*)"filesToOpen");
    xmlTextWriterWriteAttribute(ccsproject_writer, value, (xmlChar*)"");    // TODO
    xmlTextWriterEndElement(ccsproject_writer); //

    xmlTextWriterEndElement(ccsproject_writer); // projectOptions

    ret = xmlTextWriterEndDocument(ccsproject_writer);
    if (ret < 0) {
        printf ("testXmlwriterFilename: Error at xmlTextWriterEndDocument\n");
        return ret;
    }
    xmlFreeTextWriter(ccsproject_writer);

    return 0;
}

void put_project_other_builders() { }
void put_other_cconfiguration_storageModules(bool debugBuild) { }
int unbuilt_source(const char *source_path, const char *path) { return 0; }
void cat_additional_exclude_directories(char *str) { }

int main(int argc, char *argv[])
{
    int ret, opt;
    bool force = false;

    while ((opt = getopt(argc, argv, "f")) != -1) {
        switch (opt) {
            case 'f':
                force = true;
                break;
        }
    }

    isElfFormat = true;

    if ((ret = project_start(force)) < 0) {
        goto main_end;
    }

    if ((ret = cproject_start(force)) < 0) {
        goto main_end;
    }

    if ((ret = ccsproject_start(force)) < 0) {
        goto main_end;
    }

main_end:
    if (ret == -EEXIST)
        printf("-f to force overwrite\r\n");

    free_lists();
    return ret;
}

