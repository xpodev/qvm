#pragma once

#include <memory>
#include <stdint.h>
#include <string>


class QPLFileSection {
public:
	inline QPLFileSection(char const* name, size_t offset, uint8_t* data, size_t length)
		: m_name{ name }
		, m_offset{ offset }
		, m_data{ data }
		, m_length{ length }
	{

	}

	inline QPLFileSection()
		: m_name()
		, m_offset()
		, m_data()
		, m_length()
	{

	}

public:
	char const* name() const;

	size_t offset() const;

	uint8_t* data() const;

	size_t length() const;

private:
	std::string m_name;
	size_t m_offset;
	uint8_t* m_data;
	size_t m_length;
};
