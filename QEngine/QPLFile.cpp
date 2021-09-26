#include "QPLFile.h"

#include <fstream>


QPLFile QPLFile::read_from(std::ifstream& stream)
{
	QPLFile file{};

	file.m_header.load_from(stream);

	if (!file.m_header.is_valid_header()) {
		throw new std::exception("Invalid file: ");
	}

	stream.seekg(file.m_header.section_table_offset());

	uint8_t* entryData = new uint8_t[file.m_header.section_count() * sizeof(QPLFileSectionTableEntry)];
	QPLFileSectionTableEntry* entries = reinterpret_cast<QPLFileSectionTableEntry*>(entryData);

	stream.read(reinterpret_cast<char*>(entries), sizeof(QPLFileSectionTableEntry) * static_cast<size_t>(file.m_header.section_count()));

	size_t total_size = 0;

	for (int i = 0; i < file.m_header.section_count(); i++)
	{
		total_size += entries[i].length;
	}

	file.m_fileData = new uint8_t[total_size];
	char* file_data_ptr = reinterpret_cast<char*>(file.m_fileData);

	for (int i = 0; i < file.m_header.section_count(); i++)
	{
		stream.seekg(entries[i].offset).read(file_data_ptr, entries[i].length);
		file.add_section(QPLFileSection(entries[i].name, entries[i].offset, reinterpret_cast<uint8_t*>(file_data_ptr), entries[i].length));
		file_data_ptr += entries[i].length;
	}

	delete[] entries;
	
	return file;
}

void QPLFile::add_section(QPLFileSection&& section)
{
	m_sections[section.name()] = section;
}

QPLFileSection const& QPLFile::get_section(char const* name) const
{
	return m_sections.at(name);
}

void* QPLFile::file_data() const
{
	return m_fileData;
}

void QPLFile::unload()
{
	delete[] m_fileData;
}
