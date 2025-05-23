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

#include "guid.h"

#include <chrono>
#include <random>
#include <cstring>

#ifdef GUID_LIBUUID
#include <uuid/uuid.h>
#endif

#ifdef GUID_CFUUID
#include <CoreFoundation/CFUUID.h>
#endif

#ifdef GUID_WINDOWS
#include <objbase.h>
#endif

#ifdef GUID_ANDROID
#include <jni.h>
#include <cassert>
#endif

BEGIN_XG_NAMESPACE

#ifdef GUID_ANDROID
AndroidGuidInfo androidInfo;

AndroidGuidInfo AndroidGuidInfo::fromJniEnv(JNIEnv* env)
{
	AndroidGuidInfo info;
	info.env = env;
	auto localUuidClass = env->FindClass("java/util/UUID");
	info.uuidClass = (jclass)env->NewGlobalRef(localUuidClass);
	env->DeleteLocalRef(localUuidClass);
	info.newGuidMethod = env->GetStaticMethodID(
		info.uuidClass, "randomUUID", "()Ljava/util/UUID;");
	info.mostSignificantBitsMethod = env->GetMethodID(
		info.uuidClass, "getMostSignificantBits", "()J");
	info.leastSignificantBitsMethod = env->GetMethodID(
		info.uuidClass, "getLeastSignificantBits", "()J");
	info.initThreadId = std::this_thread::get_id();
	return info;
}

void initJni(JNIEnv* env)
{
	androidInfo = AndroidGuidInfo::fromJniEnv(env);
}
#endif

// overload << so that it's easy to convert to a string
std::ostream& operator<<(std::ostream& s, const GUID& guid)
{
	std::ios_base::fmtflags f(s.flags()); // politely don't leave the ostream in hex mode
	s << std::hex << std::setfill('0')
		<< std::setw(2) << (int)guid._bytes[0]
		<< std::setw(2) << (int)guid._bytes[1]
		<< std::setw(2) << (int)guid._bytes[2]
		<< std::setw(2) << (int)guid._bytes[3]
		<< "-"
		<< std::setw(2) << (int)guid._bytes[4]
		<< std::setw(2) << (int)guid._bytes[5]
		<< "-"
		<< std::setw(2) << (int)guid._bytes[6]
		<< std::setw(2) << (int)guid._bytes[7]
		<< "-"
		<< std::setw(2) << (int)guid._bytes[8]
		<< std::setw(2) << (int)guid._bytes[9]
		<< "-"
		<< std::setw(2) << (int)guid._bytes[10]
		<< std::setw(2) << (int)guid._bytes[11]
		<< std::setw(2) << (int)guid._bytes[12]
		<< std::setw(2) << (int)guid._bytes[13]
		<< std::setw(2) << (int)guid._bytes[14]
		<< std::setw(2) << (int)guid._bytes[15];
	s.flags(f);
	return s;
}

bool operator<(const XG::GUID& lhs, const XG::GUID& rhs)
{
	return lhs.bytes() < rhs.bytes();
}

bool GUID::isValid() const
{
	XG::GUID empty;
	return *this != empty;
}

// convert to string using std::snprintf() and std::string
std::string GUID::str() const
{
	char one[10], two[6], three[6], four[6], five[14];

	snprintf(one, 10, "%02x%02x%02x%02x",
		_bytes[0], _bytes[1], _bytes[2], _bytes[3]);
	snprintf(two, 6, "%02x%02x",
		_bytes[4], _bytes[5]);
	snprintf(three, 6, "%02x%02x",
		_bytes[6], _bytes[7]);
	snprintf(four, 6, "%02x%02x",
		_bytes[8], _bytes[9]);
	snprintf(five, 14, "%02x%02x%02x%02x%02x%02x",
		_bytes[10], _bytes[11], _bytes[12], _bytes[13], _bytes[14], _bytes[15]);
	const std::string sep("-");
	std::string out(one);

	out += sep + two;
	out += sep + three;
	out += sep + four;
	out += sep + five;

	return out;
}

// conversion operator for std::string
GUID::operator std::string() const
{
	return str();
}

// Access underlying bytes
const std::array<unsigned char, 16>& GUID::bytes() const
{
	return _bytes;
}

// create a guid from vector of bytes
GUID::GUID(const std::array<unsigned char, 16>& bytes) : _bytes(bytes)
{
}

// create a guid from vector of bytes
GUID::GUID(std::array<unsigned char, 16>&& bytes) : _bytes(std::move(bytes))
{
}

// converts a single hex char to a number (0 - 15)
unsigned char hexDigitToChar(char ch)
{
	// 0-9
	if (ch > 47 && ch < 58)
		return ch - 48;

	// a-f
	if (ch > 96 && ch < 103)
		return ch - 87;

	// A-F
	if (ch > 64 && ch < 71)
		return ch - 55;

	return 0;
}

bool isValidHexChar(char ch)
{
	// 0-9
	if (ch > 47 && ch < 58)
		return true;

	// a-f
	if (ch > 96 && ch < 103)
		return true;

	// A-F
	if (ch > 64 && ch < 71)
		return true;

	return false;
}

// converts the two hexadecimal characters to an unsigned char (a byte)
unsigned char hexPairToChar(char a, char b)
{
	return hexDigitToChar(a) * 16 + hexDigitToChar(b);
}

// create a guid from string
GUID::GUID(std::string_view fromString)
{
	char charOne = '\0';
	char charTwo = '\0';
	bool lookingForFirstChar = true;
	unsigned nextByte = 0;

	for (const char& ch : fromString)
	{
		if (ch == '-')
			continue;

		if (nextByte >= 16 || !isValidHexChar(ch))
		{
			// Invalid string so bail
			zeroify();
			return;
		}

		if (lookingForFirstChar)
		{
			charOne = ch;
			lookingForFirstChar = false;
		}
		else
		{
			charTwo = ch;
			auto byte = hexPairToChar(charOne, charTwo);
			_bytes[nextByte++] = byte;
			lookingForFirstChar = true;
		}
	}

	// if there were fewer than 16 bytes in the string then guid is bad
	if (nextByte < 16)
	{
		zeroify();
		return;
	}
}

// create empty guid
GUID::GUID() : _bytes{ {0} }
{
}

// set all bytes to zero
void GUID::zeroify()
{
	std::fill(_bytes.begin(), _bytes.end(), static_cast<unsigned char>(0));
}

// overload equality operator
bool GUID::operator==(const GUID& other) const
{
	return _bytes == other._bytes;
}

// overload inequality operator
bool GUID::operator!=(const GUID& other) const
{
	return !((*this) == other);
}

// member swap function
void GUID::swap(GUID& other)
{
	_bytes.swap(other._bytes);
}

// Linux
#ifdef GUID_LIBUUID
GUID createGUID()
{
	std::array<unsigned char, 16> data;
	static_assert(std::is_same<unsigned char[16], uuid_t>::value, "Wrong type!");
	uuid_generate(data.data());
	return GUID{ std::move(data) };
}
#endif

// MacOS / IOS
#ifdef GUID_CFUUID
GUID createGUID()
{
	auto newId = CFUUIDCreate(NULL);
	auto bytes = CFUUIDGetUUIDBytes(newId);
	CFRelease(newId);

	std::array<unsigned char, 16> byteArray =
	{ {
		bytes.byte0,
		bytes.byte1,
		bytes.byte2,
		bytes.byte3,
		bytes.byte4,
		bytes.byte5,
		bytes.byte6,
		bytes.byte7,
		bytes.byte8,
		bytes.byte9,
		bytes.byte10,
		bytes.byte11,
		bytes.byte12,
		bytes.byte13,
		bytes.byte14,
		bytes.byte15
	} };
	return GUID{ std::move(byteArray) };
}
#endif

// Windows
#ifdef GUID_WINDOWS
GUID createGUID()
{
	GUID newId;
	CoCreateGuid(&newId);

	std::array<unsigned char, 16> bytes =
	{
		(unsigned char)((newId.Data1 >> 24) & 0xFF),
		(unsigned char)((newId.Data1 >> 16) & 0xFF),
		(unsigned char)((newId.Data1 >> 8) & 0xFF),
		(unsigned char)((newId.Data1) & 0xff),

		(unsigned char)((newId.Data2 >> 8) & 0xFF),
		(unsigned char)((newId.Data2) & 0xff),

		(unsigned char)((newId.Data3 >> 8) & 0xFF),
		(unsigned char)((newId.Data3) & 0xFF),

		(unsigned char)newId.Data4[0],
		(unsigned char)newId.Data4[1],
		(unsigned char)newId.Data4[2],
		(unsigned char)newId.Data4[3],
		(unsigned char)newId.Data4[4],
		(unsigned char)newId.Data4[5],
		(unsigned char)newId.Data4[6],
		(unsigned char)newId.Data4[7]
	};

	return GUID{ std::move(bytes) };
}
#endif

// Android
#ifdef GUID_ANDROID
GUID createGUID(JNIEnv* env)
{
	assert(env != androidInfo.env || std::this_thread::get_id() == androidInfo.initThreadId);

	jobject javaUuid = env->CallStaticObjectMethod(
		androidInfo.uuidClass, androidInfo.newGuidMethod);
	jlong mostSignificant = env->CallLongMethod(javaUuid,
		androidInfo.mostSignificantBitsMethod);
	jlong leastSignificant = env->CallLongMethod(javaUuid,
		androidInfo.leastSignificantBitsMethod);

	std::array<unsigned char, 16> bytes =
	{
		(unsigned char)((mostSignificant >> 56) & 0xFF),
		(unsigned char)((mostSignificant >> 48) & 0xFF),
		(unsigned char)((mostSignificant >> 40) & 0xFF),
		(unsigned char)((mostSignificant >> 32) & 0xFF),
		(unsigned char)((mostSignificant >> 24) & 0xFF),
		(unsigned char)((mostSignificant >> 16) & 0xFF),
		(unsigned char)((mostSignificant >> 8) & 0xFF),
		(unsigned char)((mostSignificant) & 0xFF),
		(unsigned char)((leastSignificant >> 56) & 0xFF),
		(unsigned char)((leastSignificant >> 48) & 0xFF),
		(unsigned char)((leastSignificant >> 40) & 0xFF),
		(unsigned char)((leastSignificant >> 32) & 0xFF),
		(unsigned char)((leastSignificant >> 24) & 0xFF),
		(unsigned char)((leastSignificant >> 16) & 0xFF),
		(unsigned char)((leastSignificant >> 8) & 0xFF),
		(unsigned char)((leastSignificant) & 0xFF)
	};

	env->DeleteLocalRef(javaUuid);

	return GUID{ std::move(bytes) };
}

GUID createGUID()
{
	return createGUID(androidInfo.env);
}
#endif

// OS agnostic implementation
#if !defined(GUID_LIBUUID) && !defined(GUID_CFUUID) && !defined(GUID_WINDOWS) && !defined(GUID_ANDROID)
GUID createGUID()
{
	// Create a seed from system clock and a random device
	std::random_device rd;
	auto seed = rd() ^ (
		static_cast<std::mt19937::result_type>(
			std::chrono::high_resolution_clock::now().time_since_epoch().count()
			) +
		static_cast<std::mt19937::result_type>(
			std::chrono::system_clock::now().time_since_epoch().count()
			)
		);

	std::mt19937 gen(seed);
	std::uniform_int_distribution<unsigned int> distrib(0, 255);

	std::array<unsigned char, 16> bytes;

	// Generate 16 random bytes
	for (int i = 0; i < 16; i++) {
		bytes[i] = static_cast<unsigned char>(distrib(gen));
	}

	// Set variant and version for RFC 4122 compliance (v4 random UUID)
	// Variant 1 - bits: 10xx
	bytes[8] = (bytes[8] & 0x3F) | 0x80;
	// Version 4 - bits: 0100xxxx
	bytes[6] = (bytes[6] & 0x0F) | 0x40;

	return GUID{ std::move(bytes) };
}
#endif

END_XG_NAMESPACE

// Specialization for std::swap<GUID>() --
// call member swap function of lhs, passing rhs
namespace std
{
	template <>
	void swap(XG::GUID& lhs, XG::GUID& rhs) noexcept
	{
		lhs.swap(rhs);
	}
}