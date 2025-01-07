
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
// Sibling includes
#include "Texture_DDS_Loader_D3D.h"
#include "Texture.h"
#include "PrimeEngine/Render/IRenderer.h"

// Inter-Engine includes
#include "../../Utils/ErrorHandling.h"
#include "PrimeEngine/FileSystem/FileReader.h"

namespace PE {
#if APIABSTRACTION_D3D9
IDirect3DTexture9 * 
#else
ID3D11Texture2D * 
#endif
TextureGPU::gfxLoadDDSTexture(char *filename, bool genMips /*= true*/)
{	
	DDS::DDSTextureInSystemMemory dds;
	uint32_t test =  loadDDSIntoSystemMemory(filename,&dds);
	if(test==false) 
	{
		printf("failed to load\n");
		exit(0);
	}
	
	bool haveMips = dds.mips > 1;
	bool needToGenerateMips = !haveMips && genMips;
	bool willHaveMips = haveMips || genMips;

	if (false)
	{
		int32_t w = dds.width;
		int32_t h = dds.height;

		for (uint32_t i = 0; i < dds.mips; i++)
		{
			unsigned char r = i % 3 ? 0x00 : 0xFF;
			unsigned char g = (i+2) % 3 ? 0x00 : 0xFF;
			unsigned char b = (i+1) % 3 ? 0x00 : 0xFF;

		
			if (dds.uncompressedFormat && dds.components == 4)
			{	
				unsigned char *data = dds.image[0].pixels[i];
				for (int y = 0; y < h; y++)
					for (int x = 0; x < w; x++)
					{
						unsigned char solid = ((x % 8) > 3) ^ ((y % 8) > 3); 
						*data++ = b * solid;
						*data++ = g * solid;
						*data++ = r * solid;
						*data++ = 0;
					}
			}

			// we use componets == 3 becasue on IOS we dont have GL_BGR flag at all. event to use is as temp variable for storing dds format
			if (dds.uncompressedFormat && dds.components == 3)
			{
				unsigned char *data = dds.image[0].pixels[i];
				for (int y = 0; y < h; y++)
					for (int x = 0; x < w; x++)
					{
						*data++ = r;
						*data++ = g;
						*data++ = b;
					}
			}
			w/=2; h/=2;
			if (w == 0) w = 1;
			if (h == 0) h = 1;
		}
	}

	assert(m_samplerState != SamplerState_INVALID);

	SamplerState &ss = SamplerStateManager::getInstance()->getSamplerState(m_samplerState);

	if (willHaveMips)
	{
		//check that we want t use existing mips
		if (!ss.needsMipMaps())
		{
			PEWARN("Mipmap filtering is disabled while mipams exist and are loaded. Texture: %s", filename);
		}
	}
	else
	{
		//check that we dont want a value that assumes mip map existance
		if (ss.needsMipMaps())
		{
			PEWARN("Trying to enable mip map filtering when no mipmaps exist. Texture: %s", filename);
		}
	}
	
	int32_t w = dds.width;
	int32_t h = dds.height;

	#if APIABSTRACTION_D3D9

	D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
	LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;

	//choose api format best suited for loaded texture
	D3DFORMAT destApiFormat = D3DFMT_UNKNOWN;
	if (dds.compressedFormat)
	{
		assert(!"Dont support compressed textures yet!");
	}
	else
	{
		if (dds.components == 1)
		{
			if (dds.uncompressedFormat == DDSF_L)
				destApiFormat = D3DFMT_L8;
			else
				assert(!"Unknown format!");
		}
		else if (dds.components == 3)
		{
			assert(!"Need to add support for this format!");
		}
		else if (dds.components == 4)
		{
			if (dds.uncompressedFormat == DDSF_RGBA)
				destApiFormat = D3DFMT_A8R8G8B8;
			else if  (dds.uncompressedFormat == DDSF_RGB)
				destApiFormat = D3DFMT_X8R8G8B8;
		}
		else
		{
			assert(!"Unknown texture format!");
		}
	}

	if (!destApiFormat)
		return NULL;

	IDirect3DTexture9 * pTexture = NULL;
	
	pDevice->CreateTexture(
		dds.width,
		dds.height,
		/*Levels = 0 - generate space for all mips, otherwise use provided number*/
		haveMips ? 0 : 1, // if we have source mipmaps set to 0 (will have mips) otherwise set to 1 becasue mips will be generated on gpu memory texture)
		0, // potentially could use D3DUSAGE_AUTOGENMIPMAP on a gpu texture, not on SYSTEMMEM, 
		destApiFormat,
		D3DPOOL_SYSTEMMEM, // set it to sys mem to be able to set pixels // D3DPOOL_DEFAULT,
		&pTexture, // result
		NULL); // set to null	

	printf("file %s dds surfaces %d mips %d w %d h %d \n",filename,dds.surfaces,dds.mips,w,h);
	if(dds.surfaces!=1)	//its a cube map 
	{
		printf("ERROR: this is a cubemap\n");
		exit(0);
	}

	if(dds.compressedFormat == FOURCC_DXT1) printf("FOURCC_DXT1\n");
	if(dds.compressedFormat == FOURCC_DXT3) printf("FOURCC_DXT3\n");
	if(dds.compressedFormat == FOURCC_DXT5) printf("FOURCC_DXT5\n");

	D3DFORMAT destGpuFormatEnum = (D3DFORMAT)(DDS::getDestGPUFormatFromComponents(dds.components));

	D3DFORMAT sourceCpuFormatEnum = (D3DFORMAT)(DDS::getSourceCPUFormatFromComponents(dds.components));

	//load a compressed texture
	if(dds.compressedFormat)
	{
		assert(!"Compressed textures are not yet supported!");
	}
	else // load a non compressed texture
	{

		for (uint32_t i = 0; i < dds.mips; i++)
		{
			IDirect3DSurface9 *pSurface = NULL;
			HRESULT hr = pTexture->GetSurfaceLevel(i, &pSurface);
			assert(SUCCEEDED(hr));

			D3DLOCKED_RECT lockedRect;
			hr = pSurface->LockRect(&lockedRect, NULL, 0);
			assert(SUCCEEDED(hr));

			unsigned char *data = dds.image[0].pixels[i];
			char *destData = (char*)(lockedRect.pBits);
			for (int y = 0; y < h; y++)
			{
				memcpy(&destData[lockedRect.Pitch * y], &data[dds.components * w * y], dds.components * w);
            }
			hr = pSurface->UnlockRect();
			assert(SUCCEEDED(hr));

			w/=2; h/=2;
			if (w == 0) w = 1;
			if (h == 0) h = 1;
		} 
		//restore to default
	}

	free(dds.buffer); // at this point the file buffer is no longer needed

	int d3dUsage = 0;

	d3dUsage = needToGenerateMips ? D3DUSAGE_AUTOGENMIPMAP: 0; // potentially could use D3DUSAGE_AUTOGENMIPMAP, but we do not want to support it now, since we only support mipmaps that are in dds already

	//create GPU texture that wills tore the data
	IDirect3DTexture9 *pDestTexture = NULL;
	pDevice->CreateTexture(
		dds.width,
		dds.height,
		willHaveMips ? 0 : 1, // 0: will create mipmap surfaces too
		d3dUsage,
		destApiFormat,
		D3DPOOL_DEFAULT,
		&pDestTexture, // result
		NULL); // set to null	

	pDevice->UpdateTexture(pTexture, pDestTexture);

	pTexture->Release(); // release sys memory texture

	return pDestTexture;	// return the texture name

#else
	//choose api format best suited for loaded texture
	DXGI_FORMAT destApiFormat = DXGI_FORMAT_UNKNOWN;
	if (dds.compressedFormat)
	{
		assert(!"Dont support compressed textures yet!");
	}
	else
	{
		if (dds.components == 1)
		{
			if (dds.uncompressedFormat == DDSF_L)
				destApiFormat = DXGI_FORMAT_R8_UNORM;
			else
				assert(!"Unknown format!");
		}
		else if (dds.components == 3)
		{
			assert(!"Need to add support for this format!");
		}
		else if (dds.components == 4)
		{
			if (dds.uncompressedFormat == DDSF_RGBA)
				destApiFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
			else if  (dds.uncompressedFormat == DDSF_RGB)
				destApiFormat = DXGI_FORMAT_B8G8R8X8_UNORM;
		}
		else
		{
			assert(!"Unknown texture format!");
		}
	}

	if (!destApiFormat)
		return NULL;

	ID3D11Texture2D* pTexture = NULL;
	
	D3D11_TEXTURE2D_DESC desc;
    desc.Width = dds.width;
    desc.Height = dds.height;
    desc.MipLevels = willHaveMips ? 0 : 1; 
    desc.ArraySize = 1;
    desc.Format = destApiFormat;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_IMMUTABLE; // immutable since data wont change
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA datas[32];
	//in case mips are already loaded (and not generated) we need to pass in array of the datas
	for (uint32_t i = 0; i < dds.mips; i++)
	{
		D3D11_SUBRESOURCE_DATA &data = datas[i];
		data.pSysMem = dds.image[0].pixels[/*mip=*/i];
		data.SysMemPitch = dds.components * w;
		data.SysMemSlicePitch = 0; // unused since 2d texture

		w/=2; h/=2;
		if (w == 0) w = 1;
		if (h == 0) h = 1;
	}

	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;


	pDevice->CreateTexture2D(
		&desc,
		datas,
		&pTexture);
	
	return pTexture;
#endif

}

}; // namespace PE
