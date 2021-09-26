#include "QPLFileSection.h"

char const* QPLFileSection::name() const
{
	return m_name.c_str();
}

size_t QPLFileSection::offset() const
{
	return m_offset;
}

uint8_t* QPLFileSection::data() const
{
	return m_data;
}

size_t QPLFileSection::length() const
{
	return m_length;
}
