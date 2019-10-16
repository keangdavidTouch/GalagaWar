#include "stdafx.h"
#include "BezierPath.h"
#include "TransformComponent.h"

BezierPath::BezierPath()
	: m_PathLengths(std::vector<float>())
	, m_PathPositions(std::vector<DirectX::XMFLOAT3>())
	, m_Parent(nullptr)
	, m_Speed(80)
	, m_Offset(0)
	, m_CurrentTravelPos(0)
	, m_CurrentPathIndex(0)
	, m_CurrentPercentage(0)
	, m_IsFinish(false)
{
}

void BezierPath::Update(float deltaTime)
{
	if (m_IsFinish) return;

	if(m_Parent != nullptr && !m_Parent->IsFinish()) {

		//calculate travel position and index based on offset from parent 
		m_CurrentTravelPos = m_Parent->m_CurrentTravelPos - m_Offset; 
		m_CurrentPathIndex = m_Parent->m_CurrentPathIndex;

		if (m_CurrentTravelPos <= 0) {

			if (m_CurrentPathIndex >= CubicBezierPointCount) {

				m_CurrentPathIndex = m_CurrentPathIndex - CubicBezierPointCount; 
				m_CurrentTravelPos = m_PathLengths[m_CurrentPathIndex/CubicBezierPointCount] - (m_Offset - m_Parent->m_CurrentTravelPos);
			}
		}
	}
	else {
		m_CurrentTravelPos += deltaTime * m_Speed;
	}

	m_CurrentPercentage = GetPercentageFromTravelLength(m_CurrentPathIndex, m_CurrentTravelPos);

	//go to next cubic-Bezier point index 
	if (m_CurrentPercentage >= 1) {
		size_t nextIndex = m_CurrentPathIndex + CubicBezierPointCount;

		if (nextIndex >= m_PathPositions.size() - 1) {
			m_IsFinish = true;
		}
		else {
			m_CurrentTravelPos = 0;
			m_CurrentPercentage = 0.f;
			m_CurrentPathIndex = nextIndex;
		}
	}
}

void BezierPath::SetPath(std::vector<DirectX::XMFLOAT3>& paths)
{
	m_PathPositions = paths;

	m_IsFinish = false;
	m_CurrentTravelPos = 0;
	m_CurrentPercentage = 0;
	m_CurrentPathIndex = 0;
	m_PathLengths.clear();

	//pre-calculate paths length
	size_t count = paths.size() / CubicBezierPointCount;
	for (size_t i = 0; i < count; i++) {
		float length = CalculateCubicBezierPathLength(CubicBezierPointCount * i);
		m_PathLengths.push_back(length);
	}
}

void BezierPath::GetCurrentPathTransform(PathTransform& transform)
{
	DirectX::XMFLOAT3 targetPosFloat = GetCubicBezierCurvePosition(m_CurrentPathIndex, m_CurrentPercentage + 0.05f);
	transform.Angle = TransformComponent().GetAngleBetweenVector(transform.Position, targetPosFloat);
	transform.Position = GetCubicBezierCurvePosition(m_CurrentPathIndex, m_CurrentPercentage);
}

float BezierPath::GetPercentageFromTravelLength(int nodeIndex, float travelLength)
{
	float totalLength = m_PathLengths[nodeIndex / CubicBezierPointCount];
	if (travelLength >= totalLength)
		return 1.f;

	float pathLength = 0.0f;
	float step = 0.005f;

	DirectX::XMFLOAT3 newPosFloat;
	DirectX::XMFLOAT3 oldPosFloat = GetCubicBezierCurvePosition(nodeIndex, 0);

	for (float t = 0; t <= 1.0f; t += step) {

		newPosFloat = GetCubicBezierCurvePosition(nodeIndex, t);

		DirectX::XMVECTOR oldPosVec = DirectX::XMLoadFloat3(&oldPosFloat);
		DirectX::XMVECTOR newPosVec = DirectX::XMLoadFloat3(&newPosFloat);
		DirectX::XMVECTOR directionVec = DirectX::XMVectorSubtract(newPosVec, oldPosVec);

		DirectX::XMFLOAT3 lengthFloat;
		DirectX::XMStoreFloat3(&lengthFloat, DirectX::XMVector3Length(directionVec));
		pathLength += lengthFloat.x;

		if (pathLength >= travelLength) {
			return t;
		}

		oldPosFloat = newPosFloat;
	}

	return 0.f;
}

float BezierPath::CalculateCubicBezierPathLength(int nodeIndex)
{
	float pathLength = 0.0f;
	float step = 0.005f;

	DirectX::XMFLOAT3 newPosFloat;
	DirectX::XMFLOAT3 oldPosFloat = GetCubicBezierCurvePosition(nodeIndex, 0);

	for (float t = 0; t <= 1.0f; t += step) {

		newPosFloat = GetCubicBezierCurvePosition(nodeIndex, t);

		DirectX::XMVECTOR newPosVec = DirectX::XMLoadFloat3(&newPosFloat);
		DirectX::XMVECTOR oldPosVec = DirectX::XMLoadFloat3(&oldPosFloat);
		DirectX::XMVECTOR directionVec = DirectX::XMVectorSubtract(newPosVec, oldPosVec);

		DirectX::XMFLOAT3 lengthFloat;
		DirectX::XMStoreFloat3(&lengthFloat, DirectX::XMVector3Length(directionVec));
		pathLength += lengthFloat.x;

		oldPosFloat = newPosFloat;
	}

	return pathLength;
}

const DirectX::XMFLOAT3 BezierPath::GetCubicBezierCurvePosition(int index, float t)
{
	return GetCubicBezierCurvePosition(m_PathPositions[index], 
										m_PathPositions[index + 1], 
										m_PathPositions[index + 2], 
										m_PathPositions[index + 3], t);
}

 const DirectX::XMFLOAT3 BezierPath::GetCubicBezierCurvePosition(DirectX::XMFLOAT3& posFloat0, DirectX::XMFLOAT3& posFloat1, DirectX::XMFLOAT3& posFloat2, DirectX::XMFLOAT3& posFloat3, float t)
{
	using namespace DirectX;

	XMVECTOR p0 = XMLoadFloat3(&posFloat0);
	XMVECTOR p1 = XMLoadFloat3(&posFloat1);
	XMVECTOR p2 = XMLoadFloat3(&posFloat2);
	XMVECTOR p3 = XMLoadFloat3(&posFloat3);

	XMVECTOR bezierPosVec = pow(1 - t, 3) * p0 +
		3 * pow(1 - t, 2) * t * p1 +
		3 * (1 - t) * pow(t, 2) * p2 +
		pow(t, 3) * p3;

	XMFLOAT3 bezierPosFloat;
	XMStoreFloat3(&bezierPosFloat, bezierPosVec);

	return bezierPosFloat;
}

void BezierPath::SetParent(BezierPath * parent)
{
	 m_Parent = parent; 
}

void BezierPath::SetOffset(float offset)
{
	m_Offset = offset; 
}

void BezierPath::SetSpeed(float speed)
{
	 m_Speed = speed;
}

bool BezierPath::IsFinish()
{
	return m_IsFinish; 
}

