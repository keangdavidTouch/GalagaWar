#pragma once

struct RENDERTARGET_DESC
{
public:
	int Width;
	int Height;

	//Enable this if you want to create a DepthStencilBuffer
	bool EnableDepthBuffer;
	//Enable this if you want to use the DepthStencilBuffer
	//as a ShaderResourceView (Texture in a shader)
	bool EnableDepthSRV;
	//Enable this if you want to create a RenderTarget (color)
	bool EnableColorBuffer;
	//Enable this if you want to use the RenderTarget
	//as a ShaderResourceView (Texture in a shader)
	bool EnableColorSRV;
	//Generate MipMaps (ColorSRV Only)
	bool GenerateMipMaps_Color;

	//Depth buffer format (DepthStencil)
	DXGI_FORMAT DepthFormat;
	//Color buffer format (Rendertarget)
	DXGI_FORMAT ColorFormat;

	//Optional: Supply one of the buffers
	ID3D11Texture2D* pColor;
	bool ColorBufferSupplied;
	ID3D11Texture2D* pDepth;
	bool DepthBufferSupplied;

	RENDERTARGET_DESC() :
		Width(-1),
		Height(-1),

		EnableDepthBuffer(true),
		EnableDepthSRV(false),
		EnableColorBuffer(true),
		EnableColorSRV(false),
		GenerateMipMaps_Color(false),

		DepthFormat(DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT),
		ColorFormat(DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM),

		pColor(nullptr),
		pDepth(nullptr)
	{}

	void IsValid()
	{
		if (!(pColor || pDepth) && (Width <= 0 || Height <= 0))
			Logger::LogError(L"RenderTarget::IsValid() > Invalid Width and/or Height!");

		if (!(EnableColorBuffer || EnableDepthBuffer))
		{
			Logger::LogError(L"RenderTarget::IsValid() > Rendertarget must contain at least one buffer! (Color AND/OR Depth)");
		}

		if (EnableDepthSRV && !EnableDepthBuffer)
		{
			EnableDepthBuffer = true;
			Logger::LogWarning(L"RenderTarget::IsValid() > Forced \'EnableDepthBuffer\' flag because \'EnableDepthSRV\' is enabled by user.");
		}

		if (EnableColorSRV && !EnableColorBuffer)
		{
			EnableColorBuffer = true;
			Logger::LogWarning(L"RenderTarget::IsValid() > Forced \'EnableColorBuffer\' flag because \'EnableColorSRV\' is enabled by user.");
		}
	}
};

class RenderTarget final
{
public:
	RenderTarget(ID3D11Device* pDevice);
	~RenderTarget(void);

	RenderTarget(const RenderTarget& other) = delete;
	RenderTarget(RenderTarget&& other) noexcept = delete;
	RenderTarget& operator=(const RenderTarget& other) = delete;
	RenderTarget& operator=(RenderTarget&& other) noexcept = delete;

	HRESULT Create(RENDERTARGET_DESC desc);
	ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }
	bool HasRenderTargetView() const { return m_pRenderTargetView != nullptr; }
	ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView; }
	ID3D11ShaderResourceView* GetShaderResourceView() const;
	ID3D11ShaderResourceView* GetDepthShaderResourceView() const;

	bool HasColorSRV() const { return m_Desc.EnableColorSRV; }
	bool HasDepthSRV() const { return m_Desc.EnableDepthSRV; }

	// Update PP
	void Clear(const GameContext& gameContext, const FLOAT colorRGBA[4]) const;

private:

	RENDERTARGET_DESC m_Desc;
	ID3D11Device* m_pDevice = nullptr;
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
	ID3D11ShaderResourceView* m_pColorShaderResourceView = nullptr;
	ID3D11ShaderResourceView* m_pDepthShaderResourceView = nullptr;
	ID3D11Texture2D* m_pColor = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
	ID3D11Texture2D* m_pDepth = nullptr;

	HRESULT CreateColor();
	HRESULT CreateDepth();
	static DXGI_FORMAT GetDepthResourceFormat(DXGI_FORMAT initFormat);
	static DXGI_FORMAT GetDepthSRVFormat(DXGI_FORMAT initFormat);
	void CleanUp();

	bool m_ColorBufferSupplied = false;
	bool m_DepthBufferSupplied = false;
};

