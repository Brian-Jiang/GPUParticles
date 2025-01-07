
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
// Sibling includes
#include "Texture.h"
// Inter-Engine includes
#include "../../Utils/ErrorHandling.h"
#include "PrimeEngine/Math/CameraOps.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
namespace PE {
// note this is not implemented properly for all platforms
void TextureGPU::createColorTextureArrayGPU(const PrimitiveTypes::String textureFilenames[], PrimitiveTypes::UInt32 nTextures, const char *package)
{
	char wpathes[64][256];
	Array<char *> pathArr(*m_pContext, m_arena, 64);
	
	for (PrimitiveTypes::UInt32 iTex = 0; iTex < nTextures; iTex++)
	{
		PEString::generatePathname(*m_pContext, textureFilenames[iTex], package, "Textures", wpathes[iTex], 256);
		pathArr.add(wpathes[iTex]);
	}
	
#	if APIABSTRACTION_D3D9//Todo: create copy for cube map func
assert(!"Not Implemented!");
#if 0
	// retrieve device from IRenderer
	ID3D10Device *pD3DDevice = IRenderer::Instance()->m_device;

	ID3D10Texture2D *srcTex[64];
	for (PrimitiveTypes::UInt32 iTex = 0; iTex < nTextures; iTex++)
	{
		D3DX10_IMAGE_LOAD_INFO loadInfo;
		loadInfo.Width = D3DX10_FROM_FILE;
		loadInfo.Height = D3DX10_FROM_FILE;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = D3DX10_FROM_FILE;
		loadInfo.Usage = D3D10_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D10_CPU_ACCESS_WRITE |
			D3D10_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		loadInfo.Filter = D3DX10_FILTER_NONE;
		loadInfo.MipFilter = D3DX10_FILTER_NONE;
		loadInfo.pSrcInfo = 0;

		HRESULT hr = D3DX10CreateTextureFromFile(pD3DDevice, pathArr[iTex], &loadInfo, NULL, (ID3D10Resource**)&srcTex[iTex], NULL);
		assert(SUCCEEDED(hr));

	}

	D3D10_TEXTURE2D_DESC texElementDesc;
	srcTex[0]->GetDesc(&texElementDesc);

	D3D10_TEXTURE2D_DESC texArrayDesc;
		texArrayDesc.Width = texElementDesc.Width;
		texArrayDesc.Height = texElementDesc.Height;
		texArrayDesc.MipLevels = texElementDesc.MipLevels;
		texArrayDesc.ArraySize = nTextures;
		texArrayDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texArrayDesc.SampleDesc.Count = 1;
		texArrayDesc.SampleDesc.Quality = 0;
		texArrayDesc.Usage = D3D10_USAGE_DEFAULT;
		texArrayDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		texArrayDesc.CPUAccessFlags = 0;
		texArrayDesc.MiscFlags = 0;

	ID3D10Texture2D *texArray = 0;

	HRESULT hr = pD3DDevice->CreateTexture2D(&texArrayDesc, 0, &texArray);
	assert(SUCCEEDED(hr));

	for (PrimitiveTypes::UInt32 iTex = 0; iTex < nTextures; ++iTex)
	{
		for (PrimitiveTypes::UInt32 iMipLevel = 0; iMipLevel < texElementDesc.MipLevels; ++iMipLevel)
		{
			D3D10_MAPPED_TEXTURE2D mappedTex2D;
			srcTex[iTex]->Map(iMipLevel, D3D10_MAP_READ, 0, &mappedTex2D);
			pD3DDevice->UpdateSubresource(texArray,
				D3D10CalcSubresource(iMipLevel, iTex, texElementDesc.MipLevels),
				0, mappedTex2D.pData, mappedTex2D.RowPitch, 0);
			srcTex[iTex]->Unmap(iMipLevel);
		}
	}

	// create the view resource

	D3D10_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = nTextures;

	hr = pD3DDevice->CreateShaderResourceView(texArray, &viewDesc, &pTex->m_pShaderResourceView);
	assert(SUCCEEDED(hr));
	texArray->Release();

	for (PrimitiveTypes::UInt32 iTex = 0; iTex < nTextures; ++iTex)
	{
		srcTex[iTex]->Release();
	}
#endif
#	elif APIABSTRACTION_D3D11//Todo: create copy for cube map func
	assert(!"Not Implemented!");
#if 0
	// retrieve device from IRenderer
	ID3D11Device *pD3DDevice = IRenderer::Instance()->m_device;

	ID3D11Texture2D *srcTex[64];
	for (PrimitiveTypes::UInt32 iTex = 0; iTex < nTextures; iTex++)
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		loadInfo.Width = D3DX11_FROM_FILE;
		loadInfo.Height = D3DX11_FROM_FILE;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = D3DX11_FROM_FILE;
		loadInfo.Usage = D3D11_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE |
			D3D11_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		loadInfo.Filter = D3DX11_FILTER_NONE;
		loadInfo.MipFilter = D3DX11_FILTER_NONE;
		loadInfo.pSrcInfo = 0;

		HRESULT hr = D3DX11CreateTextureFromFileA(pD3DDevice, pathArr[iTex], &loadInfo, NULL, (ID3D11Resource**)&srcTex[iTex], NULL);
		assert(SUCCEEDED(hr));

	}

	D3D11_TEXTURE2D_DESC texElementDesc;
	srcTex[0]->GetDesc(&texElementDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
		texArrayDesc.Width = texElementDesc.Width;
		texArrayDesc.Height = texElementDesc.Height;
		texArrayDesc.MipLevels = texElementDesc.MipLevels;
		texArrayDesc.ArraySize = nTextures;
		texArrayDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texArrayDesc.SampleDesc.Count = 1;
		texArrayDesc.SampleDesc.Quality = 0;
		texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
		texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texArrayDesc.CPUAccessFlags = 0;
		texArrayDesc.MiscFlags = 0;

	ID3D11Texture2D *texArray = 0;

	HRESULT hr = pD3DDevice->CreateTexture2D(&texArrayDesc, 0, &texArray);
	assert(SUCCEEDED(hr));

	for (PrimitiveTypes::UInt32 iTex = 0; iTex < nTextures; ++iTex)
	{
		for (PrimitiveTypes::UInt32 iMipLevel = 0; iMipLevel < texElementDesc.MipLevels; ++iMipLevel)
		{
			//D3D11_MAPPED_TEXTURE2D mappedTex2D;
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			
			// TODO: srcTex[iTex]->Map(iMipLevel, D3D11_MAP_READ, 0, &mappedTex2D);
			/*
			pD3DDevice->UpdateSubresource(texArray,
				D3D11CalcSubresource(iMipLevel, iTex, texElementDesc.MipLevels),
				0, mappedTex2D.pData, mappedTex2D.RowPitch, 0);
			*/
			// TODO: srcTex[iTex]->Unmap(iMipLevel);
		}
	}

	// create the view resource

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = nTextures;

	hr = pD3DDevice->CreateShaderResourceView(texArray, &viewDesc, &m_pShaderResourceView);
	assert(SUCCEEDED(hr));
	texArray->Release();

	for (PrimitiveTypes::UInt32 iTex = 0; iTex < nTextures; ++iTex)
	{
		srcTex[iTex]->Release();
	}
#endif
#	endif

	pathArr.reset(0);
	m_family = TextureFamily::COLOR_MAP_ARRAY;

}

}; // namespace PE
