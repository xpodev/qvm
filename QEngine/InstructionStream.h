#pragma once

#include <concepts>
#include <stdint.h>

#include "QPLFileSection.h"


class InstructionStream
{
public:
	inline InstructionStream(QPLFileSection& code, void* fileData)
		: m_code{ code }
		, m_streamIndex{ 0 }
		, m_fileData{ reinterpret_cast<uint8_t*>(fileData) }
	{

	}

public:
	template <typename T>
	requires (std::is_standard_layout_v<T>)
		inline T const extract(bool advance = true) {
		T result = *reinterpret_cast<T const*>(m_fileData + m_streamIndex);
		if (advance) {
			m_streamIndex += sizeof(T);
		}
		return result;
	}

	template <typename T>
	requires (std::is_enum_v<T>)
		inline T op_code(bool advance = true) {
		return extract<T>(advance);
	}

	inline void advance(size_t amount) { m_streamIndex += amount; }

	inline size_t ip() { return m_streamIndex; }

	inline void ip(size_t nip) { m_streamIndex = nip; }

private:
	size_t m_streamIndex;

	uint8_t* m_fileData;

	QPLFileSection& m_code;
};
