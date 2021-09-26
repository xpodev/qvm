#pragma once

#include <string>

#include "Core.h"

class Member {
public:
	std::string const& name();

	Type const& declaring_type();

private:
	std::string m_name;

	Type& m_declaringType;
};
