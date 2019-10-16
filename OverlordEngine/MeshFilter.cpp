#include "stdafx.h"

#include "MeshFilter.h"
#include "Material.h"

#include <algorithm>

DirectX::XMFLOAT4 MeshFilter::m_DefaultColor = DirectX::XMFLOAT4(1,0,0,1);
DirectX::XMFLOAT4 MeshFilter::m_DefaultFloat4 = DirectX::XMFLOAT4(0, 0, 0, 0);
DirectX::XMFLOAT3 MeshFilter::m_DefaultFloat3 = DirectX::XMFLOAT3(0, 0, 0);
DirectX::XMFLOAT2 MeshFilter::m_DefaultFloat2 = DirectX::XMFLOAT2(0, 0);

MeshFilter::MeshFilter():
			m_Positions(std::vector<DirectX::XMFLOAT3>()),
			m_Normals(std::vector<DirectX::XMFLOAT3>()),
			m_Tangents(std::vector<DirectX::XMFLOAT3>()),
			m_Binormals(std::vector<DirectX::XMFLOAT3>()),
			m_TexCoords(std::vector<DirectX::XMFLOAT2>()),
			m_Colors(std::vector<DirectX::XMFLOAT4>()),
			m_Indices(std::vector<DWORD>()),
			m_BlendIndices(std::vector<DirectX::XMFLOAT4>()),
			m_BlendWeights(std::vector<DirectX::XMFLOAT4>()),
			m_AnimationClips(std::vector<AnimationClip>()),
			m_pIndexBuffer(nullptr),
			m_VertexBuffers(std::vector<VertexBufferData>()),
			m_VertexCount(0),
			m_IndexCount(0),
			m_HasElement(NONE),
			m_HasAnimations(false),
			m_BoneCount(0)
{}

MeshFilter::~MeshFilter()
{
	m_Positions.clear();
	m_Normals.clear();
	m_TexCoords.clear();
	m_Colors.clear();
	m_Indices.clear();
	m_Tangents.clear();
	m_Binormals.clear();
	m_BlendIndices.clear();
	m_BlendWeights.clear();
	m_AnimationClips.clear();

	for_each(m_VertexBuffers.begin(), m_VertexBuffers.end(), [](VertexBufferData& data)
	{
		data.Destroy();
	});

	m_VertexBuffers.clear();
	SafeRelease(m_pIndexBuffer);
}

void MeshFilter::BuildIndexBuffer(const GameContext& gameContext)
{
	if (m_pIndexBuffer != nullptr)
		return;

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(DWORD) * m_Indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = m_Indices.data();

	auto hr = gameContext.pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	Logger::LogHResult(hr, L"MeshFilter::BuildIndexBuffer()");
}

int MeshFilter::GetVertexBufferId(UINT inputLayoutId)
{
	for (UINT i = 0; i<m_VertexBuffers.size(); ++i)
	{
		if (m_VertexBuffers[i].InputLayoutID == inputLayoutId)
			return i;
	}

	return -1;
}

void MeshFilter::BuildVertexBuffer(const GameContext& gameContext, Material* pMaterial)
{
	BuildVertexBuffer(gameContext, pMaterial->m_InputLayoutID, pMaterial->m_pInputLayoutSize, pMaterial->m_pInputLayoutDescriptions);
}

void MeshFilter::BuildVertexBuffer(const GameContext& gameContext, UINT inputLayoutID, UINT inputLayoutSize, const std::vector<ILDescription>& inputLayoutDescriptions)
{
	//Check if VertexBufferInfo already exists with requested InputLayout
	if (GetVertexBufferId(inputLayoutID) >= 0)
		return;

	VertexBufferData data;
	data.VertexStride = inputLayoutSize;
	data.VertexCount = m_VertexCount;
	data.BufferSize = data.VertexStride * m_VertexCount;
	data.IndexCount = m_IndexCount;

	void *pDataLocation = malloc(data.BufferSize);
	if (pDataLocation == nullptr)
	{
		Logger::LogWarning(L"MeshFilter::BuildVertexBuffer() > Failed to allocate the required memory!");
		return;
	}

	data.pDataStart = pDataLocation;
	data.InputLayoutID = inputLayoutID;

	for (UINT i = 0; i < m_VertexCount; ++i)
	{
		for (UINT j = 0; j < inputLayoutDescriptions.size(); ++j)
		{
			auto ilDescription = inputLayoutDescriptions[j];

			if (i == 0 && !HasElement(ilDescription.SemanticType))
			{
			 std::wstring name = EffectHelper::GetIlSemanticName(ilDescription.SemanticType);
				Logger::LogFormat(LogLevel::Warning, L"MeshFilter::BuildVertexBuffer > Mesh \"%s\" has no vertex %s data, using a default value!", m_MeshName.c_str(), name.c_str());
			}

			switch (ilDescription.SemanticType)
			{
			case ILSemantic::POSITION:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType)?&m_Positions[i]:&m_DefaultFloat3, ilDescription.Offset);
				break;
			case ILSemantic::NORMAL:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_Normals[i] : &m_DefaultFloat3, ilDescription.Offset);
				break;
			case ILSemantic::COLOR:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_Colors[i] : &m_DefaultColor, ilDescription.Offset);
				break;
			case ILSemantic::TEXCOORD:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_TexCoords[i] : &m_DefaultFloat2, ilDescription.Offset);
				break;
			case ILSemantic::TANGENT:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_Tangents[i] : &m_DefaultFloat3, ilDescription.Offset);
				break;
			case ILSemantic::BINORMAL:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_Binormals[i] : &m_DefaultFloat3, ilDescription.Offset);
				break;
			case ILSemantic::BLENDINDICES:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_BlendIndices[i] : &m_DefaultFloat4, ilDescription.Offset);
				break;
			case ILSemantic::BLENDWEIGHTS:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_BlendWeights[i] : &m_DefaultFloat4, ilDescription.Offset);
				break;
			default:
				Logger::LogError(L"MeshFilter::BuildVertexBuffer() > Unsupported SemanticType!");
				break;
			}

			pDataLocation = (char *) pDataLocation + ilDescription.Offset;
		}
	}

	//fill a buffer description to copy the vertexdata into graphics memory
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = data.BufferSize;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data.pDataStart;
	//create a ID3D10Buffer in graphics memory containing the vertex info
	gameContext.pDevice->CreateBuffer(&bd, &initData, &data.pVertexBuffer);

	m_VertexBuffers.push_back(data);
}

const VertexBufferData& MeshFilter::GetVertexBufferData(const GameContext& gameContext, Material* pMaterial)
{
	int possibleBuffer = GetVertexBufferId(pMaterial->m_InputLayoutID);

	if (possibleBuffer < 0)
	{
		Logger::LogWarning(L"MeshFilter::GetVertexBufferInformation(...) => No VertexBufferInformation for this material found! Building matching VertexBufferInformation (Performance Issue).");
		BuildVertexBuffer(gameContext, pMaterial);

		//Return last created vertexbufferinformation
		return m_VertexBuffers.back();
	}

	return m_VertexBuffers[possibleBuffer];
}

const VertexBufferData& MeshFilter::GetVertexBufferData(const GameContext& gameContext, UINT inputLayoutId)
{
	UNREFERENCED_PARAMETER(gameContext);

	int possibleBuffer = GetVertexBufferId(inputLayoutId);
	if (possibleBuffer < 0)
	{
		Logger::LogError(L"MeshFilter::GetVertexBufferInformation(INPUTLAYOUT_ID) => No VertexBufferInformation for this InputLayoutID found! Initialize before retrieving.");

		//Return last created vertexbufferinformation
		return m_VertexBuffers.back();
	}
	return m_VertexBuffers[possibleBuffer];
}