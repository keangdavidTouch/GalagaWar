#pragma once
#include <vector>
#include "EffectHelper.h"

class MeshFilterLoader;
class Material;
struct GameContext;
class ModelComponent;

struct VertexBufferData
{
	VertexBufferData():
		pDataStart(nullptr),
		pVertexBuffer(nullptr),
		BufferSize(0),
		VertexStride(0),
		VertexCount(0),
		IndexCount(0),
		InputLayoutID(0){}

	void* pDataStart;
	ID3D11Buffer* pVertexBuffer;
	UINT BufferSize;
	UINT VertexStride;
	UINT VertexCount;
	UINT IndexCount;
	UINT InputLayoutID;

	void VertexBufferData::Destroy()
	{
		free(pDataStart);
		SafeRelease(pVertexBuffer);
	}
};

struct AnimationKey
{
	float Tick;
	std::vector<DirectX::XMFLOAT4X4> BoneTransforms;
};

struct AnimationClip
{
	AnimationClip() {};

	std::wstring Name;
	float Duration;
	float TicksPerSecond;
	std::vector<AnimationKey> Keys;
};

class MeshFilter final
{
public:
	MeshFilter();
	~MeshFilter();

private:
	friend class MeshFilterLoader;
	friend class ModelComponent;
	friend class ModelAnimator;
	friend class ShadowMapRenderer;

	int GetVertexBufferId(UINT inputLayoutId);
	void BuildVertexBuffer(const GameContext& gameContext, UINT inputLayoutID, UINT inputLayoutSize, const std::vector<ILDescription>& inputLayoutDescriptions);
	void BuildVertexBuffer(const GameContext& gameContext, Material* pMaterial);
	void BuildIndexBuffer(const GameContext& gameContext);
	bool HasElement(ILSemantic element) { return (m_HasElement&element) > 0 ? true : false; }
	const VertexBufferData& GetVertexBufferData(const GameContext& gameContext, Material* pMaterial);
	const VertexBufferData& GetVertexBufferData(const GameContext& gameContext, UINT inputLayoutId);

	//VERTEX DATA
	UINT m_VertexCount, m_IndexCount;
	UINT m_HasElement;
	std::vector<DirectX::XMFLOAT3> m_Positions;
	std::vector<DirectX::XMFLOAT3> m_Normals;
	std::vector<DirectX::XMFLOAT3> m_Tangents;
	std::vector<DirectX::XMFLOAT3> m_Binormals;
	std::vector<DirectX::XMFLOAT2> m_TexCoords;
	std::vector<DirectX::XMFLOAT4> m_Colors;
	std::vector<DirectX::XMFLOAT4> m_BlendIndices;
	std::vector<DirectX::XMFLOAT4> m_BlendWeights;
	std::vector<AnimationClip> m_AnimationClips;
	bool m_HasAnimations;
	USHORT m_BoneCount;

	//INDEX DATA
	std::vector<DWORD> m_Indices;

	std::vector<VertexBufferData> m_VertexBuffers;
	ID3D11Buffer* m_pIndexBuffer;

	std::wstring m_MeshName;
	static DirectX::XMFLOAT4 m_DefaultColor;
	static DirectX::XMFLOAT4 m_DefaultFloat4;
	static DirectX::XMFLOAT3 m_DefaultFloat3;
	static DirectX::XMFLOAT2 m_DefaultFloat2;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	MeshFilter(const MeshFilter &obj);
	MeshFilter& operator=(const MeshFilter& obj);
};

