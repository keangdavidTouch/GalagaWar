//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(ID3D11Device* pDevice) :
	m_pDevice(pDevice)
{}

RenderTarget::~RenderTarget(void)
{
	CleanUp();
}

void RenderTarget::CleanUp()
{
	//if (!m_ColorBufferSupplied)
	SafeRelease(m_pColor);

	//if (!m_DepthBufferSupplied)
	SafeRelease(m_pDepth);

	SafeRelease(m_pRenderTargetView);
	SafeRelease(m_pColorShaderResourceView);
	SafeRelease(m_pDepthShaderResourceView);
	SafeRelease(m_pDepthStencilView);
}

HRESULT RenderTarget::Create(RENDERTARGET_DESC desc)
{
	CleanUp();

	//Check if input is valid
	desc.IsValid();
	m_Desc = desc;

	m_ColorBufferSupplied = m_Desc.pColor != nullptr;
	m_DepthBufferSupplied = m_Desc.pDepth != nullptr;

	auto hr = CreateColor();
	if (FAILED(hr))
		return hr;

	hr = CreateDepth();
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT RenderTarget::CreateColor()
{
	if (m_Desc.pColor)
	{
		m_pColor = m_Desc.pColor;

		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));

		m_pColor->GetDesc(&texDesc);
		m_Desc.Width = texDesc.Width;
		m_Desc.Height = texDesc.Height;
		m_Desc.ColorFormat = texDesc.Format;

		m_Desc.EnableColorSRV = (texDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE) == D3D11_BIND_SHADER_RESOURCE;
	}

	if (m_Desc.EnableColorBuffer)
	{
		if (m_pColor == nullptr)
		{
			//RESOURCE
			D3D11_TEXTURE2D_DESC textureDesc;
			ZeroMemory(&textureDesc, sizeof(textureDesc));

			textureDesc.Width = m_Desc.Width;
			textureDesc.Height = m_Desc.Height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = m_Desc.ColorFormat;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | ((m_Desc.EnableColorSRV) ? D3D11_BIND_SHADER_RESOURCE : 0);
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = ((m_Desc.GenerateMipMaps_Color) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0);

			auto hr = m_pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pColor);
			if (FAILED(hr))
				return hr;

			m_Desc.pColor = m_pColor;
		}

		//RENDERTARGET SRV
		auto hr = m_pDevice->CreateRenderTargetView(m_pColor, nullptr, &m_pRenderTargetView);
		if (FAILED(hr))
			return hr;

		//SHADER SRV
		if (m_Desc.EnableColorSRV)
		{
			hr = m_pDevice->CreateShaderResourceView(m_pColor, nullptr, &m_pColorShaderResourceView);
			if (FAILED(hr))
				return hr;
		}
	}
	else
	{
		SafeRelease(m_pColor);
		SafeRelease(m_pRenderTargetView);
		SafeRelease(m_pColorShaderResourceView);
	}

	return S_OK;
}

DXGI_FORMAT RenderTarget::GetDepthResourceFormat(DXGI_FORMAT initFormat)
{
	DXGI_FORMAT resourceFormat = {};
	switch (initFormat)
	{
	case DXGI_FORMAT::DXGI_FORMAT_D16_UNORM:
		resourceFormat = DXGI_FORMAT::DXGI_FORMAT_R16_TYPELESS;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT:
		resourceFormat = DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT:
		resourceFormat = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		resourceFormat = DXGI_FORMAT::DXGI_FORMAT_R32G8X24_TYPELESS;
		break;
	default:
		Logger::LogError(L"RenderTarget::GetDepthResourceFormat(...) > Format not supported!");
		break;
	}

	return resourceFormat;
}

DXGI_FORMAT RenderTarget::GetDepthSRVFormat(DXGI_FORMAT initFormat)
{
	DXGI_FORMAT srvFormat = {};
	switch (initFormat)
	{
	case DXGI_FORMAT::DXGI_FORMAT_D16_UNORM:
		srvFormat = DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT:
		srvFormat = DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT:
		srvFormat = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		srvFormat = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		break;
	default:
		Logger::LogError(L"RenderTarget::GetDepthSRVFormat(...) > Format not supported!");
		break;
	}

	return srvFormat;
}

HRESULT RenderTarget::CreateDepth()
{
	if (m_Desc.pDepth)
	{
		m_pDepth = m_Desc.pDepth;

		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));

		m_pDepth->GetDesc(&texDesc);
		m_Desc.Width = texDesc.Width;
		m_Desc.Height = texDesc.Height;
		m_Desc.DepthFormat = texDesc.Format;
	}

	if (m_Desc.EnableDepthBuffer)
	{
		if (m_pDepth == nullptr)
		{
			//RESOURCE
			D3D11_TEXTURE2D_DESC textureDesc;
			ZeroMemory(&textureDesc, sizeof(textureDesc));

			textureDesc.Width = m_Desc.Width;
			textureDesc.Height = m_Desc.Height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = GetDepthResourceFormat(m_Desc.DepthFormat);
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | ((m_Desc.EnableDepthSRV) ? D3D11_BIND_SHADER_RESOURCE : 0);
			textureDesc.MiscFlags = 0;

			auto hr = m_pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pDepth);
			if (FAILED(hr))
				return hr;

			m_Desc.pDepth = m_pDepth;
		}

		//DEPTHSTENCIL VIEW
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));

		descDSV.Format = m_Desc.DepthFormat;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

		auto hr = m_pDevice->CreateDepthStencilView(m_pDepth, &descDSV, &m_pDepthStencilView);
		if (FAILED(hr))
			return hr;

		//SHADER SRV
		if (m_Desc.EnableDepthSRV)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC depthSrvDesc;
			ZeroMemory(&depthSrvDesc, sizeof(depthSrvDesc));

			depthSrvDesc.Format = GetDepthSRVFormat(m_Desc.DepthFormat);
			depthSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			depthSrvDesc.Texture2D.MipLevels = 1;
			depthSrvDesc.Texture2D.MostDetailedMip = 0;

			hr = m_pDevice->CreateShaderResourceView(m_pDepth, &depthSrvDesc, &m_pDepthShaderResourceView);
			if (FAILED(hr))
				return hr;
		}
	}
	else
	{
		SafeRelease(m_pColor);
		SafeRelease(m_pRenderTargetView);
		SafeRelease(m_pColorShaderResourceView);
	}

	return S_OK;
}

ID3D11ShaderResourceView* RenderTarget::GetShaderResourceView() const
{
	if (!m_Desc.EnableColorSRV)
		Logger::LogError(L"RenderTarget::GetShaderResourceView(...) > No COLOR SRV created during Creation. (Make sure to enable \'EnableColorSRV\' during RT Creation)");

	return m_pColorShaderResourceView;
}

ID3D11ShaderResourceView* RenderTarget::GetDepthShaderResourceView() const
{
	if (!m_Desc.EnableDepthSRV)
		Logger::LogError(L"RenderTarget::GetDepthShaderResourceView(...) > No DEPTH SRV created during Creation. (Make sure to enable \'EnableDepthSRV\' during RT Creation)");

	return m_pDepthShaderResourceView;
}

void RenderTarget::Clear(const GameContext& gameContext, const FLOAT colorRGBA[4]) const
{
	if (m_Desc.EnableColorBuffer)
		gameContext.pDeviceContext->ClearRenderTargetView(GetRenderTargetView(), colorRGBA);

	if (m_Desc.EnableDepthBuffer)
		gameContext.pDeviceContext->ClearDepthStencilView(GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
