#include "stdafx.h"
#include <vld.h>
#include "MainGame.h"
#include <dxgidebug.h>

int wmain(int argc, wchar_t* argv[])
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	wWinMain(GetModuleHandle(0), 0, 0, SW_SHOW);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(nCmdShow);
	UNREFERENCED_PARAMETER(pCmdLine);
	//notify user if heap is corrupt
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL,0);

	// Enable run-time memory leak check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

		typedef HRESULT(__stdcall *fPtr)(const IID&, void**); 
		HMODULE hDll = LoadLibrary("dxgidebug.dll"); 
		fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface"); 

		IDXGIDebug* pDXGIDebug;
		DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&pDXGIDebug);
		//_CrtSetBreakAlloc(8124);
	#endif

	auto pGame = new MainGame();
	auto result = pGame->Run(hInstance);
	UNREFERENCED_PARAMETER(result);
	delete pGame;
	
	return 0;
}
