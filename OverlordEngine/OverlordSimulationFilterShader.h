#pragma once
#define FILTERSHADERTYPE_CUSTOM
#ifdef FILTERSHADERTYPE_CUSTOM
inline physx::PxFilterFlags OverlordSimulationFilterShader(
	physx::PxFilterObjectAttributes attribute0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attribute1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* , physx::PxU32 )
{
	if ((filterData0.word0&filterData1.word1) == filterData0.word0 || (filterData1.word0&filterData0.word1) == filterData1.word0)
	{
		return physx::PxFilterFlag::eSUPPRESS;
	}

	if ((attribute0 & physx::PxFilterObjectFlag::eTRIGGER) != 0 || (attribute1 & physx::PxFilterObjectFlag::eTRIGGER) != 0)
	{
		//pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
		pairFlags |= physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}

	if((filterData0.word0 & filterData1.word0) != 0)
	{
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
		//pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
	}

	pairFlags |= physx::PxPairFlag::eCONTACT_DEFAULT;

	return physx::PxFilterFlag::eDEFAULT;
}
#else
PxFilterFlags OverlordSimulationFilterShader(
	PxFilterObjectAttributes attribute0, PxFilterData filterData0,
	PxFilterObjectAttributes attribute1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* , PxU32 )
{
	// let triggers through
	if (PxFilterObjectIsTrigger(attribute0) || PxFilterObjectIsTrigger(attribute1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlag::eDEFAULT;
}
#endif


