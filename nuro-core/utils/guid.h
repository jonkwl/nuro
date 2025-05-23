//
// 
// 
// ---------------------------------------
// This is a modified version of CrossGuid
// ---------------------------------------
// 
// 
//

/*
The MIT License (MIT)

Copyright (c) 2014 Graeme Hill

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

#ifdef GUID_ANDROID
#include <thread>
#include <jni.h>
#endif

#include <functional>
#include <iostream>
#include <array>
#include <sstream>
#include <string_view>
#include <utility>
#include <iomanip>

#define BEGIN_XG_NAMESPACE namespace XG {
#define END_XG_NAMESPACE }

BEGIN_XG_NAMESPACE

// Class to represent a GUID/UUID. Each instance acts as a wrapper around a
// 16 byte value that can be passed around by value. It also supports
// conversion to string (via the stream operator <<) and conversion from a
// string via constructor.
class GUID
{
public:
	explicit GUID(const std::array<unsigned char, 16>& bytes);
	explicit GUID(std::array<unsigned char, 16>&& bytes);

	explicit GUID(std::string_view fromString);
	GUID();

	GUID(const GUID& other) = default;
	GUID& operator=(const GUID& other) = default;
	GUID(GUID&& other) = default;
	GUID& operator=(GUID&& other) = default;

	bool operator==(const GUID& other) const;
	bool operator!=(const GUID& other) const;

	std::string str() const;
	operator std::string() const;
	const std::array<unsigned char, 16>& bytes() const;
	void swap(GUID& other);
	bool isValid() const;

private:
	void zeroify();

	// actual data
	std::array<unsigned char, 16> _bytes;

	// make the << operator a friend so it can access _bytes
	friend std::ostream& operator<<(std::ostream& s, const GUID& guid);
	friend bool operator<(const GUID& lhs, const GUID& rhs);
};

GUID createGUID();

#ifdef GUID_ANDROID
struct AndroidGuidInfo
{
	static AndroidGuidInfo fromJniEnv(JNIEnv* env);

	JNIEnv* env;
	jclass uuidClass;
	jmethodID newGuidMethod;
	jmethodID mostSignificantBitsMethod;
	jmethodID leastSignificantBitsMethod;
	std::thread::id initThreadId;
};

extern AndroidGuidInfo androidInfo;

void initJni(JNIEnv* env);

// overloading for multi-threaded calls
GUID createGUID(JNIEnv* env);
#endif

namespace details
{
	template <typename...> struct hash;

	template<typename T>
	struct hash<T> : public std::hash<T>
	{
		using std::hash<T>::hash;
	};


	template <typename T, typename... Rest>
	struct hash<T, Rest...>
	{
		inline std::size_t operator()(const T& v, const Rest&... rest) {
			std::size_t seed = hash<Rest...>{}(rest...);
			seed ^= hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
			return seed;
		}
	};
}

END_XG_NAMESPACE

namespace std
{
	// Template specialization for std::swap<GUID>() --
	// See guid.cpp for the function definition
	template <>
	void swap(XG::GUID& guid0, XG::GUID& guid1) noexcept;

	// Specialization for std::hash<GUID> -- this implementation
	// uses std::hash<std::string> on the stringification of the guid
	// to calculate the hash
	template <>
	struct hash<XG::GUID>
	{
		std::size_t operator()(XG::GUID const& guid) const
		{
			const uint64_t* p = reinterpret_cast<const uint64_t*>(guid.bytes().data());
			return XG::details::hash<uint64_t, uint64_t>{}(p[0], p[1]);
		}
	};
}