#include "stdafx.h"
#include "PostProcessingMaterial.h"
#include "RenderTarget.h"
#include "OverlordGame.h"
#include "ContentManager.h"

PostProcessingMaterial::PostProcessingMaterial(std::wstring effectFile, unsigned int renderIndex,
                                               std::wstring technique)
	: m_IsInitialized(false), 
	  m_pInputLayout(nullptr),
	  m_pInputLayoutSize(0),
	  m_effectFile(std::move(effectFile)),
	  m_InputLayoutID(0),
	  m_RenderIndex(renderIndex),
	  m_pRenderTarget(nullptr),
	  m_pVertexBuffer(nullptr),
	  m_pIndexBuffer(nullptr),
	  m_NumVertices(0),
	  m_NumIndices(0),
	  m_VertexBufferStride(0),
	  m_pEffect(nullptr),
	  //m_pTechnique(nullptr),
	  m_TechniqueName(std::move(technique))
{
	
}

PostProcessingMaterial::~PostProcessingMaterial()
{
	//TODO: delete and/or release necessary objects and/or resources
	SafeRelease(m_pInputLayout);
	SafeDelete(m_pRenderTarget);
	SafeRelease(m_pIndexBuffer);
	SafeRelease(m_pVertexBuffer);
}

void PostProcessingMaterial::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	if (!m_IsInitialized)
	{
		//TODO: complete
		//1. LoadEffect (LoadEffect(...))
		LoadEffect(gameContext, m_effectFile);
		//2. CreateInputLaytout (CreateInputLayout(...))
		EffectHelper::BuildInputLayout(gameContext.pDevice, m_pTechnique, &m_pInputLayout, m_pInputLayoutDescriptions, m_pInputLayoutSize, m_InputLayoutID);
		/*	EffectHelper::BuildInputLayout(gameContext.pDevice, m_pTechnique[0], &m_pInputLayout, m_pInputLayoutDescriptions, m_pInputLayoutSize, m_InputLayoutID);
			EffectHelper::BuildInputLayout(gameContext.pDevice, m_pTechnique[1], &m_pInputLayout, m_pInputLayoutDescriptions, m_pInputLayoutSize, m_InputLayoutID);*/
		//   CreateVertexBuffer (CreateVertexBuffer(...)) > As a TriangleStrip (FullScreen Quad)
		CreateVertexBuffer(gameContext);
		CreateIndexBuffer(gameContext);
		//3. Create RenderTarget (m_pRenderTarget)
		m_pRenderTarget = new RenderTarget(gameContext.pDevice);
		RENDERTARGET_DESC rtDesc;
		const auto windowSettings = OverlordGame::GetGameSettings().Window;
		rtDesc.Width = windowSettings.Width;
		rtDesc.Height = windowSettings.Height;
		rtDesc.EnableColorBuffer = true;
		rtDesc.EnableColorSRV = true;
		rtDesc.EnableDepthBuffer = true;
		rtDesc.EnableDepthSRV = true;
		rtDesc.GenerateMipMaps_Color = true;
		m_pRenderTarget->Create(rtDesc);
		m_IsInitialized = true;
	}

	m_pLUTTexture = ContentManager::Load<TextureData>(L"./Resources/Textures/lut_default.png");
}

bool PostProcessingMaterial::LoadEffect(const GameContext& /*gameContext*/, const std::wstring& effectFile)
{
	//TODO: complete
	//Load Effect through ContentManager
	m_pEffect = ContentManager::Load<ID3DX11Effect>(effectFile);
	//Check if m_TechniqueName (default constructor parameter) is set
	if (!m_TechniqueName.empty()) {
		// If SET > Use this Technique (+ check if valid)
		auto techString = std::string(m_TechniqueName.begin(), m_TechniqueName.end());
		m_pTechnique = m_pEffect->GetTechniqueByName(techString.c_str());
		//m_pTechnique[0] = m_pEffect->GetTechniqueByIndex(0);
		//m_pTechnique[1] = m_pEffect->GetTechniqueByIndex(1);
	}
	else {
		// If !SET > Use Technique with index 0
		m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
		//m_pTechnique[0] = m_pEffect->GetTechniqueByIndex(0);
		//m_pTechnique[1] = m_pEffect->GetTechniqueByIndex(1);
	}

	//Call LoadEffectVariables
	LoadEffectVariables();
	//LUT
	auto effectVar = GetEffect()->GetVariableByName("gLUT");
	m_pLUTTextureVariable = (effectVar->IsValid()) ? effectVar->AsShaderResource() : nullptr;

	return true;
}

void PostProcessingMaterial::Draw(const GameContext& gameContext, RenderTarget* previousRenderTarget)
{
	////TODO: complete
	//1. Clear the object's RenderTarget (m_pRenderTarget) [Check RenderTarget Class]
	m_pRenderTarget->Clear(gameContext, DirectX::Colors::MistyRose);
	//2. Call UpdateEffectVariables(...)
	UpdateEffectVariables(previousRenderTarget);
	if(m_pLUTTextureVariable)
		m_pLUTTextureVariable->SetResource(m_pLUTTexture->GetShaderResourceView());

	//3. Set InputLayout
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);
	//4. Set VertexBuffer
	unsigned int offset = 0;
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &offset);
	gameContext.pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, offset);
	//5. Set PrimitiveTopology (TRIANGLELIST)
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//6. Draw
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p) {
		m_pTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
	
	/*D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique[0]->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p) {
		m_pTechnique[0]->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}*/

	// Generate Mips
	gameContext.pDeviceContext->GenerateMips(m_pRenderTarget->GetShaderResourceView());

	//D3DX11_TECHNIQUE_DESC techDesc2;
	//m_pTechnique[1]->GetDesc(&techDesc2);
	//for (UINT p = 0; p < techDesc2.Passes; ++p) {
	//	m_pTechnique[1]->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
	//	gameContext.pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	//}

	//// Generate Mips
	//gameContext.pDeviceContext->GenerateMips(m_pRenderTarget->GetShaderResourceView());
}

void PostProcessingMaterial::CreateVertexBuffer(const GameContext& gameContext)
{
	m_NumVertices = 4;

	//TODO: complete
	//Create vertex array containing three elements in system memory
	using namespace DirectX;
	float uMin = 0;
	float uMax = 1;
	float vMin = 0;
	float vMax = 1;
	VertexPosTex verticesArr[4]
	{
		{ XMFLOAT3(-1, -1, 0.5f), XMFLOAT2(uMin, vMax)},
		{ XMFLOAT3(-1, 1, 0.5f),XMFLOAT2(uMin, vMin)},
		{ XMFLOAT3(1, 1, 0.5f), XMFLOAT2(uMax, vMin)},
		{ XMFLOAT3(1, -1, 0.5f), XMFLOAT2(uMax, vMax)},
	};
	//fill a buffer description to copy the vertexdata into graphics memory
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D10_BIND_FLAG::D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(VertexPosTex) * m_NumVertices;
	vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_FLAG::D3D10_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	m_VertexBufferStride = sizeof(VertexPosTex);
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = verticesArr;
	//create a ID3D10Buffer in graphics memory containing the vertex info
	const auto result = gameContext.pDevice->CreateBuffer(&vertexBufferDesc, &initData, &m_pVertexBuffer);
	Logger::LogHResult(result, L"PostProcessingMaterial::CreateVertexBuffer(...)");
}

void PostProcessingMaterial::CreateIndexBuffer(const GameContext& gameContext)
{
	m_NumIndices = 6;

	// Create index buffer
	DWORD indicesArr[6] = { 0,1,2,0,2,3 };
	D3D11_BUFFER_DESC indexBufDesc;
	indexBufDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.ByteWidth = sizeof(DWORD) * m_NumIndices;
	indexBufDesc.CPUAccessFlags = D3D10_CPU_ACCESS_FLAG::D3D10_CPU_ACCESS_WRITE;
	indexBufDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	indexBufDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = indicesArr;
	const auto result =  gameContext.pDevice->CreateBuffer(&indexBufDesc, &initData, &m_pIndexBuffer);
	Logger::LogHResult(result, L"PostProcessingMaterial::CreateIndexBuffer(...)");
}
