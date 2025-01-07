#ifndef __PYENGINE_2_0_PRIMITVE_TYPES_H__
#define __PYENGINE_2_0_PRIMITVE_TYPES_H__

namespace PrimitiveTypes {
// Types
typedef unsigned int UInt32;
typedef int Int32;
typedef short Int16;
typedef unsigned short UInt16;

typedef float Float32;
typedef double Float64;
typedef double Double;
typedef char Byte;
typedef char Char;
typedef bool Bool;
typedef char * String;

namespace Constants {
// Constants
const Float32 c_Pi_F32 = 3.1415926535897932384626433832795f;
const Float64 c_Pi_F64 = 3.1415926535897932384626433832795;
const Float32 c_Epsilon_F32 = 0.00001f;

const Float32 c_MaxFloat32 = 1000000000.0f;

const Float32 c_LargeFloat32 = 1000000000.0f;

const UInt32 c_MaxUInt32 = 0xFFFFFFFF;
const Int32 c_MaxInt32 = 0x7FFFFFFF;
const Int32 c_MinInt32 = 0x80000000;
const UInt32 c_MaxUInt16 = 0xFFFF;
}; // namespace Constants

}; // namespace PrimitiveTypes
#endif
;