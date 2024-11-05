#ifndef __PYENGINE_2_0_STRING_OPS_H__
#define __PYENGINE_2_0_STRING_OPS_H__
// This class abstracts string manipulation operations

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#if APIABSTRACTION_D3D9 | APIABSTRACTION_D3D11 | APIABSTRACTION_OGL | PE_PLAT_IS_PSVITA | PE_PLAT_IS_PS4
#include <string.h>
#include <stdio.h>
#endif

// Inter-Engine includes
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"

// Sibling/Children includes

namespace StringOps
{
inline PrimitiveTypes::UInt32 length(const char *a)
{
	return (PrimitiveTypes::UInt32)(strlen(a));
}

inline unsigned int writeToString(const char *a, char *dest, PrimitiveTypes::UInt32 maxSize)
{
	if (a)
	{
#if APIABSTRACTION_IOS || APIABSTRACTION_PS3 || PE_PLAT_IS_PSVITA
		sprintf(dest, "%s", a);
#else
		sprintf_s(dest, maxSize, "%s", a);
#endif
		return length(dest);
	}
	else
	{
		dest[0] = '\0';
		return 0;
	}
}

inline void concat(const char *a, const char *b, char *dest, PrimitiveTypes::UInt32 size)
{
#		if APIABSTRACTION_IOS || APIABSTRACTION_PS3 || PE_PLAT_IS_PSVITA
			sprintf(dest, "%s%s", a, b);
#		else
			sprintf_s(dest, size, "%s%s", a, b);
#		endif
}

inline void stringToWideString(const char *a, wchar_t *dest, PrimitiveTypes::UInt32 size)
{
	PrimitiveTypes::UInt32 len = length(a);
	for (PrimitiveTypes::UInt32 i = 0; i <= len; i++)
	{
		*dest = (wchar_t)(*a);
		dest++; a++;
	}
}

inline void wideStringToString(const wchar_t *a, char *dest, PrimitiveTypes::UInt32 size)
{
	for (PrimitiveTypes::UInt32 i = 0; i <= size; i++)
	{
		*dest = *(char *)(a);
		if (*dest == '\0')
			break;
		dest++; 
		a++;
	}
}

inline char* intToStr(PrimitiveTypes::Int32 value, char* buffer, PrimitiveTypes::UInt32 size) {
	
#		if APIABSTRACTION_IOS || APIABSTRACTION_PS3 || PE_PLAT_IS_PSVITA
			sprintf(buffer, "%d", value);
#		else
			sprintf_s(buffer, size, "%d", value);
#		endif
	return buffer;
}

inline PrimitiveTypes::Int32 strcmp(const char *a, const char *b)
{
	return (PrimitiveTypes::Int32)(::strcmp(a, b));
}

inline bool startsswith(const char *a, const char *b)
{
	PrimitiveTypes::UInt32 lena = length(a);
	PrimitiveTypes::UInt32 lenb = length(b);

	if (lena < lenb)
		return false;

	char const *starta = a;
	char const *startb = b;

	for (PrimitiveTypes::UInt32 i = 0; i < lenb; i++)
	{
		if (*starta != *startb)
			return  false;
		++starta;
		++startb;
	}

	return true;
}

inline bool endswith(const char *a, const char *b)
{
	PrimitiveTypes::UInt32 lena = length(a);
	PrimitiveTypes::UInt32 lenb = length(b);
	char const *enda = a + (lena - 1);
	char const *endb = b + (lenb - 1);

	if (lena < lenb)
		return false;

	for (PrimitiveTypes::UInt32 i = 0; i < lenb; i++)
	{
		if (*enda != *endb)
			return  false;
		enda--;
		endb--;
	}

	return true;
}

inline PrimitiveTypes::Int32 lfind(const char *a, char b)
{
	PrimitiveTypes::UInt32 lena = length(a);
	char const *starta = a;

	for (PrimitiveTypes::UInt32 i = 0; i < lena; i++)
	{
		if (*starta == b)
			return  i;
		++starta;
	}

	return -1;
}

inline void substr(const char *a, PrimitiveTypes::UInt32 start, PrimitiveTypes::UInt32 end, char *dest)
{
	PrimitiveTypes::UInt32 lena = length(a);
	
	char const *starta = a;
	char *destb = dest;

	*dest = '\0';

	if (start >= lena || (end >= lena))
		return;

	for (PrimitiveTypes::UInt32 i = start; i <= end; i++)
	{
		*destb = *starta;	
		++starta;
		++destb;
	}
	*destb = '\0';
}

};

#endif
