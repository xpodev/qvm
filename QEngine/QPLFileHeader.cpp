#include <fstream>

#include "QPLFileHeader.h"


bool QPLFileHeader::is_valid_header() {
	return
		*reinterpret_cast<std::int32_t*>(m_signature) == *reinterpret_cast<std::int32_t const*>(QPLFileHeaderSignature);
}

QPLFileType QPLFileHeader::flags()
{
	return m_type;
}

std::int32_t QPLFileHeader::section_count()
{
	return m_numSections;
}

std::int32_t QPLFileHeader::section_table_offset()
{
	return m_sectionTableOffset;
}

QPLFileHeader QPLFileHeader::read_from(std::ifstream& stream)
{
	QPLFileHeader header{};

	header.load_from(stream);

	return header;
}

void QPLFileHeader::load_from(std::ifstream& stream)
{
	stream.read(reinterpret_cast<char*>(this), sizeof(QPLFileHeader));
}
