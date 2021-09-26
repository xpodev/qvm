#pragma once

#include <stdint.h>
#include <string>

#include "QPLFileType.h"


struct QPLFileHeader {
public:
	inline QPLFileHeader()
		: m_signature()
		, m_type()
		, m_numSections()
		, m_sectionTableOffset()
	{

	}

public:
	bool is_valid_header();

	QPLFileType flags();

	std::int32_t section_count();

	std::int32_t section_table_offset();

	static QPLFileHeader read_from(std::ifstream& stream);

	void load_from(std::ifstream& stream);

private:
	static inline constexpr char const QPLFileHeaderSignature[4] = "QPL";
	static inline constexpr size_t const ArchitectureBitOffset = 8;

	std::int8_t m_signature[4]; // "QPL" or 5001297 or 0x4c5051, little endian
	QPLFileType const m_type;
	std::int32_t m_numSections;
	std::int32_t m_sectionTableOffset;
};


struct QPLFileSectionTableEntry {
	static inline constexpr size_t const MaxSectionNameLength = 8;

	char const name[MaxSectionNameLength];
	int32_t length;
	int32_t offset;
};
