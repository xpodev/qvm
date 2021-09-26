#include "Member.h"

std::string const& Member::name()
{
	return m_name;
}

Type const& Member::declaring_type()
{
	return m_declaringType;
}
