#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "ContentManager.h"
#include "ShadowMapMaterial.h"
#include "RenderTarget.h"
#include "MeshFilter.h"
#include "SceneManager.h"
#include "OverlordGame.h"

ShadowMapRenderer::~ShadowMapRenderer()
{
	//TODO: make sure you don't have memory leaks and/or resource leaks :) -> Figure out if you need to do something here
	delete m_pShadowMat;
	delete m_pShadowRT;
}

void ShadowMapRenderer::Initialize(const GameContext& gameContext)
{
	if (m_IsInitialized)
		return;

	//TODO: create shadow generator material + initialize it
	m_pShadowMat = new ShadowMapMaterial();
	m_pShadowMat->Initialize(gameContext);
	//TODO: create a rendertarget with the correct settings (hint: depth only) for the shadow generator using a RENDERTARGET_DESC
	m_pShadowRT = new RenderTarget(gameContext.pDevice);

	RENDERTARGET_DESC rtDesc;
	const auto windowSettings = OverlordGame::GetGameSettings().Window;
	rtDesc.Width = windowSettings.Width;
	rtDesc.Height = windowSettings.Height;
	rtDesc.EnableDepthBuffer = true;
	rtDesc.EnableDepthSRV = true;
	rtDesc.EnableColorBuffer = false;
	rtDesc.EnableColorSRV = false;
	m_pShadowRT->Create(rtDesc);
	
	m_IsInitialized = true;
}

void ShadowMapRenderer::SetLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction)
{
	//TODO: store the input parameters in the appropriate datamembers
	m_LightPosition = position;
	m_LightDirection = direction;
	//TODO: calculate the Light VP matrix (Directional Light only ;)) and store it in the appropriate datamember
	using namespace DirectX;

	// Why m_Size????
	const auto windowSettings = OverlordGame::GetGameSettings().Window;
	const float width = (m_Size > 0) ? windowSettings.AspectRatio * m_Size : windowSettings.Width;
	const float height = (m_Size > 0) ? m_Size : windowSettings.Height;

	DirectX::XMMATRIX projection = DirectX::XMMatrixOrthographicLH(width, height, 0.1f, 2500.0f); //Perspective: (DirectX::XM_PIDIV4, windowSettings.AspectRatio ,0.1f, 2500.0f);
	DirectX::XMVECTOR lightPos = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR lightDir = DirectX::XMLoadFloat3(&direction);
	auto up = XMFLOAT3(0,1,0);
	DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);
	DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(lightPos , lightPos + lightDir, upVec);
	XMStoreFloat4x4(&m_LightVP, view * projection);
}

void ShadowMapRenderer::Begin(const GameContext& gameContext) const
{
	//Reset Texture Register 5 (Unbind)
	ID3D11ShaderResourceView *const pSRV[] = { nullptr };
	gameContext.pDeviceContext->PSSetShaderResources(1, 1, pSRV);

	//TODO: set the appropriate render target that our shadow generator will write to (hint: use the OverlordGame::SetRenderTarget function through SceneManager)
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(m_pShadowRT);
	//TODO: clear this render target
	//gameContext.pDeviceContext->ClearRenderTargetView(m_pShadowRT->GetRenderTargetView(), nullptr);
	m_pShadowRT->Clear(gameContext, DirectX::Colors::White);
	//TODO: set the shader variables of this shadow generator material
	m_pShadowMat->SetLightVP(m_LightVP);
}

void ShadowMapRenderer::End(const GameContext& gameContext) const
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: restore default render target (hint: passing nullptr to OverlordGame::SetRenderTarget will do the trick)
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(nullptr);
}

void ShadowMapRenderer::Draw(const GameContext& gameContext, MeshFilter* pMeshFilter, DirectX::XMFLOAT4X4 world, const std::vector<DirectX::XMFLOAT4X4>& bones) const
{
	auto index = (pMeshFilter->m_HasAnimations) ? m_pShadowMat->Skinned : m_pShadowMat->Static;

	//TODO: update shader variables in material
	m_pShadowMat->SetLightVP(m_LightVP);
	m_pShadowMat->SetWorld(world);
	if (index == m_pShadowMat->Skinned)
		m_pShadowMat->SetBones((float*)(&bones.front()), bones.size());

	//TODO: set the correct inputlayout, buffers, topology (some variables are set based on the generation type Skinned or Static)
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // Why TriangleList????
	

	gameContext.pDeviceContext->IASetInputLayout(m_pShadowMat->m_pInputLayouts[index]);
	unsigned int offset = 0;
	unsigned int stride = pMeshFilter->GetVertexBufferData(gameContext, m_pShadowMat->m_InputLayoutIds[index]).VertexStride;
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &pMeshFilter->GetVertexBufferData(gameContext, m_pShadowMat->m_InputLayoutIds[index]).pVertexBuffer, &stride, &offset);
	gameContext.pDeviceContext->IASetIndexBuffer(pMeshFilter->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//TODO: invoke draw call
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pShadowMat->m_pShadowTechs[index]->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pShadowMat->m_pShadowTechs[index]->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(pMeshFilter->m_IndexCount, 0, 0);
	}
}

void ShadowMapRenderer::UpdateMeshFilter(const GameContext& gameContext, MeshFilter* pMeshFilter)
{
	//TODO: based on the type (Skinned or Static) build the correct vertex buffers for the MeshFilter (Hint use MeshFilter::BuildVertexBuffer)
	auto index = (pMeshFilter->m_HasAnimations) ? m_pShadowMat->Skinned : m_pShadowMat->Static;
	pMeshFilter->BuildVertexBuffer(gameContext, m_pShadowMat->m_InputLayoutIds[index], m_pShadowMat->m_InputLayoutSizes[index],  m_pShadowMat->m_InputLayoutDescriptions[index]);
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap() const
{
	//TODO: return the depth shader resource view of the shadow generator render target
	return m_pShadowRT->GetDepthShaderResourceView();
}
