#include "json_eclipse_cdt.h"

const char * const TITLE = "com.st.stm32cube.ide.mcu.gnu.managedbuild";
const char * const CONFIGURATION_EXE_SUPERCLASS = "%s.config.exe.%s";
const char * const TOOLCHAIN_SUPERCLASS = "%s.toolchain.exe.%s";
const char * const OPTION_MCU_SUPERCLASS = "%s.option.target_mcu";
const char * const OPTION_BOARD_SUPERCLASS = "%s.option.target_board";
const char * const BUILDER_SUPERCLASS = "%s.builder";
const char * const ASSEMBLER_INPUT_SUPERCLASS = "%s.tool.assembler.input";
const char * const ASSEMBLER_SUPERCLASS = "%s.tool.assembler";
const char * const TARGET_PLATFORM_SUPERCLASS = "%s.targetplatform";
const char * const TOOL_C_COMPILER_SUPERCLASS = "%s.tool.c.compiler";
const char * const TOOL_CPP_COMPILER_SUPERCLASS = "%s.tool.cpp.compiler";
const char * const ARCHIVER_SUPERCLASS = "%s.tool.archiver";
const char * const C_OPTIMIZATION_LEVEL_SUPERCLASS = "%s.tool.c.compiler.option.optimization.level";
const char * const CPP_OPTIMIZATION_LEVEL_SUPERCLASS = "%s.tool.cpp.compiler.option.optimization.level";
const char * const C_INPUT_C_SUPERCLASS = "%s.tool.c.compiler.input.c";
const char * const C_LINKER_SCRIPT_SUPERCLASS = "%s.tool.c.linker.option.script";
const char * const CPP_LINKER_SCRIPT_SUPERCLASS = "%s.tool.cpp.linker.option.script";
const char * const CPP_LINKER_SUPERCLASS = "%s.tool.cpp.linker";
const char * const C_LINKER_SUPERCLASS = "%s.tool.c.linker";
const char * const FDATA_SECTIONS_SUPERCLASS = "%s.TODO_FDATA";
const char * const C_INPUT_S_SUPERCLASS = NULL;
const char * const TOOLCHAIN_NAME = "MCU ARM GCC";
const char * const C_DEFINED_SYMBOLS_SUPERCLASS = "com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.definedsymbols";
const char * const C_DIALECT_SUPERCLASS = "com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.languagestandard";
const char * const C_PEDANTIC_SUPERCLASS = "com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.warnings.pedantic";
const char * const C_WARN_EXTRA_SUPERCLASS = "com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.warnings.extra";
const char * const C_INCLUDE_PATHS_SUPERCLASS = "com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.includepaths";
const char * const C_LINKER_INPUT_SUPERCLASS = "com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.linker.input";
const char * const CPP_LINKER_INPUT_SUPERCLASS = "com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.cpp.linker.input";

void put_ScannerConfigBuilder_triggers()
{
    xmlTextWriterWriteElement(project_writer, (xmlChar*)"triggers", (xmlChar*)"full,incremental,");
}

void put_target_id()
{
    char parent_str[96];
    char id_str[128];

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"option");
    strcpy(parent_str, "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.target_cpuid");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"id", (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"superClass", (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"value", (xmlChar*)"0");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"valueType", (xmlChar*)"string");
    xmlTextWriterEndElement(cproject_writer); // option


    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"option");
    strcpy(parent_str, "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.target_coreid");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"id", (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"superClass", (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"value", (xmlChar*)"0");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"valueType", (xmlChar*)"string");
    xmlTextWriterEndElement(cproject_writer); // option
}

void put_toolchain_type()
{
    char parent_str[96];
    char id_str[128];

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"option");
    strcpy(parent_str, "com.st.stm32cube.ide.mcu.option.internal.toolchain.type");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"id", (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"superClass", (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"value", (xmlChar*)"com.st.stm32cube.ide.mcu.gnu.managedbuild.toolchain.base.gnu-tools-for-stm32");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"valueType", (xmlChar*)"string");
    xmlTextWriterEndElement(cproject_writer); // option
}

void put_toolchain_version()
{
    char parent_str[96];
    char id_str[128];

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"option");
    strcpy(parent_str, "com.st.stm32cube.ide.mcu.option.internal.toolchain.version");
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"id", (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"superClass", (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"value", (xmlChar*)"7-2018-q2-update");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"valueType", (xmlChar*)"string");
    xmlTextWriterEndElement(cproject_writer); // option
}


void put_c_link_option_flags()
{
}

void put_cpp_link_option_flags()
{
}

void put_c_other_flags()
{
    char parent_str[96];
    char id_str[128];

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"option");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"IS_BUILTIN_EMPTY", (xmlChar*)"false");
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"IS_VALUE_EMPTY", (xmlChar*)"false");
    sprintf(parent_str, "%s.tool.c.compiler.option.otherflags", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"id", (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"superClass", (xmlChar*)parent_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"valueType", (xmlChar*)"stringList");
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

        xmlTextWriterStartElement(cproject_writer, (xmlChar*)"listOptionValue");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"builtIn", (xmlChar*)"false");
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"value", (xmlChar*)my_list->str);
        xmlTextWriterEndElement(cproject_writer); // listOptionValue 

        my_list->taken = true;
    }
    xmlTextWriterEndElement(cproject_writer); // option 
}

const char * const C_DIALECT_VALUE_PREFIX = "com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.languagestandard.value.";
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

void put_assembler_options(char debugging_level)
{
    char parent_str[96];
    char id_str[128];
    char str[256];

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"option");
    sprintf(parent_str, "%s.tool.assembler.option.debuglevel", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"id", (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"superClass", (xmlChar*)parent_str);
    if (debugging_level != 0) {
        strcpy(str, TITLE);
        strcat(str, ".tool.assembler.option.debuglevel.value.g");
        if (debugging_level != '2') {
            char flag[2];
            flag[0] = debugging_level;
            flag[1] = 0;
            strcat(str, flag);
        }
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"value", (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"valueType", (xmlChar*)"enumerated");
    }
    xmlTextWriterEndElement(cproject_writer); // option
}

void put_post_build_steps(const char *artifactExtension) { }

void put_cpp_debugging_default_value() { }

void get_cpp_debugging_level_superclass(char *parent_str)
{
    sprintf(parent_str, "%s.tool.cpp.compiler.option.debuglevel", TITLE);
}

void put_c_debug_level(char debugging_level)
{
    char parent_str[96];
    char id_str[128];
    char str[256];

    xmlTextWriterStartElement(cproject_writer, (xmlChar*)"option");
    sprintf(parent_str, "%s.tool.c.compiler.option.debuglevel", TITLE);
    put_id(parent_str, id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"id", (xmlChar*)id_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"superClass", (xmlChar*)parent_str);
    if (debugging_level != 0) {
        strcpy(str, TITLE);
        strcat(str, ".tool.c.compiler.option.debuglevel.value.g");
        if (debugging_level != '2') {
            char flag[2];
            flag[0] = debugging_level;
            flag[1] = 0;
            strcat(str, flag);
        }
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"value", (xmlChar*)str);
        xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"valueType", (xmlChar*)"enumerated");
    }
    xmlTextWriterEndElement(cproject_writer); // option 
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


void get_assembler_superclass(bool debugBuild, char *parent_str)
{
}

void get_bulder_name(char *out, const char *build)
{
    sprintf(out, "Gnu Make Builder.%s", build);
}

void put_cdt_project()
{
    char str[128];
    char us_str[16];

    get_us(us_str);
    sprintf(str, "%s.null.%s", from_codemodel.project_name, us_str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"id", (xmlChar*)str);
    xmlTextWriterWriteAttribute(cproject_writer, (xmlChar*)"name", (xmlChar*)from_codemodel.project_name);
}

void write_natures()
{
    xmlTextWriterWriteElement(project_writer, (xmlChar*)"nature", (xmlChar*)"com.st.stm32cube.ide.mcu.MCUProjectNature");
    xmlTextWriterWriteElement(project_writer, (xmlChar*)"nature", (xmlChar*)"com.st.stm32cube.ide.mcu.MCUCubeProjectNature");
    xmlTextWriterWriteElement(project_writer, (xmlChar*)"nature", (xmlChar*)"com.st.stm32cube.ide.mcu.MCUCubeIdeServicesRevAev2ProjectNature");
    xmlTextWriterWriteElement(project_writer, (xmlChar*)"nature", (xmlChar*)"com.st.stm32cube.ide.mcu.MCUAdvancedStructureProjectNature");
    xmlTextWriterWriteElement(project_writer, (xmlChar*)"nature", (xmlChar*)"com.st.stm32cube.ide.mcu.MCUEndUserDisabledTrustZoneProjectNature");
    xmlTextWriterWriteElement(project_writer, (xmlChar*)"nature", (xmlChar*)"com.st.stm32cube.ide.mcu.MCUSingleCpuProjectNature");
    xmlTextWriterWriteElement(project_writer, (xmlChar*)"nature", (xmlChar*)"com.st.stm32cube.ide.mcu.MCURootProjectNature");
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

