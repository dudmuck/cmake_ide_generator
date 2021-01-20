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
    "org.eclipse.cdt.core.GLDErrorParser",
    "org.eclipse.cdt.core.CWDLocator",
    "org.eclipse.cdt.core.GCCErrorParser",
    NULL
};

const char * const TITLE = "fr.ac6.managedbuild";

void put_other_storageModules()
{
    char parent_str[96];
    char id_str[128];

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"cdtBuildSystem");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"version", (xmlChar*)"4.0.0");
    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"project");

    sprintf(parent_str, "%s.target.gnu.cross.exe", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Executable");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"projectType", (xmlChar*)parent_str);

    xmlTextWriterEndElement(cproject_writer); // project
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"scannerConfiguration");
    put_scannerConfiguration("");
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"org.eclipse.cdt.core.LanguageSettingsProviders");
    xmlTextWriterEndElement(cproject_writer); // storageModule

#if 0
    /* optional storage modules in new ac6 project */
    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"refreshScope");
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"org.eclipse.cdt.make.core.buildtargets");
    xmlTextWriterEndElement(cproject_writer); // storageModule
#endif /* if 0 */
}


int cproject_init() { return 0; }

void get_c_optimization_value(char fragment_in, char *out)
{
    const char *ptr;
/* -O0   fr.ac6.managedbuild.gnu.c.optimization.level.none
 * -Os   fr.ac6.managedbuild.gnu.c.optimization.level.size
 * -O1   fr.ac6.managedbuild.gnu.c.optimization.level.optimize
 * -O2   fr.ac6.managedbuild.gnu.c.optimization.level.more
 * -O3   fr.ac6.managedbuild.gnu.c.optimization.level.most
 * -Og   fr.ac6.managedbuild.gnu.c.optimization.level.debug
*/
    switch (fragment_in) {
        case 's': ptr = "size"; break;
        case '1': ptr = "optimize"; break;
        case '2': ptr = "more"; break;
        case '3': ptr = "most"; break;
        case 'g': ptr = "debug"; break;
        default: ptr = "none"; break;
    }
    strcpy(out, TITLE);
    strcat(out, ".gnu.c.optimization.level.");
    strcat(out, ptr);
}

void get_cpp_optimization_value(char fragment_in, char *out)
{
    const char *ptr;
/* -O0   fr.ac6.managedbuild.gnu.c.optimization.level.none
 * -Os   fr.ac6.managedbuild.gnu.c.optimization.level.size
 * -O1   fr.ac6.managedbuild.gnu.c.optimization.level.optimize
 * -O2   fr.ac6.managedbuild.gnu.c.optimization.level.more
 * -O3   fr.ac6.managedbuild.gnu.c.optimization.level.most
 * -Og   fr.ac6.managedbuild.gnu.c.optimization.level.debug
*/
    switch (fragment_in) {
        case 's': ptr = "size"; break;
        case '1': ptr = "optimize"; break;
        case '2': ptr = "more"; break;
        case '3': ptr = "most"; break;
        case 'g': ptr = "debug"; break;
        default: ptr = "none"; break;
    }
    strcpy(out, TITLE);
    strcat(out, ".gnu.cpp.optimization.level.");
    strcat(out, ptr);
}

void put_post_build_steps(const char *artifactExtension)
{
    char str[768];
    char id_str[128];

    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"postannouncebuildStep", (xmlChar*)"Generating binary and Printing size information:");

    /* TODO: get post build steps from UTILITY targets */
    sprintf(id_str, "%s.%s", from_codemodel.artifactName, artifactExtension);
    sprintf(str, "arm-none-eabi-objcopy -Oihex %s %s.hex; arm-none-eabi-objcopy -Obinary %s %s.bin; arm-none-eabi-size %s", id_str, from_codemodel.artifactName, id_str, from_codemodel.artifactName, id_str);

//arm-none-eabi-objcopy -Oihex <artifactName>.elf <artifactName>.hex; arm-none-eabi-objcopy -Obinary <artifactName>.elf <artifactName>.bin; arm-none-eabi-size <artifactName>.elf
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"postbuildStep", (xmlChar*)str);
}

void put_compiler_other_flags(const char *lang, const char *super_class)
{
    char parent_str[96];
    char str[256];
    char id_str[128];

    str[0] = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken || strcmp(lang, (char*)my_list->arg) != 0)
            continue;

        if (strcmp(my_list->str, "-mthumb") == 0)   // taken care by Board + Mcu
            continue;
        if (strncmp(my_list->str, "-mcpu=", 6) == 0)    // taken care by Board + Mcu
            continue;
        if (strcmp(my_list->str, "-Wall") == 0) // added by default by sw4stm32
            continue;
        if (strcmp(my_list->str, "-ffunction-sections") == 0) // added by default by sw4stm32
            continue;

        if (str[0] == 0)
            strcat(str, "-fmessage-length=0 ");
        else
            strcat(str, " ");

        strcat(str, my_list->str);
        my_list->taken = true;
    }

    if (str[0] != 0) {
        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Other flags");
        strcpy(parent_str, super_class);  // missing 'l' in build
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
        xmlTextWriterEndElement(cproject_writer); // option
    }
}

void get_c_dialect_value_from_fragment(const char *in, const char **out)
{
    // -std=gnu11   gnu.c.compiler.dialect.c11
    // -std=gnu99   gnu.c.compiler.dialect.c99
    // -std=c90     gnu.c.compiler.dialect.c90
    if (strstr(in, "gnu11"))
        *out = "c11";
    else if (strstr(in, "gnu99"))
        *out = "c99";
    else if (strstr(in, "c90"))
        *out = "c90";
}

static void strcat_debug_level(const char *in, char *out)
{
    /*        gnu.c.debugging.level.none
     * -g1    gnu.c.debugging.level.minimal
     * -g     gnu.c.debugging.level.default  (or -g2)
     * -g3    gnu.cpp.compiler.debugging.level.max
     */
    if (in == NULL || in[0] == 0) {
        strcat(out, "none");
    } else if (strcmp(in, "-g1") == 0) {
        strcat(out, "minimal");
    } else if (strcmp(in, "-g") == 0 || strcmp(in, "-g2") == 0) {
        strcat(out, "default");
    } else if (strcmp(in, "-g3") == 0) {
        strcat(out, "max");
    } else {
        printf("debugging level fail %s\n", in);
    }
}

int _put_configuration(bool debugBuild, const char *ccfg_id, const char *cconfiguration_superClass, const char *Board, const char *Mcu, struct node_s *instance_node)
{
    int ret = 0;
    char id_str[128];
    char parent_str[96];
    char cpp_compiler_id[128];
    char str[256];
    const char * const artifactExtension = "elf";

    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"artifactExtension", (xmlChar*)artifactExtension);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"artifactName", (xmlChar*)from_codemodel.artifactName);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"buildArtefactType", (xmlChar*)"org.eclipse.cdt.build.core.buildArtefactType.exe" );
    strcpy(str, "org.eclipse.cdt.build.core.buildArtefactType=org.eclipse.cdt.build.core.buildArtefactType.exe,org.eclipse.cdt.build.core.buildType=org.eclipse.cdt.build.core.buildType.");
    strcat(str, build);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"buildProperties", (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"cleanCommand", (xmlChar*)"rm -rf");  // on windows too?
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"description", (xmlChar*)"");
    //xmlTextWriterWriteAttribute(cproject_writer, "errorParsers", "org.eclipse.cdt.core.GASErrorParser;org.eclipse.cdt.core.GmakeErrorParser;org.eclipse.cdt.core.GLDErrorParser;org.eclipse.cdt.core.CWDLocator;org.eclipse.cdt.core.GCCErrorParser");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)ccfg_id);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)Build);
    //xmlTextWriterWriteAttribute(cproject_writer, "optionalBuildProperties", "org.eclipse.cdt.docker.launcher.containerbuild.property.volumes=,org.eclipse.cdt.docker.launcher.containerbuild.property.selectedvolumes=");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"parent", (xmlChar*)cconfiguration_superClass);

    put_post_build_steps(artifactExtension);

    strcpy(str, ccfg_id);
    strcat(str, ".");

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"folderInfo");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"/");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"resourcePath", (xmlChar*)"");

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"toolChain");
    sprintf(parent_str, "%s.toolchain.gnu.cross.exe.%s", TITLE, build);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Ac6 STM32 MCU GCC");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

    if (Mcu[0] != 0) {
        xmlTextWriterStartElement(cproject_writer, option);
        sprintf(parent_str, "%s.option.gnu.cross.mcu", TITLE);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Mcu");
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)Mcu);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        xmlTextWriterEndElement(cproject_writer); // option
    } else
        printf("Warning: no Mcu defined (%s from cmake)\r\n", from_cache.board);

    const char *fpu = NULL;
    const char *float_abi = NULL;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (strncmp(my_list->str, "-mfpu=", 6) == 0) {
            fpu = my_list->str + 6;
            my_list->taken = true;
        } else if (strncmp(my_list->str, "-mfloat-abi=", 12) == 0) {
            float_abi = my_list->str + 12;
            my_list->taken = true;
        }
    }
    if (fpu) {
        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Floating point hardware");
        strcpy(parent_str, TITLE);
        strcat(parent_str, ".option.gnu.cross.fpu");
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        strcpy(str, parent_str);
        strcat(str, ".");
        strcat(str, fpu);
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        xmlTextWriterEndElement(cproject_writer); // option
    }
    if (float_abi) {
        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Floating point hardware");
        strcpy(parent_str, TITLE);
        strcat(parent_str, ".option.gnu.cross.floatabi");
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        strcpy(str, parent_str);
        strcat(str, ".");
        strcat(str, float_abi);
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        xmlTextWriterEndElement(cproject_writer); // option
    }

    if (Board[0] != 0) {
        xmlTextWriterStartElement(cproject_writer, option);
        sprintf(parent_str, "%s.option.gnu.cross.board", TITLE);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Board");
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)Board);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        xmlTextWriterEndElement(cproject_writer); // option
    } else
        printf("Warning: no Board defined (%s from cmake)\r\n", from_cache.board);

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"targetPlatform");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"archList", (xmlChar*)"all");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"binaryParser", (xmlChar*)"org.eclipse.cdt.core.ELF");
    sprintf(parent_str, "%s.targetPlatform.gnu.cross", TITLE);
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
    sprintf(parent_str, "%s.builder.gnu.cross", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"keepEnvironmentInBuildfile", (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"managedBuildOn", (xmlChar*)"true");
    strcpy(str, "Gnu Make Builder");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"parallelBuildOn", (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"parallelizationNumber", (xmlChar*)"optimal");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // builder

    sprintf(parent_str, "%s.tool.gnu.cross.c.compiler", TITLE);
    put_id(parent_str, str);
    strcat(instance_node->str, ";");
    strcat(instance_node->str, str);

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU GCC Compiler");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

    // -O
    char c_optimization_level = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken || strcmp("C", (char*)my_list->arg) != 0)
            continue;
        if (strncmp("-O", my_list->str, 2) == 0) {
            c_optimization_level = my_list->str[2];
            my_list->taken = true;
        }
    }
    if (c_optimization_level == 0) {
        for (struct node_s *my_list = c_flags; my_list != NULL; my_list = my_list->next) {
            if (strncmp("-O", my_list->str, 2) == 0) {
                c_optimization_level = my_list->str[2];
            }
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"defaultValue", (xmlChar*)"gnu.c.optimization.level.none");
    sprintf(parent_str, "%s.gnu.c.compiler.option.optimization.level", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Optimization Level");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");

    get_c_optimization_value(c_optimization_level, str);
    if (str[0] != 0) {
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    }

    xmlTextWriterEndElement(cproject_writer); // option

    // -g
    const char *c_debugging_level = NULL;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("C", (char*)my_list->arg) == 0 && strncmp("-g", my_list->str, 2) == 0) {
            c_debugging_level = my_list->str;
            my_list->taken = true;
        }
    }
    if (c_debugging_level == NULL) {
        for (struct node_s *my_list = c_flags; my_list != NULL; my_list = my_list->next) {
            if (strncmp("-g", my_list->str, 2) == 0) {
                c_debugging_level = my_list->str;
            }
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"defaultValue", (xmlChar*)"gnu.c.debugging.level.max");
    strcpy(parent_str, "gnu.c.compiler.option.debugging.level");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Debug Level");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(str, "gnu.c.debugging.level.");
    strcat_debug_level(c_debugging_level, str);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    xmlTextWriterEndElement(cproject_writer); // option

    bool c_defines = false;
    for (struct node_s *my_list = defines_list; my_list != NULL; my_list = my_list->next) {
        if (strcmp("C", (char*)my_list->arg) == 0) {
            c_defines = true;
            break;
        }
    }

    if (c_defines) {
        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Defined symbols (-D)");
        xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
        strcpy(parent_str, "gnu.c.compiler.option.preprocessor.def.symbols");
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
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
    }

    bool c_includes = false;
    for (struct node_s *my_list = includes_list; my_list != NULL; my_list = my_list->next) {
        if (strcmp("C", (char*)my_list->arg) == 0) {
            c_includes = true;
            break;
        }
    }

    if (c_includes) {
        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Include paths (-I)");
        xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
        strcpy(parent_str, "gnu.c.compiler.option.include.paths");
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
    }

    // -fdata-sections
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("C", (char*)my_list->arg) == 0 && strcmp("-fdata-sections", my_list->str) == 0) {
            xmlTextWriterStartElement(cproject_writer, option);
            sprintf(parent_str, "%s.tool.gnu.cross.c.compiler.fdata", TITLE);
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
    const char *c_dialect = "default";
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        /* go thru all args, last one takes effect */
        if (strcmp("C", (char*)my_list->arg) == 0 && strncmp("-std=", my_list->str, 5) == 0) {
            get_c_dialect_value_from_fragment(my_list->str, &c_dialect);
            my_list->taken = true;
        }
    }

    if (c_dialect) {
        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Language standard");
        strcpy(parent_str, "gnu.c.compiler.option.dialect.std");
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
        strcpy(str, "gnu.c.compiler.dialect.");
        strcat(str, c_dialect);
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
        xmlTextWriterEndElement(cproject_writer); // option
    }

    // -pedantic
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("C", (char*)my_list->arg) == 0 && strcmp("-pedantic", my_list->str) == 0) {
            xmlTextWriterStartElement(cproject_writer, option);
            strcpy(parent_str, "gnu.c.compiler.option.warnings.pedantic");
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
        if (strcmp("C", (char*)my_list->arg) == 0 && strcmp("-Wextra", my_list->str) == 0) {
            xmlTextWriterStartElement(cproject_writer, option);
            strcpy(parent_str, "gnu.c.compiler.option.warnings.extrawarn");
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
    put_compiler_other_flags("C", "fr.ac6.managedbuid.gnu.c.compiler.option.misc.other");

    sprintf(parent_str, "%s.tool.gnu.cross.c.compiler.input.c", TITLE);
    put_id(parent_str, str);
    strcat(instance_node->str, ";");
    strcat(instance_node->str, str);

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // inputType

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    sprintf(parent_str, "%s.tool.gnu.cross.c.compiler.input.s", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // inputType

    xmlTextWriterEndElement(cproject_writer); // tool  tool.gnu.cross.c.compiler

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    sprintf(parent_str, "%s.tool.gnu.cross.cpp.compiler", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU G++ Compiler");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    strcpy(cpp_compiler_id, id_str);

    // -O
    char cpp_optimization_level = 0;
    /* ? cmake variable takes precedence over flag from fragment ? */
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken || strcmp("CXX", (char*)my_list->arg) != 0)
            continue;
        if (strncmp("-O", my_list->str, 2) == 0) {
            cpp_optimization_level = my_list->str[2];
            my_list->taken = true;
        }
    }
    if (cpp_optimization_level == 0) {
        for (struct node_s *my_list = cpp_flags; my_list != NULL; my_list = my_list->next) {
            if (strncmp("-O", my_list->str, 2) == 0) {
                cpp_optimization_level = my_list->str[2];
            }
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.gnu.cpp.compiler.option.optimization.level", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Optimization Level");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    if (cpp_optimization_level != 0) {
        get_cpp_optimization_value(cpp_optimization_level, str);
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    }
    xmlTextWriterEndElement(cproject_writer); // option

    // -g
    const char *cpp_debugging_level = NULL;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("CXX", (char*)my_list->arg) == 0 && strncmp("-g", my_list->str, 2) == 0) {
            cpp_debugging_level = my_list->str;
            my_list->taken = true;
        }
    }
    if (cpp_debugging_level == NULL) {
        for (struct node_s *my_list = c_flags; my_list != NULL; my_list = my_list->next) {
            if (strncmp("-g", my_list->str, 2) == 0) {
                cpp_debugging_level = my_list->str;
            }
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Debug Level");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"defaultValue", (xmlChar*)"gnu.cpp.debugging.level.max");
    strcpy(parent_str, "gnu.cpp.compiler.option.debugging.level");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    if (cpp_debugging_level) {
        strcpy(str, "gnu.cpp.debugging.level.");
        strcat_debug_level(cpp_debugging_level, str);
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    }
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    xmlTextWriterEndElement(cproject_writer); // option

    if (cpp_input) {
        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Defined symbols (-D)");
        xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
        strcpy(parent_str, "gnu.cpp.compiler.option.preprocessor.def");
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"definedSymbols");
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
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
        strcpy(parent_str, "gnu.cpp.compiler.option.include.paths");
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
    }

    // -std=foo
    const char *cpp_dialect = NULL;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        /* go thru all args, last one takes effect */
        if (strcmp("CXX", (char*)my_list->arg) == 0 && strncmp("-std=", my_list->str, 5) == 0) {
            cpp_dialect = my_list->str + 5;
            my_list->taken = true;
        }
    }

    if (cpp_dialect) {
        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Language standard");
        strcpy(parent_str, "gnu.cpp.compiler.option.dialect.std");
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
        strcpy(str, "gnu.cpp.compiler.dialect.");
        strcat(str, cpp_dialect);
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
        xmlTextWriterEndElement(cproject_writer); // option
    }

    // -fdata-sections
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("CXX", (char*)my_list->arg) == 0 && strcmp("-fdata-sections", my_list->str) == 0) {
            xmlTextWriterStartElement(cproject_writer, option);
            sprintf(parent_str, "%s.tool.gnu.cross.cpp.compiler.fdata", TITLE);
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

    // -pedantic
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("CXX", (char*)my_list->arg) == 0 && strcmp("-pedantic", my_list->str) == 0) {
            xmlTextWriterStartElement(cproject_writer, option);
            strcpy(parent_str, "gnu.cpp.compiler.option.warnings.pedantic");
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
        if (strcmp("CXX", (char*)my_list->arg) == 0 && strcmp("-Wextra", my_list->str) == 0) {
            xmlTextWriterStartElement(cproject_writer, option);
            strcpy(parent_str, "gnu.cpp.compiler.option.warnings.extrawarn");
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
    put_compiler_other_flags("CXX", "fr.ac6.managedbuild.gnu.cpp.compiler.option.misc.other");

    if (cpp_input) {
        instance_node = add_instance(ccfg_id);
        strcat(instance_node->str, ";");
        strcat(instance_node->str, cpp_compiler_id);

        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
        sprintf(parent_str, "%s.tool.gnu.cross.cpp.compiler.input.cpp", TITLE);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        put_id(parent_str, str);
        strcat(instance_node->str, ";");
        strcat(instance_node->str, str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)str);
        xmlTextWriterEndElement(cproject_writer); // inputType

        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
        sprintf(parent_str, "%s.tool.gnu.cross.cpp.compiler.input.s", TITLE);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterEndElement(cproject_writer); // inputType
    }

    xmlTextWriterEndElement(cproject_writer); // tool       tool.gnu.cross.cpp.compiler

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    sprintf(parent_str, "%s.tool.gnu.cross.c.linker", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU GCC Linker");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

    if (!cpp_linker) {
        if (linker_script[0] != 0) {
            xmlTextWriterStartElement(cproject_writer, option);
            sprintf(parent_str, "%s.tool.gnu.cross.c.linker.script", TITLE);
            put_id(parent_str, id_str);
            xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
            xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Linker Script (-T)");
            xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
            xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
            xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)linker_script);
            xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
            xmlTextWriterEndElement(cproject_writer); // option
        }

        xmlTextWriterStartElement(cproject_writer, option);
        strcpy(parent_str, "gnu.c.link.option.ldflags");
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        str[0] = 0;
        for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
            if (!my_list->taken && strcmp("C", (char*)my_list->arg) == 0) {
                if (strstr(my_list->str, "-specs") != NULL) {
                    if (str[0] != 0)
                        strcat(str, " ");
                    strcat(str, my_list->str);
                } else if (strncmp(my_list->str, "-Wl,", 4) == 0) {
                    const char *z = strstr(my_list->str, "-z");
                    if (z) {
                        if (str[0] != 0)
                            strcat(str, " -z ");
                        strcat(str, z + 2);
                    }
                }
            }
        }
        if (str[0] != 0) {
            xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
            xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
        }
        xmlTextWriterEndElement(cproject_writer); // option

        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
        strcpy(parent_str, "cdt.managedbuild.tool.gnu.c.linker.input");
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
    } // ..if (!cpp_linker)

    xmlTextWriterEndElement(cproject_writer); // tool    tool.gnu.cross.c.linker

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    sprintf(parent_str, "%s.tool.gnu.cross.cpp.linker", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU G++ Linker");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

    if (cpp_linker) {
        if (linker_script[0] != 0) {
            xmlTextWriterStartElement(cproject_writer, option);
            sprintf(parent_str, "%s.tool.gnu.cross.cpp.linker.script", TITLE);
            put_id(parent_str, id_str);
            xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
            xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Linker Script (-T)");
            xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
            xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
            xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)linker_script);
            xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
            xmlTextWriterEndElement(cproject_writer); // option
        }

        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"true");
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Other options (-Xlinker [option])");
        strcpy(parent_str, "gnu.cpp.link.option.other");
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"stringList");
        xmlTextWriterEndElement(cproject_writer); // option

        xmlTextWriterStartElement(cproject_writer, option);
        xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"true");
        xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Other objects");
        strcpy(parent_str, "gnu.cpp.link.option.userobjs");
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"userObjs");
        xmlTextWriterEndElement(cproject_writer); // option

        xmlTextWriterStartElement(cproject_writer, option);
        strcpy(parent_str, "gnu.cpp.link.option.flags");
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
        xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
        str[0] = 0;
        for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
            if (!my_list->taken && strcmp("CXX", (char*)my_list->arg) == 0 && strstr(my_list->str, "-specs") != NULL) {
                if (str[0] != 0)
                    strcat(str, " ");
                strcat(str, my_list->str);
            }
        }
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
        xmlTextWriterEndElement(cproject_writer); // option

        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
        strcpy(parent_str, "cdt.managedbuild.tool.gnu.cpp.linker.input");
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
    } // ..if (cpp_linker)

    xmlTextWriterEndElement(cproject_writer); // tool      tool.gnu.cross.cpp.linker

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    sprintf(parent_str, "%s.tool.gnu.archiver", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU GCC Archiver");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // tool

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    if (debugBuild)
        sprintf(parent_str, "%s.tool.gnu.cross.assembler", TITLE);
    else
        sprintf(parent_str, "%s.tool.gnu.cross.assembler.exe.release", TITLE);

    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU GCC Assembler");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    strcpy(parent_str, "cdt.managedbuild.tool.gnu.assembler.input");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // inputType

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    sprintf(parent_str, "%s.tool.gnu.cross.assembler.input", TITLE);
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
    sprintf(out_superClass, "%s.config.gnu.cross.exe.%s", TITLE, debugBuild ? "debug" : "release");
    strcpy(out_id, out_superClass);
    strcat(out_id, ".");
    get_us(strb);
    strcat(out_id, strb);

    return 0;
}

void put_cdt_project()
{
}

void write_natures()
{
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"fr.ac6.mcu.ide.core.MCUProjectNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"fr.ac6.mcu.ide.core.MCUSingleCoreProjectNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"org.eclipse.cdt.managedbuilder.core.managedBuildNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"org.eclipse.cdt.managedbuilder.core.ScannerConfigNature");
}

void put_project_other_builders() { }
void put_other_cconfiguration_storageModules(bool debugBuild) { }
int unbuilt_source(const char *source_path, const char *path) { return 0;}
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

