#pragma once

struct VertexPosCol;
struct GameContext;

class DebugRenderer final
{
public:

	static void InitRenderer(ID3D11Device *pDevice, UINT bufferSize = 100);
	static void ToggleDebugRenderer();
	static void Release();

	//RENDERING
	static void Draw(const GameContext& gameContext);
	static void DrawLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color = static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Red));
	static void DrawLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT4 colorStart, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 colorEnd);
	static void DrawPhysX(physx::PxScene* pScene);

	DebugRenderer(const DebugRenderer& other) = delete;
	DebugRenderer(DebugRenderer&& other) noexcept = delete;
	DebugRenderer& operator=(const DebugRenderer& other) = delete;
	DebugRenderer& operator=(DebugRenderer&& other) noexcept = delete;
	DebugRenderer() = delete;
	~DebugRenderer() = delete;
private:

	//RENDERING
	static void CreateVertexBuffer(ID3D11Device *pDevice);
	static void CreateFixedLineList();
	static DirectX::XMFLOAT4 ConvertPxColor(physx::PxU32 color);
	static ID3DX11Effect* m_pEffect;
	static ID3DX11EffectTechnique* m_pTechnique;
	static UINT m_BufferSize, m_FixedBufferSize;
	static ID3D11InputLayout *m_pInputLayout;
	static ID3D11Buffer *m_pVertexBuffer;
	static ID3DX11EffectMatrixVariable *m_pWvpVariable;
	static std::vector<VertexPosCol> m_LineList;
	static std::vector<VertexPosCol> m_FixedLineList;
	static bool m_RendererEnabled;
	static physx::PxScene* m_pPhysxDebugScene;
};

