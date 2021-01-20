#include "json_eclipse_cdt.h"

/*
 * https://www.nxp.com/design/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-integrated-development-environment-ide:MCUXpresso-IDE
 */

const char * const GENMAKEBUILDER_TRIGGERS = "clean,full,incremental,";
const char * const GENMAKEBUILDER_ARGUMENTS = NULL;
const char * const SCANNERCONFIGBUILDER_TRIGGERS = "full,incremental,";
const char * const SCANNERCONFIGBUILDER_ARGUMENTS = "";
const char * const CDT_CORE_SETTINGS_CONFIGRELATIONS = NULL;
const char * const BINARY_PARSERS[] = {
    "org.eclipse.cdt.core.ELF",
    "org.eclipse.cdt.core.GNU_ELF",
    NULL
};
const char * const ERROR_PARSERS[] = {
    "org.eclipse.cdt.core.CWDLocator",
    "org.eclipse.cdt.core.GmakeErrorParser",
    "org.eclipse.cdt.core.GCCErrorParser",
    "org.eclipse.cdt.core.GLDErrorParser",
    "org.eclipse.cdt.core.GASErrorParser",
    NULL
};

const char * const TITLE = "com.crt.advproject";

int _put_configuration(bool debugBuild, const char *ccfg_id, const char *cconfiguration_superClass, const char *Board, const char *Mcu, struct node_s *instance_node)
{
    char parent_str[96];
    char id_str[128];
    char str[256];
    const char *build;
    const char *Build;
    if (debugBuild) {
        build = "debug";
        Build = "Debug";
    } else {
        build = "release";
        Build = "Release";
    }

    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)ccfg_id);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"artifactExtension", (xmlChar*)"axf");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"artifactName", (xmlChar*)from_codemodel.artifactName);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"buildArtefactType", (xmlChar*)"org.eclipse.cdt.build.core.buildArtefactType.exe" );
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"buildProperties", (xmlChar*)"org.eclipse.cdt.build.core.buildArtefactType=org.eclipse.cdt.build.core.buildArtefactType.exe");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"cleanCommand", (xmlChar*)"rm -rf");  // on windows too?
    strcpy(str, Build);
    strcat(str, " build");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"description", (xmlChar*)str);

    str[0] = 0;
    for (unsigned n = 0; ERROR_PARSERS[n] != NULL; n++) {
        if (str[0] == 0)
            strcpy(str, ERROR_PARSERS[n]);
        else {
            strcat(str, ";");
            strcat(str, ERROR_PARSERS[n]);
        }
    }
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"errorParsers", (xmlChar*)str);

    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"name", (xmlChar*)Build);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"parent", (xmlChar*)cconfiguration_superClass);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"postannouncebuildStep", (xmlChar*)"Performing post-build steps");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"postbuildStep", (xmlChar*)"arm-none-eabi-size \"${BuildArtifactFileName}\"; # arm-none-eabi-objcopy -v -O binary \"${BuildArtifactFileName}\" \"${BuildArtifactFileBaseName}.bin\"; # checksum -p ${TargetChip} -d \"${BuildArtifactFileBaseName}.bin\"; ");

//#if 0
    strcpy(str, ccfg_id);
    strcat(str, ".");
    strcat(instance_node->str, ";");
    strcat(instance_node->str, str);

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"folderInfo");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"/");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"resourcePath", (xmlChar*)"");

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"toolChain");
    sprintf(parent_str, "%s.toolchain.exe.%s", TITLE, build);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"NXP MCU Tools");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"targetPlatform");
    strcpy(str, "ARM-based MCU (");
    strcat(str, Build);
    strcat(str, ")");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)str);
    str[0] = 0;
    for (unsigned n = 0; BINARY_PARSERS[n] != NULL; n++) {
        if (str[0] == 0)
            strcpy(str, BINARY_PARSERS[n]);
        else {
            strcat(str, ";");
            strcat(str, BINARY_PARSERS[n]);
        }
    }
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"binaryParser", (xmlChar*)str);
    sprintf(parent_str, "%s.platform.exe.%s", TITLE, build);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // targetPlatform

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"builder");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Gnu Make Builder");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"managedBuildOn", (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"keepEnvironmentInBuildfile", (xmlChar*)"false");
    sprintf(parent_str, "%s.builder.exe.%s", TITLE, build);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    strcpy(str, from_codemodel.buildPath);
    strcat(str, "/");
    strcat(str, Build);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"buildPath", (xmlChar*)str);
    xmlTextWriterEndElement(cproject_writer); // builder

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU C++ Compiler");
    sprintf(parent_str, "%s.cpp.exe.%s", TITLE, build);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Library headers");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.cpp.hdrlib", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Floating point");
    sprintf(parent_str, "%s.cpp.fpu", TITLE);
    strcpy(str, parent_str);
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp(my_list->str, "-mfpu=", 6) == 0) {
            if (strstr(my_list->str, "fpv4") != NULL) {
                strcat(str, ".fpv4");
                break;
            } else {
                printf("\e[31mline %d %s\e[0m\n", __LINE__, my_list->str);
                return -1;
            }
        }
    }
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp(my_list->str, "-mfloat-abi=", 12) == 0) {
            strcat(str, ".");
            strcat(str, my_list->str+12);
        }
    }
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Architecture");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    sprintf(parent_str, "%s.cpp.arch", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    strcpy(str, TITLE);
    strcat(str, ".cpp.target.");
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp(my_list->str, "-mcpu=", 6) == 0) {
            if (strcmp(my_list->str+6, "cortex-m4") == 0) {
                strcat(str, "cm4");
            } else {
                printf("\e[31mline %d %s\e[0m\n", __LINE__, my_list->str);
                return -1;
            }
        }
    }
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Language standard");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    sprintf(parent_str, "%s.cpp.misc.dialect", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Other dialect flags");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    strcpy(parent_str, "gnu.cpp.compiler.option.dialect.flags");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Do not search system directories (-nostdinc)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.preprocessor.nostdinc");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Preprocess only (-E)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.preprocessor.preprocess");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Defined symbols (-D)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.preprocessor.def");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Undefined symbols (-U)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.preprocessor.undef");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Include files (-include)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.include.files");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Include paths (-I)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.include.paths");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Optimization Level");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    sprintf(parent_str, "%s.cpp.exe.debug.option.optimization.level", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Other optimization flags");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"-fno-common");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.optimization.flags");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Debug Level");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.cpp.exe.debug.option.debugging.level", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Other debugging flags");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.debugging.other");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Generate prof information (-p)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.debugging.prof");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Generate gprof information (-pg)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.debugging.gprof");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Generate gcov information (-ftest-coverage -fprofile-arcs)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.debugging.codecov");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Check syntax only (-fsyntax-only)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.warnings.syntax");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Pedantic (-pedantic)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.warnings.pedantic");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Pedantic warnings as errors (-pedantic-errors)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.warnings.pedantic.error");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Inhibit all warnings (-w)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.warnings.nowarn");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"All warnings (-Wall)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.warnings.allwarn");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Extra warnings (-Wextra)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.warnings.extrawarn");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Warnings as errors (-Werror)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.warnings.toerrors");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Implicit conversion warnings (-Wconversion)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.warnings.wconversion");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Other flags");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.other.other");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Verbose (-v)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.other.verbose");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Position Independent Code (-fPIC)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.cpp.compiler.option.other.pic");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Enable Link-time optimization (-flto)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.cpp.lto", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Fat lto objects (-ffat-lto-objects)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.cpp.lto.fat", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Merge Identical Constants (-fmerge-constants)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.cpp.merge.constants", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Remove path from __FILE__ (-fmacro-prefix-map)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.cpp.prefixmap", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Thumb mode");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.cpp.thumb", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Enable Thumb interworking");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.cpp.thumbinterwork", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"TrustZone Project Type");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.cpp.securestate", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Generate Stack Usage Info (-fstack-usage)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.cpp.stackusage", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Specs");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.cpp.specs", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Obsolete (Config)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.cpp.config", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Obsolete (Store)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.cpp.store", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterEndElement(cproject_writer); // tool  cpp.exe.

    sprintf(parent_str, "%s.gcc.exe.%s", TITLE, build);
    put_id(parent_str, id_str);
    strcat(instance_node->str, ";");
    strcat(instance_node->str, id_str);

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU C Compiler");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Library headers");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.gcc.hdrlib", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Defined symbols (-D)");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"definedSymbols");
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.preprocessor.def.symbols");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    if (defines_list) {
        struct node_s *my_list;
        for (my_list = defines_list; my_list != NULL; my_list = my_list->next) {
            put_listOptionValue(cproject_writer, false, my_list->str);
        }
    }
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Floating point");
    sprintf(parent_str, "%s.gcc.fpu", TITLE);
    strcpy(str, parent_str);
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp(my_list->str, "-mfpu=", 6) == 0) {
            if (strstr(my_list->str, "fpv4") != NULL) {
                strcat(str, ".fpv4");
                break;
            } else {
                printf("\e[31mline %d %s\e[0m\n", __LINE__, my_list->str);
                return -1;
            }
        }
    }
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp(my_list->str, "-mfloat-abi=", 12) == 0) {
            strcat(str, ".");
            strcat(str, my_list->str+12);
        }
    }
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Thumb mode");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.gcc.thumb", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Architecture");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    sprintf(parent_str, "%s.gcc.arch", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    strcpy(str, TITLE);
    strcat(str, ".gcc.target.");
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp(my_list->str, "-mcpu=", 6) == 0) {
            if (strcmp(my_list->str+6, "cortex-m4") == 0) {
                strcat(str, "cm4");
            } else {
                printf("\e[31mline %d %s\e[0m\n", __LINE__, my_list->str);
                return -1;
            }
        }
    }
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Language standard");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    sprintf(parent_str, "%s.c.misc.dialect", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Other dialect flags");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    strcpy(parent_str, "gnu.c.compiler.option.dialect.flags");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Do not search system directories (-nostdinc)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.preprocessor.nostdinc");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Preprocess only (-E)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.preprocessor.preprocess");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Undefined symbols (-U)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.preprocessor.undef.symbol");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Include paths (-I)");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"includePath");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.include.paths");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    if (includes_list) {
        struct node_s *my_list;
        for (my_list = includes_list; my_list != NULL; my_list = my_list->next) {
            put_listOptionValue(cproject_writer, false, my_list->str);
        }
    }
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Include files (-include)");
    strcpy(parent_str, "gnu.c.compiler.option.include.files");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Optimization Level");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    sprintf(parent_str, "%s.gcc.exe.debug.option.optimization.level", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Other optimization flags");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"-fno-common");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.optimization.flags");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Debug Level");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.gcc.exe.debug.option.debugging.level", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Other debugging flags");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.debugging.other");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Generate prof information (-p)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.debugging.prof");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Generate gprof information (-pg)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.debugging.gprof");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Generate gcov information (-ftest-coverage -fprofile-arcs)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.debugging.codecov");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Check syntax only (-fsyntax-only)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.warnings.syntax");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Pedantic (-pedantic)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.warnings.pedantic");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Pedantic warnings as errors (-pedantic-errors)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.warnings.pedantic.error");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Inhibit all warnings (-w)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.warnings.nowarn");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"All warnings (-Wall)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.warnings.allwarn");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Extra warnings (-Wextra)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.warnings.extrawarn");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Warnings as errors (-Werror)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.warnings.toerrors");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Implicit conversion warnings (-Wconversion)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.warnings.wconversion");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Other flags");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.other.other");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Verbose (-v)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.other.verbose");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Position Independent Code (-fPIC)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.other.pic");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Other flags");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"-c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.misc.other");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Verbose (-v)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.misc.verbose");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Support ANSI programs (-ansi)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.misc.ansi");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Position Independent Code (-fPIC)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.c.compiler.option.misc.pic");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Enable Link-time optimization (-flto)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.gcc.lto", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Fat lto objects (-ffat-lto-objects)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.gcc.lto.fat", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Merge Identical Constants (-fmerge-constants)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.gcc.merge.constants", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Remove path from __FILE__ (-fmacro-prefix-map)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.gcc.prefixmap", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Enable Thumb interworking");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.gcc.thumbinterwork", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"TrustZone Project Type");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.gcc.securestate", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Generate Stack Usage Info (-fstack-usage)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.gcc.stackusage", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Specs");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.gcc.specs", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Obsolete (Config)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.gcc.config", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Obsolete (Store)");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.gcc.store", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    sprintf(parent_str, "%s.compiler.input", TITLE);
    put_id(parent_str, id_str);
    strcat(instance_node->str, ";");
    strcat(instance_node->str, id_str);

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterEndElement(cproject_writer); // inputType

    instance_node->next = malloc(sizeof(struct node_s));
    instance_node = instance_node->next;
    instance_node->next = NULL;
    instance_node->str = malloc(INSTANCE_MAX_STRLEN);
    strcpy(instance_node->str, ccfg_id);
    strcat(instance_node->str, ";");
    strcat(instance_node->str, ccfg_id);
    strcat(instance_node->str, ".");

    xmlTextWriterEndElement(cproject_writer); // tool  gcc.exe.

    sprintf(parent_str, "%s.gas.exe.%s", TITLE, build);
    put_id(parent_str, str);
    strcat(instance_node->str, ";");
    strcat(instance_node->str, str);

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU Assembler");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)str);

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Library headers");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.gas.hdrlib", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Floating point");
    sprintf(parent_str, "%s.gas.fpu", TITLE);
    strcpy(str, parent_str);
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp(my_list->str, "-mfpu=", 6) == 0) {
            if (strstr(my_list->str, "fpv4") != NULL) {
                strcat(str, ".fpv4");
                break;
            } else {
                printf("\e[31mline %d %s\e[0m\n", __LINE__, my_list->str);
                return -1;
            }
        }
    }
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp(my_list->str, "-mfloat-abi=", 12) == 0) {
            strcat(str, ".");
            strcat(str, my_list->str+12);
        }
    }
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Thumb mode");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.gas.thumb", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Architecture");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    sprintf(parent_str, "%s.gas.arch", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    strcpy(str, TITLE);
    strcat(str, ".gas.target.");
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp(my_list->str, "-mcpu=", 6) == 0) {
            if (strcmp(my_list->str+6, "cortex-m4") == 0) {
                strcat(str, "cm4");
            } else {
                printf("\e[31mline %d %s\e[0m\n", __LINE__, my_list->str);
                return -1;
            }
        }
    }
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"-c -x assembler-with-cpp -D__REDLIB__");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Assembler flags");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    strcpy(parent_str, "gnu.both.asm.option.flags.crt");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Include paths (-I)");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"includePath");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    strcpy(parent_str, "gnu.both.asm.option.include.paths");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    if (includes_list) {
        struct node_s *my_list;
        for (my_list = includes_list; my_list != NULL; my_list = my_list->next) {
            put_listOptionValue(cproject_writer, false, my_list->str);
        }
    }
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Inhibit all warnings (-w)");
    strcpy(parent_str, "gnu.both.asm.option.warnings.nowarn");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Announce version (-v)");
    strcpy(parent_str, "gnu.both.asm.option.version");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Debug Level");
    sprintf(parent_str, "%s.gas.exe.debug.option.debugging.level", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Enable Thumb interworking");
    sprintf(parent_str, "%s.gas.thumbinterwork", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Specs");
    sprintf(parent_str, "%s.gas.specs", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Obsolete (Config)");
    sprintf(parent_str, "%s.gas.config", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Obsolete (Store)");
    sprintf(parent_str, "%s.gas.store", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    strcpy(parent_str, "cdt.managedbuild.tool.gnu.assembler.input");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // inputType

    strcpy(parent_str, "com.crt.advproject.assembler.input");
    put_id(parent_str, id_str);
    strcat(instance_node->str, ";");
    strcat(instance_node->str, id_str);

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Additional Assembly Source Files");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // inputType

    xmlTextWriterEndElement(cproject_writer); // tool  gas.exe.

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU C++ Linker");
    sprintf(parent_str, "%s.link.cpp.exe.%s", TITLE, build);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Library");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    sprintf(parent_str, "%s.link.cpp.hdrlib", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Floating point");
    sprintf(parent_str, "%s.link.cpp.fpu", TITLE);
    strcpy(str, parent_str);
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp(my_list->str, "-mfpu=", 6) == 0) {
            if (strstr(my_list->str, "fpv4") != NULL) {
                strcat(str, ".fpv4");
                break;
            } else {
                printf("\e[31mline %d %s\e[0m\n", __LINE__, my_list->str);
                return -1;
            }
        }
    }
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp(my_list->str, "-mfloat-abi=", 12) == 0) {
            strcat(str, ".");
            strcat(str, my_list->str+12);
        }
    }
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Architecture");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    sprintf(parent_str, "%s.link.cpp.arch", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    strcpy(str, TITLE);
    strcat(str, ".link.cpp.target.");
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp(my_list->str, "-mcpu=", 6) == 0) {
            if (strcmp(my_list->str+6, "cortex-m4") == 0) {
                strcat(str, "cm4");
            } else {
                printf("\e[31mline %d %s\e[0m\n", __LINE__, my_list->str);
                return -1;
            }
        }
    }
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Do not use standard start files (-nostartfiles)");
    strcpy(parent_str, "gnu.cpp.link.option.nostart");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Do not use default libraries (-nodefaultlibs)");
    strcpy(parent_str, "gnu.cpp.link.option.nodeflibs");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"No startup or default libs (-nostdlib)");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    strcpy(parent_str, "gnu.cpp.link.option.nostdlibs");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Omit all symbol information (-s)");
    strcpy(parent_str, "gnu.cpp.link.option.strip");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Libraries (-l)");
    strcpy(parent_str, "gnu.cpp.link.option.libs");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Library search path (-L)");
    strcpy(parent_str, "gnu.cpp.link.option.paths");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Linker flags");
    strcpy(parent_str, "gnu.cpp.link.option.flags");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Other options (-Xlinker [option])");
    strcpy(parent_str, "gnu.cpp.link.option.other");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Other objects");
    strcpy(parent_str, "gnu.cpp.link.option.userobjs");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Shared (-shared)");
    strcpy(parent_str, "gnu.cpp.link.option.shared");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Shared object name (-Wl,-soname=)");
    strcpy(parent_str, "gnu.cpp.link.option.soname");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Import Library name (-Wl,--out-implib=)");
    strcpy(parent_str, "gnu.cpp.link.option.implname");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"DEF file name (-Wl,--output-def=)");
    strcpy(parent_str, "gnu.cpp.link.option.defname");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Generate prof information (-p)");
    strcpy(parent_str, "gnu.cpp.link.option.debugging.prof");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Generate gprof information (-pg)");
    strcpy(parent_str, "gnu.cpp.link.option.debugging.gprof");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Generate gcov information (-ftest-coverage -fprofile-arcs)");
    strcpy(parent_str, "gnu.cpp.link.option.debugging.codecov");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Enable Link-time optimization (-flto)");
    sprintf(parent_str, "%s.link.cpp.lto", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Link-time optimization level");
    sprintf(parent_str, "%s.link.cpp.lto.optmization.level", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Thumb mode");
    sprintf(parent_str, "%s.link.cpp.thumb", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Manage linker script");
    sprintf(parent_str, "%s.link.cpp.manage", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Linker script");
    sprintf(parent_str, "%s.link.cpp.script", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Script path");
    sprintf(parent_str, "%s.link.cpp.scriptdir", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Enable automatic placement of Code Read Protection field in image");
    sprintf(parent_str, "%s.link.cpp.crpenable", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Enable automatic placement of Flash Configuration field in image");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    sprintf(parent_str, "%s.link.cpp.flashconfigenable", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Enhanced CRP");
    sprintf(parent_str, "%s.link.cpp.ecrp", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Enable printf float");
    sprintf(parent_str, "%s.link.cpp.nanofloat", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Enable scanf float");
    sprintf(parent_str, "%s.link.cpp.nanofloat.scanf", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Link application to RAM");
    sprintf(parent_str, "%s.link.cpp.toram", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Plain load image");
    sprintf(parent_str, "%s.link.memory.load.image.cpp", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Heap and Stack placement");
    sprintf(parent_str, "%s.link.memory.heapAndStack.style.cpp", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Stack offset");
    sprintf(parent_str, "%s.link.cpp.stackOffset", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Heap and Stack options");
    sprintf(parent_str, "%s.link.memory.heapAndStack.cpp", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Global data placement");
    sprintf(parent_str, "%s.link.memory.data.cpp", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Extra linker script input sections");
    sprintf(parent_str, "%s.link.memory.sections.cpp", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Multicore configuration");
    sprintf(parent_str, "%s.link.cpp.multicore.slave", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Multicore master");
    sprintf(parent_str, "%s.link.cpp.multicore.master", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"No Multicore options for this project");
    sprintf(parent_str, "%s.link.cpp.multicore.empty", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Slave Objects (not visible)");
    sprintf(parent_str, "%s.link.cpp.multicore.master.userobjs", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Obsolete (Config)");
    sprintf(parent_str, "%s.link.cpp.config", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Obsolete (Store)");
    sprintf(parent_str, "%s.link.cpp.store", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"TrustZone Project Type");
    sprintf(parent_str, "%s.link.cpp.securestate", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Secure Gateway Placement");
    sprintf(parent_str, "%s.link.cpp.sgstubs.placement", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Enable generation of Secure Gateway Import Library");
    sprintf(parent_str, "%s.link.cpp.sgstubenable", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Secure Gateway Import Library");
    sprintf(parent_str, "%s.link.cpp.nonsecureobject", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Input Secure Gateway Import Library");
    sprintf(parent_str, "%s.link.cpp.inimplib", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterEndElement(cproject_writer); // tool  link.cpp.exe.build

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU Linker");
    sprintf(parent_str, "%s.link.exe.%s", TITLE, build);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Library");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"com.crt.advproject.gcc.link.hdrlib.codered.semihost_nf");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    sprintf(parent_str, "%s.link.gcc.hdrlib", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Floating point");
    sprintf(parent_str, "%s.link.fpu", TITLE);
    strcpy(str, parent_str);
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp(my_list->str, "-mfpu=", 6) == 0) {
            if (strstr(my_list->str, "fpv4") != NULL) {
                strcat(str, ".fpv4");
                break;
            } else {
                printf("\e[31mline %d %s\e[0m\n", __LINE__, my_list->str);
                return -1;
            }
        }
    }
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp(my_list->str, "-mfloat-abi=", 12) == 0) {
            strcat(str, ".");
            strcat(str, my_list->str+12);
        }
    }
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    sprintf(parent_str, "%s.link.thumb", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"");
    sprintf(parent_str, "%s.link.memory.load.image", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"defaultValue", (xmlChar*)"com.crt.advproject.heapAndStack.mcuXpressoStyle");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    sprintf(parent_str, "%s.link.memory.heapAndStack.style", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"&Heap:Default;Post Data;Default&Stack:Default;End;Default");
    sprintf(parent_str, "%s.link.memory.heapAndStack", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"");
    sprintf(parent_str, "%s.link.memory.data", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.memory.sections", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"stringList");
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.gcc.multicore.master.userobjs", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"userObjs");
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, enumerated);
    sprintf(parent_str, "%s.link.arch", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    strcpy(str, TITLE);
    strcat(str, ".link.target.");
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp(my_list->str, "-mcpu=", 6) == 0) {
            if (strcmp(my_list->str+6, "cortex-m4") == 0) {
                strcat(str, "cm4");
            } else {
                printf("\e[31mline %d %s\e[0m\n", __LINE__, my_list->str);
                return -1;
            }
        }
    }
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, "gnu.c.link.option.nostart");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, "gnu.c.link.option.nodeflibs");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    strcpy(parent_str, "gnu.c.link.option.nostdlibs");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, "gnu.c.link.option.strip");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, "gnu.c.link.option.noshared");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, "gnu.c.link.option.libs");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
/*    const char *linker_paths = NULL;
    for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (strncmp(my_list->str, "-L", 2) == 0) {
            linker_paths = my_list->str + 2;
            printf(">>>> %s <<<<\n", my_list->str);
        }
    }*/
    strcpy(parent_str, "gnu.c.link.option.paths");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
/*    if (linker_paths) {
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"libPaths");
        put_listOptionValue(cproject_writer, false, linker_paths);
    }*/
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, "gnu.c.link.option.ldflags");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, "gnu.c.link.option.other");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"stringList");
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    put_listOptionValue(cproject_writer, false, "-Map=\"${BuildArtifactFileBaseName}.map\"");
    put_listOptionValue(cproject_writer, false, "--gc-sections");
    put_listOptionValue(cproject_writer, false, "-print-memory-usage");
    put_listOptionValue(cproject_writer, false, "--sort-section=alignment");
    put_listOptionValue(cproject_writer, false, "--cref");
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, "gnu.c.link.option.userobjs");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, "gnu.c.link.option.shared");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, "gnu.c.link.option.soname");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, "gnu.c.link.option.implname");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, "gnu.c.link.option.defname");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, "gnu.c.link.option.debugging.prof");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, "gnu.c.link.option.debugging.gprof");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, "gnu.c.link.option.debugging.codecov");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.gcc.lto", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.gcc.lto.optmization.level", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    /* mcuXpresso would create its own linker script instead of using the one from original project */
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"false");
    sprintf(parent_str, "%s.link.manage", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (strncmp(my_list->str, "-T", 2) == 0) {
            xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)my_list->str+2);
        }
    }
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    sprintf(parent_str, "%s.link.script", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    const char *linker_paths = NULL;
    for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (strncmp(my_list->str, "-L", 2) == 0) {
            linker_paths = my_list->str + 2;
            printf(">>>> %s <<<<\n", my_list->str);
        }
    }
    if (linker_paths) {
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)linker_paths);
    }
    sprintf(parent_str, "%s.link.scriptdir", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.crpenable", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    sprintf(parent_str, "%s.link.flashconfigenable", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.ecrp", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.gcc.nanofloat", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.gcc.nanofloat.scanf", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.toram", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.stackOffset", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.gcc.multicore.slave", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.gcc.multicore.master", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.gcc.multicore.empty", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.config", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.store", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.securestate", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.sgstubs.placement", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.sgstubenable", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.nonsecureobject", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.link.inimplib", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    strcpy(parent_str, "cdt.managedbuild.tool.gnu.c.linker.input");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    put_additionalInput(cproject_writer, "additionalinputdependency", "$(USER_OBJS)");
    put_additionalInput(cproject_writer, "additionalinput", "$(LIBS)");
    xmlTextWriterEndElement(cproject_writer); // inputType

    xmlTextWriterEndElement(cproject_writer); // tool  link.exe.build

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"MCU Debugger");
    sprintf(parent_str, "%s.tool.debug.%s", TITLE, build);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.linkserver.debug.prevent.debug", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.miscellaneous.end_of_heap", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.miscellaneous.pvHeapStart", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.miscellaneous.pvHeapLimit", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.debugger.security.nonsecureimageenable", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterStartElement(cproject_writer, option);
    sprintf(parent_str, "%s.debugger.security.nonsecureimage", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // option 

    xmlTextWriterEndElement(cproject_writer); // tool  tool.debug.build

    xmlTextWriterEndElement(cproject_writer); // toolChain
    xmlTextWriterEndElement(cproject_writer); // folderInfo
//#endif /* if 0 */

    return 0;
}

void put_other_storageModules()
{
    char str[128];
    char us_str[16];

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"cdtBuildSystem");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"version", (xmlChar*)"4.0.0");
    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"project");

    get_us(us_str);
    sprintf(str, "%s.null.%s", from_codemodel.project_name, us_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)from_codemodel.project_name);
    sprintf(str, "%s.projecttype.exe", TITLE);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"projectType", (xmlChar*)str);

    xmlTextWriterEndElement(cproject_writer); // project
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"scannerConfiguration");
    put_scannerConfiguration("true");
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"org.eclipse.cdt.core.LanguageSettingsProviders");
    xmlTextWriterEndElement(cproject_writer); // storageModule

    /* com.nxp.mcuxpresso.core.datamodels, com.crt.config */

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"org.eclipse.cdt.make.core.buildtargets");
    xmlTextWriterEndElement(cproject_writer); // storageModule
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

void write_natures()
{
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"com.nxp.mcuxpresso.core.datamodels.sdkNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"org.eclipse.cdt.managedbuilder.core.managedBuildNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"org.eclipse.cdt.managedbuilder.core.ScannerConfigNature");
}

int unbuilt_source(const char *source_path, const char *path) { return 0;}
void cat_additional_exclude_directories(char *str) { }
void put_other_cconfiguration_storageModules(bool debugBuild) { }
void put_project_other_builders() { }
int cproject_init() { return 0; }

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
