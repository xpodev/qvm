#pragma once


#include <iostream>
#include <unordered_map>

#include "QPLFileHeader.h"
#include "QPLFileSection.h"


class QPLFile {
public:
	static QPLFile read_from(std::ifstream& stream);

	void add_section(QPLFileSection&& section);

	QPLFileSection const& get_section(char const* name) const;

	void* file_data() const;

	void unload();

private:
	QPLFileHeader m_header;
	std::unordered_map<std::string, QPLFileSection> m_sections;
	uint8_t* m_fileData;
};
