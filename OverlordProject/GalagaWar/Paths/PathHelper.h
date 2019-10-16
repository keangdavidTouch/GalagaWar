#pragma once

class PathHelper
{
public:
	PathHelper() = default;
	~PathHelper() = default;

	//void Update(const GameContext& context);
	void GetPathFromFile(const std::string& filename);
	const std::vector<DirectX::XMFLOAT3>& GetPathatIndex(int i) { return m_Paths[i]; };
	std::vector<DirectX::XMFLOAT3>& GetNextPath() { return m_Paths[m_EnemyPathIndex]; };

private:
	int m_EnemyPathIndex = 0;
	std::vector<std::vector<DirectX::XMFLOAT3>> m_Paths{};
};