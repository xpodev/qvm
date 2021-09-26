#include <iostream>
#include <fstream>
#include <functional>

#include "ExecutionContext.h"
#include "QPLFile.h"


enum class TypeIndex : uint8_t {
	Unknown,
	Void,
	Bool,
	Pointer,
	RelativePointer,
	NativeInt,
	Int8,
	Int16,
	Int32,
	Int64,
	NativeFloat,
	Float32,
	Float64,
	String,
	Raw,
	Local,
	Argument
};


enum class OpCodes : uint8_t {
	Nop,
	DLog,
	Push,
	Pop,
	Call,
	UnsafeCall,
	Return,
	Jump,
	JumpIfTrue,
	JumpIfFalse,
	CompareGreater,
	CompareLower,
	CompareGreaterOrEqual,
	CompareLowerOrEqual,
	CompareEqual,
	CompareNotEqual,
	Add,
	Subtract,
	Multiply,
	Divide,
	Remainder,
	PushFromMemory,
	PopToMemory,
	New,
	Free,
	Exit = -1
};


enum class Comparison {
	LowerThan = 1,
	GreaterThan = 2,
	EqualTo = 4
};


constexpr Comparison operator|(Comparison a, Comparison b) {
	return static_cast<Comparison>(static_cast<size_t>(a) | static_cast<size_t>(b));
}

constexpr size_t operator&(Comparison a, Comparison b) {
	return (static_cast<size_t>(a) & static_cast<size_t>(b));
}


using bool_t = bool;

using float32_t = float;
using float64_t = double;

using ptr_t = void*;

using string_t = char const*;
using bytes_t = char*;


#ifdef WIN32
using ssize_t = int32_t;
using native_float = float32_t;
#else
using ssize_t = int64_t;
using native_float = float64_t;
#endif // WIN32


static size_t ip;


namespace qpl {
	struct RawData
	{
		inline constexpr RawData() noexcept { clear(); }

		inline constexpr RawData(bool v) noexcept : RawData() { m_data.m_bool = v; }
		inline constexpr RawData(int8_t v) noexcept : RawData() { m_data.m_i8 = v; }
		inline constexpr RawData(uint8_t v) noexcept : RawData() { m_data.m_u8 = v; }
		inline constexpr RawData(int16_t v) noexcept : RawData() { m_data.m_i16 = v; }
		inline constexpr RawData(uint16_t v) noexcept : RawData() { m_data.m_u16 = v; }
		inline constexpr RawData(int32_t v) noexcept : RawData() { m_data.m_i32 = v; }
		inline constexpr RawData(uint32_t v) noexcept : RawData() { m_data.m_u32 = v; }
		inline constexpr RawData(int64_t v) noexcept : RawData() { m_data.m_i64 = v; }
		inline constexpr RawData(uint64_t v) noexcept : RawData() { m_data.m_u64 = v; }
		inline constexpr RawData(float32_t v) noexcept : RawData() { m_data.m_f32 = v; }
		inline constexpr RawData(float64_t v) noexcept : RawData() { m_data.m_f64 = v; }

		template <typename T>
		inline constexpr RawData(T* v) noexcept : RawData() { m_data.m_ptr = v; }

		union {
			bool m_bool;

			int8_t m_i8;
			uint8_t m_u8;
			int16_t m_i16;
			uint16_t m_u16;
			int32_t m_i32;
			uint32_t m_u32;
			int64_t m_i64;
			uint64_t m_u64;

			size_t m_size_t;

			ptr_t m_ptr;

			float32_t m_f32;
			float64_t m_f64;

			template <typename T>
			constexpr inline T* ptr() const { return reinterpret_cast<T*>(m_ptr); }
		} m_data;

		inline constexpr void clear() { m_data.m_i64 = { 0 }; }

		template <typename T>
		constexpr inline explicit operator T() {
			return *reinterpret_cast<T*>(this);
		}

		constexpr inline explicit operator bool() { return m_data.m_bool; }

		constexpr inline explicit operator int8_t() { return m_data.m_i8; }

		constexpr inline explicit operator uint8_t() { return m_data.m_u8; }

		constexpr inline explicit operator int16_t() { return m_data.m_i16; }

		constexpr inline explicit operator uint16_t() { return m_data.m_u16; }

		constexpr inline explicit operator int32_t() { return m_data.m_i32; }

		constexpr inline explicit operator uint32_t() { return m_data.m_u32; }

		constexpr inline explicit operator int64_t() { return m_data.m_i64; }

		constexpr inline explicit operator uint64_t() { return m_data.m_u64; }

		constexpr inline explicit operator float() { return m_data.m_f32; }

		constexpr inline explicit operator double() { return m_data.m_f64; }

		template <typename T>
		constexpr inline explicit operator T* () { return m_data.ptr<T>(); }
	};


	struct Data : public RawData
	{
		template <typename T>
		inline constexpr Data(TypeIndex type, T v)
			: RawData(v)
			, m_type(type) {

		}

		inline constexpr Data(bool v) noexcept : Data(TypeIndex::Bool, v) { }
		inline constexpr Data(int8_t v) noexcept : Data(TypeIndex::Int8, v) { }
		inline constexpr Data(uint8_t v) noexcept : Data(TypeIndex::Int8, v) { }
		inline constexpr Data(int16_t v) noexcept : Data(TypeIndex::Int16, v) { }
		inline constexpr Data(uint16_t v) noexcept : Data(TypeIndex::Int16, v) { }
		inline constexpr Data(int32_t v) noexcept : Data(TypeIndex::Int32, v) { }
		inline constexpr Data(uint32_t v) noexcept : Data(TypeIndex::Int32, v) { }
		inline constexpr Data(int64_t v) noexcept : Data(TypeIndex::Int64, v) { }
		inline constexpr Data(uint64_t v) noexcept : Data(TypeIndex::Int64, v) { }
		inline constexpr Data(float32_t v) noexcept : Data(TypeIndex::Float32, v) { }
		inline constexpr Data(float64_t v) noexcept : Data(TypeIndex::Float64, v) { }

		template <typename T>
		inline constexpr Data(T* v) noexcept : Data(TypeIndex::Pointer, v) { }

		inline constexpr Data(TypeIndex type, RawData const& data)
			: RawData(data)
			, m_type(type)
		{

		}

		inline constexpr Data(TypeIndex type, RawData&& data)
			: RawData(data)
			, m_type(type)
		{

		}


		inline constexpr TypeIndex type() const { return m_type; }

#if 0
		inline std::ostream& operator<<(std::ostream& s) {
			switch (m_type)
			{
			case TypeIndex::Void:
				return s << "void";
			case TypeIndex::Bool:
				return s << m_data.m_bool;
			case TypeIndex::Pointer:
				return s << m_data.m_ptr;
			case TypeIndex::NativeInt:
				return s << size_t(m_data.m_ptr);
			case TypeIndex::Int8:
				return s << m_data.m_i8;
			case TypeIndex::Int16:
				return s << m_data.m_i16;
			case TypeIndex::Int32:
				return s << m_data.m_i32;
			case TypeIndex::Int64:
				return s << m_data.m_i64;
			case TypeIndex::NativeFloat:
				return s << "Can't print native float";
			case TypeIndex::Float32:
				return s << m_data.m_f32;
			case TypeIndex::Float64:
				return s << m_data.m_f64;
			case TypeIndex::String:
			case TypeIndex::Raw:
				return s << m_data.ptr<char const>();
			case TypeIndex::Unknown:
			default:
				return s << "Unknown Type: " << static_cast<uint8_t>(m_type);
			}
		}
#endif

		template <typename T>
		inline constexpr T safe_cast() {
			switch (m_type)
			{
			case TypeIndex::Unknown:
				break;
			case TypeIndex::Void:
				break;
			case TypeIndex::Bool:
				return (T)(m_data.m_bool);
			case TypeIndex::Pointer:
				return (T)size_t(m_data.m_ptr);
			case TypeIndex::NativeInt:
				return (T)size_t(m_data.m_ptr);
			case TypeIndex::Int8:
				return (T)(m_data.m_i8);
			case TypeIndex::Int16:
				return (T)(m_data.m_i16);
			case TypeIndex::Int32:
				return (T)(m_data.m_i32);
			case TypeIndex::Int64:
				return (T)(m_data.m_i64);
			case TypeIndex::NativeFloat:
				break;
			case TypeIndex::Float32:
				return (T)(m_data.m_f32);
			case TypeIndex::Float64:
				return (T)(m_data.m_f64);
			case TypeIndex::String:
			case TypeIndex::Raw:
				return (T)size_t(m_data.ptr<char>());
			case TypeIndex::Local:
			case TypeIndex::Argument:
			default:
				break;
			}
		}

		friend std::ostream& operator<<(std::ostream& s, Data const& data) {
			switch (data.m_type)
			{
			case TypeIndex::Void:
				return s << "void";
			case TypeIndex::Bool:
				return s << data.m_data.m_bool;
			case TypeIndex::RelativePointer:
			case TypeIndex::Pointer:
				return s << data.m_data.m_ptr;
			case TypeIndex::NativeInt:
				return s << size_t(data.m_data.m_ptr);
			case TypeIndex::Int8:
				return s << data.m_data.m_i8;
			case TypeIndex::Int16:
				return s << data.m_data.m_i16;
			case TypeIndex::Int32:
				return s << data.m_data.m_i32;
			case TypeIndex::Int64:
				return s << data.m_data.m_i64;
			case TypeIndex::NativeFloat:
				return s << "Can't print native float";
			case TypeIndex::Float32:
				return s << data.m_data.m_f32;
			case TypeIndex::Float64:
				return s << data.m_data.m_f64;
			case TypeIndex::String:
			case TypeIndex::Raw:
				return s << data.m_data.ptr<char const>();
			case TypeIndex::Unknown:
			default:
				return s << "Unknown Type: " << static_cast<uint8_t>(data.m_type);
			}
		}

	private:
		TypeIndex m_type;
	};


	struct IDataHandler
	{
		virtual bool lower_than(Data& a, Data& b) { return false; }

		virtual bool greater_than(Data& a, Data& b) { return false; }

		virtual bool equal_to(Data& a, Data& b) { return false; }

		virtual bool add(ExecutionContext& ctx, Data& a, Data& b) { return false; }

		virtual bool subtract(ExecutionContext& ctx, Data& a, Data& b) { return false; }

		virtual bool multiply(ExecutionContext& ctx, Data& a, Data& b) { return false; }

		virtual bool divide(ExecutionContext& ctx, Data& a, Data& b) { return false; }

		virtual bool remainder(ExecutionContext& ctx, Data& a, Data& b) { return false; }

		virtual bool push_from_memory(ExecutionContext& ctx, bool relative) { return false; }

		virtual bool pop_to_memory(ExecutionContext& ctx, bool relative) { return false; }

		virtual bool push_local(ExecutionContext& ctx, size_t index) { return false; }

		virtual bool pop_local(ExecutionContext& ctx, size_t index) { return false; }

		virtual bool push_argument(ExecutionContext& ctx, size_t index) { return false; }

		virtual bool pop_argument(ExecutionContext& ctx, size_t index) { return false; }
	};

	template <TypeIndex TIndex, typename T>
	struct DataHandlerBase : IDataHandler {
		static inline constexpr TypeIndex const TypeIndex = TIndex;

		bool push(ExecutionContext& ctx, T&& v) {
			ctx.stack().current().push(v);
			return true;
		}

		bool lower_than(Data& a, Data& b) override {
			return a.safe_cast<T>() < b.safe_cast<T>();
		}

		bool greater_than(Data& a, Data& b) override {
			return a.safe_cast<T>() > b.safe_cast<T>();
		}

		bool equal_to(Data& a, Data& b) override {
			return a.safe_cast<T>() == b.safe_cast<T>();
		}

		bool add(ExecutionContext& ctx, Data& a, Data& b) override {
			return push(ctx, a.safe_cast<T>() + b.safe_cast<T>());
		}

		bool subtract(ExecutionContext& ctx, Data& a, Data& b) override {
			return push(ctx, a.safe_cast<T>() - b.safe_cast<T>());
		}

		bool multiply(ExecutionContext& ctx, Data& a, Data& b) override {
			return push(ctx, a.safe_cast<T>() * b.safe_cast<T>());
		}

		bool divide(ExecutionContext& ctx, Data& a, Data& b) override {
			return push(ctx, a.safe_cast<T>() / b.safe_cast<T>());
		}

		bool remainder(ExecutionContext& ctx, Data& a, Data& b) override {
			return push(ctx, std::fmod(a.safe_cast<T>(), b.safe_cast<T>()));
		}

		bool push_from_memory(ExecutionContext& ctx, bool relative) override {
			T* ptr = reinterpret_cast<T*>(ctx.stack().current().pop<size_t>() + (relative ? ctx.base() : 0));
			return push(ctx, std::move(*ptr));
		}

		bool pop_to_memory(ExecutionContext& ctx, bool relative) override {
			T value = ctx.stack().current().pop<T>();
			T* ptr = reinterpret_cast<T*>(ctx.stack().current().pop<size_t>() + (relative ? ctx.base() : 0));
			*ptr = value;
			return true;
		}

		bool push_local(ExecutionContext& ctx, size_t index) override {
			return push(ctx, std::move(ctx.stack().current().local<T>(index)));
		}

		bool pop_local(ExecutionContext& ctx, size_t index) override {
			ctx.stack().current().local<T>(index) = ctx.stack().current().pop<T>();
			return true;
		}

		bool push_argument(ExecutionContext& ctx, size_t index) override {
			return push(ctx, std::move(ctx.stack().current().argument<T>(index)));
		}

		bool pop_argument(ExecutionContext& ctx, size_t index) override {
			ctx.stack().current().argument<T>(index) = ctx.stack().current().pop<T>();
			return true;
		}
	};

	template <TypeIndex TIndex, typename T>
	struct DataHandler : public DataHandlerBase<TIndex, T> {

	};

	template <typename T>
	using DataHandlerFunction = T(*)(qpl::Data...);

	std::unordered_map<TypeIndex, IDataHandler*> handlers;

	template <TypeIndex I, typename T>
	void add_handler() {
		handlers[I] = new DataHandler<I, T>();
	}

	void setup_handlers() {
		add_handler<TypeIndex::Bool, bool_t>();
		add_handler<TypeIndex::NativeInt, size_t>();
		add_handler<TypeIndex::Int8, int8_t>();
		add_handler<TypeIndex::Int16, int16_t>();
		add_handler<TypeIndex::Int32, int32_t>();
		add_handler<TypeIndex::Int64, int64_t>();
		add_handler<TypeIndex::NativeFloat, native_float>();
		add_handler<TypeIndex::Float32, float32_t>();
		add_handler<TypeIndex::Float64, float64_t>();
		add_handler<TypeIndex::String, size_t>();
		add_handler<TypeIndex::Raw, size_t>();
		add_handler<TypeIndex::Pointer, size_t>();
		add_handler<TypeIndex::RelativePointer, size_t>();
	}

	void free_handlers() {
		for (auto& item : handlers)
		{
			delete item.second;
		}
	}


	RawData extract(ExecutionContext& ctx, TypeIndex index) {
		switch (index)
		{
		case TypeIndex::Bool:
			return RawData(ctx.code().extract<bool>());
		case TypeIndex::String:
		case TypeIndex::Raw:
		case TypeIndex::Pointer:
			return RawData(ctx.code().extract<size_t>());
		case TypeIndex::RelativePointer:
		case TypeIndex::NativeInt:
			return RawData(ctx.code().extract<size_t>());
		case TypeIndex::Int8:
			return RawData(ctx.code().extract<int8_t>());
		case TypeIndex::Int16:
			return RawData(ctx.code().extract<int16_t>());
		case TypeIndex::Int32:
			return RawData(ctx.code().extract<int32_t>());
		case TypeIndex::Int64:
			return RawData(ctx.code().extract<int64_t>());
		case TypeIndex::NativeFloat:
			return RawData(ctx.code().extract<native_float>());
		case TypeIndex::Float32:
			return RawData(ctx.code().extract<float32_t>());
		case TypeIndex::Float64:
			return RawData(ctx.code().extract<float64_t>());
		case TypeIndex::Unknown:
		case TypeIndex::Void:
		default:
			return RawData();
		}
	}


	Data extract(ExecutionContext& ctx) {
		TypeIndex index = ctx.code().extract<TypeIndex>();
		return Data(index, extract(ctx, index));
	}
}


void push(ExecutionContext& ctx, qpl::RawData data, TypeIndex type) {
	StackFrame& frame = ctx.stack().current();
	switch (type)
	{
	case TypeIndex::Unknown:
		break;
	case TypeIndex::Void:
		break;
	case TypeIndex::Bool:
		frame.push(static_cast<bool_t>(data));
		break;
	case TypeIndex::RelativePointer:
		frame.push(static_cast<size_t>(data) + ip);
		break;
	case TypeIndex::Pointer:
	case TypeIndex::NativeInt:
		frame.push(static_cast<size_t>(data));
		break;
	case TypeIndex::Int8:
		frame.push(static_cast<int8_t>(data));
		break;
	case TypeIndex::Int16:
		frame.push(static_cast<int16_t>(data));
		break;
	case TypeIndex::Int32:
		frame.push(static_cast<int32_t>(data));
		break;
	case TypeIndex::Int64:
		frame.push(static_cast<int64_t>(data));
		break;
	case TypeIndex::NativeFloat:
		frame.push(static_cast<native_float>(data));
		break;
	case TypeIndex::Float32:
		frame.push(static_cast<float32_t>(data));
		break;
	case TypeIndex::Float64:
		frame.push(static_cast<float64_t>(data));
		break;
	case TypeIndex::String:
		frame.push(static_cast<size_t>(data) + ip);
		break;
	case TypeIndex::Raw:
		frame.push(static_cast<size_t>(data));
		break;
	case TypeIndex::Local:
		break;
	case TypeIndex::Argument:
		break;
	default:
		break;
	}
}


size_t extract_ptr(ExecutionContext& ctx) {
	return ctx.code().extract<size_t>() + size_t(ctx.base());
}


qpl::Data pop(ExecutionContext& ctx, TypeIndex type) {
	size_t value = ctx.stack().current().pop<size_t>();
	if (
		type == TypeIndex::Pointer ||
		type == TypeIndex::String
		) {
		value += size_t(ctx.base());
	}
	return qpl::Data(type, value);
}


void print(ExecutionContext& ctx) {
	TypeIndex type = ctx.code().extract<TypeIndex>();
	qpl::Data data = pop(ctx, type);
	std::cout << data << std::endl;
}


void push(ExecutionContext& ctx) {
	TypeIndex type = ctx.code().extract<TypeIndex>();
	switch (type)
	{
	case TypeIndex::Local:
		type = ctx.code().extract<TypeIndex>();
		qpl::handlers.at(type)->push_local(ctx, ctx.code().extract<uint8_t>());
		break;
	case TypeIndex::Argument:
		type = ctx.code().extract<TypeIndex>();
		qpl::handlers.at(type)->push_argument(ctx, ctx.code().extract<uint8_t>());
		break;
	default:
		push(ctx, qpl::extract(ctx, type), type);
		break;
	}
}


void pop(ExecutionContext& ctx) {
	TypeIndex type = ctx.code().extract<TypeIndex>();
	switch (type)
	{
	case TypeIndex::Local:
		type = ctx.code().extract<TypeIndex>();
		qpl::handlers.at(type)->pop_local(ctx, ctx.code().extract<uint8_t>());
		break;
	case TypeIndex::Argument:
		type = ctx.code().extract<TypeIndex>();
		qpl::handlers.at(type)->pop_argument(ctx, ctx.code().extract<uint8_t>());
		break;
	default:
		pop(ctx, type);
		break;
	}
}


void call(ExecutionContext& ctx) {
	size_t address = ctx.code().extract<size_t>() + ip;
	uint8_t num_params = ctx.code().extract<uint8_t>();
	uint8_t num_locals = ctx.code().extract<uint8_t>();
	StackFrame& current = ctx.stack().current();
	ctx.stack().push(StackFrame(num_params, num_locals, ctx.code().ip()));
	for (int i = 0; i < num_params; ++i) {
		ctx.stack().current().argument<size_t>(i) = current.pop<size_t>();
	}
	ctx.code().ip(address);
}


void ret(ExecutionContext& ctx) {
	size_t return_value = ctx.stack().current().pop<size_t>();
	ctx.code().ip(ctx.stack().current().return_address());
	if (!ctx.stack().pop()) {
		std::cout << "StackFrame is not empty and couldn't be popped " << ctx.stack().current().return_address() << std::endl;
	}
	ctx.stack().current().push(return_value);
}


void jmp(ExecutionContext& ctx) {
	size_t address = ctx.code().extract<size_t>() + ip;
	ctx.code().ip(address);
}


template <bool V>
void jmp_if(ExecutionContext& ctx) {
	size_t address = ctx.code().extract<size_t>() + ip;
	if (ctx.stack().current().pop<bool_t>() == V) {
		ctx.code().ip(address);
	}
}


template <Comparison C>
void compare(ExecutionContext& ctx, qpl::Data& da, qpl::Data& db) {
	bool result = false;
	if (C & Comparison::GreaterThan) {
		result |= qpl::handlers.at(da.type())->greater_than(da, db);
	}
	if (C & Comparison::LowerThan) {
		result |= qpl::handlers.at(da.type())->lower_than(da, db);
	}
	if (C & Comparison::EqualTo) {
		result |= qpl::handlers.at(da.type())->equal_to(da, db);
	}
	ctx.stack().current().push(result);
}


template <Comparison C>
void compare(ExecutionContext& ctx) {
	qpl::Data data_a = pop(ctx, ctx.code().extract<TypeIndex>());
	qpl::Data data_b = pop(ctx, ctx.code().extract<TypeIndex>());
	compare<C>(ctx, data_a, data_b);
}


void add(ExecutionContext& ctx) {
	TypeIndex type_a = ctx.code().extract<TypeIndex>();
	TypeIndex type_b = ctx.code().extract<TypeIndex>();
	qpl::Data data_a = pop(ctx, type_a);
	qpl::Data data_b = pop(ctx, type_b);
	if (qpl::handlers.at(type_a)->add(ctx, data_a, data_b)) return;
	if (qpl::handlers.at(type_b)->add(ctx, data_a, data_b)) return;
	std::cout << "Could not add types: " << size_t(type_a) << ", " << size_t(type_b) << " (" << data_a << ", " << data_b << ")" << std::endl;
}


void sub(ExecutionContext& ctx) {
	TypeIndex type_a = ctx.code().extract<TypeIndex>();
	TypeIndex type_b = ctx.code().extract<TypeIndex>();
	qpl::Data data_a = pop(ctx, type_a);
	qpl::Data data_b = pop(ctx, type_b);
	if (qpl::handlers.at(type_a)->subtract(ctx, data_a, data_b)) return;
	if (qpl::handlers.at(type_b)->subtract(ctx, data_a, data_b)) return;
	std::cout << "Could not subtract types: " << size_t(type_a) << ", " << size_t(type_b) << " (" << data_a << ", " << data_b << ")" << std::endl;
}


void mult(ExecutionContext& ctx) {
	TypeIndex type_a = ctx.code().extract<TypeIndex>();
	TypeIndex type_b = ctx.code().extract<TypeIndex>();
	qpl::Data data_a = pop(ctx, type_a);
	qpl::Data data_b = pop(ctx, type_b);
	if (qpl::handlers.at(type_a)->multiply(ctx, data_a, data_b)) return;
	if (qpl::handlers.at(type_b)->multiply(ctx, data_a, data_b)) return;
	std::cout << "Could not multiply types: " << size_t(type_a) << ", " << size_t(type_b) << " (" << data_a << ", " << data_b << ")" << std::endl;
}


void div(ExecutionContext& ctx) {
	TypeIndex type_a = ctx.code().extract<TypeIndex>();
	TypeIndex type_b = ctx.code().extract<TypeIndex>();
	qpl::Data data_a = pop(ctx, type_a);
	qpl::Data data_b = pop(ctx, type_b);
	if (qpl::handlers.at(type_a)->divide(ctx, data_a, data_b)) return;
	if (qpl::handlers.at(type_b)->divide(ctx, data_a, data_b)) return;
	std::cout << "Could not divide types: " << size_t(type_a) << ", " << size_t(type_b) << " (" << data_a << ", " << data_b << ")" << std::endl;
}


void mod(ExecutionContext& ctx) {
	TypeIndex type_a = ctx.code().extract<TypeIndex>();
	TypeIndex type_b = ctx.code().extract<TypeIndex>();
	qpl::Data data_a = pop(ctx, type_a);
	qpl::Data data_b = pop(ctx, type_b);
	if (qpl::handlers.at(type_a)->remainder(ctx, data_a, data_b)) return;
	if (qpl::handlers.at(type_b)->remainder(ctx, data_a, data_b)) return;
	std::cout << "Could not get remainder of types: " << size_t(type_a) << ", " << size_t(type_b) << " (" << data_a << ", " << data_b << ")" << std::endl;
}


void push_mem(ExecutionContext& ctx) {
	TypeIndex type = ctx.code().extract<TypeIndex>();
	TypeIndex ptrType = ctx.code().extract<TypeIndex>();
	bool relative = ptrType != TypeIndex::Pointer;
	if (relative && ptrType != TypeIndex::RelativePointer) {
		std::cout << "Interpreting type " << static_cast<size_t>(ptrType) << " as relative address" << std::endl;
	}
	qpl::handlers.at(type)->push_from_memory(ctx, relative);
}


void pop_mem(ExecutionContext& ctx) {
	TypeIndex type = ctx.code().extract<TypeIndex>();
	TypeIndex ptrType = ctx.code().extract<TypeIndex>();
	bool relative = ptrType != TypeIndex::Pointer;
	if (relative && ptrType != TypeIndex::RelativePointer) {
		std::cout << "Interpreting type " << static_cast<size_t>(ptrType) << " as relative address" << std::endl;
	}
	qpl::handlers.at(type)->pop_to_memory(ctx, relative);
}


void mem_alloc(ExecutionContext& ctx) {
	size_t type_size = ctx.code().extract<size_t>();
	size_t length = ctx.code().extract<size_t>();
	ptr_t ptr = (length != 0 && type_size != 0) ? malloc(length * type_size) : nullptr;
	ctx.stack().current().push(size_t(ptr));
}


void mem_free(ExecutionContext& ctx) {
	size_t ptr = ctx.stack().current().pop<size_t>();
	free(ptr_t(ptr));
	ptr = 0;
}


void exit(ExecutionContext& ctx) {
	ctx.signal_stop();
}


void run(ExecutionContext& ctx) {
	ctx.signal_run();
	while (ctx.is_running()) {
		ip = ctx.code().ip();
		OpCodes opCode = ctx.code().op_code<OpCodes>();
		switch (opCode)
		{
		case OpCodes::Nop:
			break;
		case OpCodes::DLog:
			print(ctx);
			break;
		case OpCodes::Push:
			push(ctx);
			break;
		case OpCodes::Pop:
			pop(ctx);
			break;
		case OpCodes::Call:
			call(ctx);
			break;
		case OpCodes::UnsafeCall:
			break;
		case OpCodes::Return:
			ret(ctx);
			break;
		case OpCodes::Jump:
			jmp(ctx);
			break;
		case OpCodes::JumpIfTrue:
			jmp_if<true>(ctx);
			break;
		case OpCodes::JumpIfFalse:
			jmp_if<false>(ctx);
			break;
		case OpCodes::CompareGreater:
			compare<Comparison::GreaterThan>(ctx);
			break;
		case OpCodes::CompareGreaterOrEqual:
			compare<Comparison::GreaterThan | Comparison::EqualTo>(ctx);
			break;
		case OpCodes::CompareLower:
			compare<Comparison::LowerThan>(ctx);
			break;
		case OpCodes::CompareLowerOrEqual:
			compare<Comparison::LowerThan | Comparison::EqualTo>(ctx);
			break;
		case OpCodes::CompareEqual:
			compare<Comparison::EqualTo>(ctx);
			break;
		case OpCodes::CompareNotEqual:
			compare<Comparison::GreaterThan | Comparison::LowerThan>(ctx);
			break;
		case OpCodes::Add:
			add(ctx);
			break;
		case OpCodes::Subtract:
			sub(ctx);
			break;
		case OpCodes::Multiply:
			mult(ctx);
			break;
		case OpCodes::Divide:
			div(ctx);
			break;
		case OpCodes::Remainder:
			mod(ctx);
			break;
		case OpCodes::PushFromMemory:
			push_mem(ctx);
			break;
		case OpCodes::PopToMemory:
			pop_mem(ctx);
			break;
		case OpCodes::New:
			mem_alloc(ctx);
			break;
		case OpCodes::Free:
			mem_free(ctx);
			break;
		case OpCodes::Exit:
			exit(ctx);
		default:
			break;
		}
	}
}


struct EntryPointInformation {
	size_t offset;
};


int main(int argc, char const** argv) {
	char const* path;

	if (argc > 1) {
		path = argv[1];
	}
	else {
		path = "C:\\Users\\binya\\Desktop\\QASM\\tests\\hello_world.qpl";
	}

	std::ifstream file_stream;
	file_stream.open(path, file_stream.binary);

	if (!file_stream.is_open()) {
		std::cout << "Could not open file \"" << path << "\"" << std::endl;
		return 15;
	}

	qpl::setup_handlers();

	QPLFile file = QPLFile::read_from(file_stream);

	QPLFileSection config = file.get_section("config");

	EntryPointInformation entry_point = *reinterpret_cast<EntryPointInformation*>(config.data());

	QPLFileSection code_section = file.get_section("code");

	InstructionStream code(code_section, file.file_data());

	ExecutionContext ctx(code, file.file_data());

	ctx.stack().push(StackFrame(0, 0, 0));

	code.ip(entry_point.offset);

	run(ctx);

	ssize_t status_code = -10;

	if (ctx.stack().size() != 1) {
		std::cout << "Program exited abnormally. Some call frames are still present (" << ctx.stack().size() << " frames)" << std::endl;
	}
	else {
		status_code = ctx.stack().current().pop<ssize_t>();
		std::cout << "Execution finished with status code: " << status_code << std::endl;
	}

	qpl::free_handlers();

	return status_code;
}
