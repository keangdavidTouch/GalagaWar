#include "stdafx.h"

#include "TerrainComponent.h"
 #include <utility>

#include "ContentManager.h"
#include "EffectHelper.h"
#include "TransformComponent.h"
#include "TextureData.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "GameObject.h"
#include "GameScene.h"

TerrainComponent::TerrainComponent(std::wstring heightMapFile, std::wstring textureFile,
                                   unsigned int nrOfRows, unsigned int nrOfColumns, float width, float depth,
                                   float maxHeight) :
	m_HeightMapFile(std::move(heightMapFile)),
	m_TextureFile(std::move(textureFile)),
	m_NrOfRows(nrOfRows),
	m_NrOfColumns(nrOfColumns),
	m_NrOfVertices(nrOfRows * nrOfColumns),
	m_Width(width),
	m_Depth(depth),
	m_MaxHeight(maxHeight)
{}

TerrainComponent::~TerrainComponent()
{
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);
}

void TerrainComponent::Initialize(const GameContext& gameContext)
{
	//Load Effect
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"Resources/Effects/PosNormTex3D.fx");
	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);

	//TODO: Error handling
	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_pMatWorldVariable = m_pEffect->GetVariableByName("gWorld")->AsMatrix();
	m_pDiffuseVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();

	UINT ilSize = 0;
	if (!EffectHelper::BuildInputLayout(gameContext.pDevice, m_pTechnique, &m_pInputLayout, ilSize))
		Logger::LogError(L"TerrainComponent::Initialize >> BuildInputLayout failed!");

	//Texture
	m_pTextureData = ContentManager::Load<TextureData>(m_TextureFile);

	//Load Height Map
	ReadHeightMap();

	//Create Vertices & Triangles
	CalculateVerticesAndIndices();

	if (!m_VecVertices.empty())
	{
		//Build Vertexbuffer
		BuildVertexBuffer(gameContext);

		//Build Indexbuffer
		BuildIndexBuffer(gameContext);

		//Create PhysX Heightfield
		CreatePxHeightField();
	}
}

void TerrainComponent::Update(const GameContext& )
{}

void TerrainComponent::Draw(const GameContext& gameContext)
{
	if (m_VecVertices.empty())
		return;

	DirectX::XMMATRIX world = XMLoadFloat4x4(&GetTransform()->GetWorld());
	const DirectX::XMMATRIX viewProj = XMLoadFloat4x4(&gameContext.pCamera->GetViewProjection());
	DirectX::XMMATRIX wvp = XMMatrixMultiply(world, viewProj);
	m_pMatWorldVariable->SetMatrix(reinterpret_cast<float*>(&world));
	m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<float*>(&wvp));
	m_pDiffuseVariable->SetResource(m_pTextureData->GetShaderResourceView());

	// Set vertex buffer
	const unsigned int stride = sizeof(VertexPosNormTex);
	unsigned int offset = 0;
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set index buffer
	gameContext.pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the input layout
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);

	// Set primitive topology
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render a cube
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}

//Exercise - Heightmap
void TerrainComponent::ReadHeightMap()
{
	//TODO: complete

	// make sure the value has enough space for all the height values
	m_VecHeightValues.resize(m_NrOfVertices, 0);

	// open binary file
	std::ifstream inFile;
	inFile.open(m_HeightMapFile.c_str(), std::ios_base::binary);
	if (!inFile) {
		Logger::LogFormat(LogLevel::Error, L"Loading terrain \'%ls\' failed!", m_HeightMapFile.c_str());
		return;
	}

	// read height info
	inFile.read(reinterpret_cast<char*>(&m_VecHeightValues[0]), static_cast<std::streamsize>(m_NrOfVertices * sizeof(unsigned short)));
	inFile.close();
}

//Exercise - Flat Grid
void TerrainComponent::CalculateVerticesAndIndices()
{
	using namespace DirectX;

	//TODO: complete
	//**VERTICES
	//Reserve spots in the buffer
	m_VecVertices.resize(m_NrOfVertices, VertexPosNormTex{});

	//Calculate the Initial Position (Terrain centered on the origin)
	//Reset the cellXPos Position for each Column
	float cellWidth = m_Width / m_NrOfColumns;
	float cellDepth = m_Depth / m_NrOfRows;
	float cellXPos = -m_Width / 2.f;
	float cellZPos = m_Depth / 2.f;

	for (unsigned int row = 0; row < m_NrOfRows; ++row) {

		cellXPos = -m_Width / 2.f;
		for (unsigned int col = 0; col < m_NrOfColumns; ++col)
		{
			int vertexId = row * m_NrOfColumns + col;
			m_VecVertices[vertexId].Position.x = cellXPos;
			m_VecVertices[vertexId].Position.y = (float(m_VecHeightValues[vertexId]) / std::numeric_limits<unsigned short>::max()) * m_MaxHeight;
			m_VecVertices[vertexId].Position.z = cellZPos;

			cellXPos += cellWidth;

			// UV Coordinate
			m_VecVertices[vertexId].TexCoord.x = float(col) / (m_NrOfColumns - 1);
			m_VecVertices[vertexId].TexCoord.y = float(row) / (m_NrOfRows - 1);
		}

		cellZPos -= cellDepth;
	}

	int nrQuadRow = m_NrOfRows - 1;
	int nrQuadColumn = m_NrOfColumns - 1;

	for (int row = 0; row < nrQuadRow; ++row) {

		for (int col = 0; col < nrQuadColumn; ++col) {

			int a = row * m_NrOfColumns + col;
			int b = a + 1;
			int c = a + m_NrOfColumns;
			int d = c + 1;
			AddQuad(a, b, c, d);
		}
	}

	//1. Position -- Partially Exercise - Heightmap --
	//2. Normal
	//3. TexCoord -- Exercise - UV --

	//Move the cellXPos Position (Down)
	//Move the cellZPos Position (Right)

	//Exercise - Normals
	//For each face...
	//Get the positions of 6 vertices
	//first triangle
	//second triangle
	
	for (unsigned int i = 0; i < m_VecIndices.size(); i += 6) {

		//Get the position of 6 vertices
		XMVECTOR a, b, c, d, e, f;
		a = XMLoadFloat3(&m_VecVertices.at(m_VecIndices.at(i)).Position);
		b = XMLoadFloat3(&m_VecVertices.at(m_VecIndices.at(i+1)).Position);
		c = XMLoadFloat3(&m_VecVertices.at(m_VecIndices.at(i+2)).Position);
		d = XMLoadFloat3(&m_VecVertices.at(m_VecIndices.at(i+3)).Position);
		e = XMLoadFloat3(&m_VecVertices.at(m_VecIndices.at(i+4)).Position);
		f = XMLoadFloat3(&m_VecVertices.at(m_VecIndices.at(i+5)).Position);

		//First Triangle
		XMVECTOR v1, v2, normal;
		v1 = c - a;
		v2 = b - a;
		normal = XMVector3Cross(v1, v2); //problem: left handed
		normal = XMVector3Normalize(normal);
		XMFLOAT3 normalFloat3{};
		XMStoreFloat3(&normalFloat3, normal);
		m_VecFaceNormals.push_back(normalFloat3);

		//Second Triangle
		v1 = e - f;
		v2 = d - f;
		normal = XMVector3Cross(v1, v2);
		normal = XMVector3Normalize(normal);
		XMStoreFloat3(&normalFloat3, normal);
		m_VecFaceNormals.push_back(normalFloat3);
	}


	//iterate through the vertices and calculate a normal for each one of them using the average of the 6 adjacent faces
	int numFacesPerRow = (m_NrOfColumns - 1) * 2;
	//XMFLOAT3 normals[6] {};
	int index[6] {};
	//from left front to right back
	for (unsigned int row = 0; row < m_NrOfRows; ++row) {

		for (unsigned int col = 0; col < m_NrOfColumns; ++col) {
			
			int centerIndex = numFacesPerRow * row + col * 2;
			index[0] = centerIndex - 1;
			index[1] = centerIndex;
			index[2] = centerIndex + 1;
			index[3] = centerIndex - numFacesPerRow - 2;
			index[4] = centerIndex - numFacesPerRow - 1;
			index[5] = centerIndex - numFacesPerRow - 0;

			//if col==0 is on left edge, there are 
			//no vertices on the left, fill in a illegal index
			if (col == 0)
			{
				index[0] = -1;
				index[3] = -1;
				index[4] = -1;
			}

			//if col==m_NumColumns-1 is on right edge, there are 
			//no vertices on the right, fill in a illegal index
			if (col == m_NrOfColumns - 1) {
				index[1] = -1;
				index[2] = -1;
				index[5] = -1;
			}

			//if index<0 or out of range: front or back edge 
			//it may not be used to calculate the average
			XMVECTOR sum{};
			for (int i = 0; i < 6; ++i) {

				if ((index[i] >= 0) && (index[i] < (int)m_VecFaceNormals.size())) {

					sum += DirectX::XMLoadFloat3(&m_VecFaceNormals.at(index[i]));
				}
			}

			//calculate average by normalizing
			sum = XMVector3Normalize(sum);
			XMFLOAT3 normal{};
			XMStoreFloat3(&normal, sum);
			m_VecVertices[row * m_NrOfColumns + col].Normal = normal;
		}
	}
	
}

//Exercise - Flat Grid
void TerrainComponent::AddTriangle(const unsigned int a, const unsigned int b, const unsigned c)
{
	m_VecIndices.push_back(a);
	m_VecIndices.push_back(b);
	m_VecIndices.push_back(c);
}

//Exercise - Flat Grid
void TerrainComponent::AddQuad(const unsigned int a, const unsigned int b, const unsigned c, const unsigned d)
{
	AddTriangle(a, c, d);
	AddTriangle(a, d, b);
}

//Exercise - PhysX
void TerrainComponent::CreatePxHeightField()
{
	//TODO: Implement PhysX HeightField
	//After implementing the physX representation - Fix the lagginess from the PhysX Debugger! (If any)

	using namespace physx;
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto material = physX->createMaterial(1,1,1);

	PxHeightFieldSample* samples = new PxHeightFieldSample[m_NrOfVertices];
	memset(samples, 0, m_NrOfVertices*sizeof(PxHeightFieldSample));

	for (unsigned int i = 0; i < m_VecHeightValues.size(); i++)
	{
		samples[i].height = (PxI16)(m_VecHeightValues[i] * std::numeric_limits<short>::max() / std::numeric_limits<unsigned short>::max());
	}

	PxHeightFieldDesc desc{};
	desc.format = PxHeightFieldFormat::eS16_TM;
	desc.nbColumns = m_NrOfColumns;
	desc.nbRows = m_NrOfRows;
	/*desc.thickness = 3.f;*/
	desc.samples.data = &samples[0];
	desc.samples.stride = sizeof(PxHeightFieldSample);

	PxHeightField* pHeightField = physX->createHeightField(desc);
	
	//float heightScale{ m_MaxHeight / m_NrOfRows };
	float rowScale { m_Depth / m_NrOfRows }; 
	float columnScale{ m_Width / m_NrOfColumns };

	PxHeightFieldGeometry* heightFieldGeometry = new PxHeightFieldGeometry(pHeightField, PxMeshGeometryFlags(), m_MaxHeight / std::numeric_limits<short>::max(), rowScale, columnScale);
	auto actor = physX->createRigidStatic(PxTransform{ PxVec3{-m_Width/2, 0, m_Depth/2}, PxQuat{PxHalfPi, {0, 1, 0}}});
	actor->userData = this;
	auto shape = actor->createShape(*heightFieldGeometry, *material, PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION);
	shape->setSimulationFilterData(PxFilterData(CollisionGroupFlag::Group0, 0, 0, 0));
	GetGameObject()->GetScene()->GetPhysxProxy()->GetPhysxScene()->addActor(*actor);
}

void TerrainComponent::BuildVertexBuffer(const GameContext& gameContext)
{
	//Vertexbuffer
	D3D11_BUFFER_DESC bd = {};
	D3D11_SUBRESOURCE_DATA initData = {nullptr};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(VertexPosNormTex) * m_NrOfVertices;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = m_VecVertices.data();
	const HRESULT hr = gameContext.pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	Logger::LogHResult(hr, L"Failed to Create Vertexbuffer");
}

void TerrainComponent::BuildIndexBuffer(const GameContext& gameContext)
{
	D3D11_BUFFER_DESC bd = {};
	D3D11_SUBRESOURCE_DATA initData = {nullptr};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(DWORD) * m_VecIndices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = m_VecIndices.data();
	const HRESULT hr = gameContext.pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	Logger::LogHResult(hr, L"Failed to Create Indexbuffer");
	m_NumIndices = m_VecIndices.size();
}