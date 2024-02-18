#pragma once
#ifndef Hasher_h__
#define Hasher_h__

#include "MurmurHash3.h"
#include "Engine_Defines.h"

BEGIN(Engine)

struct ENGINE_DLL djb2Hasher
{
	size_t operator() (const wstring& str) const
	{
		_ulong	hash = 5381;
		size_t	size = str.length();

		for (size_t i = 0; i < size; i++)
			hash = ((hash << 5) + hash) + (str[i]); /* hash * 33 + c */

		return hash;
	}

	size_t operator() (const string& str) const
	{
		_ulong	hash = 5381;
		size_t	size = str.length();

		for (size_t i = 0; i < size; i++)
			hash = ((hash << 5) + hash) + (str[i]); /* hash * 33 + c */

		return hash;
	}

	size_t operator() (_ubyte*& str) const
	{
		_ulong hash = 5381;
		_int c;
		while (c = *str++)
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		return hash;
	}
};	// HashFunctionObject (djb2)

struct ENGINE_DLL MurmurHash3Hashers
{
	size_t operator() (const uint64& key) const
	{
		uint64 hashResult[2];
		MurmurHash3_x64_128(&key, sizeof(key), 0, hashResult);
		size_t combinedHash = static_cast<size_t>(hashResult[0]) ^ static_cast<size_t>(hashResult[1]);
		return combinedHash;
	}
};	// HashFunctionObject (MurmurHash3)

END

#endif