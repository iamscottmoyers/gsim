#ifndef _NAVVIE_UML_QUALIFIER_H_
#define _NAVVIE_UML_QUALIFIER_H_

enum UMLQualifier {
	NV_LEAF          = (1 << 0),
	NV_STATIC        = (1 << 1),
	NV_ABSTRACT      = (1 << 2),
	NV_QUERY         = (1 << 3),
	NV_UNIQUE        = (1 << 4),
	NV_READONLY      = (1 << 5),
	NV_ORDERED       = (1 << 6),
	NV_DERIVEDUNION  = (1 << 7),
	NV_DERIVED       = (1 << 8)
};

#endif
