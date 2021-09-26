#pragma once

#include <stack>


#include "StackFrame.h"


class EvaluationStack {
public:
	inline StackFrame& current() {
		return m_stack.top();
	}

	inline void push(StackFrame&& frame) {
		m_stack.push(frame);
	}

	inline bool pop() {
		if (m_stack.top().evaluation_stack_size() != 0) return false;
		m_stack.pop();
		return true;
	}

	inline size_t size() const {
		return m_stack.size();
	}

private:
	std::stack<StackFrame> m_stack;
};
