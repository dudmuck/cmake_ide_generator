#include <string.h>
#include <stdint.h>
#include <libxml/xmlreader.h>
#include "xml.h"

const char * const ScannerConfigBuilder = "org.eclipse.cdt.managedbuilder.core.ScannerConfigBuilder";
const char * const genmakebuilder = "org.eclipse.cdt.managedbuilder.core.genmakebuilder";
const char * const ScannerConfigNature = "org.eclipse.cdt.managedbuilder.core.ScannerConfigNature";
const char * const managedBuildNature = "org.eclipse.cdt.managedbuilder.core.managedBuildNature";
const char * const cnature = "org.eclipse.cdt.core.cnature";
const char * const ccnature = "org.eclipse.cdt.core.ccnature";  // optional nature
const char * const MCUProjectNature = "com.st.stm32cube.ide.mcu.MCUProjectNature";
const char * const genmakebuilder_triggers = "clean,full,incremental,";
const char * const ScannerConfigBuilder_triggers = "full,incremental,";
const char * const MCUCubeProjectNature = "com.st.stm32cube.ide.mcu.MCUCubeProjectNature";
const char * const MCUCubeIdeServicesRevAev2ProjectNature = "com.st.stm32cube.ide.mcu.MCUCubeIdeServicesRevAev2ProjectNature";
const char * const MCUCubeIdeServicesRevAProjectNature = "com.st.stm32cube.ide.mcu.MCUCubeIdeServicesRevAProjectNature";
const char * const MCUAdvancedStructureProjectNature = "com.st.stm32cube.ide.mcu.MCUAdvancedStructureProjectNature";
const char * const MCUEndUserDisabledTrustZoneProjectNature = "com.st.stm32cube.ide.mcu.MCUEndUserDisabledTrustZoneProjectNature";
const char * const MCUSingleCpuProjectNature = "com.st.stm32cube.ide.mcu.MCUSingleCpuProjectNature";
const char * const MCURootProjectNature = "com.st.stm32cube.ide.mcu.MCURootProjectNature";

int test_cproject(void);

typedef union {
    struct {
        uint32_t name                       : 1;    // 0
        uint32_t comment                    : 1;    // 1
        uint32_t projects                   : 1;    // 2
        uint32_t genmakebuilder             : 1;    // 3
        uint32_t ScannerConfigBuilder       : 1;    // 4
        uint32_t cnature                    : 1;    // 5
        uint32_t managedBuildNature         : 1;    // 6
        uint32_t ScannerConfigNature        : 1;    // 7
        uint32_t MCUProjectNature           : 1;    // 8
        uint32_t MCUCubeProjectNature: 1;    // 9
        uint32_t MCUCubeIdeServicesRevAProjectNature: 1;    // 10
        uint32_t MCUAdvancedStructureProjectNature: 1;    // 11
        uint32_t MCUEndUserDisabledTrustZoneProjectNature: 1;    // 12
        uint32_t MCUSingleCpuProjectNature: 1;    // 13
        uint32_t MCURootProjectNature: 1;    // 14

    } bits;
    uint32_t dword;
} project_has_t;

project_has_t project_has;

char build_command_name[96];

typedef enum {
    /*  0 */ PROJECT_STATE_NONE = 0,
    /*  1 */ PROJECT_STATE_DESCRIPTION,
    /*  2 */ PROJECT_STATE_NAME,
    /*  3 */ PROJECT_STATE_BUILDSPEC,
    /*  4 */ PROJECT_STATE_BUILDCOMMAND,
    /*  5 */ PROJECT_STATE_BUILDCOMMAND_NAME,
    /*  6 */ PROJECT_STATE_BUILDCOMMAND_TRIGGERS,
    /*  7 */ PROJECT_STATE_BUILDCOMMAND_ARGUMENTS,
    /*  8 */ PROJECT_STATE_NATURES,
    /*  9 */ PROJECT_STATE_NATURE,
    /* 10 */ PROJECT_STATE_LINKEDRESOURCES,
    /* 11 */ PROJECT_STATE_LINKEDRESOURCES_LINK,
    /* 12 */ PROJECT_STATE_LINKEDRESOURCES_LINK_NAME,
    /* 13 */ PROJECT_STATE_LINKEDRESOURCES_LINK_TYPE,
    /* 14 */ PROJECT_STATE_LINKEDRESOURCES_LINK_LOCATIONURI,
    /* 15 */ PROJECT_STATE_LINKEDRESOURCES_LINK_LOCATION,
} project_state_e;
#define PROJECT_DEPTH       5
project_state_e project_state[PROJECT_DEPTH];

static int processNode(xmlTextReaderPtr reader)
{
    const char *name_in, *value_in;
    int depth, nodeType = xmlTextReaderNodeType(reader);
    int ret = 0;
    if (nodeType == XML_NODE_TYPE_WHITE_SPACE)
        return 0;   // white space

    name_in = (char*)xmlTextReaderConstName(reader);
    depth = xmlTextReaderDepth(reader);

    printf("depth%d %s name:%s isEmpty:%d hasValue%d", 
	    depth,
	    nodeTypeToString(nodeType),
	    name_in,
	    xmlTextReaderIsEmptyElement(reader),
	    xmlTextReaderHasValue(reader)
    );

    value_in = (char*)xmlTextReaderConstValue(reader);
    printf(" %s ", value_in);

    if (nodeType == XML_NODE_TYPE_START_ELEMENT) {
        switch (depth) {
            case 0:
                if (strcmp(name_in, "projectDescription") != 0) {
                    fprintf(stderr, ".project not projectDescription ");
                    ret = -1;
                } else
                    project_state[0] = PROJECT_STATE_DESCRIPTION;
                break;
            case 1:
                if (project_state[0] != PROJECT_STATE_DESCRIPTION) {
                    fprintf(stderr, ".project depth1: depth0 not projectDescription ");
                    ret = -1;
                    break;
                }
                if (strcmp("name", name_in) == 0) {
                    project_state[1] = PROJECT_STATE_NAME;
                    project_has.bits.name = 1;
                } else if (strcmp("comment", name_in) == 0) {
                    project_has.bits.comment = 1;
                } else if (strcmp("projects", name_in) == 0) {
                    project_has.bits.projects = 1;
                } else if (strcmp("buildSpec", name_in) == 0) {
                    project_state[1] = PROJECT_STATE_BUILDSPEC;
                } else if (strcmp("natures", name_in) == 0) {
                    project_state[1] = PROJECT_STATE_NATURES;
                } else if (strcmp("linkedResources", name_in) == 0) {
                    project_state[1] = PROJECT_STATE_LINKEDRESOURCES;
                } else {
                    printf("\e[31m.project line %d unknown element %s\e[0m ", __LINE__, name_in);
                    ret = -1;
                }
                break;
            case 2:
                if (project_state[1] == PROJECT_STATE_BUILDSPEC) {
                    if (strcmp("buildCommand", name_in) == 0) {
                        project_state[2] = PROJECT_STATE_BUILDCOMMAND;
                    } else {
                        printf("\e[31m.project line %d @depth2 buildSpec unknown element %s\e[0m ", __LINE__, name_in);
                        ret = -1;
                    }
                } else if (project_state[1] == PROJECT_STATE_NAME) {
                    printf(".project @depth2 PROJECT_STATE_NAME name=%s ", name_in);
                } else if (project_state[1] == PROJECT_STATE_LINKEDRESOURCES) {
                    if (strcmp("link", name_in) == 0) {
                        project_state[2] = PROJECT_STATE_LINKEDRESOURCES_LINK;
                    } else {
                        printf("\e[31m linked resources %s\[0m ", name_in);
                        ret = -1;
                    }
                } else if (project_state[1] == PROJECT_STATE_NATURES) {
                    if (strcmp("nature", name_in) == 0) {
                        project_state[2] = PROJECT_STATE_NATURE;
                    } else {
                        printf("\e[31m.project line %d @depth2 natures unknown element %s\e[0m ", __LINE__, name_in);
                        ret = -1;
                    }
                } else {
                    printf(".project line:%d @depth2 depth_state[1]=%d name=%s ", __LINE__, project_state[1], name_in);
                    ret = -1;
                }
                break;
            case 3:
                if (project_state[2] == PROJECT_STATE_BUILDCOMMAND) {
                    if (strcmp(name_in, "name") == 0)
                        project_state[3] = PROJECT_STATE_BUILDCOMMAND_NAME;
                    else if (strcmp(name_in, "triggers") == 0)
                        project_state[3] = PROJECT_STATE_BUILDCOMMAND_TRIGGERS;
                    else if (strcmp(name_in, "arguments") == 0)
                        project_state[3] = PROJECT_STATE_BUILDCOMMAND_ARGUMENTS;
                    else {
                        printf("PROJECT_STATE_BUILDCOMMAND unknown name=%s ", name_in);
                        ret = -1;
                    }
                } else if (project_state[2] == PROJECT_STATE_LINKEDRESOURCES_LINK) {
                    if (strcmp(name_in, "name") == 0)
                        project_state[3] = PROJECT_STATE_LINKEDRESOURCES_LINK_NAME;
                    else if (strcmp(name_in, "type") == 0)
                        project_state[3] = PROJECT_STATE_LINKEDRESOURCES_LINK_TYPE;
                    else if (strcmp(name_in, "locationURI") == 0)
                        project_state[3] = PROJECT_STATE_LINKEDRESOURCES_LINK_LOCATIONURI;
                    else if (strcmp(name_in, "location") == 0)
                        project_state[3] = PROJECT_STATE_LINKEDRESOURCES_LINK_LOCATION;
                    else
                        printf("\e[31mline %d LINK name=%s\e[0m ", __LINE__, name_in);
                } else {
                    printf("\e[31m.project line %d @depth3 depth_state[2]=%d\e[0m ", __LINE__, project_state[2]);
                    ret = -1;
                }
                break;
            default:
                break;
        } // ..switch (depth)
    } // ..if (nodeType == XML_NODE_TYPE_START_ELEMENT)
    else if (nodeType == XML_NODE_TYPE_END_OF_ELEMENT) {
        project_state[depth] = PROJECT_STATE_NONE;
        if (strcmp("buildCommand", name_in) == 0) {
            build_command_name[0] = 0;
        }
    } else if (nodeType == XML_NODE_TYPE_TEXT) {
        switch (project_state[depth-1]) {
            case PROJECT_STATE_NAME:
                break;
            case PROJECT_STATE_BUILDCOMMAND_NAME:
                printf("PROJECT_STATE_BUILDCOMMAND_NAME %s ", value_in);
                strcpy(build_command_name, value_in);
                if (strcmp(value_in, genmakebuilder) == 0)
                    project_has.bits.genmakebuilder = 1;
                else if (strcmp(value_in, ScannerConfigBuilder) == 0)
                    project_has.bits.ScannerConfigBuilder = 1;
                break;
            case PROJECT_STATE_BUILDCOMMAND_TRIGGERS:
                if (strcmp(build_command_name, genmakebuilder) == 0) {
                    if (strcmp(value_in, genmakebuilder_triggers) != 0) {
                        fprintf(stderr, "line %d %s expecting triggers %s ", __LINE__, build_command_name, genmakebuilder_triggers);
                        ret = -1;
                    }
                } else if (strcmp(build_command_name, ScannerConfigBuilder) == 0) {
                    if (strcmp(value_in, ScannerConfigBuilder_triggers) != 0) {
                        fprintf(stderr, "line %d %s expecting triggers %s ", __LINE__, build_command_name, ScannerConfigBuilder);
                        ret = -1;
                    }
                }
                break;
            case PROJECT_STATE_NATURE:
                if (strcmp(cnature, value_in) == 0)
                    project_has.bits.cnature = 1;
                else if (strcmp(ccnature, value_in) == 0) {
                    /* ccnature optional */
                } else if (strcmp(managedBuildNature, value_in) == 0)
                    project_has.bits.managedBuildNature = 1;
                else if (strcmp(ScannerConfigNature, value_in) == 0)
                    project_has.bits.ScannerConfigNature = 1;
                else if (strcmp(MCUProjectNature, value_in) == 0)
                    project_has.bits.MCUProjectNature = 1;
                else if (strcmp(MCUCubeProjectNature, value_in) == 0)
                    project_has.bits.MCUCubeProjectNature = 1;
                else if (strcmp(MCUCubeIdeServicesRevAProjectNature, value_in) == 0)
                    project_has.bits.MCUCubeIdeServicesRevAProjectNature = 1;
                else if (strcmp(MCUCubeIdeServicesRevAev2ProjectNature , value_in) == 0)
                    project_has.bits.MCUCubeIdeServicesRevAProjectNature = 1;
                else if (strcmp(MCUAdvancedStructureProjectNature, value_in) == 0)
                    project_has.bits.MCUAdvancedStructureProjectNature = 1;
                else if (strcmp(MCUEndUserDisabledTrustZoneProjectNature, value_in) == 0)
                    project_has.bits.MCUEndUserDisabledTrustZoneProjectNature = 1;
                else if (strcmp(MCUSingleCpuProjectNature, value_in) == 0)
                    project_has.bits.MCUSingleCpuProjectNature = 1;
                else if (strcmp(MCURootProjectNature, value_in) == 0)
                    project_has.bits.MCURootProjectNature = 1;
                else {
                    fprintf(stderr, "\e[31mline %d unknown nature %s\e[0m\r\n", __LINE__, value_in);
                    ret = -1;
                }
                break;
            case PROJECT_STATE_LINKEDRESOURCES_LINK_NAME:
                break;
            case PROJECT_STATE_LINKEDRESOURCES_LINK_TYPE:
                if (strcmp(value_in, "2") == 0) {
                    /* folder */
                } else if (strcmp(value_in, "1") == 0) {
                    /* file */
                } else {
                    printf("\e[31mline %d unknown link type %s\e[0m ", __LINE__, value_in);
                }
                break;
            case PROJECT_STATE_LINKEDRESOURCES_LINK_LOCATIONURI:
                if (strcmp(value_in, "virtual:/virtual") != 0) {
                    printf("\e[31munkown locationURI %s\e[0m ", value_in);
                    ret = -1;
                }
                break;
            case PROJECT_STATE_LINKEDRESOURCES_LINK_LOCATION:
                /* filename of project source file */
                break;
            default:
                printf("\e[31mline %d text-parent-state:%d %s\e[0m ", __LINE__, project_state[depth-1], value_in);
                ret = -1;
                break;
        } // ..switch (project_state[depth-1])

    }
    printf("\n");

    return ret;
}

void show_project_missing()
{
    printf("missing ");
    if (!project_has.bits.name)
        printf("name ");
    if (!project_has.bits.comment)
        printf("comment ");
    if (!project_has.bits.projects)
        printf("projects ");
    if (!project_has.bits.genmakebuilder)
        printf("genmakebuilder ");
    if (!project_has.bits.ScannerConfigBuilder)
        printf("ScannerConfigBuilder ");
    if (!project_has.bits.cnature)
        printf("cnature ");
    if (!project_has.bits.managedBuildNature)
        printf("managedBuildNature ");
    if (!project_has.bits.ScannerConfigNature)
        printf("ScannerConfigNature ");
    if (!project_has.bits.MCUProjectNature)
        printf("MCUProjectNature ");
    if (!project_has.bits.MCUCubeProjectNature)
        printf("MCUCubeProjectNature ");
    if (!project_has.bits.MCUCubeIdeServicesRevAProjectNature)
        printf("MCUCubeIdeServicesRevAProjectNature ");
    if (!project_has.bits.MCUAdvancedStructureProjectNature)
        printf("MCUAdvancedStructureProjectNature ");
    if (!project_has.bits.MCUEndUserDisabledTrustZoneProjectNature)
        printf("MCUEndUserDisabledTrustZoneProjectNature ");
    if (!project_has.bits.MCUSingleCpuProjectNature)
        printf("MCUSingleCpuProjectNature ");
    if (!project_has.bits.MCURootProjectNature)
        printf("MCURootProjectNature ");
    printf("\n");
}

int main(int argc, char *argv[])
{
    int ret;
    const char * const project_file_name = ".project";
    xmlTextReaderPtr project_reader;

    LIBXML_TEST_VERSION

    project_reader = xmlReaderForFile(project_file_name, NULL,
        XML_PARSE_DTDATTR |  /* default DTD attributes */
		 XML_PARSE_NOENT    /* substitute entities */
    ); 
//		 XML_PARSE_DTDVALID  /* validate with the DTD */
    if (!project_reader) {
        fprintf(stderr, "Unable to open %s\n", project_file_name);
        return -1;
    }

    project_has.dword = 0;
    for (unsigned n = 0; n < PROJECT_DEPTH; n++)
        project_state[n] = PROJECT_STATE_NONE;

    ret = xmlTextReaderRead(project_reader);
    while (ret == 1) {
        if (processNode(project_reader) < 0) {
            ret = -1;
            break;
        }
        ret = xmlTextReaderRead(project_reader);
    }
	/*
	 * Once the document has been fully parsed check the validation results
	 *
	if (xmlTextReaderIsValid(project_reader) != 1) {
	    fprintf(stderr, "Document %s does not validate\n", project_file_name);
	}*/

    xmlFreeTextReader(project_reader);
    if (project_has.dword != 0x7fff) {
        show_project_missing(project_has);
        printf("\e[31mFAIL: ");
        ret = -1;
    }
    printf("project_has:%x\e[0m #####################################\r\n", project_has.dword);

    if (ret < 0)
        return ret;
    else
        return test_cproject();
}

