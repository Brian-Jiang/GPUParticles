#ifndef __PYENGINE_2_0_TEXTURE_DDS_Loader_Common__
#define __PYENGINE_2_0_TEXTURE_DDS_Loader_Common__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes

#include "PrimeEngine/Render/IRenderer.h"

// Sibling/Children includes
#include "SamplerState.h"


#include <stdlib.h>
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;

// surface description flags
#define 	DDSF_MAX_MIPMAPS	16
#define 	DDSF_MAX_TEXTURES	16		

#define		DDSF_CAPS           0x00000001
#define 	DDSF_HEIGHT         0x00000002
#define 	DDSF_WIDTH          0x00000004
#define 	DDSF_PITCH          0x00000008
#define 	DDSF_PIXELFORMAT    0x00001000
#define 	DDSF_MIPMAPCOUNT    0x00020000
#define 	DDSF_LINEARSIZE     0x00080000
#define 	DDSF_DEPTH			0x00800000

// pixel format flags
#define		DDSF_ALPHAPIXELS	0x00000001
#define		DDSF_FOURCC			0x00000004
#define		DDSF_RGB			0x00000040
#define		DDSF_RGBA			0x00000041
#define		DDSF_L				0x00020000

// dwCaps1 flags
#define DDSF_COMPLEX			0x00000008
#define DDSF_TEXTURE			0x00001000
#define DDSF_MIPMAP				0x00400000

// dwCaps2 flags
#define DDSF_CUBEMAP			0x00000200l
#define DDSF_CUBEMAP_POSITIVEX  0x00000400l
#define DDSF_CUBEMAP_NEGATIVEX  0x00000800l
#define DDSF_CUBEMAP_POSITIVEY  0x00001000l
#define DDSF_CUBEMAP_NEGATIVEY  0x00002000l
#define DDSF_CUBEMAP_POSITIVEZ  0x00004000l
#define DDSF_CUBEMAP_NEGATIVEZ  0x00008000l
#define DDSF_CUBEMAP_ALL_FACES  0x0000FC00l
#define DDSF_VOLUME				0x00200000l

// compressed texture types
#define FOURCC_DXT1				0x31545844
#define FOURCC_DXT3				0x33545844
#define FOURCC_DXT5				0x35545844

namespace PE {

namespace DDS
{

	struct DXTColBlock
	{
		unsigned short col0;
		unsigned short col1;
		unsigned char row[4];
	};

	struct DXT3AlphaBlock
	{
		unsigned short row[4];
	};

	struct DXT5AlphaBlock
	{
		unsigned char alpha0;
		unsigned char alpha1;
		unsigned char row[6];
	};

	struct DDS_PIXELFORMAT
	{
		uint32_t dwSize;
		uint32_t dwFlags;
		uint32_t dwFourCC;
		uint32_t dwRGBBitCount;
		uint32_t dwRBitMask;
		uint32_t dwGBitMask;
		uint32_t dwBBitMask;
		uint32_t dwABitMask;
	};

	struct DDSFileHeader
	{
		uint32_t dwSize;
		uint32_t dwFlags;
		uint32_t dwHeight;
		uint32_t dwWidth;
		uint32_t dwPitchOrLinearSize;
		uint32_t dwDepth;
		uint32_t dwMipMapCount;
		uint32_t dwReserved1[11];
		DDS_PIXELFORMAT ddspf;
		uint32_t dwCaps1;
		uint32_t dwCaps2;
		uint32_t dwReserved2[3];
	};

	typedef enum TextureType
	{
		TextureNone,
		TextureFlat,    // 1D, 2D, and rectangle textures
		Texture3D,
		TextureCubemap
	}TextureType;

	typedef struct _DDS_IMAGE
	{
		unsigned char *pixels[DDSF_MAX_MIPMAPS];		//the mip map images 
	}_DDS_IMAGE;

	struct DDSTextureInSystemMemory
	{
		unsigned char		*buffer;	//pointer to loaded dds file
		uint32_t			compressedFormat; // format of compressed texels, 0 if not compressed (in which case use format)
		uint32_t			uncompressedFormat;	//format of the texels if not compressed, 0 if compressed
		uint32_t			components; //number of channels 
		uint32_t			width;		//width of base image in pixels
		uint32_t			height;		//height of based image in pixels
		uint32_t			mips;		//number of mip levels
		uint32_t			surfaces;	//number of surfaces ( 1 = a texture 6 = a cube map)

		_DDS_IMAGE image[6];
	};

	//-----------------------------------------------------------------------------
	// bool DDSFormatToApiFormat(DDS_HEADER *ddsh, uint32_t * compressedFormat, uint32_t * uncompressedFormat, uint32_t *components)
	// Description: 
	// helper function that returns the format and number of components in a dds
	// header
	// Returns: 
	// false on failure
	// Notes:
	//-----------------------------------------------------------------------------
	bool decodeDDSFormat(DDS::DDSFileHeader *ddsh, uint32_t * compressedFormat, uint32_t * uncompressedFormat, uint32_t *components);

	
	//-----------------------------------------------------------------------------
	// int32_t getImageSize(uint32_t w,uint32_t h,uint32_t components,, uint32_t compressedFormat, uint32_t uncompressedFormat)
	// Description: 
	// returns the size of a texture buffer
	// header
	// Returns: 
	// Notes:
	//-----------------------------------------------------------------------------
	int32_t getImageSize(uint32_t w, uint32_t h, uint32_t components, uint32_t compressedFormat, uint32_t uncompressedFormat);

	
	//-----------------------------------------------------------------------------
	// GLenum getFormatFromComponents(uint32_t components)
	// Description: 
	// this is just a helper function used when loading textures.
	// Returns: 
	// a GLenim based the number of channels input
	// Notes
	//-----------------------------------------------------------------------------
	uint32_t getDestGPUFormatFromComponents(uint32_t components);
	
	uint32_t getSourceCPUFormatFromComponents(uint32_t components);

	//-----------------------------------------------------------------------------
	// bool loadDDSIntoSystemMemory(char *filename,DDSTextureInSystemMemory* dds)
	// Description: 
	// loads a dds texture file and fills out the dds structure
	// Returns: 
	// false on failure
	// Notes:
	//-----------------------------------------------------------------------------
	bool loadDDSIntoSystemMemory(char *filename, DDS::DDSTextureInSystemMemory* dds);

}; // namespace DDS

}; // namespace PE

#endif
