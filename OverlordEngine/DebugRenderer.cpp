#include "stdafx.h"
#include "DebugRenderer.h"
#include "Logger.h"
#include "ContentManager.h"

ID3DX11Effect* DebugRenderer::m_pEffect = nullptr;
ID3DX11EffectTechnique* DebugRenderer::m_pTechnique = nullptr;
unsigned int DebugRenderer::m_BufferSize = 100;
ID3D11Buffer* DebugRenderer::m_pVertexBuffer = nullptr;
physx::PxScene* DebugRenderer::m_pPhysxDebugScene = nullptr;
ID3D11InputLayout* DebugRenderer::m_pInputLayout = nullptr;
ID3DX11EffectMatrixVariable* DebugRenderer::m_pWvpVariable = nullptr;
std::vector<VertexPosCol> DebugRenderer::m_LineList = std::vector<VertexPosCol>();
std::vector<VertexPosCol> DebugRenderer::m_FixedLineList = std::vector<VertexPosCol>();
unsigned int DebugRenderer::m_FixedBufferSize = 0;
bool DebugRenderer::m_RendererEnabled = true; 

void DebugRenderer::Release()
{
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexBuffer);
}

void DebugRenderer::InitRenderer(ID3D11Device *pDevice, unsigned int bufferSize)
{
	m_BufferSize = bufferSize;
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/DebugRenderer.fx");
	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	m_pWvpVariable = m_pEffect->GetVariableBySemantic("WORLDVIEWPROJECTION")->AsMatrix();

	if(!m_pWvpVariable->IsValid())
		Logger::LogWarning(L"Debug Renderer: Invalid Shader Variable! (WVP)");

	//Input Layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3DX11_PASS_DESC passDesc;
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
	pDevice->CreateInputLayout(layout, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout);

	CreateFixedLineList();
	CreateVertexBuffer(pDevice);
}

void DebugRenderer::CreateVertexBuffer(ID3D11Device *pDevice)
{
	SafeRelease(m_pVertexBuffer);

	//Vertexbuffer
	D3D11_BUFFER_DESC buffDesc;
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.ByteWidth = sizeof(VertexPosCol) * (m_BufferSize + m_FixedBufferSize);
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.MiscFlags = 0;
	
	pDevice->CreateBuffer(&buffDesc,nullptr,&m_pVertexBuffer);
	
	if(m_FixedBufferSize > 0)
	{
		//Map Fixed data manually
		ID3D11DeviceContext* pDeviceContext;
		pDevice->GetImmediateContext(&pDeviceContext);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
		memcpy(mappedResource.pData, &m_FixedLineList[0], sizeof(VertexPosCol) * m_FixedBufferSize);
		pDeviceContext->Unmap(m_pVertexBuffer, 0);
		pDeviceContext->Release();
	}
}

void DebugRenderer::CreateFixedLineList()
{
	//*GRID*
	const unsigned int numGridLines = 20;
	const float gridSpacing = 1.0f;

	const float startOffset = -(static_cast<int>(numGridLines)/2)*gridSpacing;
	const float size = (numGridLines - 1) * gridSpacing;
	const auto gridColor = static_cast<DirectX::XMFLOAT4>(DirectX::Colors::LightGray);
	for(unsigned int i = 0; i < numGridLines; ++i)
	{
		//VERTICAL
		const float lineOffset = startOffset + gridSpacing * i;
		auto vertStart = DirectX::XMFLOAT3(startOffset, 0, lineOffset);
		m_FixedLineList.emplace_back(VertexPosCol(vertStart, gridColor));
		vertStart.x += size;
		m_FixedLineList.emplace_back(VertexPosCol(vertStart, gridColor));

		//HORIZONTAL
		vertStart = DirectX::XMFLOAT3(lineOffset, 0, startOffset);
		m_FixedLineList.emplace_back(VertexPosCol(vertStart, gridColor));
		vertStart.z += size;
		m_FixedLineList.emplace_back(VertexPosCol(vertStart, gridColor));
	}

	//*AXIS
	m_FixedLineList.emplace_back(VertexPosCol(DirectX::XMFLOAT3(0,0,0), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::DarkRed)));
	m_FixedLineList.emplace_back(VertexPosCol(DirectX::XMFLOAT3(30,0,0), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::DarkRed)));
	m_FixedLineList.emplace_back(VertexPosCol(DirectX::XMFLOAT3(0,0,0), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::DarkGreen)));
	m_FixedLineList.emplace_back(VertexPosCol(DirectX::XMFLOAT3(0,30,0), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::DarkGreen)));
	m_FixedLineList.emplace_back(VertexPosCol(DirectX::XMFLOAT3(0,0,0), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::DarkBlue)));
	m_FixedLineList.emplace_back(VertexPosCol(DirectX::XMFLOAT3(0,0,30), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::DarkBlue)));

	//@END!
	m_FixedBufferSize = m_FixedLineList.size();
}

void DebugRenderer::ToggleDebugRenderer()
{
	m_RendererEnabled = !m_RendererEnabled;
}

void DebugRenderer::DrawLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color)
{
	if(!m_RendererEnabled)
		return;

	m_LineList.emplace_back(VertexPosCol(start, color));
	m_LineList.emplace_back(VertexPosCol(end, color));
}

void DebugRenderer::DrawLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT4 colorStart, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 colorEnd)
{
	if(!m_RendererEnabled)
		return;

	m_LineList.emplace_back(VertexPosCol(start, colorStart));
	m_LineList.emplace_back(VertexPosCol(end, colorEnd));
}

void DebugRenderer::DrawPhysX(physx::PxScene* pScene)
{
	if(!m_RendererEnabled)
		return;

	//m_pPhysxDebugScene = pScene;
	const auto pxDebugRenderer = &pScene->getRenderBuffer();
	const auto debugLines = pxDebugRenderer->getLines();

	for(unsigned int i = 0; i < pxDebugRenderer->getNbLines(); ++i)
	{
		const auto line = debugLines[i];
		DrawLine(DirectX::XMFLOAT3(line.pos0.x, line.pos0.y, line.pos0.z),ConvertPxColor(line.color0), DirectX::XMFLOAT3(line.pos1.x, line.pos1.y, line.pos1.z),ConvertPxColor(line.color1));
	}
}

DirectX::XMFLOAT4 DebugRenderer::ConvertPxColor(physx::PxU32 color)
{
	//TODO: Check performance, Bitshift+divide vs switch
	// Alex: maybe check implementation design too.
	switch(color)
	{
		case 0xFF000000:
			return static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Black);
		case 0xFFFF0000:
			return static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Red);
		case 0xFF00FF00:
			return static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Green);
		case 0xFF0000FF:
			return static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Blue);
		case 0xFFFFFF00:
			return static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Yellow);
		case 0xFFFF00FF:
			return static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Magenta);
		case 0xFF00FFFF:
			return static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Cyan);
		case 0xFFFFFFFF:
			return static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White);
		case 0xFF808080:
			return static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Gray);
		case 0x88880000:
			return static_cast<DirectX::XMFLOAT4>(DirectX::Colors::DarkRed);
		case 0x88008800:
			return static_cast<DirectX::XMFLOAT4>(DirectX::Colors::DarkGreen);
		case 0x88000088:
			return static_cast<DirectX::XMFLOAT4>(DirectX::Colors::DarkBlue);
		default:
			return static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Black);
	}
}

void DebugRenderer::Draw(const GameContext& gameContext)
{
	if(!m_RendererEnabled)
		return;


	//PhysX RenderBuffer
	unsigned int pxBuffSize = 0;
	const physx::PxRenderBuffer* pPxRenderBuffer = nullptr;
	if (m_pPhysxDebugScene)
	{
		pPxRenderBuffer = &m_pPhysxDebugScene->getRenderBuffer();
		pxBuffSize = pPxRenderBuffer->getNbLines(); //PxDebugLine == 2* VertexPosCol
		m_pPhysxDebugScene = nullptr;
	}

	//Current Size (DebugDraw + PhysXDraw)
	const unsigned int regularSize = m_LineList.size();
	const unsigned int dynamicSize = regularSize + pxBuffSize;

	//Total size
	const unsigned int totalSize = dynamicSize + m_FixedBufferSize;

	if(totalSize <= 0)
		return;
	
	if(dynamicSize > m_BufferSize)
	{
		Logger::LogInfo(L"DebugRenderer::Draw() > Increasing Vertexbuffer Size!");
		m_BufferSize = dynamicSize;
		CreateVertexBuffer(gameContext.pDevice);
	}

	auto pDevContext = gameContext.pDeviceContext;

	if(dynamicSize>0)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		pDevContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);

		if (regularSize > 0)
		{
			memcpy(&(static_cast<VertexPosCol*>(mappedResource.pData)[m_FixedBufferSize]), &m_LineList[0], sizeof(VertexPosCol) * regularSize); //Engine Lines
		}

		if (pxBuffSize > 0)
		{
			auto lineBuffer = pPxRenderBuffer->getLines();
			memcpy(&(static_cast<VertexPosCol*>(mappedResource.pData)[m_FixedBufferSize + regularSize]), &lineBuffer, sizeof(VertexPosCol) * pxBuffSize); //PhysX Lines
		}

		pDevContext->Unmap(m_pVertexBuffer, 0);

	}

	//Set Render Pipeline
	pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	unsigned int stride = sizeof(VertexPosCol);
	unsigned int offset = 0;
	pDevContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pDevContext->IASetInputLayout(m_pInputLayout);

	auto viewProj = gameContext.pCamera->GetViewProjection();
	const DirectX::XMMATRIX wvp = DirectX::XMMatrixIdentity() * XMLoadFloat4x4(&viewProj);
	DirectX::XMFLOAT4X4 wvpConverted;
	XMStoreFloat4x4( &wvpConverted, wvp);

	m_pWvpVariable->SetMatrix(reinterpret_cast<float*>(&wvpConverted));

	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);
	for(unsigned int i = 0; i < techDesc.Passes; ++i)
	{
		m_pTechnique->GetPassByIndex(i)->Apply(0, pDevContext);
		pDevContext->Draw(totalSize, 0);
	}
	
	m_LineList.clear();
}
