#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "EffectHelper.h"
#include "ContentManager.h"
#include "TextureDataLoader.h"
#include "Particle.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "DropParticle.h"

ParticleEmitterComponent::ParticleEmitterComponent(std::wstring  assetFile, Type type, int particleCount):
	m_pVertexBuffer(nullptr),
	m_pEffect(nullptr),
	m_pParticleTexture(nullptr),
	m_pInputLayout(nullptr),
	m_pInputLayoutSize(0),
	m_Settings(ParticleEmitterSettings()),
	m_ParticleCount(particleCount),
	m_ActiveParticles(0),
	m_LastParticleInit(0.0f),
	m_AssetFile(std::move(assetFile)),
	m_BillboardRotation(DirectX::XMFLOAT3(0,0,0)),
	m_BillboardScale(DirectX::XMFLOAT3(1,1,1))
{
	for (int i= 0 ; i < particleCount; i++)
	{
		switch(type){
		case Type::Explode: m_Particles.push_back(new Particle(m_Settings)); break;
		case Type::Rain: m_Particles.push_back(new DropParticle(m_Settings));  break;
		default: break;
		}
	}

}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	for (auto particle : m_Particles)
		SafeDelete(particle);
	
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexBuffer);
}

void ParticleEmitterComponent::Restart()
{
	for (int i = 0; i < m_ParticleCount; i++) {
		m_Particles[i]->Reset();
		m_Particles[i]->Init(GetGameObject()->GetTransform()->GetPosition(), i);
	}
}

void ParticleEmitterComponent::Initialize(const GameContext& gameContext)
{
	LoadEffect(gameContext);
	CreateVertexBuffer(gameContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::LoadEffect(const GameContext& gameContext)
{
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/ParticleRenderer.fx");
	m_pDefaultTechnique = m_pEffect->GetTechniqueByIndex(0);
	auto effectVar = m_pEffect->GetVariableByName("gWorldViewProj");
	m_pWvpVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
	effectVar = m_pEffect->GetVariableByName("gViewInverse");
	m_pViewInverseVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
	effectVar = m_pEffect->GetVariableByName("gParticleTexture");
	m_pTextureVariable = (effectVar->IsValid()) ? effectVar->AsShaderResource() : nullptr;
	effectVar = m_pEffect->GetVariableByName("gIsMoving");
	m_pMovingVariable = (effectVar->IsValid()) ? effectVar->AsScalar() : nullptr;

	EffectHelper::BuildInputLayout(gameContext.pDevice, m_pDefaultTechnique, &m_pInputLayout, m_pInputLayoutSize);
}

void ParticleEmitterComponent::CreateVertexBuffer(const GameContext& gameContext)
{
	if (m_pVertexBuffer) {
		SafeRelease(m_pVertexBuffer);
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D10_BIND_FLAG::D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(ParticleVertex) * m_ParticleCount;
	vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_FLAG::D3D10_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	const auto result = gameContext.pDevice->CreateBuffer(&vertexBufferDesc, nullptr, &m_pVertexBuffer);
	Logger::LogHResult(result, L"ParticleEmitterComponent::CreateVertexBuffer(...)");
}

void ParticleEmitterComponent::Update(const GameContext& gameContext)
{
	float particleInterval = (m_Settings.MaxSize - m_Settings.MinSize) / m_ParticleCount; //TODO::FIX IT

	m_LastParticleInit += gameContext.pGameTime->GetElapsed();

	m_ActiveParticles = 0;

	//BUFFER MAPPING CODE 
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	gameContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ParticleVertex* pBuffer = (ParticleVertex*)mappedResource.pData;

	for (int i = 0; i < m_ParticleCount; i++) {

		//If Loop : Don't Stop
		if(!m_Settings.Loop && m_Particles[i]->IsFinish())
			continue;

		m_Particles[i]->Update(gameContext);

		if (m_Particles[i]->IsActive()) {
			
			pBuffer[m_ActiveParticles] = m_Particles[i]->GetVertexInfo();
			m_ActiveParticles++;
		}
		else {

			if (!m_ImediateParticle) {
				if (m_LastParticleInit >= particleInterval) {

					m_Particles[i]->Init(GetGameObject()->GetTransform()->GetPosition(), i);
					pBuffer[m_ActiveParticles] = m_Particles[i]->GetVertexInfo();
					m_ActiveParticles++;
					m_LastParticleInit = 0;
				}
			}
			else {
				m_Particles[i]->Init(GetGameObject()->GetTransform()->GetPosition(), i);
				pBuffer[m_ActiveParticles] = m_Particles[i]->GetVertexInfo();
				m_ActiveParticles++;
				m_LastParticleInit = 0;
			}
		}

	}

	gameContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::Draw(const GameContext& )
{}

void ParticleEmitterComponent::PostDraw(const GameContext& gameContext)
{
	using namespace DirectX;

	auto w = XMLoadFloat4x4(&GetTransform()->GetWorld());
	auto vp = XMLoadFloat4x4(&gameContext.pCamera->GetViewProjection());
	auto inverse = XMLoadFloat4x4(&gameContext.pCamera->GetViewInverse());
	//auto wvp =  w * vp;
	auto wvp = XMMatrixIdentity();
	if (GetGameObject()->GetParent()) {
		wvp =  w * vp;
	}
	else {
		wvp = vp;
	}
	m_pWvpVariable->SetMatrix(reinterpret_cast<float*>(&wvp));

	auto billboardMatrix = XMMatrixScaling(m_BillboardScale.x, m_BillboardScale.y, m_BillboardScale.z);

	if (m_BillboardRotation.x != 0) {
		auto rotation = DirectX::XMFLOAT4(m_BillboardRotation.x, 0, 0, m_BillboardRotation.x);
		billboardMatrix *= XMMatrixRotationQuaternion(XMLoadFloat4(&rotation));
	}

	if (m_BillboardRotation.y != 0) {
		auto rotation = DirectX::XMFLOAT4(0, m_BillboardRotation.y, 0, m_BillboardRotation.y);
		billboardMatrix *= XMMatrixRotationQuaternion(XMLoadFloat4(&rotation));
	}

	if (m_BillboardRotation.z != 0) {
		auto rotation = DirectX::XMFLOAT4(0, 0, m_BillboardRotation.z, m_BillboardRotation.z);
		billboardMatrix *= XMMatrixRotationQuaternion(XMLoadFloat4(&rotation));
	}

	m_pViewInverseVariable->SetMatrix(reinterpret_cast<float*>(&billboardMatrix));
	m_pTextureVariable->SetResource(m_pParticleTexture->GetShaderResourceView());

	m_pMovingVariable->SetBool(!GetGameObject()->GetParent());

	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);

	unsigned int offset = 0;
	unsigned int stride = sizeof(ParticleVertex);
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	D3DX11_TECHNIQUE_DESC techDesc;
	m_pDefaultTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pDefaultTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}
