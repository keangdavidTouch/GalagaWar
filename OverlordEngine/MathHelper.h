#pragma once

inline DirectX::XMFLOAT3 QuaternionToEuler(DirectX::XMFLOAT4 q)
{
	DirectX::XMFLOAT3 euler;

	euler.y = atan2f(2.f * q.x * q.w + 2.f * q.y * q.z, 1.f - 2.f * (q.z*q.z + q.w*q.w));     // Yaw 
	euler.x = asinf(2.f * (q.x * q.z - q.w * q.y));                             // Pitch 
	euler.z = atan2f(2.f * q.x * q.y + 2.f * q.z * q.w, 1.f - 2.f * (q.y*q.y + q.z*q.z));      // Roll 

	return euler;
}

inline bool XMFloat4Equals(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

inline bool XMFloat3Equals(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

inline bool XMFloat2Equals(const DirectX::XMFLOAT2& a, const DirectX::XMFLOAT2& b)
{
	return a.x == b.x && a.y == b.y;
}

inline float randF(float min, float max)
{
	const float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	const float diff = max - min;
	const float r = random * diff;
	return min + r;
}