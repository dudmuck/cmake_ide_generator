#include "json_eclipse_cdt.h"

const char * const TITLE = "fr.ac6.managedbuild";
const char * const CONFIGURATION_EXE_SUPERCLASS = "%s.config.gnu.cross.exe.%s";
const char * const TOOLCHAIN_SUPERCLASS = "%s.toolchain.gnu.cross.exe.%s";
const char * const OPTION_MCU_SUPERCLASS = "%s.option.gnu.cross.mcu";
const char * const OPTION_BOARD_SUPERCLASS = "%s.option.gnu.cross.board";
const char * const TARGET_PLATFORM_SUPERCLASS = "%s.targetPlatform.gnu.cross";
const char * const BUILDER_SUPERCLASS = "%s.builder.gnu.cross";
const char * const TOOL_C_COMPILER_SUPERCLASS = "%s.tool.gnu.cross.c.compiler";
const char * const TOOL_CPP_COMPILER_SUPERCLASS = "%s.tool.gnu.cross.cpp.compiler";
const char * const C_OPTIMIZATION_LEVEL_SUPERCLASS = "%s.gnu.c.compiler.option.optimization.level";
const char * const CPP_OPTIMIZATION_LEVEL_SUPERCLASS = "%s.gnu.cpp.compiler.option.optimization.level";
const char * const FDATA_SECTIONS_SUPERCLASS = "%s.tool.gnu.cross.c.compiler.fdata";
const char * const C_INPUT_C_SUPERCLASS = "%s.tool.gnu.cross.c.compiler.input.c";
const char * const C_INPUT_S_SUPERCLASS = "%s.tool.gnu.cross.c.compiler.input.s";
const char * const C_LINKER_SUPERCLASS = "%s.tool.gnu.cross.c.linker";
const char * const CPP_LINKER_SUPERCLASS = "%s.tool.gnu.cross.cpp.linker";
const char * const C_LINKER_SCRIPT_SUPERCLASS = "%s.tool.gnu.cross.c.linker.script";
const char * const CPP_LINKER_SCRIPT_SUPERCLASS = "%s.tool.gnu.cross.cpp.linker.script";
const char * const ARCHIVER_SUPERCLASS = "%s.tool.gnu.archiver";
const char * const ASSEMBLER_SUPERCLASS = NULL;  // calling get_assembler_superclass() instead 
const char * const ASSEMBLER_INPUT_SUPERCLASS = "%s.tool.gnu.cross.assembler.input";
const char * const TOOLCHAIN_NAME = "Ac6 STM32 MCU GCC";
const char * const C_DEFINED_SYMBOLS_SUPERCLASS = "gnu.c.compiler.option.preprocessor.def.symbols";
const char * const C_DIALECT_SUPERCLASS = "gnu.c.compiler.option.dialect.std";
const char * const C_PEDANTIC_SUPERCLASS = "gnu.c.compiler.option.warnings.pedantic";
const char * const C_WARN_EXTRA_SUPERCLASS = "gnu.c.compiler.option.warnings.extrawarn";
const char * const C_INCLUDE_PATHS_SUPERCLASS = "gnu.c.compiler.option.include.paths";
const char * const C_LINKER_INPUT_SUPERCLASS = "cdt.managedbuild.tool.gnu.c.linker.input";
const char * const CPP_LINKER_INPUT_SUPERCLASS = "cdt.managedbuild.tool.gnu.cpp.linker.input";

void put_ScannerConfigBuilder_triggers() { }

void put_toolchain_type() { }
void put_toolchain_version() { }

void put_target_id() { }

void put_c_link_option_flags()
{
    char str[256];
    char parent_str[96];
    char id_str[128];

    xmlTextWriterStartElement(cproject_writer, "option");
    strcpy(parent_str, "gnu.cpp.link.option.flags");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, "useByScannerDiscovery", "false");
    if (from_codemodel.specs_list) {
        struct node_s *my_list;
        str[0] = 0;
        for (my_list = from_codemodel.specs_list; my_list != NULL; my_list = my_list->next) {
            if (str[0] != 0)
                strcat(str, " ");
            strcat(str, my_list->str);
        }
        xmlTextWriterWriteAttribute(cproject_writer, "value", str);
    }
    xmlTextWriterWriteAttribute(cproject_writer, "valueType", "string");
    xmlTextWriterEndElement(cproject_writer); // option 
}

void put_cpp_link_option_flags()
{
    char str[256];
    char parent_str[96];
    char id_str[128];

    xmlTextWriterStartElement(cproject_writer, "option");
    xmlTextWriterWriteAttribute(cproject_writer, "IS_BUILTIN_EMPTY", "false");
    xmlTextWriterWriteAttribute(cproject_writer, "IS_VALUE_EMPTY", "true");
    xmlTextWriterWriteAttribute(cproject_writer, "name", "Other options (-Xlinker [option])");
    strcpy(parent_str, "gnu.cpp.link.option.other");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, "useByScannerDiscovery", "false");
    xmlTextWriterWriteAttribute(cproject_writer, "valueType", "stringList");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, "option");
    xmlTextWriterWriteAttribute(cproject_writer, "IS_BUILTIN_EMPTY", "false");
    xmlTextWriterWriteAttribute(cproject_writer, "IS_VALUE_EMPTY", "true");
    xmlTextWriterWriteAttribute(cproject_writer, "name", "Other objects");
    strcpy(parent_str, "gnu.cpp.link.option.userobjs");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, "useByScannerDiscovery", "false");
    xmlTextWriterWriteAttribute(cproject_writer, "valueType", "userObjs");
    xmlTextWriterEndElement(cproject_writer); // option

    xmlTextWriterStartElement(cproject_writer, "option");
    strcpy(parent_str, "gnu.cpp.link.option.flags");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, "useByScannerDiscovery", "false");
    if (from_codemodel.specs_list) {
        struct node_s *my_list;
        str[0] = 0;
        for (my_list = from_codemodel.specs_list; my_list != NULL; my_list = my_list->next) {
            if (str[0] != 0)
                strcat(str, " ");
            strcat(str, my_list->str);
        }
        xmlTextWriterWriteAttribute(cproject_writer, "value", str);
    }
    xmlTextWriterWriteAttribute(cproject_writer, "valueType", "string");
    xmlTextWriterEndElement(cproject_writer); // option 
}

void put_c_other_flags()
{
    char parent_str[96];
    char str[256];
    char id_str[128];

    str[0] = 0;
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

        if (str[0] == 0)
            strcat(str, "-fmessage-length=0 ");
        else
            strcat(str, " ");

        strcat(str, my_list->str);
        my_list->taken = true;
    }

    if (str[0] != 0) {
        xmlTextWriterStartElement(cproject_writer, "option");
        strcpy(parent_str, "fr.ac6.managedbuid.gnu.c.compiler.option.misc.other");  // missing 'l' in build
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
        xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, "value", str);
        xmlTextWriterWriteAttribute(cproject_writer, "valueType", "string");
        xmlTextWriterEndElement(cproject_writer); // option 
    }
}

const char * const C_DIALECT_VALUE_PREFIX = "gnu.c.compiler.dialect.";
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

void put_assembler_options()
{
    char parent_str[96];
    char id_str[128];

    xmlTextWriterStartElement(cproject_writer, "inputType");
    strcpy(parent_str, "cdt.managedbuild.tool.gnu.assembler.input");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
    xmlTextWriterEndElement(cproject_writer); // inputType
}

void put_post_build_steps(const char *artifactExtension)
{
    char str[768];
    char id_str[128];

    xmlTextWriterWriteAttribute(cproject_writer, "postannouncebuildStep", "Generating binary and Printing size information:");

    /* TODO: get post build steps from UTILITY targets */
    sprintf(id_str, "%s.%s", from_codemodel.artifactName, artifactExtension);
    sprintf(str, "arm-none-eabi-objcopy -Oihex %s %s.hex; arm-none-eabi-objcopy -Obinary %s %s.bin; arm-none-eabi-size %s", id_str, from_codemodel.artifactName, id_str, from_codemodel.artifactName, id_str);

//arm-none-eabi-objcopy -Oihex <artifactName>.elf <artifactName>.hex; arm-none-eabi-objcopy -Obinary <artifactName>.elf <artifactName>.bin; arm-none-eabi-size <artifactName>.elf
    xmlTextWriterWriteAttribute(cproject_writer, "postbuildStep", str);
}

void put_cpp_debugging_default_value()
{
    xmlTextWriterWriteAttribute(cproject_writer, "defaultValue", "gnu.cpp.debugging.level.max");
}

void get_cpp_debugging_level_superclass(char *parent_str)
{
    strcpy(parent_str, "gnu.cpp.compiler.option.debugging.level");
}

void put_c_debug_level(char debugging_level)
{
    char parent_str[96];
    char id_str[128];
    char str[256];

    if (debugging_level != 0) {
        xmlTextWriterStartElement(cproject_writer, "option");
        xmlTextWriterWriteAttribute(cproject_writer, "defaultValue", "gnu.c.debugging.level.max");
        strcpy(parent_str, "gnu.c.compiler.option.debugging.level");
        put_id(parent_str, id_str);
        xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
        xmlTextWriterWriteAttribute(cproject_writer, "name", "Debug Level");
        xmlTextWriterWriteAttribute(cproject_writer, "superClass", parent_str);
        xmlTextWriterWriteAttribute(cproject_writer, "useByScannerDiscovery", "false");
        /*        gnu.c.debugging.level.none  (debugging_level == 0)
         * -g1    gnu.c.debugging.level.minimal
         * -g     gnu.c.debugging.level.default  (or -g2)
         * -g3    gnu.cpp.compiler.debugging.level.max
         */
        strcpy(str, "gnu.c.debugging.level.");
        switch (debugging_level) {
            case 0: strcat(str, "none"); break;
            case '1': strcat(str, "minimal"); break;
            case ' ': case '2': strcat(str, "default"); break;
            case '3': strcat(str, "max"); break;
        }
        xmlTextWriterWriteAttribute(cproject_writer, "value", str);
        xmlTextWriterWriteAttribute(cproject_writer, "valueType", "enumerated");
        xmlTextWriterEndElement(cproject_writer); // option 
    }
}

void get_bulder_name(char *out, const char *build)
{
    strcpy(out, "Gnu Make Builder");
}

void put_cdt_project()
{
    char parent_str[96];
    char id_str[128];

    sprintf(parent_str, "%s.target.gnu.cross.exe", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "id", id_str);
    xmlTextWriterWriteAttribute(cproject_writer, "name", "Executable");
    xmlTextWriterWriteAttribute(cproject_writer, "projectType", parent_str);
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

void get_assembler_superclass(bool debugBuild, char *parent_str)
{
    if (debugBuild)
        sprintf(parent_str, "%s.tool.gnu.cross.assembler", TITLE);
    else
        sprintf(parent_str, "%s.tool.gnu.cross.assembler.exe.release", TITLE);

}

void write_natures()
{
    xmlTextWriterWriteElement(project_writer, "nature", "fr.ac6.mcu.ide.core.MCUProjectNature");
    xmlTextWriterWriteElement(project_writer, "nature", "fr.ac6.mcu.ide.core.MCUSingleCoreProjectNature");
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

main_end:
    if (ret == -EEXIST)
        printf("-f to force overwrite\r\n");

    free_lists();
    return ret;
}

