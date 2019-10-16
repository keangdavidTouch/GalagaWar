#pragma once

inline physx::PxVec3 ToPxVec3(DirectX::XMFLOAT3 vec)
{
	return physx::PxVec3(vec.x, vec.y, vec.z);
}

inline physx::PxExtendedVec3 ToPxExtendedVec3(DirectX::XMFLOAT3 vec)
{
	return physx::PxExtendedVec3(vec.x, vec.y, vec.z);
}

inline physx::PxVec2 ToPxVec2(DirectX::XMFLOAT2 vec)
{
	return physx::PxVec2(vec.x, vec.y);
}

inline physx::PxVec4 ToPxVec4(DirectX::XMFLOAT4 vec)
{
	return physx::PxVec4(vec.x, vec.y, vec.z, vec.w);
}

inline physx::PxQuat ToPxQuat(DirectX::XMFLOAT4 vec)
{
	return physx::PxQuat(vec.x, vec.y, vec.z, vec.w);
}
inline DirectX::XMFLOAT3 ToXMFLOAT3(physx::PxVec3 vec)
{
	return DirectX::XMFLOAT3(vec.x, vec.y, vec.z);
}

inline DirectX::XMFLOAT3 ToXMFLOAT3(physx::PxExtendedVec3 vec)
{
	return DirectX::XMFLOAT3(static_cast<float>(vec.x), static_cast<float>(vec.y), static_cast<float>(vec.z));
}