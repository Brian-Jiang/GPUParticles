
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
// Sibling includes
#include "Texture_DDS_Loader_Common.h"

#include "PrimeEngine/Render/IRenderer.h"

// Inter-Engine includes
#include "../../Utils/ErrorHandling.h"
#include "PrimeEngine/FileSystem/FileReader.h"

namespace PE {
namespace DDS {

//-----------------------------------------------------------------------------
// Description: Swap the bytes in a 32 bit value
// Parameters:	
// Returns:
// Notes:
// PCs, PS2 and PSP are little endian.  PS3 is big endian so need to swap 
// endian for some of the data
//-----------------------------------------------------------------------------
void swapEndian(void *val)
{
	unsigned int *ival = (unsigned int *)val;
	*ival = ((*ival >> 24) & 0x000000ff) |
		((*ival >>  8) & 0x0000ff00) |
		((*ival <<  8) & 0x00ff0000) |
		((*ival << 24) & 0xff000000);
}


bool decodeDDSFormat(DDS::DDSFileHeader *ddsh, uint32_t * compressedFormat, uint32_t * uncompressedFormat, uint32_t *components)
{
	assert(compressedFormat);
	assert(uncompressedFormat);
	assert(components);
	
	*compressedFormat = 0;
	*uncompressedFormat = 0;
	*components = 0;

	if (ddsh->ddspf.dwFlags & DDSF_FOURCC)	//its a compressed texture
	{
		switch(ddsh->ddspf.dwFourCC)
		{
		#if !APIABSTRACTION_IOS
			case FOURCC_DXT1:
				*compressedFormat = FOURCC_DXT1;
				*components = 3;
				break;
			case FOURCC_DXT3:
				*compressedFormat = FOURCC_DXT3;
				*components = 4;
				break;
			case FOURCC_DXT5:
				*compressedFormat = FOURCC_DXT5;
				*components = 4;
				break;
		#endif
		default:
			printf("ERROR: Uses a compressed texture of unsupported type\n");
			return false;
		}
	}
	else
		if (ddsh->ddspf.dwFlags == DDSF_RGBA && ddsh->ddspf.dwRGBBitCount == 32)
		{
			*uncompressedFormat = DDSF_RGBA; 
			*components = 4;
			printf("decodeDDSFormat(): DDS format (was DDSF_RGBA with RGBBitCount = 32), components = 4\n");
		}
		else if (ddsh->ddspf.dwFlags == DDSF_RGB  && ddsh->ddspf.dwRGBBitCount == 32)
		{
			*uncompressedFormat = DDSF_RGB; 
			*components = 4;
			printf("decodeDDSFormat(): DDS format (was DDSF_RGB with RGBBitCount = 32), components = 4\n");
		}

		else if (ddsh->ddspf.dwFlags == DDSF_RGB  && ddsh->ddspf.dwRGBBitCount == 24)
		{
			*uncompressedFormat = DDSF_RGB;
			*components = 3;
			printf("decodeDDSFormat(): DDS format (was DDSF_RGB with RGBBitCount = 24), components = 3\n");
		}

		else if (ddsh->ddspf.dwRGBBitCount == 8)
		{
			*uncompressedFormat = DDSF_L;
			*components = 1;
			printf("decodeDDSFormat(): DDS format (was RGBBitCount = 8), components = 1\n");
		}
		else 
		{
			printf("ERROR: Uses a texture of unsupported type");
			return false;
		}

		return true;
}

/*
bool DDSFormatToApiFormat(DDS::DDSFileHeader *ddsh, uint32_t * apiFormat,uint32_t *components)
{
	assert(apiFormat);
	assert(components);

	if (ddsh->ddspf.dwFlags & DDSF_FOURCC)	//its a compressed texture
	{
		switch(ddsh->ddspf.dwFourCC)
		{
		#if !APIABSTRACTION_IOS
			case FOURCC_DXT1:
				*apiFormat = API_CHOOSE_DX11_DX9_OGL(DXGI_FORMAT_BC1_TYPELESS, D3DFMT_DXT1, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);
				*components = 3;
				break;
			case FOURCC_DXT3:
				*apiFormat = API_CHOOSE_DX11_DX9_OGL(DXGI_FORMAT_BC3_TYPELESS, D3DFMT_DXT3, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
				*components = 4;
				break;
			case FOURCC_DXT5:
				*apiFormat = API_CHOOSE_DX11_DX9_OGL(DXGI_FORMAT_BC5_TYPELESS, D3DFMT_DXT5, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
				*components = 4;
				break;
		#endif
		default:
			printf("ERROR: Uses a compressed texture of unsupported type\n");
			return false;
		}
	}
	else
		if (ddsh->ddspf.dwFlags == DDSF_RGBA && ddsh->ddspf.dwRGBBitCount == 32)
		{
			*apiFormat = API_CHOOSE_DX11_DX9_OGL(DXGI_FORMAT_R8G8B8A8_TYPELESS, D3DFMT_A8R8G8B8, GL_BGRA); 
			*components = 4;
			printf("DDSFormatToApiFormat(): DDS format (was DDSF_RGBA with RGBBitCount = 32) = GL_BGRA, components = 4\n");
		}
		else if (ddsh->ddspf.dwFlags == DDSF_RGB  && ddsh->ddspf.dwRGBBitCount == 32)
		{
			*apiFormat = API_CHOOSE_DX11_DX9_OGL(DXGI_FORMAT_R8G8B8A8_TYPELESS, D3DFMT_X8R8G8B8, GL_BGRA); 
			*components = 4;
			printf("DDSFormatToApiFormat(): DDS format (was DDSF_RGB with RGBBitCount = 32) we set to = GL_BGRA, components = 4\n");
		}

		else if (ddsh->ddspf.dwFlags == DDSF_RGB  && ddsh->ddspf.dwRGBBitCount == 24)
		{
			#if APIABSTRACTION_IOS
				assert(!"This case not tested on IOS! It is ok to comment out this assert, but make sure to test the textures show up correctly!");
				*apiFormat = 0; //  GL_BGR; cant store GL_BGR because not defined on IOS. use components == 3 test for this case
			#else
				API_CHOOSE_DX11_DX9_OGL(assert(!"Not supported!"), , );
				*apiFormat = API_CHOOSE_DX11_DX9_OGL(0, D3DFMT_R8G8B8, GL_BGR);
			#endif
			*components = 3;
			printf("DDSFormatToApiFormat(): DDS format (was DDSF_RGB with RGBBitCount = 24) we set to = GL_BGR, components = 3\n");
		}

		else if (ddsh->ddspf.dwRGBBitCount == 8)
		{
			*apiFormat = API_CHOOSE_DX11_DX9_OGL(DXGI_FORMAT_R8_TYPELESS, D3DFMT_L8, GL_LUMINANCE); 
			*components = 1;
			printf("DDSFormatToApiFormat(): DDS format (was RGBBitCount = 8) we set to = GL_LUMINANCE, components = 1\n");
		}
		else 
		{
			printf("ERROR: Uses a texture of unsupported type");
			return false;
		}

		return true;
}
*/

int32_t getImageSize(uint32_t w, uint32_t h, uint32_t components, uint32_t compressedFormat, uint32_t uncompressedFormat)
{
	if (compressedFormat)
	{
		switch(compressedFormat)
		{
		#if !APIABSTRACTION_IOS
			case FOURCC_DXT1:
				return ((w+3)/4)* ((h+3)/4)* 8;		
			case FOURCC_DXT3:
			case FOURCC_DXT5:
				return ((w+3)/4)*((h+3)/4)* 16;   
		#endif
		default:
			printf("ERROR: Uses a compressed texture of unsupported type\n");
			return 0;
		}
	}
	else
	{
		return w * h * components;
	}
}
#if !PE_PLAT_IS_PSVITA
uint32_t getDestGPUFormatFromComponents(uint32_t components)
{
	uint32_t formatEnum = API_CHOOSE_DX11_DX9_OGL(DXGI_FORMAT_R8_TYPELESS, D3DFMT_L8, GL_LUMINANCE);

	switch(components)
	{
	case 1:
		formatEnum = API_CHOOSE_DX11_DX9_OGL(DXGI_FORMAT_R8_TYPELESS, D3DFMT_L8, GL_LUMINANCE);
		printf("getDestGPUFormatFromComponents() : dest gpu formatEnum = GL_LUMINANCE\n");
		break;
	case 3:
		// this means in dds is stored as BGR

		#if APIABSTRACTION_PS3 /* || APIABSTRACTION_GLPC*/
			// PSGL give us ability to specify correct format. Out of additional options: GL_DEPTH_COMPONENT, GL_ABGR, , GL_BGR, GL_BGRA, GL_RED, GL_GREEN, GL_BLUE	
			// on PC this doesnt work (idk why) so we let TextImage2D() handle conversion
			formatEnum = GL_BGR;
			assert(!"This code is not tested! From what i saw in docs GL_BGR is NOT supported as GPU storage format. It is ok to comment out this assert but then see if this works! It will probably not work in which case you can have it = GL_RGB and see it complain about per pixel byte swaps. You can manually swap bytes like we do for IOS, but GPU probably does it faster");
			printf("getSourceCPUFormatFromComponents() : dest gpu formatEnum = GL_BGR (PS GL Special)\n");
		#else
			// in GL ES this is the only one we can use. Out of: GL_ALPHA, GL_LUMINANCE_ALPHA, GL_LUMINANCE, GL_RGB, GL_RGBA
			// after this we will have to manually swap the bytes
			API_CHOOSE_DX11_DX9_OGL(assert(!"Not supported!"), assert(!"Not supported!") , false );
			//we choose not to support 3 component format since it has issues on dx11
			formatEnum = API_CHOOSE_DX11_DX9_OGL(0, 0, GL_RGB);
			printf("getSourceCPUFormatFromComponents() : dest gpu formatEnum = GL_RGB\n");
		#endif
		break;
	case 4:
		// this means in dds is stored as BGRA
		#if APIABSTRACTION_PS3 /* || APIABSTRACTION_GLPC*/
			// PSGL give us ability to specify correct format. Out of additional options: GL_DEPTH_COMPONENT, GL_ABGR, , GL_BGR, GL_BGRA, GL_RED, GL_GREEN, GL_BLUE	
			//on PC this doesnt work (idk why) so we have to let texImage2D() handle conversion
			formatEnum = GL_BGRA;
			printf("getSourceCPUFormatFromComponents() : dest gpu formatEnum = GL_BGRA (PS GL Special)\n");
		#else
			// in GL ES this is the only one we can use. Out of: GL_ALPHA, GL_LUMINANCE_ALPHA, GL_LUMINANCE, GL_RGB, GL_RGBA
			// after this we will have to manually swap the bytes
			formatEnum = API_CHOOSE_DX11_DX9_OGL(DXGI_FORMAT_R8G8B8A8_TYPELESS, D3DFMT_A8R8G8B8, GL_RGBA);
			printf("getSourceCPUFormatFromComponents() : dest gpu formatEnum = GL_RGBA\n");
		#endif
		break;
	default:
		printf("ERROR: Unknown pixel type\n");
		exit(0);
	}

	return(formatEnum);
}

uint32_t getSourceCPUFormatFromComponents(uint32_t components)
{
	uint32_t formatEnum = API_CHOOSE_DX11_DX9_OGL(DXGI_FORMAT_R8_TYPELESS, D3DFMT_L8, GL_LUMINANCE);

	switch(components)
	{
	case 1:
		formatEnum = API_CHOOSE_DX11_DX9_OGL(DXGI_FORMAT_R8_TYPELESS, D3DFMT_L8, GL_LUMINANCE);
		printf("getSourceCPUFormatFromComponents() : formatEnum = GL_LUMINANCE\n");
		break;
	case 3:
		// this means in dds is stored as BGR

#if APIABSTRACTION_PS3 || APIABSTRACTION_GLPC
		// PSGL give us ability to specify correct format. Out of additional options: GL_DEPTH_COMPONENT, GL_ABGR, , GL_BGR, GL_BGRA, GL_RED, GL_GREEN, GL_BLUE	
		formatEnum = GL_BGR;
		printf("getSourceCPUFormatFromComponents() : formatEnum = GL_BGR (PS GL Special)\n");
#else
		// in GL ES this is the only one we can use. Out of: GL_ALPHA, GL_LUMINANCE_ALPHA, GL_LUMINANCE, GL_RGB, GL_RGBA
		// after this we will have to manually swap the bytes
            API_CHOOSE_DX11_DX9_OGL(assert(!"Not supported!"), assert(!"Not supported!"), false);
				
		formatEnum = API_CHOOSE_DX11_DX9_OGL(0, 0, GL_RGB);;
		printf("getSourceCPUFormatFromComponents() : formatEnum = GL_RGB\n");
#endif
		break;
	case 4:
		// this means in dds is stored as BGRA
#if APIABSTRACTION_PS3 || APIABSTRACTION_GLPC
		// PC and PSGL give us ability to specify correct format. Out of additional options: GL_DEPTH_COMPONENT, GL_ABGR, , GL_BGR, GL_BGRA, GL_RED, GL_GREEN, GL_BLUE	
		formatEnum = GL_BGRA;
		printf("getSourceCPUFormatFromComponents() : formatEnum = GL_BGRA (PS GL Special)\n");
#else
		// in GL ES this is the only one we can use. Out of: GL_ALPHA, GL_LUMINANCE_ALPHA, GL_LUMINANCE, GL_RGB, GL_RGBA
		// after this we will have to manually swap the bytes
		formatEnum = API_CHOOSE_DX11_DX9_OGL(DXGI_FORMAT_R8G8B8A8_TYPELESS, D3DFMT_A8R8G8B8, GL_RGBA);
		printf("getSourceCPUFormatFromComponents() : formatEnum = GL_RGBA\n");
#endif
		break;
	default:
		printf("ERROR: Unknown pixel type\n");
		exit(0);
	}

	return(formatEnum);
}
#endif

bool loadDDSIntoSystemMemory(char *filename, DDS::DDSTextureInSystemMemory* dds)
{
	uint32_t fileLen;

	//load the file
	fileLen = FileReader::getFileLen(filename);
	PEASSERT(fileLen, "ERROR: failed to load %s", filename);

	//unsigned char *buffer;
	unsigned char *buffer = (unsigned char*)malloc(fileLen);
	
	PEASSERT(buffer, "ERROR: failed to malloc %d bytes",fileLen);
	
	// Note: Either LoadFile and fileLen should both be the same type, probably unsigned
	PEASSERT(FileReader::LoadFile(filename,fileLen,buffer)==((long)fileLen), "ERROR: failed to load %s", filename);

	unsigned char *buff;
	buff = buffer;

	// read in file marker, make sure its a DDS file
	if(strncmp((char*)buff,"DDS ",4)!=0)
	{
		printf("ERROR: %s is not a dds file",filename);
		return false;
	}
	buff+=4;  //skip over header 

	//read the dds header data
	DDS::DDSFileHeader *ddsh;

	//memcpy(&ddsh,buffer+4,sizeof(DDS_HEADER));
	ddsh=(DDS::DDSFileHeader*)buff;
	buff+=sizeof(DDS::DDSFileHeader);

#if APIABSTRACTION_PS3
	uint32_t* t = (uint32_t*)ddsh;
	for(unsigned int ddsCounterHeader=0; ddsCounterHeader<sizeof(DDS::DDSFileHeader)/4; ddsCounterHeader++)
	{
		swapEndian(t+ddsCounterHeader);
	}
#endif
	DDS::TextureType type = DDS::TextureFlat;
	if (ddsh->dwCaps2 & DDSF_CUBEMAP)	type = DDS::TextureCubemap;

	// check if image is a volume texture
	if ((ddsh->dwCaps2 & DDSF_VOLUME) && (ddsh->dwDepth > 0))
	{
		printf("ERROR: %s is a volume texture ",filename);
		return false;
	}

	// get the format of the image
	uint32_t	compressedFormat;
	uint32_t	uncompressedFormat;
	uint32_t	components;

	//get the texture format and number of color channels
	DDS::decodeDDSFormat(ddsh,&compressedFormat, &uncompressedFormat, &components);

	uint32_t width, height;
	width = ddsh->dwWidth;
	height = ddsh->dwHeight;

	dds->buffer		= buffer;
	dds->compressedFormat	= compressedFormat; // compressed pixel format
	dds->uncompressedFormat	= uncompressedFormat; // pixel format 
	dds->components = components;
	dds->height		= height;
	dds->width		= width;

	if(ddsh->dwMipMapCount==0) ddsh->dwMipMapCount++;

	dds->mips		= ddsh->dwMipMapCount;

	dds->surfaces = 1;
	if(type ==DDS::TextureCubemap) dds->surfaces = 6;

	//get pointers to the pixel data
	uint32_t i,j;
	for(i=0; i<dds->surfaces; i++)
	{
		printf("Surface %d\n",i);
		height=dds->height;
		width=dds->width;

		for(j=0; j<ddsh->dwMipMapCount; j++)
		{
			dds->image[i].pixels[j] = buff;
			buff+=DDS::getImageSize(width, height, components, compressedFormat, uncompressedFormat);
			width/=2; height/=2;
		}
	}

	return true;
}

}; // DDS
}; // namespace PE
