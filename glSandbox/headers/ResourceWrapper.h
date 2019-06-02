#pragma once
#include "Timestamp.h"

template <typename Resource>
class ResourceWrapper
{
public:
	Timestamp timestamp;
	Resource const* handle = nullptr;
};