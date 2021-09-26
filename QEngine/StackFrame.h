#pragma once


#include <stack>
#include <vector>


class StackFrame {
public:
	inline StackFrame(size_t numArguments, size_t numLocals, size_t returnAddress)
		: m_arguments(numArguments)
		, m_locals(numLocals)
		, m_returnAddress(returnAddress)
	{

	}

public:
	template <typename T>
	inline T& argument(size_t index) {
		return *reinterpret_cast<T*>(&m_arguments[index]);
	}

	template <typename T>
	inline T& local(size_t index) {
		return *reinterpret_cast<T*>(&m_locals[index]);
	}

	template <typename T>
	inline void push(T&& value) {
		m_eval.push(static_cast<size_t>(value));
	}

	template <typename T>
	inline T pop() {
		T value = static_cast<T>(m_eval.top());
		m_eval.pop();
		return value;
	}

	inline void pop() {
		m_eval.pop();
	}

	inline size_t evaluation_stack_size() { return m_eval.size(); }

	inline size_t return_address() { return m_returnAddress; }

private:
	size_t m_returnAddress;

	std::vector<void*> m_arguments;

	std::vector<void*> m_locals;

	std::stack<size_t> m_eval;
};
