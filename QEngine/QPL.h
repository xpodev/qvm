#pragma once

#ifndef QPL_REMOVE_NAMESPACES

#define BEGIN_NAMESPACE(x) namespace x {
#define END_NAMESPACE(x) }

#else

#undef QPL_REMOVE_NAMESPACES

#define BEGIN_NAMESPACE(x)
#define END_NAMESPACE(x)

#endif

#define NS_QPL qpl

