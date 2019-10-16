#include "stdafx.h"
#include "PhysxErrorCallback.h"
#include <string>
#include <sstream>

void PhysxErrorCallback::reportError(physx::PxErrorCode::Enum code , const char* message, const char* file, int line)
{
	std::wstringstream ss;
	ss<<L"PHYSX CALLBACK REPORT:"<< std::endl;
	ss<<"Error Code: "<<code<< std::endl;
	ss<<"Message: "<<message<< std::endl;
	ss<<"File: "<<file<<" (line: "<<line<<")";

	switch(code)
	{
	case physx::PxErrorCode::eDEBUG_INFO:
		Logger::LogInfo(ss.str());
		break;
	case physx::PxErrorCode::ePERF_WARNING:
	case physx::PxErrorCode::eDEBUG_WARNING:
			Logger::LogWarning(ss.str());
			break;
	default:
		Logger::LogError(ss.str());
		break;
	}
}