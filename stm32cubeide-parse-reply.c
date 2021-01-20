#include "json_eclipse_cdt.h"

const char * const GENMAKEBUILDER_TRIGGERS = "clean,full,incremental,";
const char * const GENMAKEBUILDER_ARGUMENTS = NULL;
const char * const SCANNERCONFIGBUILDER_TRIGGERS = "full,incremental,";
const char * const SCANNERCONFIGBUILDER_ARGUMENTS = "";
const char * const CDT_CORE_SETTINGS_CONFIGRELATIONS = NULL;
const char * const BINARY_PARSERS[] = {
    "org.eclipse.cdt.core.ELF",
    NULL
};
const char * const ERROR_PARSERS[] = {
    "org.eclipse.cdt.core.GASErrorParser",
    "org.eclipse.cdt.core.GmakeErrorParser",
    "org.eclipse.cdt.core.CWDLocator",
    "org.eclipse.cdt.core.GCCErrorParser",
    "org.eclipse.cdt.core.GLDErrorParser",
    NULL
};


const char * const TITLE = "com.st.stm32cube.ide.mcu.gnu.managedbuild";

void put_other_storageModules()
{
    char str[128];
    char us_str[16];

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"org.eclipse.cdt.core.pathentry");
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"cdtBuildSystem");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"version", (xmlChar*)"4.0.0");
    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"project");

    get_us(us_str);
    sprintf(str, "%s.null.%s", from_codemodel.project_name, us_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)from_codemodel.project_name);

    xmlTextWriterEndElement(cproject_writer); // project
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"scannerConfiguration");
    put_scannerConfiguration("true");
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"org.eclipse.cdt.core.LanguageSettingsProviders");
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"refreshScope");
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"org.eclipse.cdt.make.core.buildtargets");
    xmlTextWriterEndElement(cproject_writer); // storageModule
}


int cproject_init() { return 0; }

void put_compiler_debug_level(char debugging_level, const char *parent_str)
{
    char id_str[128];
    char str[256];

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Debug level");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    if (debugging_level != 0) {
        strcpy(str, parent_str);
        strcat(str, ".value.g");
        if (debugging_level != '2') {
            char flag[2];
            flag[0] = debugging_level;
            flag[1] = 0;
            strcat(str, flag);
        }
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    }
    xmlTextWriterEndElement(cproject_writer); // option
}

void get_c_optimization_value(char fragment_in, char *out)
{
    char _str[2];
/* -O0   
 * -Os   
 * -O1   
 * -O2   
 * -O3   
 * -Og   
*/
    strcpy(out, TITLE);
    
    strcat(out, ".tool.c.compiler.option.optimization.level.value.o");
    _str[0] = fragment_in;
    _str[1] = 0;
    strcat(out, _str);
}

void get_cpp_optimization_value(char fragment_in, char *out)
{
    char _str[2];
/* -O0   
 * -Os   
 * -O1   
 * -O2   
 * -O3
 * -Og   
*/
    strcpy(out, TITLE);
    
    strcat(out, ".tool.cpp.compiler.option.optimization.level.value.o");
    _str[0] = fragment_in;
    _str[1] = 0;
    strcat(out, _str);
}

void put_c_other_flags()
{
    char parent_str[96];
    char id_str[128];

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    sprintf(parent_str, "%s.tool.c.compiler.option.otherflags", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"stringList");
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;

        if (strcmp(my_list->str, "-mthumb") == 0)   // taken care by Board + Mcu
            continue;
        if (strncmp(my_list->str, "-mcpu=", 6) == 0)    // taken care by Board + Mcu
            continue;
        if (strcmp(my_list->str, "-Wall") == 0) // added by default by sw4stm32
            continue;
        if (strcmp(my_list->str, "-ffunction-sections") == 0) // added by default by sw4stm32
            continue;

        put_listOptionValue(cproject_writer, false, my_list->str);
        my_list->taken = true;
    }
    xmlTextWriterEndElement(cproject_writer); // option
}

void get_c_dialect_value_from_fragment(const char *in, const char **out)
{
    if (strstr(in, "c90"))
        *out = "isoc90";
    else if (strstr(in, "1994"))
        *out = "iso9899_199409";
    else if (strstr(in, "c99"))
        *out = "isoc99";
    else if (strstr(in, "c11"))
        *out = "isoc11";
    else if (strstr(in, "gnu90"))
        *out = "gnu90";
    else if (strstr(in, "gnu99"))
        *out = "gnu99";
    else if (strstr(in, "gnu11"))
        *out = "gnu11";
    else
        *out = "gccdefault";
}

int _put_configuration(bool debugBuild, const char *ccfg_id, const char *cconfiguration_superClass, const char *Board, const char *Mcu, struct node_s *instance_node)
{
    int ret = 0;
    char id_str[128];
    char parent_str[96];
    char str[256];
    char cpp_compiler_id[128];
    const char * const artifactExtension = "elf";

    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"artifactExtension", (xmlChar*)artifactExtension);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"artifactName", (xmlChar*)from_codemodel.artifactName);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"buildArtefactType", (xmlChar*)"org.eclipse.cdt.build.core.buildArtefactType.exe" );
    strcpy(str, "org.eclipse.cdt.build.core.buildArtefactType=org.eclipse.cdt.build.core.buildArtefactType.exe,org.eclipse.cdt.build.core.buildType=org.eclipse.cdt.build.core.buildType.");
    strcat(str, build);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"buildProperties", (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"cleanCommand", (xmlChar*)"rm -rf");  // on windows too?
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"description", (xmlChar*)"");
    /* stm32cubeIDE doesnt like "errorParsers" */
    //xmlTextWriterWriteAttribute(cproject_writer, "errorParsers", "org.eclipse.cdt.core.GASErrorParser;org.eclipse.cdt.core.GmakeErrorParser;org.eclipse.cdt.core.GLDErrorParser;org.eclipse.cdt.core.CWDLocator;org.eclipse.cdt.core.GCCErrorParser");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)ccfg_id);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)Build);
    //xmlTextWriterWriteAttribute(cproject_writer, "optionalBuildProperties", "org.eclipse.cdt.docker.launcher.containerbuild.property.volumes=,org.eclipse.cdt.docker.launcher.containerbuild.property.selectedvolumes=");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"parent", (xmlChar*)cconfiguration_superClass);

    strcpy(str, ccfg_id);
    strcat(str, ".");

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"folderInfo");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"/");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"resourcePath", (xmlChar*)"");

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"toolChain");
    sprintf(parent_str, "%s.toolchain.exe.%s", TITLE, build);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU ARM GCC");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Internal Toolchain Type");
    strcpy(parent_str, "com.st.stm32cube.ide.mcu.option.internal.toolchain.type");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"com.st.stm32cube.ide.mcu.gnu.managedbuild.toolchain.base.gnu-tools-for-stm32");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Internal Toolchain Version");
    strcpy(parent_str, "com.st.stm32cube.ide.mcu.option.internal.toolchain.version");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"7-2018-q2-update");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option

    if (Mcu[0] != 0) {
        xmlTextWriterStartElement(cproject_writer, option);
        sprintf(parent_str, "%s.option.target_mcu", TITLE);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Mcu");
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)Mcu);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
        xmlTextWriterEndElement(cproject_writer); // option
    } else
        printf("Warning: no Mcu defined (\"%s\" from cmake)\r\n", from_cache.board);


    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"CpuId");
    strcpy(parent_str, "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.target_cpuid");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"0");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option


    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"CpuCoreId");
    strcpy(parent_str, "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.target_coreid");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"0");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option

    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp(my_list->str, "-mfpu=", 6) == 0) {
            xmlTextWriterStartElement(cproject_writer, option);
            xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Floating-point unit");
            strcpy(parent_str, "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.fpu");
            put_id(parent_str, id_str);
            xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
            xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
            strcpy(str, parent_str);
            strcat(str, ".value.");
            strcat(str, my_list->str + 6);  // +6: -mfpu=
            xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
            xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
            xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
            xmlTextWriterEndElement(cproject_writer); // option
            my_list->taken = true;
        } else if (strncmp(my_list->str, "-mfloat-abi=", 12) == 0) {
            xmlTextWriterStartElement(cproject_writer, option);
            xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Floating-point ABI");
            strcpy(parent_str, "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.floatabi");
            put_id(parent_str, id_str);
            xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
            xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
            strcpy(str, parent_str);
            strcat(str, ".value.");
            strcat(str, my_list->str + 12);  // +12: -mmfloat-abi=
            xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
            xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
            xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
            xmlTextWriterEndElement(cproject_writer); // option
            my_list->taken = true;
        }
    }

    if (Board[0] != 0) {
        xmlTextWriterStartElement(cproject_writer, option);
        sprintf(parent_str, "%s.option.target_board", TITLE);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Board");
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)Board);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
        xmlTextWriterEndElement(cproject_writer); // option
    } else
        printf("Warning: no Board defined (\"%s\" from cmake)\r\n", from_cache.board);

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"targetPlatform");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"archList", (xmlChar*)"all");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"binaryParser", (xmlChar*)"org.eclipse.cdt.core.ELF");
    sprintf(parent_str, "%s.targetplatform", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"isAbstract", (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"osList", (xmlChar*)"all");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // targetPlatform

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"builder");
    strcpy(str, from_codemodel.buildPath);
    strcat(str, "/");
    strcat(str, Build);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"buildPath", (xmlChar*)str);
    sprintf(parent_str, "%s.builder", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"keepEnvironmentInBuildfile", (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"managedBuildOn", (xmlChar*)"true");
    sprintf(str, "Gnu Make Builder.%s", build);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"parallelBuildOn", (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"parallelizationNumber", (xmlChar*)"optimal");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // builder

    sprintf(parent_str, "%s.tool.c.compiler", TITLE);
    put_id(parent_str, str);
    strcat(instance_node->str, ";");
    strcat(instance_node->str, str);

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU GCC Compiler");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

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
        xmlTextWriterStartElement(cproject_writer, option);
        sprintf(parent_str, "%s.tool.c.compiler.option.optimization.level", TITLE);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Optimization Level");
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");

        get_c_optimization_value(optimization_level, str);
        if (str[0] != 0) {
            xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
            xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
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

    sprintf(parent_str, "%s.tool.c.compiler.option.debuglevel", TITLE);
    put_compiler_debug_level(debugging_level, parent_str);

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    strcpy(parent_str, "com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.definedsymbols");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Defined symbols (-D)");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"definedSymbols");
    for (struct node_s *my_list = defines_list; my_list != NULL; my_list = my_list->next) {
        if (strcmp("C", (char*)my_list->arg) == 0)
            put_listOptionValue(cproject_writer, false, my_list->str);
    }
    for (struct node_s *my_list = c_flags; my_list != NULL; my_list = my_list->next) {
        if (strncmp("-D", my_list->str, 2) == 0)
            put_listOptionValue(cproject_writer, false, my_list->str + 2);
    }
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Include paths (-I)");
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    strcpy(parent_str, "com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.includepaths");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"includePath");
    for (struct node_s *my_list = includes_list; my_list != NULL; my_list = my_list->next) {
        if (strcmp("C", (char*)my_list->arg) == 0)
            put_listOptionValue(cproject_writer, false, my_list->str);
    }
    xmlTextWriterEndElement(cproject_writer); // option

    // -fdata-sections
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-fdata-sections", my_list->str) == 0) {
            xmlTextWriterStartElement(cproject_writer, option);
            sprintf(parent_str, "%s.tool.c.compiler.option.fdata", TITLE);
            put_id(parent_str, id_str);
            xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
            xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
            xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
            xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
            xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
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
        xmlTextWriterStartElement(cproject_writer, option);
        strcpy(parent_str, "com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.languagestandard");
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
        strcpy(str, "com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.languagestandard.value.");
        strcat(str, dialect);
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
        xmlTextWriterEndElement(cproject_writer); // option
    }

    // -pedantic
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-pedantic", my_list->str) == 0) {
            xmlTextWriterStartElement(cproject_writer, option);
            strcpy(parent_str, "com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.warnings.pedantic");
            put_id(parent_str, id_str);
            xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
            xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
            xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
            xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
            xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
            xmlTextWriterEndElement(cproject_writer); // option
            my_list->taken = true;
        }
    }

    // -Wextra
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-Wextra", my_list->str) == 0) {
            xmlTextWriterStartElement(cproject_writer, option);
            strcpy(parent_str, "com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.warnings.extra");
            put_id(parent_str, id_str);
            xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
            xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
            xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
            xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
            xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
            xmlTextWriterEndElement(cproject_writer); // option
            my_list->taken = true;
        }
    }

    // any -W -f -m not already added
    put_c_other_flags();

    sprintf(parent_str, "%s.tool.c.compiler.input.c", TITLE);
    put_id(parent_str, str);
    strcat(instance_node->str, ";");
    strcat(instance_node->str, str);

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // inputType


    xmlTextWriterEndElement(cproject_writer); // tool  tool.gnu.cross.c.compiler

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU G++ Compiler");
    sprintf(parent_str, "%s.tool.cpp.compiler", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    strcpy(cpp_compiler_id, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);

    if (optimization_level != 0) {
        xmlTextWriterStartElement(cproject_writer, option);
        sprintf(parent_str, "%s.tool.cpp.compiler.option.optimization.level", TITLE);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Optimization Level");
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        get_cpp_optimization_value(optimization_level, str);
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
        xmlTextWriterEndElement(cproject_writer); // option
    }

    sprintf(parent_str, "%s.tool.cpp.compiler.option.debuglevel", TITLE);
    put_compiler_debug_level(debugging_level, parent_str);

    if (cpp_input) {
        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
        sprintf(parent_str, "%s.tool.cpp.compiler.option.definedsymbols", TITLE);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Defined symbols (-D)");
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"definedSymbols");
        for (struct node_s *my_list = defines_list; my_list != NULL; my_list = my_list->next) {
            if (strcmp("C", (char*)my_list->arg) == 0 || strcmp("CXX", (char*)my_list->arg) == 0)
                put_listOptionValue(cproject_writer, false, my_list->str);
        }
        for (struct node_s *my_list = cpp_flags; my_list != NULL; my_list = my_list->next) {
            if (strncmp("-D", my_list->str, 2) == 0)
                put_listOptionValue(cproject_writer, false, my_list->str + 2);
        }
        xmlTextWriterEndElement(cproject_writer); // option

        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Include paths (-I)");
        xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
        sprintf(parent_str, "%s.tool.cpp.compiler.option.includepaths", TITLE);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"includePath");

        for (struct node_s *my_list = includes_list; my_list != NULL; my_list = my_list->next) {
            if (strcmp("CXX", (char*)my_list->arg) == 0 || strcmp("C", (char*)my_list->arg) == 0)
                put_listOptionValue(cproject_writer, false, my_list->str);
        }
        xmlTextWriterEndElement(cproject_writer); // option

        instance_node = add_instance(ccfg_id);
        strcat(instance_node->str, ";");
        strcat(instance_node->str, cpp_compiler_id);

        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");

        sprintf(parent_str, "%s..tool.cpp.compiler.input.cpp", TITLE);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        put_id(parent_str, id_str);
        strcat(instance_node->str, ";");
        strcat(instance_node->str, str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterEndElement(cproject_writer); // inputType
    } // ..if (cpp_input)

    xmlTextWriterEndElement(cproject_writer); // tool          .tool.cpp.compiler

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    sprintf(parent_str, "%s.tool.c.linker", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU GCC Linker");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

    if (!cpp_linker) {
        if (linker_script[0] != 0) {
            xmlTextWriterStartElement(cproject_writer, option);
            sprintf(parent_str, "%s.tool.c.linker.option.script", TITLE);
            put_id(parent_str, id_str);
            xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
            xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Linker Script (-T)");
            xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
            xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
            xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)linker_script);
            xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
            xmlTextWriterEndElement(cproject_writer); // option
        }

        bool other_link_libs = false;
        for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
            if (strcmp("C", (char*)my_list->arg) == 0 && strncmp(my_list->str, "-l", 2) == 0) {
                other_link_libs = true;
            }
        }

        if (other_link_libs) {
            xmlTextWriterStartElement(cproject_writer, option);
            sprintf(parent_str, "%s.tool.c.linker.option.libraries", TITLE);
            xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
            put_id(parent_str, id_str);
            xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
            xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"libs");
            xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
            xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
            for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
                if (strcmp("C", (char*)my_list->arg) == 0 && strncmp(my_list->str, "-l", 2) == 0) {
                    put_listOptionValue(cproject_writer, false, my_list->str+2);
                    my_list->taken = true;
                }
            }
            xmlTextWriterEndElement(cproject_writer); // option
        }

        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
        sprintf(parent_str, "%s.tool.c.linker.option.otherflags", TITLE);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"stringList");
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
            str[0] = 0;
            if (strncmp(my_list->str, "-Wl,", 4) == 0) {
                const char *z = strstr(my_list->str, "-z");
                if (z) {
                    if (str[0] != 0)
                        strcat(str, " ");
                    strcat(str, "-z ");
                    strcat(str, z + 2);
                }
            }
            if (str[0] != 0)
                put_listOptionValue(cproject_writer, false, str);
        }
        xmlTextWriterEndElement(cproject_writer); // option

        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
        strcpy(parent_str, "com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.linker.input");
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        put_additionalInput(cproject_writer, "additionalinputdependency", "$(USER_OBJS)");
        put_additionalInput(cproject_writer, "additionalinput", "$(LIBS)");
        xmlTextWriterEndElement(cproject_writer); // inputType
    } // ..if (!cpp_linker)

    xmlTextWriterEndElement(cproject_writer); // tool    tool.gnu.cross.c.linker

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    sprintf(parent_str, "%s.tool.cpp.linker", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU G++ Linker");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

    if (cpp_linker) {
        if (linker_script[0] != 0) {
            xmlTextWriterStartElement(cproject_writer, option);
            sprintf(parent_str, "%s.tool.cpp.linker.option.script", TITLE);
            put_id(parent_str, id_str);
            xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
            xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Linker Script (-T)");
            xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
            xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
            xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)linker_script);
            xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
            xmlTextWriterEndElement(cproject_writer); // option
        }

        bool other_link_libs = false;
        for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
            if (strcmp("CXX", (char*)my_list->arg) == 0 && strncmp(my_list->str, "-l", 2) == 0) {
                other_link_libs = true;
            }
        }

        if (other_link_libs) {
            xmlTextWriterStartElement(cproject_writer, option);
            sprintf(parent_str, "%s.tool.cpp.linker.option.libraries", TITLE);
            xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
            put_id(parent_str, id_str);
            xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
            xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"libs");
            xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
            xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
            for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
                if (strcmp("CXX", (char*)my_list->arg) == 0 && strncmp(my_list->str, "-l", 2) == 0) {
                    put_listOptionValue(cproject_writer, false, my_list->str+2);
                    my_list->taken = true;
                }
            }
            xmlTextWriterEndElement(cproject_writer); // option
        }

        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
        strcpy(parent_str, "com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.cpp.linker.input");
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"additionalInput");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"kind", (xmlChar*)"additionalinputdependency");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"paths", (xmlChar*)"$(USER_OBJS)");
        xmlTextWriterEndElement(cproject_writer); // additionalInput

        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"additionalInput");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"kind", (xmlChar*)"additionalinput");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"paths", (xmlChar*)"$(LIBS)");
        xmlTextWriterEndElement(cproject_writer); // additionalInput

        xmlTextWriterEndElement(cproject_writer); // inputType
    }

    xmlTextWriterEndElement(cproject_writer); // tool      tool.gnu.cross.cpp.linker

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    sprintf(parent_str, "%s.tool.archiver", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU GCC Archiver");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // tool

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU Size");
    sprintf(parent_str, "%s.tool.size", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // tool

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU Output Converter list file");
    sprintf(parent_str, "%s.tool.objdump.listfile", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // tool

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU Output Converter Hex");
    sprintf(parent_str, "%s.tool.objcopy.hex", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // tool

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU Output Converter Binary");
    sprintf(parent_str, "%s.tool.objcopy.binary", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // tool

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU Output Converter Verilog");
    sprintf(parent_str, "%s.tool.objcopy.verilog", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // tool

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU Output Converter Motorola S-rec");
    sprintf(parent_str, "%s.tool.objcopy.srec", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // tool

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU Output Converter Motorola S-rec with symbols");
    sprintf(parent_str, "%s.tool.objcopy.symbolsrec", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // tool

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    sprintf(parent_str, "%s.tool.assembler", TITLE);

    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU GCC Assembler");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Debug level");
    sprintf(parent_str, "%s.tool.assembler.option.debuglevel", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    if (debugging_level != 0) {
        strcpy(str, TITLE);
        strcat(str, ".tool.assembler.option.debuglevel.value.g");
        if (debugging_level != '2') {
            char flag[2];
            flag[0] = debugging_level;
            flag[1] = 0;
            strcat(str, flag);
        }
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    }
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    sprintf(parent_str, "%s.tool.assembler.input", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // inputType

    xmlTextWriterEndElement(cproject_writer); // tool   tool.gnu.cross.assembler.exe.*

    xmlTextWriterEndElement(cproject_writer); // toolChain
    xmlTextWriterEndElement(cproject_writer); // folderInfo

    return ret;
}

int get_cconfiguration_id(bool debugBuild, const char *mcu, char *out_id, char *out_superClass)
{
    char strb[16];
    sprintf(out_superClass, "%s.config.exe.%s", TITLE, debugBuild ? "debug" : "release");
    strcpy(out_id, out_superClass);
    strcat(out_id, ".");
    get_us(strb);
    strcat(out_id, strb);

    return 0;
}

void get_cpp_debugging_level_superclass(char *parent_str)
{
    sprintf(parent_str, "%s.tool.cpp.compiler.option.debuglevel", TITLE);
}

void write_natures()
{
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"com.st.stm32cube.ide.mcu.MCUProjectNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"com.st.stm32cube.ide.mcu.MCUCubeProjectNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"com.st.stm32cube.ide.mcu.MCUCubeIdeServicesRevAev2ProjectNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"com.st.stm32cube.ide.mcu.MCUAdvancedStructureProjectNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"com.st.stm32cube.ide.mcu.MCUEndUserDisabledTrustZoneProjectNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"com.st.stm32cube.ide.mcu.MCUSingleCpuProjectNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"com.st.stm32cube.ide.mcu.MCURootProjectNature");

    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"org.eclipse.cdt.managedbuilder.core.managedBuildNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"org.eclipse.cdt.managedbuilder.core.ScannerConfigNature");
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

    if ((ret = project_start(force)) < 0) {
        goto main_end;
    }

    if ((ret = cproject_start(force)) < 0) {
        goto main_end;
    }

main_end:
    if (ret == -EEXIST)
        printf("-f to force overwrite\r\n");

    free_lists();
    return ret;
}

