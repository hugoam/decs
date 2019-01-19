#pragma once
#include <infra/Config.h>

#define STRING_TINYSTL
#ifndef STRING_TINYSTL
#ifndef CPP_20
#include <string>
namespace mud
{
	using std::string;
}
namespace tinystl
{
	static inline size_t hash(const std::string& value) {
		return std::hash<std::string>()(value);
	}
}
#endif
#else
#include <TINYSTL/string.h>
namespace mud
{
	using tinystl::string;

	inline string operator+(const string& lhs, const string& rhs)
	{
		string result;
		result.reserve(lhs.size() + rhs.size());
		result.append(lhs);
		result.append(rhs);
		return result;
	}
}
#endif
