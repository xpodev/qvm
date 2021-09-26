#pragma once


#include "EvaluationStack.h"
#include "InstructionStream.h"


class ExecutionContext
{
public:
	inline ExecutionContext(InstructionStream& code, void* base)
		: m_code{ code }
		, m_running{ false }
		, m_base{ reinterpret_cast<uint8_t*>(base) }
	{

	}

	inline ExecutionContext(InstructionStream&& code, uint8_t* base)
		: m_code{ code }
		, m_running{ false }
		, m_base{ base }
	{

	}

public:
	EvaluationStack& stack() { return m_stack; }

	InstructionStream& code() { return m_code; }

	inline bool is_running() { return m_running; }

	inline void signal_run() { m_running = true; }

	inline void signal_stop() { m_running = false; }

	inline uint8_t* base() { return m_base; }

private:
	bool m_running;

	uint8_t* m_base;

	EvaluationStack m_stack;
	InstructionStream& m_code;
};
