#pragma once

#include <stdint.h>


enum class QPLFileFlags : uint8_t {
	EntryPoint = 0x1,
	ExportTable = 0x2
};


enum class Architecture : uint8_t {
	Unknown,
	X86_32,
	X86_64
};


struct QPLFileType
{
	QPLFileFlags m_flags;
	Architecture m_architecture;
	uint16_t RESERVED;
};

