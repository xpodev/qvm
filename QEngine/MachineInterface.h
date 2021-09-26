#pragma once

#include "QPL.h"


BEGIN_NAMESPACE(NS_QPL)

template <typename T>
concept IMachine = requires (T & machine) {
	{ machine } -> IMachine;
};

END_NAMESPACE(NS_QPL)
