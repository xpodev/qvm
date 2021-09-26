#pragma once


// A member is any item that can be stored inside a type definition
class Member;

// A m_type is a collection of methods, properties, fields and nested types that define the behaviour and data of an object
class Type;

// A field is something that stores data
class Field;

// A function is an items that can be called
class Function;

// A method is a function belonging a type
class Method;

// A property is like a field, but with wrapper methods for getter & setter (and maybe other custom ones).
// not used by the VM
class Property;
