#pragma once

struct PathTransform {
	
	float Angle;
	DirectX::XMFLOAT3 Position;
};

class BezierPath
{
public:
	BezierPath();
	~BezierPath() = default;

	void Update(float deltaTime);
	void GetCurrentPathTransform(PathTransform& transform);
	void SetPath(std::vector<DirectX::XMFLOAT3>& paths);
	void SetParent(BezierPath* parent);
	void SetOffset(float offset);
	void SetSpeed(float speed);
	bool IsFinish();
	
private:
	
	const static size_t CubicBezierPointCount = 4;

	bool m_IsFinish;
	float m_Offset;
	float m_Speed;
	float m_CurrentTravelPos;
	float m_CurrentPercentage;
	size_t m_CurrentPathIndex;
	BezierPath* m_Parent;
	std::vector<float> m_PathLengths;
	std::vector<DirectX::XMFLOAT3> m_PathPositions;

	// Cubic-Bezier Curve Functions
	float CalculateCubicBezierPathLength(int nodeIndex);
	float GetPercentageFromTravelLength(int nodeIndex, float length);
	const DirectX::XMFLOAT3 GetCubicBezierCurvePosition(int index, float t);
	const DirectX::XMFLOAT3 GetCubicBezierCurvePosition(DirectX::XMFLOAT3& posFloat0, DirectX::XMFLOAT3& posFloat1, DirectX::XMFLOAT3& posFloat2, DirectX::XMFLOAT3& posFloat3, float t);
};

