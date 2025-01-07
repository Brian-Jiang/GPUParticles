
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// ghetto file guard until premake supports per-file configuration
#if APIABSTRACTION_OGL

// Outer-Engine includes
// Sibling includes
#include "Texture_DDS_Loader_GL.h"
#include "Texture.h"
#include "PrimeEngine/Render/IRenderer.h"

// Inter-Engine includes
#include "../../Utils/ErrorHandling.h"
#include "PrimeEngine/FileSystem/FileReader.h"

namespace PE {

GLuint TextureGPU::gfxLoadDDSTexture(char *filename, bool genMips /*= true*/)
{
	GLuint texture;

	DDS::DDSTextureInSystemMemory dds;
	uint32_t test =  loadDDSIntoSystemMemory(filename,&dds);
	if(test==false) 
	{
		printf("failed to load\n");
		exit(0);
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	int32_t w = dds.width;
	int32_t h = dds.height;

	printf("file %s dds surfaces %d mips %d w %d h %d \n",filename,dds.surfaces,dds.mips,w,h);
	if(dds.surfaces!=1)	//its a cube map 
	{
		printf("ERROR: this is a cubemap\n");
		exit(0);
	}

	GLenum destApiFormat = GL_ZERO; // tells GL how to store the texture
	GLenum sourceCpuFormatEnum = GL_ZERO; // tells GL how the texels are stored in memory
	if (dds.compressedFormat)
	{
		assert(!"Dont support compressed textures yet!");
	}
	else
	{
		if (dds.components == 1)
		{
			if (dds.uncompressedFormat == DDSF_L)
			{
					destApiFormat = sourceCpuFormatEnum = GL_LUMINANCE;
			}
			else
				assert(!"Unknown format!");
		}
		else if (dds.components == 3)
		{
			assert(!"Need to add support for this format!");
			#if APIABSTRACTION_PS3 || APIABSTRACTION_GLPC
				// PSGL give us ability to specify correct format. Out of additional options: GL_DEPTH_COMPONENT, GL_ABGR, GL_ARGB_SCE, GL_BGR, GL_BGRA, GL_RED, GL_GREEN, GL_BLUE	
				sourceCpuFormatEnum = GL_BGR;
				printf("getSourceCPUFormatFromComponents() : formatEnum = GL_BGR (PS GL Special)\n");
			#else
				// in GL ES this is the only one we can use. Out of: GL_ALPHA, GL_LUMINANCE_ALPHA, GL_LUMINANCE, GL_RGB, GL_RGBA
				// after this we will have to manually swap the bytes
            API_CHOOSE_DX11_DX9_OGL(assert(!"Not supported!"), false, false);

				sourceCpuFormatEnum = GL_RGB;
				printf("getSourceCPUFormatFromComponents() : formatEnum = GL_RGB\n");
			#endif
		}
		else if (dds.components == 4)
		{
			if (dds.uncompressedFormat == DDSF_RGBA)
				destApiFormat = GL_RGBA;
			else if  (dds.uncompressedFormat == DDSF_RGB)
				destApiFormat = GL_RGBA;

			// this means in dds is stored as BGRA
			#if APIABSTRACTION_PS3 || APIABSTRACTION_GLPC
				// PC and PSGL give us ability to specify correct format. Out of additional options: GL_DEPTH_COMPONENT, GL_ABGR, GL_ARGB_SCE, GL_BGR, GL_BGRA, GL_RED, GL_GREEN, GL_BLUE	
				sourceCpuFormatEnum = GL_BGRA;
				printf("getSourceCPUFormatFromComponents() : formatEnum = GL_BGRA (PS GL Special)\n");
			#else
				// in GL ES this is the only one we can use. Out of: GL_ALPHA, GL_LUMINANCE_ALPHA, GL_LUMINANCE, GL_RGB, GL_RGBA
				// after this we will have to manually swap the bytes
				sourceCpuFormatEnum = GL_RGBA;
				printf("getSourceCPUFormatFromComponents() : formatEnum = GL_RGBA\n");
			#endif
		}
		else
		{
			assert(!"Unknown texture format!");
		}
	}

	if (!destApiFormat)
		return NULL;


#if 0
	//load a compressed texture
	if( (dds.apiFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT) ||
		(dds.apiFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ||
		(dds.apiFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT) ||
		(dds.apiFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT))
	{
		for (uint32_t i = 0; i < dds.mips; i++)
		{
			uint32_t size = DDS::getImageSize(w,h,dds.components,dds.apiFormat);
			//printf("compressed texture %d %d %d %d 0x%x\n",i,w,h,size,dds.image[0].pixels[i]);
			glCompressedTexImage2D(GL_TEXTURE_2D, i, dds.apiFormat, w, h, 0, size, dds.image[0].pixels[i]);
			w/=2; h/=2;
			if (w == 0) w = 1;
			if (h == 0) h = 1;
		}
	}
	else // load a non compressed texture
#endif
	{
		// glPixelStorei with UNPACK_ALIGNMENT affects how pixels are read into GPU from CPU in glTexImage2D et al.
		if (sourceCpuFormatEnum==GL_RGB || sourceCpuFormatEnum==GL_LUMINANCE) glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		
		#if APIABSTRACTION_IOS
			// in case we use OS GL provided option
			//if (sourceCpuFormatEnum==GL_BGR) glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		#endif
        
        int mipOffset = 0;
        
        
        int tw = w;
        int th = h;
        for (uint32_t i = 0; i < dds.mips; i++)
        {
            if (tw > MAX_TEXTURE_LOAD_WIDTH || th > MAX_TEXTURE_LOAD_WIDTH)
            {
                --mipOffset;
            }
            else
                break;
            
            tw /= 2;
            th /= 2;
        }
            

		for (uint32_t i = 0; i < dds.mips; i++)
		{
			//uint32_t size = getImageSize(w,h,dds.components,dds.format);
			//printf("non compressed texture %d w %d h%d format %d size %d 0x%x\n",i,w,h,dds.format,size,dds.image[0].pixels[i]);
			//todo: on ps3 this yields Slow texture color format conversion for each texel: format GL_RGBA -> GL_RGBA, type GL_UNSIGNED_BYTE -> GL_UNSIGNED_INT_8_8_8_8
			// may be to avoid it, use different ordering (RGBA vs ARGB)
            
			
			#if APIABSTRACTION_IOS
                if (w <= MAX_TEXTURE_LOAD_WIDTH && h <= MAX_TEXTURE_LOAD_WIDTH)  
				{
                    // on unix based systems, byte ordering is different that win32;
                    // so to have colors show up correctly we swap the bytes in data
                    // we do it on IOS only because IOS GL ES has limited formats for source data and doesnt allow to pass in data with GL_BGRA format
				
                    if (sourceCpuFormatEnum == GL_RGBA && dds.components == 4)
                    {
                        printf("PE: Texture Load: Swapping byte order in source pixel data\n");
				
                        unsigned char *data = dds.image[0].pixels[i];
                        for (int y = 0; y < h; y++)
                            for (int x = 0; x < w; x++)
                            {
                                unsigned char B = *data;
                                *data = *(data+2); //B <- R
                                *(data+2) = B; //R <- B
                                data += 4;
                            }
                    }

                    // we use componets == 3 becasue on IOS we dont have GL_BGR flag at all. event to use is as temp variable for storing dds format
                    if (sourceCpuFormatEnum == GL_RGB && (dds.components == 3 ) )
                    {
                        printf("PE: Texture Load: Swapping byte order in source pixel data\n");
                        unsigned char *data = dds.image[0].pixels[i];
                        for (int y = 0; y < h; y++)
                            for (int x = 0; x < w; x++)
                            {
                                unsigned char B = *data;
                                *data = *(data+2); //B <- R
                                *(data+2) = B; //R <- B
                                data += 3;
                            }
                    }
                }
			#endif

        if (w <= MAX_TEXTURE_LOAD_WIDTH && h <= MAX_TEXTURE_LOAD_WIDTH)    
			// see "Loading a Texture Image" in PSGL-Programming_Guide_e.pdf [94]
			glTexImage2D(
				// for OpenGL ES must be GL_TEXTURE_2D; in Gneeral OpenGL is GL_TEXTURE_2D, GL_PROXY_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, or GL_PROXY_TEXTURE_CUBE_MAP.

				//PSGL also allows cube maps
				GL_TEXTURE_2D, 

				i + mipOffset, // mip level >= 0; 0 - highest

				// how texels are stored in
				//in OpenGL ES: Specifies the color components in the texture. Must be same as format. 
				//how texels stored internally on GPU
				//GL ES: The following symbolic values are accepted: GL_ALPHA, GL_RGB, GL_RGBA, GL_LUMINANCE, or GL_LUMINANCE_ALPHA.
				//PSGL offers A LOT more. the notable ones are the ones storing floats and depth
				// we still want to store on gpu in the conventional format like GL_RGBA GL_RGB GL_LUMINANCE
				destApiFormat, 
				w,
				h,
				0, //in PSGL texture borders are not supported
				
				//how texels stored in input cpu data
				// in GL ES: GL_ALPHA GL_LUMINANCE_ALPHA GL_LUMINANCE GL_RGB GL_RGBA
				// + in PSGL: GL_DEPTH_COMPONENT GL_ABGR GL_ARGB_SCE GL_BGR GL_BGRA GL_RED GL_GREEN GL_BLUE 
				
				sourceCpuFormatEnum, 
				
				//how texels stored in input cpu data
				#if APIABSTRACTION_PS3
					//PS GL offers additional choices including GL_UNSIGNED_INT_8_8_8_8
					// artem: i always get slow performance warning when using GL_UNSIGNED_BYTE with 24 or 32 bit textures
					// I did not see color difference when using GL_UNSIGNED_BYTE vs GL_UNSIGNED_INT_8_8_8_8
					// first i thought slow performance was because of automatic endiannes swap but that does not seem to be the case (colors are the same)
					// so i use GL_UNSIGNED_INT_8_8_8_8 since doesnt give warning and is definitely faster
					dds.components == 4 ? GL_UNSIGNED_INT_8_8_8_8 : GL_UNSIGNED_BYTE,
				#else
					// GL ES offers this as only choice for this case. choices: GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_5_5_1, 
					GL_UNSIGNED_BYTE,
				#endif
					
				/*data */dds.image[0].pixels[i]);
			w/=2; h/=2;
			if (w == 0) w = 1;
			if (h == 0) h = 1;
		} 
		//restore to default
		if (sourceCpuFormatEnum==GL_RGB || sourceCpuFormatEnum==GL_LUMINANCE) glPixelStorei(GL_UNPACK_ALIGNMENT,4);
		#if APIABSTRACTION_IOS
			// in case we use OS GL provided option
			//if (sourceCpuFormatEnum==GL_BGR) glPixelStorei(GL_UNPACK_ALIGNMENT,4);
		#endif

	}

	assert(m_samplerState != SamplerState_INVALID);

	SamplerState &ss = SamplerStateManager::getInstance()->getSamplerState(m_samplerState);

	bool haveMips = dds.mips > 1;
	// Basic OpenGL texture state setup
	if(dds.mips==1)
	{
		
        if (dds.width > MAX_TEXTURE_LOAD_WIDTH && dds.height > MAX_TEXTURE_LOAD_WIDTH)
        {
            assert(!"Have only one mip that is bigger than max allowed size. so it was not created thus we can't make mipmaps on it. Change source of the image to be smaller or build mipmaps offline (in which case the higher mips will be just skipped)");
            genMips = false;
        }
       
        if (genMips)
		{
			// build our texture mipmaps
			GLenum res = gluBuild2DMipmaps(
				GL_TEXTURE_2D, 
				destApiFormat,
				dds.width, dds.height,
				sourceCpuFormatEnum,
				GL_UNSIGNED_BYTE, //both IOS GL ES and PS GL require this to be GL_UNSIGNED_BYTE
				dds.image[0].pixels[0] );
			printf("gluBuild2DMipmaps result: %d\n", res);
			#if !APIABSTRACTION_IOS
				if (res)
				{
					PEINFO("%s\n", (const char*)gluErrorString(res));
				}
			#endif
			haveMips = res == 0;
		}
	}
	else
	{
		// mip maps exist
	}

	if (haveMips)
	{
		//check that we want t use existing mips
		if (ss.needsMipMaps())
		{
			PEINFO("Warning: Mipmap filtering is disabled while mipams exist and are loaded. Texture: %s\n", filename);
		}
	}
	else
	{
		//check that we dont want a value that assumes mip map existance
		if (!ss.needsMipMaps())
		{
			PEINFO("Warning: Trying to enable mip map filtering when no mipmaps exist. Texture: %s\n", filename);
		}
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ss.val_GL_TEXTURE_MIN_FILTER);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ss.val_GL_TEXTURE_MAG_FILTER);
	
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ss.val_GL_TEXTURE_WRAP_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ss.val_GL_TEXTURE_WRAP_T);

	glFlush();

	free(dds.buffer); // at this point the file buffer is no longer needed

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;	// return the texture name
}

GLuint TextureGPU::gfxLoadDDSTextureCubeMap(char *filename)
{
	PEASSERT(false, "Not supported right now. Though code below probably works after compilation fixes");
	GLuint texture;

	DDS::DDSTextureInSystemMemory dds;
	uint32_t test =  loadDDSIntoSystemMemory(filename,&dds);
	if(test==false) 
	{
		printf("failed to load\n");
		exit(0);
	}

	glEnable(GL_TEXTURE_CUBE_MAP);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	int32_t w = dds.width;
	int32_t h = dds.height;

	printf("file %s dds surfaces %d mips %d w %d h%d \n",filename,dds.surfaces,dds.mips,w,h);
	if(dds.surfaces!=6)	//its not a cube map 
	{
		printf("ERROR: This is not a cube map\n");
		exit(0);
	}
    
#if APIABSTRACTION_IOS
    printf("ERROR: dont support compressed textures on IOS\n");
    exit(0);
#endif

#if 0   
	//	dds.mips =1; //set this to 1
	// loop through cubemap faces and load them as 2D textures 
	for (unsigned int n = 0; n < 6; n++)
	{
		GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + n;
		w = dds.width;
		h = dds.height;

		if(dds.apiFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT) printf("GL_COMPRESSED_RGB_S3TC_DXT1_EXT\n");
		if(dds.apiFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) printf("GL_COMPRESSED_RGBA_S3TC_DXT1_EXT\n");
		if(dds.apiFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT) printf("GL_COMPRESSED_RGBA_S3TC_DXT3_EXT\n");
		if(dds.apiFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) printf("GL_COMPRESSED_RGBA_S3TC_DXT5_EXT\n");

		//load a compressed texture
		if( (dds.apiFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT) ||
			(dds.apiFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ||
			(dds.apiFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT) ||
			(dds.apiFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT))
		{
			for (uint32_t i = 0; i < dds.mips; i++)
			{
				uint32_t size = DDS::getImageSize(w,h,dds.components,dds.apiFormat);
				//		printf("compressed texture %d %d %d %d 0x%x\n",i,w,h,size,dds.image[n].pixels[i]);
				glCompressedTexImage2D(target,i,dds.apiFormat,w,h,0,size,dds.image[n].pixels[i]); //changed n to 0
				w/=2; h/=2;
			}
		}
		else
		{
			//dont think I'll support uncompressed cubemaps....
			printf("Uncompressed cubemap not a good idea!!\n");
			exit(0);
		}

	}

#endif

	// Basic OpenGL texture state setup
	if(dds.mips==1)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  
	}
	else
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  

	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glDisable(GL_TEXTURE_CUBE_MAP);

	free(dds.buffer); // at this point the file buffer is no longer needed
	return texture;	// return the texture name
}
//----------------------------------------------------------------------------


}; // namespace PE
#endif
