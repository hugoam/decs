#pragma once

#define MAP_TINYSTL
#ifndef MAP_TINYSTL
#ifndef CPP_20
#include <map>
namespace mud
{
	using std::map;
}
#endif
#else
#include <TINYSTL/unordered_map.h>
namespace mud
{
	template <class K, class T>
	using map = tinystl::unordered_map<K, T>;
}
#endif
