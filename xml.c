#include <stdio.h>
#include "xml.h"

char otherNodeType[16];

const char *nodeTypeToString(int nt)
{
    switch (nt) {
        case XML_NODE_TYPE_START_ELEMENT: return "start-element";
        case XML_NODE_TYPE_END_OF_ELEMENT: return "end-of-element";
        case 2: return "attribute";
        case XML_NODE_TYPE_TEXT: return "text-node";
        case 4: return "CData-section";
        case 5: return "entity-reference";
        case 6: return "entity-declaration";
        case 7: return "PI";
        case XML_NODE_TYPE_COMMENT: return "comment";
        case 9: return "the document";
        case 10: return "DTD/Doctype";
        case 11: return "document-fragment";
        case 12: return "notation-node";
        case XML_NODE_TYPE_WHITE_SPACE: return "whiteSpace";
        default:
            sprintf(otherNodeType, "<%d>", nt);
            return otherNodeType;
    }
}
