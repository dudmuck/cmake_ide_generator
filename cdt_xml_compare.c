#include <stdbool.h>
#include <string.h>
#include <libxml/xmlreader.h>
#include "xml.h"

#define MAX_DEPTH       12

const char * const CCS = "com.ti.ccstudio.";
const char * const NXP = "com.crt.advproject.";

const char * const openAttributes[] = {
    "artifactName",
    "buildPath",
    "postbuildStep",
    NULL
};

/* elements in control file which arent required to be in FUT */
const char * const optionalElements[] = {
    "listOptionValue",
    "sourceEntries",
    "entry",
    NULL
};

/* superClasses in control file which arent required to be in FUT */
const char * const optionalSuperClasses[] = {
    "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.defaults" ,
    NULL
};

typedef struct {
    const char *superClass_prefix;
    const char *value_prefix;
} enumeration_exception_t;

enumeration_exception_t enumeration_exceptions[] = {
    {"gnu.c.compiler.option.", "gnu.c."}, /* for ac6 sw4stm32 */
    {"gnu.cpp.compiler.option.", "gnu.cpp.compiler."}, /* for ac6 sw4stm32 */
    {"fr.ac6.managedbuild.gnu.c.compiler.option.", "fr.ac6.managedbuild.gnu.c."},
    {"fr.ac6.managedbuild.gnu.cpp.compiler.option.", "fr.ac6.managedbuild.gnu.cpp."},
    {"ilg.gnuarmeclipse.managedbuild.cross.option.arm.target.family", "ilg.gnuarmeclipse.managedbuild.cross.option.arm.target.mcpu"},
    {"com.crt.advproject.link.cpp.arch", "com.crt.advproject.link.cpp.target."},  /* mcuXpresso */
    {"com.crt.advproject.link.gcc.hdrlib", "com.crt.advproject.gcc.link.hdrlib"},  /* mcuXpresso */ 
    {"com.crt.advproject.cpp.arch", "com.crt.advproject.cpp.target."},  /* mcuXpresso */
    {"com.crt.advproject.gcc.arch", "com.crt.advproject.gcc.target."},  /* mcuXpresso */
    {"com.crt.advproject.gas.arch", "com.crt.advproject.gas.target."},  /* mcuXpresso */
    {"com.crt.advproject.link.arch", "com.crt.advproject.link.target."},  /* mcuXpresso */
    {NULL, NULL}
};

typedef struct {
    const char *context;
    const char *name;
    const bool value_open;  /* element value anything allowed */
    const char *required_text;
} fut_optional_elements_t;

const fut_optional_elements_t fut_optional_elements[] = {
    { "projectDescription", "linkedResources", false, NULL},
    { "projectDescription-linkedResources", "link", false, NULL},
    { "projectDescription-linkedResources-link", "name", true, NULL},
    { "projectDescription-linkedResources-link", "type", true, NULL},
    { "projectDescription-linkedResources-link", "locationURI", false, "virtual:/virtual"},
    { "projectDescription-linkedResources-link", "location", true, NULL},
    { NULL, NULL, false, NULL }
};

struct attr_node_s {
    char *name;
    char *value;
    struct attr_node_s *next;
};

struct node_s {
    unsigned serialNumber;
    bool empty;
    char *name;
    char *value;
    int depth;
    int node_type;
    struct attr_node_s *attrs;
    bool checked;
    struct node_s *next;    // at same depth
};

struct node_s *_control;
struct node_s *_fut;

struct name_depth {
    const char *name;
    int depth;
};

struct name_depth ignore_until_end;

volatile unsigned sn;
static int processNode(xmlTextReaderPtr reader, const char *title, struct node_s **node, bool isFut)
{
    int ret = 0;
    int acnt, depth, nodeType = xmlTextReaderNodeType(reader);
    const char *name_in = (char*)xmlTextReaderConstName(reader);
    const char *value_in = (char*)xmlTextReaderConstValue(reader);

    if (nodeType == XML_NODE_TYPE_WHITE_SPACE)
        return 0;   // white space

    acnt = xmlTextReaderAttributeCount(reader);
    depth = xmlTextReaderDepth(reader);

    printf("save-%s(D%d %s %s AT%d) ", title,
            depth, nodeTypeToString(nodeType), name_in, acnt
    );
    if (depth >= MAX_DEPTH) {
        printf(" max depth too small\n");
        return -1;
    }
    if (value_in != NULL)
        printf(" %d=\e[7m%s\e[0m ", (int)strlen(value_in), value_in);

    if (*node) {
        while ((*node)->next) {
            node = &(*node)->next;
        }
        node = &(*node)->next;
    }
    (*node) = malloc(sizeof(struct node_s));
    (*node)->name = malloc(strlen(name_in)+1);
    (*node)->serialNumber = sn++;
    strcpy((*node)->name, name_in);
    (*node)->empty = xmlTextReaderIsEmptyElement(reader);
    (*node)->depth = depth;
    (*node)->checked = false;
    (*node)->node_type = nodeType;
    (*node)->attrs = NULL;
    (*node)->next = NULL;

    if (value_in) {
        (*node)->value = malloc(strlen(value_in)+1);
        strcpy((*node)->value, value_in);
    } else
        (*node)->value = NULL;

    if (acnt > 0) {
        struct attr_node_s **attrs = &(*node)->attrs;
        while (xmlTextReaderMoveToNextAttribute(reader)) {
            char *a_name = (char*)xmlTextReaderName(reader);
            char *a_value = (char*)xmlTextReaderValue(reader);
            if (*attrs) {
                while ((*attrs)->next) {
                    attrs = &(*attrs)->next;
                }
                attrs = &(*attrs)->next;
            }
            (*attrs) = malloc(sizeof(struct attr_node_s));
            (*attrs)->next = NULL;
            (*attrs)->name = malloc(strlen(a_name)+1);
            (*attrs)->value = malloc(strlen(a_value)+1);
            strcpy((*attrs)->name, a_name);
            strcpy((*attrs)->value, a_value);
        } // ..attribute iterator
    }


    printf("\n");

    return ret;
}

int save_xml_file(xmlTextReaderPtr reader, struct node_s **dest, const char *title, bool isFut)
{
    struct node_s **node = dest;
    int ret = xmlTextReaderRead(reader);
    sn = 0;
    while (ret == 1) {
        ret = processNode(reader, title, node, isFut);
        if (ret < 0) {
            printf("processNode failed\n");
            break;
        }
        ret = xmlTextReaderRead(reader);
    }
    return ret;
}

void print_nodes(struct node_s *list, const char *title)
{
    printf("\n");
    for (; list != NULL; list = list->next) {
        printf("print-%s sn%d D%d %s %s ", title, list->serialNumber, list->depth, nodeTypeToString(list->node_type), list->name);
        if (list->value)
            printf(" \e[7m%d=%s\e[0m ", (int)strlen(list->value), list->value);
        for (struct attr_node_s *attrs = list->attrs; attrs != NULL; attrs = attrs->next) {
            printf("\e[32m%s\e[0m=\e[35m%s\e[0m ", attrs->name, attrs->value);
        }
        printf("\n");
    }
}

void show_unchecked(struct node_s *list, bool is_fut, const char *title)
{
    struct node_s *node_context[MAX_DEPTH];

    for (; list != NULL; list = list->next) {
        node_context[list->depth] = list;
        char context_str[256];
        strcpy(context_str, node_context[0]->name);
        for (unsigned n = 1; n < list->depth; n++) {
            strcat(context_str, "-");
            strcat(context_str, node_context[n]->name);
        }

        if (!list->checked) {
            if (is_fut && list->node_type == XML_NODE_TYPE_TEXT) {
                /* permit extra text values in fut */
                if (strcmp(context_str, "projectDescription-natures-nature") == 0 && strcmp(list->value, "org.eclipse.cdt.core.ccnature") == 0)
                    continue;
            }
            printf("\e[33m%s \e[31munchecked-%s\e[0m sn%d D%d %s %s ", context_str, title, list->serialNumber, list->depth, nodeTypeToString(list->node_type), list->name);
            if (list->value)
                printf(" \e[7m%d=%s\e[0m ", (int)strlen(list->value), list->value);
            for (struct attr_node_s *attrs = list->attrs; attrs != NULL; attrs = attrs->next) {
                printf("\e[32m%s\e[0m=\e[35m%s\e[0m ", attrs->name, attrs->value);
            }
            printf("\n");
        }
    }
}

int check_fut_optional()
{
    struct node_s *fut_node_context_opt[MAX_DEPTH];
    bool value_open = false;
    const char *required_text = NULL;

    for (struct node_s *futList = _fut; futList != NULL; futList = futList->next) {
        char fut_context_str[256];
        struct node_s *prev_fut_node_context = fut_node_context_opt[futList->depth];
        fut_node_context_opt[futList->depth] = futList;
        strcpy(fut_context_str, fut_node_context_opt[0]->name);
        for (unsigned n = 1; n < futList->depth; n++) {
            strcat(fut_context_str, "-");
            strcat(fut_context_str, fut_node_context_opt[n]->name);
        }
        if (futList->checked)
            continue;


        if (futList->node_type == XML_NODE_TYPE_START_ELEMENT) {
            for (unsigned n = 0; fut_optional_elements[n].context != NULL; n++) {
                const fut_optional_elements_t *foe = fut_optional_elements + n;
                if (strcmp(fut_context_str, foe->context) == 0 && strcmp(futList->name, foe->name) == 0) {
                    value_open = foe->value_open;   // element value (text anything allowed)
                    required_text = foe->required_text;
                }
            }
        } else if (futList->node_type == XML_NODE_TYPE_END_OF_ELEMENT) {
            for (unsigned n = 0; fut_optional_elements[n].context != NULL; n++) {
                const fut_optional_elements_t *foe = fut_optional_elements + n;
                if (strcmp(fut_context_str, foe->context) == 0 && strcmp(futList->name, foe->name) == 0) {
                    if (strcmp(prev_fut_node_context->name, foe->name) != 0) {
                        printf("line %d fail start-end name %s -> %s\n", __LINE__, prev_fut_node_context->name, futList->name);
                        return -1;
                    }
                    futList->checked = true;    //  end ok
                    prev_fut_node_context->checked = true;  // start ok
                }
            }
        } else if (futList->node_type == XML_NODE_TYPE_TEXT) {
            if (value_open) {
                futList->checked = true;    //  text allowed to be anything
                value_open = false;
            } else if (required_text != NULL) {
                if (strcmp(futList->value, required_text) != 0) {
                    printf("line %d text fail %s != %s\n", __LINE__, futList->value, required_text);
                    return -1;
                }
                futList->checked = true;    //  text ok
                required_text = NULL;
            }
        }
    }

    return 0;
} // ..check_fut_optional()

int id_strcmp(const char *ctrl_str_, const char *test_str_)
{
    const char *ctrl_ptr = ctrl_str_;
    const char *test_ptr = test_str_;
    while (*ctrl_ptr != 0 && *test_ptr != 0) {
        while (*ctrl_ptr > '/' && *ctrl_ptr < ':')
            ctrl_ptr++;
        while (*test_ptr > '/' && *test_ptr < ':')
            test_ptr++;
        if (*ctrl_ptr != *test_ptr) {
            int len;
            char ctrl[96], test[96];
            len = (ctrl_ptr - ctrl_str_) + 1;
            strncpy(ctrl, ctrl_str_, len);
            ctrl[len] = 0;
            len = (test_ptr - test_str_) + 1;
            strncpy(test, test_str_, len);
            test[len] = 0;
            return *ctrl_ptr - *test_ptr;
        } else if (*ctrl_ptr == 0)
            return 0;   /* end of string, no match failures */
         
        ctrl_ptr++;
        test_ptr++;
    }
    return 0;
}

typedef struct {
    char value_[128];
    bool use_strstr;    // false = strcmp entire value
    unsigned value_length;  // 0=use strcmp, else use strncmp
    const char *name;
} must_match_attrib_t;


char control_build[32];
char fut_cconfiguration_id[128];
char fut_cconfiguration_name[64];

#define MAX_BUILDS      3
unsigned builds_idx;
char builds[MAX_BUILDS][32];

#define MAX_MUST_MATCH_ATTRIBS       2
int check_fut(struct node_s *ctrlList, struct node_s *fut)
{
    static struct node_s *control_context[MAX_DEPTH];
    static struct node_s *fut_context[MAX_DEPTH];
    unsigned cnt = 0;
    int ret = 0;
    must_match_attrib_t must_match_attrib[MAX_MUST_MATCH_ATTRIBS];
    const char *required_element_name = NULL;
    char control_context_str[256];//, inputId[96];
    bool futChecked;
    bool openTextValue = false;

    for (unsigned n = 0; n < MAX_MUST_MATCH_ATTRIBS; n++) {
        must_match_attrib[n].value_length = 0;
        must_match_attrib[n].value_[must_match_attrib[n].value_length] = 0;
        must_match_attrib[n].use_strstr = false;
        must_match_attrib[n].name = NULL;
    }

    control_context[ctrlList->depth] = ctrlList;
    strcpy(control_context_str, control_context[0]->name);
    for (unsigned n = 1; n < ctrlList->depth; n++) {
        strcat(control_context_str, "-");
        strcat(control_context_str, control_context[n]->name);
    }

    if (XML_NODE_TYPE_START_ELEMENT == ctrlList->node_type) {
        if (strcmp(ctrlList->name, "storageModule") == 0) {
            const char *control_id_value = NULL;
            required_element_name = "storageModule";
            for (struct attr_node_s *ctl_attrs = ctrlList->attrs; ctl_attrs != NULL; ctl_attrs = ctl_attrs->next) {
                if (strcmp(ctl_attrs->name, "id") == 0) {
                    control_id_value = ctl_attrs->value;
                    printf("got-ctl-id ");
                    fflush(stdout);
                } else if (strcmp(ctl_attrs->name, "moduleId") == 0) {
                    strcpy(must_match_attrib[0].value_, ctl_attrs->value);
                    must_match_attrib[0].value_length = strlen(ctl_attrs->value);
                    must_match_attrib[0].name = "moduleId";
                    if (control_id_value != NULL && strcmp("org.eclipse.cdt.core.settings", ctl_attrs->value) == 0) {
                        int len;
                        printf("control_id_value:%s\n", control_id_value);
                        fflush(stdout);
                        len = strlen(control_id_value);
                        must_match_attrib[1].name = "id";
                        strcpy(must_match_attrib[1].value_, control_id_value);
                        while (control_id_value[len] != '.' && len > 0)
                            len--;
                        must_match_attrib[1].value_length = len;
                    }
                }
            }
        } else if (strcmp(ctrlList->name, "cconfiguration") == 0) {
            for (struct attr_node_s *ctl_attrs = ctrlList->attrs; ctl_attrs != NULL; ctl_attrs = ctl_attrs->next) {
                if (strcmp(ctl_attrs->name, "id") == 0) {
                    int len = strlen(ctl_attrs->value);
                    fut_cconfiguration_id[0] = 0;
                    while (ctl_attrs->value[len] != '.' && len > 0)
                        len--;
                    strncpy(must_match_attrib[0].value_, ctl_attrs->value, len);
                    must_match_attrib[0].value_length = len;
                    must_match_attrib[0].value_[must_match_attrib[0].value_length] = 0;
                    must_match_attrib[0].name = "id";
                }
            }
        } else if (strcmp(ctrlList->name, "extension") == 0) {
            for (struct attr_node_s *ctl_attrs = ctrlList->attrs; ctl_attrs != NULL; ctl_attrs = ctl_attrs->next) {
                if (strcmp(ctl_attrs->name, "point") == 0) {
                    strcpy(must_match_attrib[0].value_, ctl_attrs->value);
                    must_match_attrib[0].value_length = strlen(ctl_attrs->value);
                    must_match_attrib[0].name = "point";
                } else if (strcmp(ctl_attrs->name, "id") == 0) {
                    strcpy(must_match_attrib[1].value_, ctl_attrs->value);
                    must_match_attrib[1].value_length = strlen(ctl_attrs->value);
                    must_match_attrib[1].name = "id";
                }
            }
        } else if (strcmp(ctrlList->name, "configuration") == 0) {
            strcpy(must_match_attrib[1].value_, fut_cconfiguration_id);
            must_match_attrib[1].value_length = strlen(fut_cconfiguration_id);
            must_match_attrib[1].name = "id";
        } else if (strcmp(ctrlList->name, "folderInfo") == 0) {
            strcpy(must_match_attrib[1].value_, fut_cconfiguration_id);
            must_match_attrib[1].value_length = strlen(fut_cconfiguration_id);
            must_match_attrib[1].name = "id";
            for (struct attr_node_s *ctl_attrs = ctrlList->attrs; ctl_attrs != NULL; ctl_attrs = ctl_attrs->next) {
                int idx, build_end, build_start;
                if (strcmp(ctl_attrs->name, "id") != 0)
                    continue;
                idx = strlen(ctl_attrs->value) - 2; // null terminator and '.'
                while (ctl_attrs->value[idx] != '.') {
                    idx--; /* skip over number */
                }
                build_end = idx;
                idx--;
                while (ctl_attrs->value[idx] != '.') {
                    idx--; /* capture build name */
                }
                build_start = idx + 1;
                strncpy(control_build, ctl_attrs->value + build_start, build_end - build_start);
                control_build[build_end - build_start] = 0;
                if (builds_idx >= MAX_BUILDS) {
                    printf("\e[31mline %d MAX_BUILDS\e[0m", __LINE__);
                    return -1;
                }
                strcpy(builds[builds_idx++], control_build);
            }
        } else if (strcmp(ctrlList->name, "toolChain") == 0) {
            strcpy(must_match_attrib[0].value_, control_build);
            must_match_attrib[0].use_strstr = true;
            must_match_attrib[0].name = "id";
        } else if (strcmp(ctrlList->name, "option") == 0 || strcmp(ctrlList->name, "tool") == 0 || strcmp(ctrlList->name, "inputType") == 0) {
            for (struct attr_node_s *ctl_attrs = ctrlList->attrs; ctl_attrs != NULL; ctl_attrs = ctl_attrs->next) {
                if (strcmp(ctl_attrs->name, "superClass") == 0) {
                    must_match_attrib[0].name = "superClass";
                    strcpy(must_match_attrib[0].value_, ctl_attrs->value);
                    must_match_attrib[0].value_length = 0;  /* must match entire value */
                }
            }
        } else if (strcmp(ctrlList->name, "additionalInput") == 0) {
            for (struct attr_node_s *ctl_attrs = ctrlList->attrs; ctl_attrs != NULL; ctl_attrs = ctl_attrs->next) {
                if (strcmp(ctl_attrs->name, "kind") == 0) {
                    must_match_attrib[0].name = "kind";
                    strcpy(must_match_attrib[0].value_, ctl_attrs->value);
                    must_match_attrib[0].value_length = strlen(ctl_attrs->value);
                } else if (strcmp(ctl_attrs->name, "paths") == 0) {
                    must_match_attrib[1].name = "paths";
                    strcpy(must_match_attrib[1].value_, ctl_attrs->value);
                    must_match_attrib[1].value_length = strlen(ctl_attrs->value);
                }
            }
        } else if (strcmp(ctrlList->name, "scannerConfigBuildInfo") == 0) {
            for (struct attr_node_s *ctl_attrs = ctrlList->attrs; ctl_attrs != NULL; ctl_attrs = ctl_attrs->next) {
                if (strcmp(ctl_attrs->name, "instanceId") == 0) {
                    unsigned n = 0;
                    const char *src_ptr = strrchr(ctl_attrs->value, ';') + 1;
                    must_match_attrib[0].name = "instanceId";
                    must_match_attrib[1].name = must_match_attrib[0].name;

                    must_match_attrib[0].use_strstr = true;
                    while (src_ptr[n] != 0) {
                        must_match_attrib[0].value_[n] = src_ptr[n];
                        n++;
                        if (src_ptr[n-1] == '.' && (src_ptr[n] <= '9' && src_ptr[n] >= '0'))
                            break;
                    }
                    must_match_attrib[0].value_[n] = 0;

                    src_ptr = ctl_attrs->value;
                    must_match_attrib[1].use_strstr = true;
                    n = 0;
                    while (src_ptr[n] != 0) {
                        must_match_attrib[1].value_[n] = src_ptr[n];
                        n++;
                        if (src_ptr[n-1] == '.' && (src_ptr[n] <= '9' && src_ptr[n] >= '0'))
                            break;
                    }
                    must_match_attrib[1].value_[n] = 0;
                }
            }
        } else if (strcmp(ctrlList->name, "targetPlatform") == 0 || strcmp(ctrlList->name, "builder") == 0 ) {
            if (strncmp(CCS, fut_cconfiguration_id, strlen(CCS)) == 0 ||
                strncmp(NXP, fut_cconfiguration_id, strlen(NXP)) == 0)
            {
                must_match_attrib[0].use_strstr = true;
                must_match_attrib[0].name = "id";
                strcpy(must_match_attrib[0].value_, control_build);
                must_match_attrib[0].value_length = strlen(control_build);
            }
        }
    } // ..if (XML_NODE_TYPE_START_ELEMENT == ctrlList->node_type)
    else if (XML_NODE_TYPE_TEXT == ctrlList->node_type) {
        printf(" text-control=%s ", ctrlList->value);
        if (strcmp(control_context_str, "projectDescription-name") == 0)
            openTextValue = true;
    }

    
    for (unsigned n = 0; n < MAX_MUST_MATCH_ATTRIBS; n++) {
        if (must_match_attrib[n].value_[0] != 0) {
            char my_value[256];
            if (must_match_attrib[n].value_length == 0)
                strcpy(my_value, must_match_attrib[n].value_);
            else {
                strncpy(my_value, must_match_attrib[n].value_, must_match_attrib[n].value_length);
                my_value[must_match_attrib[n].value_length] = 0;
            }
            printf("[must-match %s %s (%d) %s] ", must_match_attrib[n].name, must_match_attrib[n].use_strstr ? "~=" : "=", must_match_attrib[n].value_length, my_value);
        }
    }

    const char *ctrl_attrib_valueType = NULL;
    const char *ctrl_attrib_value = NULL;
    for (struct attr_node_s *ctl_attrs = ctrlList->attrs; ctl_attrs != NULL; ctl_attrs = ctl_attrs->next) {
        if (strcmp(ctl_attrs->name, "valueType") == 0)
            ctrl_attrib_valueType = ctl_attrs->value;
        else if (strcmp(ctl_attrs->name, "value") == 0)
            ctrl_attrib_value = ctl_attrs->value;
    }

    futChecked = false;
    struct node_s *failedFutNode = NULL;
    for (struct node_s *futList = fut; futList != NULL; futList = futList->next) {
        char fut_context_str[256];
        unsigned failed_attributes = 0;
        bool id_is_cconfiguration = false;
        struct node_s *prev_fut_node_context = fut_context[futList->depth];
        cnt++;

        fut_context[futList->depth] = futList;
        strcpy(fut_context_str, fut_context[0]->name);
        for (unsigned n = 1; n < ctrlList->depth; n++) {
            strcat(fut_context_str, "-");
            strcat(fut_context_str, fut_context[n]->name);
        }

        if (strcmp(control_context_str, fut_context_str) != 0)
            continue;

        if (required_element_name != NULL) {
            if (strcmp(futList->name, required_element_name) != 0)
                continue;
        }

        for (unsigned n = 0; n < MAX_MUST_MATCH_ATTRIBS; n++) {
            bool has_attribute;
            if (must_match_attrib[n].name == NULL)
                continue;
            has_attribute = false;
            for (struct attr_node_s *fut_attrs = futList->attrs; fut_attrs != NULL; fut_attrs = fut_attrs->next) {
                if (strcmp(fut_attrs->name, must_match_attrib[n].name) == 0) {
                    has_attribute = true;
                    if (must_match_attrib[n].use_strstr) {
                        if (strstr(fut_attrs->value, must_match_attrib[n].value_) == NULL)
                            failed_attributes++;
                    } else {
                        if (must_match_attrib[n].value_length == 0) {
                            if (strcmp(fut_attrs->value, must_match_attrib[n].value_) != 0)
                                failed_attributes++;
                        } else {
                            if (strncmp(fut_attrs->value, must_match_attrib[n].value_, must_match_attrib[n].value_length) != 0)
                                failed_attributes++;
                        }
                    }
                }
            }
            if (!has_attribute)
                failed_attributes++;
        }

        if (failed_attributes > 0)
            continue;

        /******** only get here if all required attributes pass ******/

        if (fut_cconfiguration_id[0] == 0 && strcmp(futList->name, "cconfiguration") == 0) {
            for (struct attr_node_s *fut_attrs = futList->attrs; fut_attrs != NULL; fut_attrs = fut_attrs->next) {
                if (strcmp(fut_attrs->name, "id") == 0) {
                    char _fut_cconfiguration_superClass[32];
                    int len = strlen(fut_attrs->value);
                    strcpy(fut_cconfiguration_id, fut_attrs->value);
                    while (fut_attrs->value[len] != '.' && len > 0)
                        len--;
                    strncpy(_fut_cconfiguration_superClass, fut_attrs->value, len);
                    _fut_cconfiguration_superClass[len] = 0;
                }
            }
        }

        if (ctrlList->depth == futList->depth &&
            ctrlList->node_type == futList->node_type &&
            strcmp(ctrlList->name, futList->name) == 0)
        {
            if (futList->node_type == XML_NODE_TYPE_PI) {
                if (strcmp(ctrlList->name, "fileVersion") == 0) {
                    futList->checked = true;    // value same
                } else {
                    printf("line %d PI value %s\n", __LINE__, futList->name);
                    return -1;
                }
            } else if (futList->node_type == XML_NODE_TYPE_END_OF_ELEMENT) {
                if (prev_fut_node_context->node_type == XML_NODE_TYPE_START_ELEMENT) {
                    printf("\e[32mend-ok-%d\e[0m ", futList->depth);
                    futList->checked = true;    // end ok
                    prev_fut_node_context->checked = true; // start ok
                }
            } else if (futList->node_type == XML_NODE_TYPE_START_ELEMENT) {
                if (ctrlList->empty && futList->empty) {
                    futList->checked = true;    // both empty element
                }
            } else if (futList->node_type == XML_NODE_TYPE_TEXT && !openTextValue) {
                printf("text-fut=%s ", futList->value);
                if (strcmp(ctrlList->value, futList->value) == 0)
                    printf("***");
                else
                    continue;   // mismatched text
                printf(" ");
            }

            char fut_attrib_id[192];
            char fut_attrib_superClass[102];
            char fut_attrib_value[192];
            fut_attrib_id[0] = 0;
            fut_attrib_superClass[0] = 0;
            fut_attrib_value[0] = 0;

            unsigned a_ok = 0;
            for (struct attr_node_s *ctl_attrs = ctrlList->attrs; ctl_attrs != NULL; ctl_attrs = ctl_attrs->next) {
                bool has_defaultValue = false;
                bool ctrl_attrib_ok = false;
                printf("checkAttrib-%s ", ctl_attrs->name);
                for (struct attr_node_s *fut_attrs = futList->attrs; fut_attrs != NULL; fut_attrs = fut_attrs->next) {
                    const char *control_value;
                    int len;
                    bool openValue;
                    if (strcmp(fut_attrs->name, "id") == 0)
                        strcpy(fut_attrib_id, fut_attrs->value);
                    else if (strcmp(fut_attrs->name, "superClass") == 0)
                        strcpy(fut_attrib_superClass, fut_attrs->value);
                    else if (strcmp(fut_attrs->name, "value") == 0)
                        strcpy(fut_attrib_value, fut_attrs->value);
                    else if (strcmp(fut_attrs->name, "defaultValue") == 0) {
                        has_defaultValue = true;
                        strcpy(fut_attrib_value, fut_attrs->value);
                    }

                    if (strcmp(ctl_attrs->name, fut_attrs->name) != 0)
                        continue;

                    openValue = false;
                    for (unsigned n = 0; openAttributes[n] != NULL; n++) {
                        if (strcmp(fut_attrs->name, openAttributes[n]) == 0) {
                            openValue = true;
                            break;
                        }
                    }

                    if (openValue) {
                        a_ok++;
                        ctrl_attrib_ok = true;
                        continue;
                    }

                    control_value = ctl_attrs->value;
                    len = strlen(ctl_attrs->value);

                    if (strcmp(ctl_attrs->name, "id") == 0) {
                        while (ctl_attrs->value[len] != '.' && len > 0)
                            len--;
                        if (id_is_cconfiguration)
                            control_value = fut_cconfiguration_id;
                    }

                    if (strcmp(ctrlList->name, "project") == 0) {
                        if (strcmp(ctl_attrs->name, "id") == 0 || strcmp(ctl_attrs->name, "name") == 0)
                            control_value = NULL;
                    } else if (strcmp(ctrlList->name, "folderInfo") == 0) {
                        if (strcmp(ctl_attrs->name, "id") == 0) {
                            control_value = fut_cconfiguration_id;
                            len = strlen(control_value);
                        }
                    } else if (strcmp(ctrlList->name, "autodiscovery") == 0) {
                        if (strcmp(ctl_attrs->name, "enabled") == 0) {
                            if (strcmp(fut_attrs->value, "true") == 0 || strcmp(fut_attrs->value, "false") == 0)
                                control_value = NULL;
                        } else if (strcmp(ctl_attrs->name, "problemReportingEnabled") == 0)
                            control_value = NULL;
                    } else if (strcmp(ctrlList->name, "scannerConfigBuildInfo") == 0) {
                        if (strcmp(ctl_attrs->name, "instanceId") == 0) {
                            control_value = NULL;
                            if (id_strcmp(ctl_attrs->value, fut_attrs->value) != 0) {
                                printf("\e[31minstanceId fail\e[0m ");
                                return -1;
                            } else
                                ctrl_attrib_ok = true;
                        }
                    } else if (strcmp(ctrlList->name, "toolChain") == 0) {
                        if (strcmp(ctl_attrs->name, "targetTool") == 0)
                        {
                            while (ctl_attrs->value[len] != '.' && len > 0)
                                len--;
                        }
                    } else if (strcmp(ctrlList->name, "entry") == 0 && strcmp(ctl_attrs->name, "flags") == 0) {
                        control_value = NULL;
                    } else if (strcmp(ctl_attrs->name, "name") == 0) {
                        if (id_is_cconfiguration) {
                            strcpy(fut_cconfiguration_name, fut_attrs->value);
                        }
                        control_value = NULL;   /* name values dont care */
                    } else if ((strcmp(ctl_attrs->name, "value") == 0 || strcmp(ctl_attrs->name, "defaultValue") == 0) && ctrl_attrib_valueType != NULL && strcmp(ctrl_attrib_valueType, "enumerated") == 0) {
                        control_value = NULL;   /* enumerated (default)value checked by containing superClass */
                    } else if (/*strcmp(ctrlList->name, "listOptionValue") == 0 &&*/ strcmp(ctl_attrs->name, "value") == 0) {
                        control_value = NULL;
                    } else if (strcmp(ctl_attrs->name, "IS_VALUE_EMPTY") == 0) {
                        if (strcmp(ctl_attrs->value, "true") == 0 || strcmp(ctl_attrs->value, "false") == 0)
                            control_value = NULL;
                    }

                    if (control_value != NULL) {
                        if (control_value != NULL && strncmp(control_value, fut_attrs->value, len) != 0) {
                            printf("\e[31m(%d)line %d %s:\nCONTROL %s\n    FUT %s\e[0m\n", cnt, __LINE__, ctl_attrs->name, control_value, fut_attrs->value);
                            printf("        ");
                            for (unsigned n = 0; n < len; n++) {
                                if (ctl_attrs->value[n] == fut_attrs->value[n])
                                    printf("-");
                                else
                                    printf("*");
                            }
                            printf("\n");
                            return -1;
                        } else {
                            a_ok++;
                            ctrl_attrib_ok = true;
                        }
                    } else {
                        ctrl_attrib_ok = true;
                    }
                } // ..fut attribute iterator

                if (!ctrl_attrib_ok && strcmp(ctl_attrs->name, "value") == 0 && has_defaultValue)
                    ctrl_attrib_ok = true;  // defaultValue is permitted to stand in place of value

                if (!ctrl_attrib_ok) {
                    printf("\e[31mcontrol-attribute-fail\e[0m %s=%s\n", ctl_attrs->name, ctl_attrs->value);
                    return -1;
                }
            } // ..control attribute iterator

            if (fut_attrib_superClass[0] != 0) {
                if (strncmp(fut_attrib_superClass, fut_attrib_id, strlen(fut_attrib_superClass)) != 0) {
                    printf("\e[31mline %d superClass not in id:\n", __LINE__);
                    printf("superClass \"%s\"\n", fut_attrib_superClass);
                    printf("id         \"%s\"\e[0m\n", fut_attrib_id);
                    return -1;
                }
            }

            if (ctrl_attrib_valueType != NULL) {
                /* control file can have valueType without value: in that case, not checking */
                /* TODO: also check defaultValue */
                if (ctrl_attrib_value != NULL && strcmp(ctrl_attrib_valueType, "enumerated") == 0) {
                    const char *valuePrefix = fut_attrib_superClass;

                    for (unsigned n = 0; enumeration_exceptions[n].superClass_prefix != NULL; n++) {
                        const char *sp = enumeration_exceptions[n].superClass_prefix;
                        if (strncmp(fut_attrib_superClass, sp, strlen(sp)) == 0)
                            valuePrefix = enumeration_exceptions[n].value_prefix;
                    }

                    if (strncmp(valuePrefix, fut_attrib_value, strlen(valuePrefix)) != 0) {
                        printf("\e[31mline %d superClass not in value:\n", __LINE__);
                        printf("startsWith \"%s\"\n", valuePrefix);
                        printf("value      \"%s\"\e[0m\n", fut_attrib_value);
                        return -1;
                    }
                }
            }

            if (a_ok > 0)
                printf("a_ok:%d ", a_ok);

            futList->checked = true;
            futChecked = true;
        } else
            failedFutNode = futList;
    } // ..fut node iterator

    if (futChecked) {
        /* if was validated in fut, then mark it off in control, later unvalidated control elements can be shown */
        ctrlList->checked = true;

    } else if (ctrlList->node_type == XML_NODE_TYPE_END_OF_ELEMENT) {
        printf("optional-endOfElement ");
    } else {
        bool optional = false;
        for (unsigned n = 0; optionalElements[n] != NULL; n++) {
            if (strcmp(ctrlList->name, optionalElements[n]) == 0) {
                optional = true;
                printf("optionalElement-%s ", optionalElements[n]);
                break;
            }
        }

        /*
        if (strcmp(ctrlList->name, "storageModule") == 0) {
            for (struct attr_node_s *ctl_attrs = ctrlList->attrs; ctl_attrs != NULL; ctl_attrs = ctl_attrs->next) {
                if (strcmp(ctl_attrs->name, "moduleId") == 0) {
                    if (strcmp(ctl_attrs->value, "com.nxp.mcuxpresso.core.datamodels") == 0 ||
                        strcmp(ctl_attrs->value, "com.crt.config") == 0)
                    {
                        ignore_until_end.depth = ctrlList->depth;
                        ignore_until_end.name = ctrlList->name;
                        optional = true;
                    }
                }
            }
        }
        */

        if (!optional) {
            for (struct attr_node_s *ctl_attrs = ctrlList->attrs; ctl_attrs != NULL; ctl_attrs = ctl_attrs->next) {
                if (strcmp(ctl_attrs->name, "superClass") == 0) {
                    for (unsigned n = 0; optionalSuperClasses[n] != NULL; n++) {
                        if (strcmp(ctl_attrs->value, optionalSuperClasses[n]) == 0) {
                            optional = true;
                        }
                    }
                    break;
                }
            }
        }

        if (!optional) {
            printf("\n%s \e[31mline %d not-futChecked\n", control_context_str, __LINE__);
            if (failedFutNode) {
                printf("control %d %s %s\n", ctrlList->depth, nodeTypeToString(ctrlList->node_type), ctrlList->name);
                printf("    fut %d %s %s\n", failedFutNode->depth, nodeTypeToString(failedFutNode->node_type), failedFutNode->name);
            }
            printf("\e[0m");
            ret = -1;
        }
    }

    return ret;
} // ..check_fut()


int compare(struct node_s *control, struct node_s *fut)
{
    int ret = 0;

    control_build[0] = 0;
    fut_cconfiguration_id[0] = 0;
    fut_cconfiguration_name[0] = 0;
    builds_idx = 0;
    for (unsigned n = 0; n < MAX_BUILDS; n++)
        builds[n][0] = 0;

    ignore_until_end.depth = -1;
    ignore_until_end.name = NULL;

    for (struct node_s *ctrlList = control; ctrlList != NULL; ctrlList = ctrlList->next) {
        printf("%d compare(D%d %s %s) ", ctrlList->serialNumber, ctrlList->depth, nodeTypeToString(ctrlList->node_type), ctrlList->name);
        if (ignore_until_end.depth != -1) {
            if (ctrlList->depth < ignore_until_end.depth) {
                ret = -1;
                break;
            } else if (ctrlList->depth == ignore_until_end.depth && ctrlList->node_type == XML_NODE_TYPE_END_OF_ELEMENT) {
                if (strcmp(ctrlList->name, ignore_until_end.name) == 0) {
                    ignore_until_end.depth = -1;
                    ignore_until_end.name = NULL;
                }
            }
            printf("\e[33mignore\e[0m\n");
            continue;
        }

        if (check_fut(ctrlList, fut) < 0) {
            ret = -1;
            break;
        }

        if (ctrlList->node_type == XML_NODE_TYPE_END_OF_ELEMENT) {
            if (strcmp(ctrlList->name, "cconfiguration") == 0) {
                control_build[0] = 0;
                fut_cconfiguration_id[0] = 0;
                fut_cconfiguration_name[0] = 0;
            }
        }

        printf("\e[0m\n");
    } // ..control node iterator

    printf("\e[0m\n");
    return ret;
}

int main(int argc, char *argv[])
{
    xmlTextReaderPtr xml_reader;
    int ret = 0;

    if (argc < 3) {
        printf("usage: %s <controlFile> <fileUnderTest>\n", argv[0]);
        return -1;
    }

    LIBXML_TEST_VERSION

    xml_reader = xmlReaderForFile(argv[1], NULL, 0);
    if (!xml_reader) {
        fprintf(stderr, "Unable to open %s\n", argv[1]);
        return -1;
    }

    ret = save_xml_file(xml_reader, &_control, "control", false);
    printf("#############################################################\n");
    if (ret < 0) {
        printf("%d = save_xml_file(control)\n", ret);
        return ret;
    }
    print_nodes(_control, "Control");

    xmlFreeTextReader(xml_reader);

    printf("#############################################################\n");

    xml_reader = xmlReaderForFile(argv[2], NULL, 0);
    if (!xml_reader) {
        fprintf(stderr, "Unable to open %s\n", argv[1]);
        return -1;
    }

    ret = save_xml_file(xml_reader, &_fut, "fut", true);
    if (ret < 0) {
        printf("%d = save_xml_file(fut)\n", ret);
        return ret;
    }
    printf("#############################################################\n");
    print_nodes(_fut, "Fut");

    printf("#############################################################\n");
    if (compare(_control, _fut) < 0)
        return -1;

    printf("###################### check optional... #######################################\n");
    if (check_fut_optional() < 0)
        return -1;

    printf("##################### unchecked... ############################\n");
    show_unchecked(_fut, true, "fut");
    show_unchecked(_control, false, "control");

    return ret;
}
