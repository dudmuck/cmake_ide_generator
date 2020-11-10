#include "json_eclipse_cdt.h"
#include "board-mcu-hack.h"

/*
 * https://www.renesas.com/us/en/products/software-tools/tools/ide/e2studio.html
 */

const char * const GENMAKEBUILDER_TRIGGERS = "clean,full,incremental,";
const char * const GENMAKEBUILDER_ARGUMENTS = NULL;
const char * const SCANNERCONFIGBUILDER_TRIGGERS = "full,incremental,";
const char * const SCANNERCONFIGBUILDER_ARGUMENTS = "";
const char * const CDT_CORE_SETTINGS_CONFIGRELATIONS = NULL;
const char * const BINARY_PARSER = "org.eclipse.cdt.core.ELF";
const char * const ERROR_PARSERS[] = {
    "org.eclipse.cdt.core.GASErrorParser",
    "org.eclipse.cdt.core.GmakeErrorParser",
    "org.eclipse.cdt.core.GLDErrorParser",
    "org.eclipse.cdt.core.CWDLocator",
    "org.eclipse.cdt.core.GCCErrorParser",
    NULL
};

const char * const TITLE_RENESAS = "com.renesas.cdt.managedbuild.gnuarm";
const char * const TITLE_ILG = "ilg.gnuarmeclipse.managedbuild.cross";
const char * const TITLE_ILG_OPTION = "ilg.gnuarmeclipse.managedbuild.cross.option";

char board[64];

void write_natures()
{
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"org.eclipse.cdt.core.cnature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"org.eclipse.cdt.managedbuilder.core.managedBuildNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"org.eclipse.cdt.managedbuilder.core.ScannerConfigNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"com.renesas.cdt.ddsc.contentgen.ddscNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"com.renesas.cdt.ra.contentgen.raNature");
    xmlTextWriterWriteElement(project_writer, nature, (xmlChar*)"org.eclipse.xtext.ui.shared.xtextNature");

}

int cproject_init() { return 0; }


int get_cconfiguration_id(bool debugBuild, const char *mcu, char *out_id, char *out_superClass)
{
    char strb[16];
    sprintf(out_superClass, "%s.config.elf.%s", TITLE_RENESAS, debugBuild ? "debug" : "release");
    strcpy(out_id, out_superClass);
    strcat(out_id, ".");
    get_us(strb);
    strcat(out_id, strb);
    return 0;
}

int _put_configuration(bool debugBuild, instance_t *instance, const char *cconfiguration_superClass, const char *Board, const char *Mcu)
{
    char id_str[128];
    char parent_str[96];
    char str[512];
    int ret = 0;
    const char *build;
    const char *Build;
    if (debugBuild) {
        build = "debug";
        Build = "Debug";
    } else {
        build = "release";
        Build = "Release";
    }

    strcpy(board, Board);

    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)instance->config_gnu_cross_exe);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"artifactName", (xmlChar*)from_codemodel.artifactName);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"buildArtefactType", (xmlChar*)"org.eclipse.cdt.build.core.buildArtefactType.exe");
    strcpy(str, "org.eclipse.cdt.build.core.buildArtefactType=org.eclipse.cdt.build.core.buildArtefactType.exe,org.eclipse.cdt.build.core.buildType=org.eclipse.cdt.build.core.buildType.");
    strcat(str, build);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"buildProperties", (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"cleanCommand", (xmlChar*)"${cross_rm} -rf");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"description", (xmlChar*)"");
    if (debugBuild) {
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
    }
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)Build);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"parent", (xmlChar*)cconfiguration_superClass);
     
    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"folderInfo");
    strcpy(str, instance->config_gnu_cross_exe);
    strcat(str, ".");   // what is this trailing dot for?
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"/");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"resourcePath", (xmlChar*)"");

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"toolChain");
    sprintf(parent_str, "%s.toolchain.elf.%s", TITLE_RENESAS, build);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"GCC ARM Embedded");
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".addtools.createflash");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Create flash image");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".addtools.createlisting");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Create extended listing");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".addtools.printsize");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Print size");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option


    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".optimization.level");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Optimization Level");
    strcpy(str, parent_str);
    strcat(str, ".");
    char optimization_level = '0';
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp("-O", my_list->str, 2) == 0) {
            optimization_level = my_list->str[2];
            my_list->taken = true;
        }
    }
    switch (optimization_level) {
        case '0': strcat(str, "none"); break;
        case '1': strcat(str, "optimize"); break;
        case '2': strcat(str, "more"); break;
        case '3': strcat(str, "most"); break;
        case 's': strcat(str, "size"); break;
        case 'g': strcat(str, "debug"); break;
    }
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"enumerated");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    char message_length = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-fmessage-length=0", my_list->str) == 0) {
            message_length = 1;
            my_list->taken = true;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".optimization.messagelength");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Message length (-fmessage-length=0)");
    xmlTextWriterWriteAttribute(cproject_writer, value, message_length ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    char signed_char = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-fsigned-char", my_list->str) == 0) {
            signed_char = 1;
            my_list->taken = true;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".optimization.signedchar");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"'char' is signed (-fsigned-char)");
    xmlTextWriterWriteAttribute(cproject_writer, value, signed_char ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    char function_sections = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-ffunction-sections", my_list->str) == 0) {
            function_sections = 1;
            my_list->taken = true;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".optimization.functionsections");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Function sections (-ffunction-sections)");
    xmlTextWriterWriteAttribute(cproject_writer, value, function_sections ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    char data_sections = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-fdata-sections", my_list->str) == 0) {
            data_sections = 1;
            my_list->taken = true;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".optimization.datasections");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Data sections (-fdata-sections)");
    xmlTextWriterWriteAttribute(cproject_writer, value, data_sections ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Debug level");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".debugging.level");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Debug format");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".debugging.format");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    char wunused = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-Wunused", my_list->str) == 0) {
            wunused = 1;
            my_list->taken = true;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".warnings.unused");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Warn on various unused elements (-Wunused)");
    xmlTextWriterWriteAttribute(cproject_writer, value, wunused ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    char wuninit = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-Wuninitialized", my_list->str) == 0) {
            wuninit = 1;
            my_list->taken = true;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".warnings.uninitialized");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Warn on uninitialized variables (-Wuninitialised)");
    xmlTextWriterWriteAttribute(cproject_writer, value, wuninit ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    char wall = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-Wall", my_list->str) == 0) {
            wall = 1;
            my_list->taken = true;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".warnings.allwarn");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Enable all common warnings (-Wall)");
    xmlTextWriterWriteAttribute(cproject_writer, value, wall ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    char wextra = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-Wextra", my_list->str) == 0) {
            wextra = 1;
            my_list->taken = true;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".warnings.extrawarn");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Enable extra warnings (-Wextra)");
    xmlTextWriterWriteAttribute(cproject_writer, value, wextra ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    char wmissing_declarations = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-Wmissing-declarations", my_list->str) == 0) {
            wmissing_declarations = 1;
            my_list->taken = true;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".warnings.missingdeclaration");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Warn on undeclared global function (-Wmissing-declaration)");
    xmlTextWriterWriteAttribute(cproject_writer, value, wmissing_declarations ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    char wconversion = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-Wconversion", my_list->str) == 0) {
            wconversion = 1;
            my_list->taken = true;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".warnings.conversion");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Warn on implicit conversions (-Wconversion)");
    xmlTextWriterWriteAttribute(cproject_writer, value, wconversion ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    char wpointer_arith = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-Wpointer-arith", my_list->str) == 0) {
            wpointer_arith = 1;
            my_list->taken = true;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".warnings.pointerarith");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Warn if pointer arithmetic (-Wpointer-arith)");
    xmlTextWriterWriteAttribute(cproject_writer, value, wpointer_arith ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    char wshadow = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-Wshadow", my_list->str) == 0) {
            wshadow = 1;
            my_list->taken = true;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".warnings.shadow");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Warn if shadowed variable (-Wshadow)");
    xmlTextWriterWriteAttribute(cproject_writer, value, wshadow ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    char wlogical_op = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-Wlogical-op", my_list->str) == 0) {
            wlogical_op = 1;
            my_list->taken = true;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".warnings.logicalop");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Warn if suspicious logical ops (-Wlogical-op)");
    xmlTextWriterWriteAttribute(cproject_writer, value, wlogical_op ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    char wagreggate_return = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-Wagreggate-return", my_list->str) == 0) {
            wagreggate_return = 1;
            my_list->taken = true;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".warnings.agreggatereturn");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Warn if struct is returned (-Wagreggate-return)");
    xmlTextWriterWriteAttribute(cproject_writer, value, wagreggate_return ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    char wfloat_equal = 0;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-Wfloat-equal", my_list->str) == 0) {
            wfloat_equal = 1;
            my_list->taken = true;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".warnings.floatequal");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Warn if floats are compared as equal (-Wfloat-equal)");
    xmlTextWriterWriteAttribute(cproject_writer, value, wfloat_equal ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    const char *family_ptr = NULL;
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        //           123456
        if (strncmp("-mcpu=", my_list->str, 6) == 0) {
            family_ptr = my_list->str + 6;
            wfloat_equal = 1;
            my_list->taken = true;
        }
    }

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".arm.target.family");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"ARM family");
    if (family_ptr) {
        strcpy(str, TITLE_ILG_OPTION);
        strcat(str, ".arm.target.mcpu.");
        strcat(str, family_ptr);
        xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"enumerated");
        xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    }
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".toolchain.name");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"GNU Tools for ARM Embedded Processors");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".architecture");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Architecture");
    strcpy(str, parent_str);
    strcat(str, ".");
    strcat(str, "arm"); // TODO arm64
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"enumerated");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".arm.target.instructionset");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Instruction set");
    strcpy(str, parent_str);
    strcat(str, ".");
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("-mthumb", my_list->str) == 0) {
            strcat(str, "thumb");
            my_list->taken = true;
        } else if (strcmp("-marm", my_list->str) == 0) {
            strcat(str, "arm");
            my_list->taken = true;
        }
    }
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"enumerated");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".command.prefix");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Prefix");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"arm-none-eabi-");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".command.c");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"C compiler");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"gcc");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option
                            							
    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".command.cpp");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"C++ compiler");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"g++");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option
                            							
    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".command.ar");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Archiver");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"ar");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option
                            							
    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".command.objcopy");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Hex/Bin converter");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"objcopy");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option
                            							
    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".command.objdump");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Listing generator");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"objdump");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option
                            							
    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".command.size");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Size command");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"size");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option
                            							
    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".command.make");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Build command");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"make");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option
                            							
    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".command.rm");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Remove command");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"rm");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"targetPlatform");
    strcpy(parent_str, TITLE_ILG);
    strcat(parent_str, ".targetPlatform");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"osList", (xmlChar*)"all");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"isAbstract", (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"archList", (xmlChar*)"all");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"binaryParser", (xmlChar*)"org.eclipse.cdt.core.ELF");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // targetPlatform

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"builder");
    strcpy(parent_str, TITLE_RENESAS);
    strcat(parent_str, ".builder");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Gnu Make Builder");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"keepEnvironmentInBuildfile", (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"managedBuildOn", (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"parallelBuildOn", (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"parallelizationNumber", (xmlChar*)"optimal");
    strcpy(str, from_codemodel.buildPath);
    strcat(str, "/");
    strcat(str, Build);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"buildPath", (xmlChar*)str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // builder

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"GNU ARM Cross Assembler");
    strcpy(parent_str, TITLE_ILG);
    strcat(parent_str, ".tool.assembler");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"commandLinePattern", (xmlChar*)"${SECURE_BUILD_COMMAND} ${COMMAND} ${cross_toolchain_flags} ${FLAGS} -c ${OUTPUT_FLAG} ${OUTPUT_PREFIX}${OUTPUT} ${INPUTS}");

    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".assembler.usepreprocessor");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Use preprocessor");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Include paths (-I)");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"includePath");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".assembler.include.paths");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
            //put_listOptionValue(cproject_writer, false, my_list->str);
    xmlTextWriterEndElement(cproject_writer); // option   assembler include paths

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Defined symbols (-D)");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"definedSymbols");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".assembler.defs");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
            //put_listOptionValue(cproject_writer, false, my_list->str);
    xmlTextWriterEndElement(cproject_writer); // option   assembler include paths

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    strcpy(parent_str, TITLE_ILG);
    strcat(parent_str, ".tool.assembler.input");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // inputType

    xmlTextWriterEndElement(cproject_writer); // tool   assembler

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"GNU ARM Cross C Compiler");
    strcpy(parent_str, TITLE_ILG);
    strcat(parent_str, ".tool.c.compiler");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, instance->tool_gnu_cross_c_compiler);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)instance->tool_gnu_cross_c_compiler);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"commandLinePattern", (xmlChar*)"${SECURE_BUILD_COMMAND} ${COMMAND} ${cross_toolchain_flags} ${FLAGS} -c ${OUTPUT_FLAG} ${OUTPUT_PREFIX}${OUTPUT} -x c ${INPUTS}");

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Language standard");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".c.compiler.std");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    strcpy(str, parent_str);
    strcat(str, ".");
    const char *std = "c99";
    for (struct node_s *my_list = from_codemodel.compile_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        /* go thru all args, last one takes effect */
        if (strncmp("-std=", my_list->str, 5) == 0) {
            std = my_list->str + 4;
            my_list->taken = true;
        } else if (strcmp("-ansi", my_list->str) == 0) {
            std = "ansi";
            my_list->taken = true;
        }
    }
    strcat(str, std);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"enumerated");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Include paths (-I)");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"includePath");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".c.compiler.include.paths");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    if (includes_list) {
        struct node_s *my_list;
        for (my_list = includes_list; my_list != NULL; my_list = my_list->next) {
            put_listOptionValue(cproject_writer, false, my_list->str);
        }
    }
    xmlTextWriterEndElement(cproject_writer); // option   assembler include paths

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Defined symbols (-D)");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"definedSymbols");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".c.compiler.defs");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    if (defines_list) {
        struct node_s *my_list;
        for (my_list = defines_list; my_list != NULL; my_list = my_list->next) {
            put_listOptionValue(cproject_writer, false, my_list->str);
        }
    }
    xmlTextWriterEndElement(cproject_writer); // option   assembler include paths

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    strcpy(parent_str, TITLE_ILG);
    strcat(parent_str, ".tool.c.compiler.input");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, instance->tool_gnu_cross_c_compiler_input);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)instance->tool_gnu_cross_c_compiler_input);
    xmlTextWriterEndElement(cproject_writer); // inputType

    xmlTextWriterEndElement(cproject_writer); // tool   c.compiler

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"GNU ARM Cross C++ Compiler");
    strcpy(parent_str, TITLE_ILG);
    strcat(parent_str, ".tool.cpp.compiler");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"commandLinePattern", (xmlChar*)"${COMMAND} ${cross_toolchain_flags} ${FLAGS} -c ${OUTPUT_FLAG} ${OUTPUT_PREFIX}${OUTPUT} -x c++ ${INPUTS}");

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Language standard");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".cpp.compiler.std");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    strcpy(str, parent_str);
    strcat(str, ".");
    strcat(str, "cpp11"); // TODO other C++ language standards
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"enumerated");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"true");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterEndElement(cproject_writer); // tool   cpp.compiler

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"GNU ARM Cross C Linker");
    strcpy(parent_str, TITLE_ILG);
    strcat(parent_str, ".tool.c.linker");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"commandLinePattern", (xmlChar*)"${COMMAND} ${cross_toolchain_flags} ${FLAGS} ${OUTPUT_FLAG} ${OUTPUT_PREFIX}${OUTPUT} -Wl,--start-group ${INPUTS} -Wl,--end-group");


    char gc_sections = 0;
    for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("--gc-sections", my_list->str) == 0) {
            gc_sections = 1;
            my_list->taken = true;
        }
    }
    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Remove unused sections (-Xlinker --gc-sections)");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".c.linker.gcsections");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, gc_sections ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option
                                								
                                								
    char nano_specs = 0;
    for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strcmp("--specs=nano.specs", my_list->str) == 0) {
            nano_specs = 1;
            my_list->taken = true;
        }
    }
    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Use newlib-nano (--specs=nano.specs)");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".c.linker.usenewlibnano");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, nano_specs ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option

    str[0] = 0;
    for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp("--specs=", my_list->str, 8) == 0) {    // TODO anything else besides --specs=
            if (str[0] == 0)
                strcpy(str, my_list->str);
            else {
                strcat(str, " ");
                strcat(str, my_list->str);
            }
            my_list->taken = true;
        }
    }
    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".c.linker.other");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Other linker flags");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option
                                			
    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Script files (-T)");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"stringList");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".c.linker.scriptfile");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    if (defines_list) {
        struct node_s *my_list;
        for (my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
            if (strncmp(my_list->str, "-T", 2) == 0) {
                put_listOptionValue(cproject_writer, false, my_list->str+2);
            }
        }
    }
    xmlTextWriterEndElement(cproject_writer); // option   linker script

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Linker search path (-L)");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"libPaths");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".c.linker.paths");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    if (defines_list) {
        struct node_s *my_list;
        for (my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
            if (strncmp(my_list->str, "-L", 2) == 0) {
                put_listOptionValue(cproject_writer, false, my_list->str+2);
            }
        }
    }
    xmlTextWriterEndElement(cproject_writer); // option   linker paths

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"inputType");
    strcpy(parent_str, TITLE_ILG);
    strcat(parent_str, ".tool.c.linker.input");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    put_additionalInput(cproject_writer, "additionalinputdependency", "$(USER_OBJS)");
    put_additionalInput(cproject_writer, "additionalinput", "$(LIBS)");
    put_additionalInput(cproject_writer, "additionaldependency", "$(LINKER_SCRIPT)");
    xmlTextWriterEndElement(cproject_writer); // inputType

    xmlTextWriterEndElement(cproject_writer); // tool   c.linker


    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"GNU ARM Cross C++ Linker");
    strcpy(parent_str, TITLE_ILG);
    strcat(parent_str, ".tool.cpp.linker");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"commandLinePattern", (xmlChar*)"${COMMAND} ${cross_toolchain_flags} ${FLAGS} ${OUTPUT_FLAG} ${OUTPUT_PREFIX}${OUTPUT} -Wl,--start-group ${INPUTS} -Wl,--end-group");

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Remove unused sections (-Xlinker --gc-sections)");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".cpp.linker.gcsections");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, gc_sections ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterEndElement(cproject_writer); // option

    str[0] = 0;
    for (struct node_s *my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
        if (my_list->taken)
            continue;
        if (strncmp("--specs=", my_list->str, 8) == 0) {    // TODO anything else besides --specs=
            if (str[0] == 0)
                strcpy(str, my_list->str);
            else {
                strcat(str, " ");
                strcat(str, my_list->str);
            }
            my_list->taken = true;
        }
    }
    xmlTextWriterStartElement(cproject_writer, option);
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".cpp.linker.other");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Other linker flags");
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"string");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Use newlib-nano (--specs=nano.specs)");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".cpp.linker.usenewlibnano");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, nano_specs ? (xmlChar*)"true" : (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Script files (-T)");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"stringList");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".cpp.linker.scriptfile");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, IS_BUILTIN_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, IS_VALUE_EMPTY, (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    if (defines_list) {
        struct node_s *my_list;
        for (my_list = from_codemodel.linker_fragment_list; my_list != NULL; my_list = my_list->next) {
            if (strncmp(my_list->str, "-T", 2) == 0) {
                put_listOptionValue(cproject_writer, false, my_list->str+2);
            }
        }
    }
    xmlTextWriterEndElement(cproject_writer); // option   linker script

    xmlTextWriterEndElement(cproject_writer); // tool   cpp.linker

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"GNU ARM Cross Archiver");
    strcpy(parent_str, TITLE_ILG);
    strcat(parent_str, ".tool.archiver");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // tool   archiver
                            							
    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"GNU ARM Cross Create Flash Image");
    strcpy(parent_str, TITLE_ILG);
    strcat(parent_str, ".tool.createflash");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Output file format (-O)");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".createflash.choice");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    strcpy(str, parent_str);
    strcat(str, ".");
    strcat(str, "srec"); // TODO other output formats
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"enumerated");
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterEndElement(cproject_writer); // tool   createflash

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"GNU ARM Cross Create Listing");
    strcpy(parent_str, TITLE_ILG);
    strcat(parent_str, ".tool.createlisting");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Display source (--source|-S)");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".createlisting.source");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Display all headers (--all-headers|-x)");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".createlisting.allheaders");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterEndElement(cproject_writer); // option
                                								
    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Demangle names (--demangle|-C)");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".createlisting.demangle");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterEndElement(cproject_writer); // option
                                								
    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Display line numbers (--line-numbers|-l)");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".createlisting.linenumbers");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterEndElement(cproject_writer); // option
                                								
    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Wide lines (--wide|-w)");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".createlisting.wide");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, value, (xmlChar*)"true");
    xmlTextWriterWriteAttribute(cproject_writer, valueType, (xmlChar*)"boolean");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterEndElement(cproject_writer); // tool   create listing


    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"tool");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"GNU ARM Cross Print Size");
    strcpy(parent_str, TITLE_ILG);
    strcat(parent_str, ".tool.printsize");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);

    xmlTextWriterStartElement(cproject_writer, option);
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Size format");
    strcpy(parent_str, TITLE_ILG_OPTION);
    strcat(parent_str, ".printsize.format");
    xmlTextWriterWriteAttribute(cproject_writer, superClass, (xmlChar*)parent_str);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, useByScannerDiscovery, (xmlChar*)"false");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterEndElement(cproject_writer); // tool   printsize

    xmlTextWriterEndElement(cproject_writer); // toolChain
    xmlTextWriterEndElement(cproject_writer); // folderInfo

    return ret;
}

void put_other_storageModules(const instance_t *debugInstance, const instance_t *releaseInstance)
{
    char str[128];
    char id_str[196];
    char us_str[16];

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"cdtBuildSystem");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"version", (xmlChar*)"4.0.0");

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"project");
    xmlTextWriterWriteAttribute(cproject_writer, name, (xmlChar*)"Executable");
    strcpy(str, TITLE_RENESAS);
    strcat(str, ".target.elf");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"projectType", (xmlChar*)str);
    strcpy(id_str, from_codemodel.project_name);
    strcat(id_str, ".");
    strcat(id_str, str);
    strcat(id_str, ".");
    get_us(us_str);
    strcat(id_str, us_str);
    xmlTextWriterWriteAttribute(cproject_writer, id, (xmlChar*)id_str);
    xmlTextWriterEndElement(cproject_writer); // project

    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"org.eclipse.cdt.core.LanguageSettingsProviders");
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"org.eclipse.cdt.make.core.buildtargets");
    xmlTextWriterEndElement(cproject_writer); // storageModule

    xmlTextWriterStartElement(cproject_writer, storageModule);
    xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"scannerConfiguration");
    put_scannerConfiguration(debugInstance, releaseInstance, "true");
    xmlTextWriterEndElement(cproject_writer); // storageModule
}

void put_project_other_builders()
{
    xmlTextWriterStartElement(project_writer, (xmlChar*)"buildCommand");
    xmlTextWriterWriteElement(project_writer, name, (xmlChar*)"org.eclipse.xtext.ui.shared.xtextBuilder");
    xmlTextWriterWriteElement(project_writer, (xmlChar*)"arguments", (xmlChar*)"");
    xmlTextWriterEndElement(project_writer); // buildCommand

    xmlTextWriterStartElement(project_writer, (xmlChar*)"buildCommand");
    xmlTextWriterWriteElement(project_writer, name, (xmlChar*)"com.renesas.cdt.ddsc.contentgen.ddscBuilder");
    xmlTextWriterWriteElement(project_writer, (xmlChar*)"arguments", (xmlChar*)"");
    xmlTextWriterEndElement(project_writer); // buildCommand
    
    xmlTextWriterStartElement(project_writer, (xmlChar*)"buildCommand");
    xmlTextWriterWriteElement(project_writer, name, (xmlChar*)"com.renesas.cdt.ddsc.contentgen.ddscInterlockBundleBuilder");
    xmlTextWriterWriteElement(project_writer, (xmlChar*)"arguments", (xmlChar*)"");
    xmlTextWriterEndElement(project_writer); // buildCommand
    
}

void put_other_cconfiguration_storageModules(bool debugBuild)
{
    if (debugBuild) {
        xmlTextWriterStartElement(cproject_writer, storageModule);
        xmlTextWriterWriteAttribute(cproject_writer, moduleId, (xmlChar*)"com.renesas.cdt.managedbuild.core.toolchainInfo");
        xmlTextWriterEndElement(cproject_writer); // storageModule
    }
}

int unbuilt_source(const char *source_path, const char *path)
{
    struct stat st;
    FILE *fp_in, *fp_out;
    char in_full[256];
    char *withoutPath;
    char *buffer = NULL, *in_copy = NULL;
    int read_len, ret;

    strcpy(in_full, source_path);
    strcat(in_full, "/");
    strcat(in_full, path);
    if (stat(in_full, &st) < 0) {
        perror("stat-unbuilt");
        return -1;
    }
    fp_in = fopen(in_full, "r");
    if (!fp_in) {
        printf("cannot open %s\n", in_full);
        return -1;
    }
    buffer = malloc(st.st_size);
#ifdef __WIN32__
    ret = fread_(buffer, st.st_size, fp_in);
#else
    ret = fread(buffer, 1, st.st_size, fp_in);
#endif
    if (ret == 0) {
        printf("%d = fread %s, size %d\n", ret, in_full, (int)st.st_size);
        return -1;
    }
    read_len = ret;

    in_copy = malloc(strlen(path)+1);
    strcpy(in_copy, path);
    withoutPath = basename(in_copy);

    // destination:from_codemodel.buildPath, or just CWD
    fp_out = fopen(withoutPath, "w");
    ret = fwrite(buffer, read_len, 1, fp_out);
    fclose(fp_out);

    fclose(fp_in);
    free(buffer);
    free(in_copy);
    return 0;
}

#if 0
/* use original configuration.xml as-is, will block IDE-generated code from being discovered, which would cause duplication of source code from cmake project */
int cfg_start(bool force)
{
    xmlTextWriterPtr cfg_writer;
    struct stat st;
    const char *xml_filename = "configuration.xml";
    int ret;

    if (!force && stat(xml_filename, &st) == 0) {
        printf("%s already exists ", xml_filename);
        return -EEXIST;
    }

    cfg_writer = xmlNewTextWriterFilename(xml_filename, 0);
    if (cfg_writer == NULL) {
        printf("cannot create xml writer\r\n");
        return -1;
    }
    ret = xmlTextWriterStartDocument(cfg_writer, NULL, "UTF-8", "no");
    if (ret < 0) {
        printf("Error at xmlTextWriterStartDocument\n");
        return ret;
    }

    ret = xmlTextWriterSetIndent(cfg_writer, 1);
    if (ret < 0) {
        printf("Error at xmlTextWriterSetIndent\n");
        return ret;
    }
    xmlTextWriterSetIndentString(cfg_writer, (xmlChar*)"  ");

    xmlTextWriterStartElement(cfg_writer, (xmlChar*)"raConfiguration");
    xmlTextWriterWriteAttribute(cfg_writer, (xmlChar*)"version", (xmlChar*)"3");

    xmlTextWriterStartElement(cfg_writer, (xmlChar*)"generalSettings");

    xmlTextWriterStartElement(cfg_writer, (xmlChar*)option);
    xmlTextWriterWriteAttribute(cfg_writer, (xmlChar*)"key", (xmlChar*)"#Board#");
    xmlTextWriterWriteAttribute(cfg_writer, value, (xmlChar*)"board.ra2a1_ek");
    xmlTextWriterEndElement(cfg_writer); // option

    xmlTextWriterEndElement(cfg_writer); // generalSettings 

    printf("board %s\n", board);

    xmlTextWriterEndElement(cfg_writer); // raConfiguration

    ret = xmlTextWriterEndDocument(cfg_writer);
    if (ret < 0) {
        printf ("testXmlwriterFilename: Error at xmlTextWriterEndDocument\n");
        return ret;
    }
    xmlFreeTextWriter(cfg_writer);

    return 0;
}
#endif /* if 0 */

void cat_additional_exclude_directories(char *str)
{
    /* ignore HAL-factory generated source, since these are already in cmake project */
    strcat(str, "|ra");
    strcat(str, "|ra_cfg");
    strcat(str, "|ra_gen");
    strcat(str, "|src");
}

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

    /*
    if ((ret = cfg_start(force)) < 0) {
        goto main_end;
    }
    */

main_end:
    if (ret == -EEXIST)
        printf("-f to force overwrite\r\n");

    free_lists();
    return ret;
}

