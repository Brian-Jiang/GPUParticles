
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
#if 1
void TextureGPU::createDrawableIntoDepthTexture(PrimitiveTypes::UInt32 w, PrimitiveTypes::UInt32 h, ESamplerState sampler)
{
	StringOps::writeToString("DrawableIntoDepthTexture", m_name, 256);

#if PE_PLAT_IS_PSVITA
	createDrawableIntoColorTextureWithDepth(w, h, sampler, true); // todo: is there a way to have pure depth texture?
#elif APIABSTRACTION_D3D9
	createDrawableIntoColorTextureWithDepth(w, h, sampler, true); // todo: is there a way to have pure depth texture?
#elif APIABSTRACTION_OGL
	createDrawableIntoColorTextureWithDepth(w, h, sampler, true); // todo: is there a way to have pure depth texture?
#elif APIABSTRACTION_D3D11
	m_samplerState = sampler;

	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = w;
	m_viewport.Height = h;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;


	ID3D11Texture2D *pDepthMap = 0;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = w;
	texDesc.Height = h;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL |
		D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	HRESULT hr = pDevice->CreateTexture2D(&texDesc, 0, &pDepthMap);
	assert(SUCCEEDED(hr));

	// setting up view for rendering itno depth buffer/and reading (stenciling) from it (z-buffer algorithm red/write)
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = 0;//D3D11_DSV_READ_ONLY_DEPTH;

	hr = pDevice->CreateDepthStencilView(pDepthMap, &dsvDesc, &m_DepthStencilView);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = pDevice->CreateShaderResourceView(pDepthMap, &srvDesc, &m_pDepthShaderResourceView);
	assert(SUCCEEDED(hr));

	pDepthMap->Release();
#endif
}
#endif

}; // namespace PE
