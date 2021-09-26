#pragma once


#include "Member.h"


class Type : public Member {
private:
	Type const& m_baseType;

	Field* const m_fields;

	Method* const m_methods;

	Property* const m_properties;

	Type* const m_nestedTypes;
};
