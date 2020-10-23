
const char *nodeTypeToString(int nt);

typedef enum {
    XML_NODE_TYPE_NONE = 0,
    XML_NODE_TYPE_START_ELEMENT, // 1
    XML_NODE_TYPE_ATTRIBUTE, // 2
    XML_NODE_TYPE_TEXT, // 3
    XML_NODE_TYPE_CDATA, // 4
    XML_NODE_TYPE_ENTITY_REF, // 5
    XML_NODE_TYPE_ENTITY_DECLARE, // 6
    XML_NODE_TYPE_PI, // 7
    XML_NODE_TYPE_COMMENT, // 8
    XML_NODE_TYPE_DOC, // 9
    XML_NODE_TYPE_DOCTYPE, // 10
    XML_NODE_TYPE_DOC_FRAG, // 11
    XML_NODE_TYPE_NOTATION, // 12
    XML_NODE_TYPE_FOO, // 13
    XML_NODE_TYPE_WHITE_SPACE, // 14
    XML_NODE_TYPE_END_OF_ELEMENT, // 15
    XML_NODE_TYPE_, // 16
} xml_node_type_e;


extern const xmlChar * const storageModule;
extern const xmlChar * const autodiscovery;
extern const xmlChar * const enabled;
extern const xmlChar * const moduleId;
extern const xmlChar * const nature;
extern const xmlChar * const useByScannerDiscovery;
extern const xmlChar * const superClass;
extern const xmlChar * const IS_VALUE_EMPTY;
extern const xmlChar * const IS_BUILTIN_EMPTY;
extern const xmlChar * const option;
extern const xmlChar * const id;
extern const xmlChar * const valueType;
extern const xmlChar * const name;
extern const xmlChar * const value;
extern const xmlChar * const enumerated;

