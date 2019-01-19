#pragma once

#define UNORDERED_MAP_TINYSTL
#ifndef UNORDERED_MAP_TINYSTL
#ifndef CPP_20
#include <unordered_map>
namespace mud
{
	using std::unordered_map;
}
#endif
#else
#include <TINYSTL/unordered_map.h>
namespace mud
{
	template <class K, class T>
	using unordered_map = tinystl::unordered_map<K, T>;
}
#endif
