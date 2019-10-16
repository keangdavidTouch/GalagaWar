
#include "stdafx.h"

#include "MainGame.h"
#include "GeneralStructs.h"
#include "SceneManager.h"
#include "PhysxProxy.h"
#include "DebugRenderer.h"

//#define W9
//#define W10

#ifdef W1
#include "CourseObjects/Week 1/MinionScene.h"
#include "CourseObjects/Week 1/TutorialScene.h"
#include "CourseObjects/Week 1/ComponentTestScene.h"
#endif

#ifdef W2
#include "CourseObjects/Week 2/ModelTestScene.h"
#include "CourseObjects/Week 2/CharacterScene.h"
#endif

#ifdef W3_A
#include "CourseObjects/Week 3/SpriteTestScene.h"
#include "CourseObjects/Week 3/UberScene.h"
#endif

#ifdef W3_B
#include "CourseObjects/Week 3/SpikeyScene.h"
#endif

#ifdef W4_A
#include "CourseObjects/Week 4/FontTestScene.h"
#endif

#ifdef W4_B
#include "CourseObjects/Week 4/PickScene.h"
#endif

#ifdef W5
#include "CourseObjects/Week 5/SoftwareSkinningScene_1.h"
#include "CourseObjects/Week 5/SoftwareSkinningScene_2.h"
#include "CourseObjects/Week 5/SoftwareSkinningScene_3.h"
#endif

#ifdef W6
#include "CourseObjects/Week 6/HardwareSkinningScene.h"
#endif

#ifdef W7_A
#include "CourseObjects/Week 7/TerrainTestScene.h"
#endif

#ifdef W7_B
#include "CourseObjects/Week 7/SkyBoxScene.h"
#endif

#ifdef W8
#include "CourseObjects/Week 8/ShadowMappingScene.h"
#endif

#ifdef W9
#include "CourseObjects/Week 9/ParticleScene.h"
#endif

#ifdef W10
#include "CourseObjects/Week 10/PostProcessingScene.h"
#endif

#ifdef GalagaWar
#include "CourseObjects/GalagaWar/GalagaWarScene.h"
#endif

MainGame::MainGame(void)
{}

MainGame::~MainGame(void)
{}

//Game is preparing
void MainGame::OnGamePreparing(GameSettings& gameSettings)
{
	UNREFERENCED_PARAMETER(gameSettings);
	//Nothing to do atm.
}

void MainGame::Initialize()
{
	
#ifdef GalagaWar

	//SceneManager::GetInstance()->AddGameScene(new StartScreen());
	SceneManager::GetInstance()->AddGameScene(new GalagaWarScene());
	//SceneManager::GetInstance()->SetActiveGameScene(L"StartScreen");
	SceneManager::GetInstance()->SetActiveGameScene(L"GalagaWarScene");
#endif
#ifdef W1
	SceneManager::GetInstance()->AddGameScene(new MinionScene());
	SceneManager::GetInstance()->AddGameScene(new TutorialScene());
	SceneManager::GetInstance()->AddGameScene(new ComponentTestScene());
	SceneManager::GetInstance()->AddGameScene(new PongScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"MinionScene");
#endif

#ifdef W2
	SceneManager::GetInstance()->AddGameScene(new ModelTestScene());
	SceneManager::GetInstance()->AddGameScene(new CharacterScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"ModelTestScene");
#endif

#ifdef W3_A
	SceneManager::GetInstance()->AddGameScene(new SpriteTestScene());
	SceneManager::GetInstance()->AddGameScene(new UberScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"SpriteTestScene");
#endif

#ifdef W3_B
	SceneManager::GetInstance()->AddGameScene(new SpikeyScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"SpikeyScene");
#endif

#ifdef W4_A
	SceneManager::GetInstance()->AddGameScene(new FontTestScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"FontTestScene");
#endif

#ifdef W4_B
	SceneManager::GetInstance()->AddGameScene(new PickScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"PickScene");
#endif

#ifdef W5
	SceneManager::GetInstance()->AddGameScene(new SoftwareSkinningScene_1());
	SceneManager::GetInstance()->AddGameScene(new SoftwareSkinningScene_2());
	SceneManager::GetInstance()->AddGameScene(new SoftwareSkinningScene_3());
	SceneManager::GetInstance()->SetActiveGameScene(L"SoftwareSkinningScene_1");
#endif

#ifdef W6
	SceneManager::GetInstance()->AddGameScene(new HardwareSkinningScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"HardwareSkinningScene");
#endif

#ifdef W7_A
	SceneManager::GetInstance()->AddGameScene(new TerrainTestScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"TerrainTestScene");
#endif

#ifdef W7_B
	SceneManager::GetInstance()->AddGameScene(new SkyBoxScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"SkyBoxScene");
#endif

#ifdef W8
	SceneManager::GetInstance()->AddGameScene(new ShadowMappingScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"ShadowMappingScene");
#endif

#ifdef W9
	SceneManager::GetInstance()->AddGameScene(new ParticleScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"ParticleScene");
#endif
	
#ifdef W10
	SceneManager::GetInstance()->AddGameScene(new PostProcessingScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"PostProcessingScene");
#endif
}

LRESULT MainGame::WindowProcedureHook(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(message);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
		case WM_KEYUP:
		{
			if ((lParam & 0x80000000) != 0x80000000)
				return -1;

			//NextScene
			if (wParam == VK_F3)
			{
				SceneManager::GetInstance()->NextScene();
				return 0;
			}
			//PreviousScene
			else if (wParam == VK_F2)
			{
				SceneManager::GetInstance()->PreviousScene();
				return 0;
			}
			else if (wParam == VK_F4)
			{
				DebugRenderer::ToggleDebugRenderer();
				return 0;
			}
			else if (wParam == VK_F6)
			{
				auto activeScene = SceneManager::GetInstance()->GetActiveScene();
				activeScene->GetPhysxProxy()->NextPhysXFrame();
			}
		}
	}

	return -1;
}
