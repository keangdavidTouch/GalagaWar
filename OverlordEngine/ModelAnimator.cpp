#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
m_pMeshFilter(pMeshFilter),
m_Transforms(std::vector<DirectX::XMFLOAT4X4>()),
m_IsPlaying(false), 
m_Reversed(false),
m_ClipSet(false),
m_TickCount(0),
m_AnimationSpeed(1.0f)
{
	SetAnimation(0);
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	//TODO: complete
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	if (clipNumber < GetClipCount()) {
		//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		auto clip = m_pMeshFilter->m_AnimationClips[clipNumber];
		//	Call SetAnimation(AnimationClip clip)
		SetAnimation(clip);
	}
	else {
		Reset();
		Logger::LogFormat(LogLevel::Warning, L"clipNumber is NOT smaller than the actual m_AnimationClips vector size");
	}
	
}

void ModelAnimator::SetAnimation(std::wstring clipName)
{
	//TODO: complete
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	auto nameLamda = [clipName](AnimationClip clip) { return clip.Name == clipName; };
	auto findClip = std::find_if(m_pMeshFilter->m_AnimationClips.begin(), m_pMeshFilter->m_AnimationClips.end(), nameLamda);
	if (findClip != m_pMeshFilter->m_AnimationClips.end()) {
		SetAnimation(*findClip);
	}
	else {
		Reset();
		Logger::LogFormat(LogLevel::Warning, L"Can't find AnimationClip with name...");
	}
}

void ModelAnimator::SetAnimation(AnimationClip clip)
{
	//TODO: complete
	//Set m_ClipSet to true
	m_ClipSet = true;
	//Set m_CurrentClip
	m_CurrentClip = clip;
	//Call Reset(false)
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	UNREFERENCED_PARAMETER(pause);
	//TODO: complete
	//If pause is true, set m_IsPlaying to false
	if (pause) 
		m_IsPlaying = false;

	//Set m_TickCount to zero
	m_TickCount = 0;
	//Set m_AnimationSpeed to 1.0f
	m_AnimationSpeed = 1.0f;

	//If m_ClipSet is true
	//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
	//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
	//Else
	//	Create an IdentityMatrix 
	//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
	if (m_ClipSet) {
		auto boneTransform = m_CurrentClip.Keys.front().BoneTransforms;
		m_Transforms.assign(boneTransform.begin(), boneTransform.end());
	}
	else {
		DirectX::XMFLOAT4X4 boneTransform{}; // IdenityMatrix?????
		DirectX::XMStoreFloat4x4(&boneTransform, DirectX::XMMatrixIdentity());
		m_Transforms.assign(m_pMeshFilter->m_BoneCount, boneTransform);
	}
}

void ModelAnimator::Update(const GameContext& gameContext)
{
	//TODO: complete
	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		auto passedTicks = gameContext.pGameTime->GetElapsed() * m_CurrentClip.TicksPerSecond * GetAnimationSpeed();
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		passedTicks = fmod(passedTicks, m_CurrentClip.Duration);

		//2. 
		//IF m_Reversed is true
		//	Subtract passedTicks from m_TickCount
		//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
		//ELSE
		//	Add passedTicks to m_TickCount
		//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
		if (m_Reversed) {
			m_TickCount -= passedTicks;
			if (m_TickCount < 0) 
				m_TickCount += m_CurrentClip.Duration;
		}
		else {
			m_TickCount += passedTicks;
			if (m_TickCount > m_CurrentClip.Duration) 
				m_TickCount -= m_CurrentClip.Duration;
		}

		//3.
		//Find the enclosing keys
		AnimationKey keyA, keyB;
		//Iterate all the keys of the clip and find the following keys:
		//keyA > Closest Key with Tick before/smaller than m_TickCount
		//keyB > Closest Key with Tick after/bigger than m_TickCount
		for (size_t i = 0; i < m_CurrentClip.Keys.size(); i++) {
			
			if (m_CurrentClip.Keys[i].Tick >= m_TickCount) {
				keyB = m_CurrentClip.Keys[i];
				keyA = m_CurrentClip.Keys[i - 1];
			 	break;
			}
		}

		//std::cout << "AnimationKey: " << m_TickCount << " keyA: " << keyA.Tick <<  " keyB: " << keyB.Tick << std::endl;

		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		//function(val, max, min) { return (val - min) / (max - min); }
		float blendFactor2 = (m_TickCount - keyA.Tick) / (keyB.Tick - keyA.Tick);
	//	float blendFactor1 = 1 - blendFactor2;
		//Clear the m_Transforms vector
		m_Transforms.clear();
		//FOR every boneTransform in a key (So for every bone)
		for (size_t i = 0; i < keyA.BoneTransforms.size(); i++) {

			//	Retrieve the transform from keyA (transformA)
			auto transformA = DirectX::XMLoadFloat4x4(&keyA.BoneTransforms[i]);
			// 	Retrieve the transform from keyB (transformB)
			auto transformB = DirectX::XMLoadFloat4x4(&keyB.BoneTransforms[i]);
			//	Decompose both transforms
			DirectX::XMVECTOR scaleA{};
			DirectX::XMVECTOR scaleB{};
			DirectX::XMVECTOR translationA{};
			DirectX::XMVECTOR translationB{};
			DirectX::XMVECTOR rotationA{};
			DirectX::XMVECTOR rotationB{};
			DirectX::XMMatrixDecompose(&scaleA, &rotationA, &translationA, transformA);
			DirectX::XMMatrixDecompose(&scaleB, &rotationB, &translationB, transformB);
			//	Lerp between all the transformations (Position, Scale, Rotation)
			auto lerpScale = DirectX::XMVectorLerp(scaleA, scaleB, blendFactor2);
			auto lerpRotation = DirectX::XMQuaternionSlerp(rotationA, rotationB, blendFactor2);
			auto lerpTranslation = DirectX::XMVectorLerp(translationA, translationB, blendFactor2);

			//	Compose a transformation matrix with the lerp-results
			DirectX::XMMATRIX composeMatrix = DirectX::XMMatrixAffineTransformation(lerpScale, DirectX::XMVECTOR{0,0,0}, lerpRotation, lerpTranslation);
			DirectX::XMFLOAT4X4 composeTransform{};
			DirectX::XMStoreFloat4x4(&composeTransform, composeMatrix);
			//	Add the resulting matrix to the m_Transforms vector
			m_Transforms.push_back(composeTransform);
		}

	}
}
