#include "stdafx.h"
#include <ctime>
#include "GalagaWarScene.h"
#include "GameObject.h"
#include "OverlordGame.h"
#include "GalagaCamera.h"
//Renderers
#include "DebugRenderer.h"
#include "TextRenderer.h"
//Managers
#include "SoundManager.h"
#include "ContentManager.h"
#include "Managers/StateManager.h"
#include "Managers/PickupManager.h"
//Components
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "ModelComponent.h"
#include "ParticleEmitterComponent.h"
//Prefabs
#include "SkyBoxPrefab.h"
#include "Prefabs/EnemyPrefab.h"
#include "Prefabs/SpaceShipPrefab.h"
#include "Prefabs/AsteroidPrefab.h"
#include "Prefabs/CoinPrefab.h"
//Materials
#include "../Materials/SkinnedDiffuseMaterial.h"
#include "../Materials/UberMaterial.h"
#include "../Materials/DiffuseMaterial.h"
//UI
#include "SpriteFont.h"
#include "UI/TextObject.h"
#include "UI/SettingsUIObject.h"
#include "UI/PauseUIObject.h"
#include  "UI/IntroUIObject.h"
#include "UI/PickupUIObject.h"
//Paths
#include "Paths/BezierPath.h"
//PostProcessing
#include "PostProcessing/PostColorGrading.h"
#include "Sound/SoundObject.h"
#include "Prefabs/RainParticleObject.h"

GalagaWarScene::GalagaWarScene()
	: GameScene(L"GalagaWarScene")
	, m_SpaceShip(new SpaceShipPrefab())
	, m_PathHelper()
{}	

GalagaWarScene::~GalagaWarScene()
{
	StateManager::DestroyInstance();
	PickupManager::DestroyInstance();

	for(auto ui : m_UIObjects) {
		if (ui.second) {
			if(!ui.second->GetScene())
			AddChild(ui.second);
		}
	}
}

void GalagaWarScene::Initialize()
{
	DebugRenderer::ToggleDebugRenderer();
	const auto gameContext = GetGameContext();
	srand((unsigned)time(NULL));

	AddUI();
	AddInputs();
	AddSounds();
	AddMaterials();
	AddFixedCamera();

	AddChild(new SkyBoxPrefab(L"./Resources/Textures/BlueSpace.dds"));
	AddEnemies();
	AddSceneProps();
	AddSpaceShip();
	AddChild(new RainParticleObject());
	AddPostProcessingEffect(new PostColorGrading());
	AddResetGameCallback();

	PickupManager::GetInstance()->SetCoinPickupCount(this, 5);
}

void GalagaWarScene::Update()
{
	SoundManager::GetInstance()->GetSystem()->update();
	StateManager::GetInstance()->Update(GetGameContext());
	UpdateDynamicUI();
	UpdateSceneProps();
	UpdateEnemyMovement();
	HandleInputTrigger();
}

void GalagaWarScene::Draw()
{
}

#pragma region ENEMY
void GalagaWarScene::AddLeadEnemy(EnemyPrefab* enemy, int followerNumber)
{
	m_LeadEnemies.push_back(enemy);
	enemy->SetTarget(m_SpaceShip); //Enemy's Target For Shooting
	AddChild(enemy);

	for(int i = 0; i < followerNumber; ++i) {
		auto follower = new EnemyPrefab();
		enemy->AddFollower(follower);
		if (i == followerNumber - 1)
			follower->SetTarget(m_SpaceShip);
	}

	SetEnemyPath(enemy);
}

void GalagaWarScene::AddEnemies()
{
	m_PathHelper.GetPathFromFile("./Resources/Data/Paths.txt");
	AddLeadEnemy(new EnemyPrefab(), 4);
}

void GalagaWarScene::SetEnemyPath(EnemyPrefab* obj)
{
	obj->SetPath(m_PathHelper.GetNextPath());
}

void GalagaWarScene::UpdateEnemyMovement()
{
	if (StateManager::GetInstance()->GetState() != State::Playing) return;
	
	for (size_t i = 0; i < m_LeadEnemies.size(); i++)
	{
		//Change to new Path when all followers finish or dead
		if (m_LeadEnemies[i]->IsFinish() || m_LeadEnemies[i]->ShouldRestart())
		{
			SetEnemyPath(m_LeadEnemies[i]);
		}
	}
}
#pragma endregion 

#pragma region SCENE_GAMEOBJECTS
void GalagaWarScene::AddSpaceShip()
{
	m_SpaceShip->GetTransform()->Translate(0,0,-200);
	m_SpaceShip->GetTransform()->Rotate(0,180,0);
	AddChild(m_SpaceShip);
}

void GalagaWarScene::AddSounds()
{
	m_SoundObject = SoundObject();
	m_SoundObject.AddSound("./Resources/Sounds/Startup.mp3", true, false);
	m_SoundObject.AddSound("./Resources/Sounds/Theme.mp3", true, true);
	m_SoundObject.Play(0);
}

void GalagaWarScene::AddUI()
{
	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/SF TransRobotics_64.fnt");

	auto startUI = new IntroUIObject();
	auto pickupUI = new PickupUIObject(m_pFont);
	m_UIObjects["StartUI"] = startUI;
	m_UIObjects["PickupUI"] = pickupUI;
	AddChild(startUI);
	AddChild(pickupUI);
}

void GalagaWarScene::AddFixedCamera()
{
	m_GalagaCamera = new GalagaCamera();
	m_GalagaCamera->SetTarget(m_SpaceShip);
	AddChild(m_GalagaCamera);
#ifndef GalagaFreeCamera
	auto camera = m_GalagaCamera->GetComponent<CameraComponent>();
	camera->SetActive();
#endif
}

void GalagaWarScene::AddSceneProps()
{
#pragma region
	m_Asteroids.push_back(new AsteroidPrefab(1, 4.5f));
	m_Asteroids.push_back(new AsteroidPrefab(2, 4.5f));
	m_Asteroids.push_back(new AsteroidPrefab(3, 4.5f));
	for(auto asteroid : m_Asteroids)
		AddChild(asteroid);

	m_Asteroids[0]->GetTransform()->Translate(-80,-500, -30);
	m_Asteroids[1]->GetTransform()->Translate(-200,-500,-70);
	m_Asteroids[2]->GetTransform()->Translate(-170,-500,-90);

	m_PlanetAsteroids.push_back(new AsteroidPrefab(1));
	m_PlanetAsteroids.push_back(new AsteroidPrefab(2));
	m_PlanetAsteroids.push_back(new AsteroidPrefab(3));
	m_PlanetAsteroids.push_back(new AsteroidPrefab(1));
	m_PlanetAsteroids.push_back(new AsteroidPrefab(3));
	m_PlanetAsteroids.push_back(new AsteroidPrefab(2));

	//Set Position Manually
	m_PlanetAsteroids[0]->GetTransform()->Translate(120,50,50);
	m_PlanetAsteroids[1]->GetTransform()->Translate(-135,50,50);
	m_PlanetAsteroids[2]->GetTransform()->Translate(0,140,50);
	m_PlanetAsteroids[3]->GetTransform()->Translate(-70,130,50);
	m_PlanetAsteroids[4]->GetTransform()->Translate(-120,-100,50);
	m_PlanetAsteroids[5]->GetTransform()->Translate(50, -160,50); //x,z,y
#pragma endregion Asteroids

#pragma region
	auto planet2 = new GameObject();
	ModelComponent* model2 = new ModelComponent(L"./Resources/Meshes/Sphere.ovm");
	model2->SetMaterial(16);
	planet2->AddComponent(model2);
	planet2->GetTransform()->Scale(1, 1, 1);
	planet2->GetTransform()->Translate(70, -190, -90);
	planet2->GetTransform()->Rotate(-0.45f, 0.3f, 0, false);
	AddChild(planet2);

	m_Planet = new GameObject();
	ModelComponent* model = new ModelComponent(L"./Resources/Meshes/Sphere.ovm");
	model->SetMaterial(15);
	m_Planet->AddComponent(model);
	m_Planet->GetTransform()->Scale(0.325f, 0.325f, 0.325f);
	m_Planet->GetTransform()->Translate(-40, -60, 50);
	AddChild(m_Planet);
	
	auto planetRing = new GameObject();
	auto planetRingModel = new ModelComponent(L"./Resources/Meshes/UnitPlane.ovm"); 
	planetRingModel->SetMaterial(20);
	planetRing->AddComponent(planetRingModel);
	planetRing->GetTransform()->Scale(40, 40, 40);
	planetRing->GetTransform()->Rotate(90.f, 0, 0);
	m_Planet->AddChild(planetRing);

	//Attach Asteroids
	for(auto asteroid : m_PlanetAsteroids)
		m_Planet->AddChild(asteroid);

#pragma endregion Planets
}
#pragma endregion 

#pragma region UPDATE_GAMEOBJECTS
void GalagaWarScene::UpdateSceneProps()
{
	const auto gameContext = GetGameContext();
	m_Planet->GetTransform()->Rotate(-0.75f, 0.75f, (DirectX::XM_PI / 10)*gameContext.pGameTime->GetTotal(), false); 
}

void GalagaWarScene::UpdateDynamicUI()
{
	const auto gameContext = GetGameContext();

	if (gameContext.pInput->IsActionTriggered(16))
	{
		const auto key = "SettingsUI";
		auto settingUI = m_UIObjects[key];

		if (!settingUI) 
			settingUI = m_UIObjects[key] = new SettingUIObject(m_pFont);

		if (!m_UIObjects[key]->GetScene()) 
			AddChild(m_UIObjects[key]);
	}

	auto stateManager = StateManager::GetInstance();
	auto currentState = stateManager->GetState();

	if (currentState == State::Playing 
		|| currentState == State::Pause)
	{
		if (gameContext.pInput->IsActionTriggered(11) || stateManager->GetWinState() != WinState::None)
		{
			stateManager->SetState(State::Pause);

			const auto key = "PauseUI";
			auto pauseUI = m_UIObjects[key];

			if (!pauseUI)
				pauseUI = m_UIObjects[key] = new PauseUIObject(m_pFont, m_ResetGameCallback);

			if (!pauseUI->GetScene())
				AddChild(pauseUI);
		}
	}
}

#pragma endregion 

#pragma region GAMEPLAY

void GalagaWarScene::StartGame()
{
	m_SoundObject.Play(1);
	m_SpaceShip->GetTransform()->Translate(0, 0, -200);
	m_SpaceShip->GetTransform()->Rotate(0, 180, 0);
	RemoveChild(m_UIObjects["StartUI"], false);

	auto animationFinishCallback = []() {
		StateManager::GetInstance()->SetState(State::Playing);
	};

	m_GalagaCamera->Animate(animationFinishCallback);
}

void GalagaWarScene::AddResetGameCallback()
{
	m_ResetGameCallback = [&]() {

		for (auto enemy : m_LeadEnemies)
			SetEnemyPath(enemy);

		m_SoundObject.Play(0);
		m_SpaceShip->Reset();
		AddChild(m_UIObjects["StartUI"]);

		PickupManager::GetInstance()->ResetCoins();
		StateManager::GetInstance()->SetState(State::Intro);
		StateManager::GetInstance()->SetWinState(WinState::None);
	};
}
#pragma endregion 

#pragma region INPUTS

void GalagaWarScene::HandleInputTrigger()
{
	if (GetGameContext().pInput->IsActionTriggered(10))
	{
		if (StateManager::GetInstance()->GetState() == State::Intro) {

			StartGame();
		}
	}
}

void GalagaWarScene::AddInputs()
{
	auto gameContext = GetGameContext();
	const InputAction startInput = InputAction{ 10, InputTriggerState::Pressed, VK_RETURN, -1, XINPUT_GAMEPAD_A };
	const InputAction exitInput = InputAction{ 11, InputTriggerState::Pressed, VK_ESCAPE, -1, XINPUT_GAMEPAD_B };
	const InputAction settingInput = InputAction{ 16, InputTriggerState::Pressed, 'P', -1, XINPUT_GAMEPAD_START };
	const InputAction selectButton = InputAction{ 20, InputTriggerState::Pressed, VK_RETURN, -1, XINPUT_GAMEPAD_A };
	const InputAction goDown = InputAction{ 12, InputTriggerState::Pressed, VK_DOWN, -1, XINPUT_GAMEPAD_DPAD_DOWN };
	const InputAction goUp = InputAction{ 13, InputTriggerState::Pressed, VK_UP, -1, XINPUT_GAMEPAD_DPAD_UP };
	const InputAction goLeft = InputAction{ 15, InputTriggerState::Pressed, VK_LEFT, -1, XINPUT_GAMEPAD_DPAD_LEFT };
	const InputAction goRight = InputAction{ 14, InputTriggerState::Pressed, VK_RIGHT, -1, XINPUT_GAMEPAD_DPAD_RIGHT };
	gameContext.pInput->AddInputAction(startInput);
	gameContext.pInput->AddInputAction(exitInput);
	gameContext.pInput->AddInputAction(selectButton);
	gameContext.pInput->AddInputAction(settingInput);
	gameContext.pInput->AddInputAction(goDown);
	gameContext.pInput->AddInputAction(goUp);
	gameContext.pInput->AddInputAction(goLeft);
	gameContext.pInput->AddInputAction(goRight);
}
#pragma endregion 

#pragma region MATERIALS
void GalagaWarScene::AddMaterials()
{
	const auto gameContext = GetGameContext();

	auto enemyWingTransparency = 0.75f;
	auto enemyTexturePath = L"./Resources/Textures/SMALLBUG.png";
	 auto enemyMaterial = new SkinnedDiffuseMaterial();
	enemyMaterial->SetDiffuseTexture(enemyTexturePath);
	enemyMaterial->SetTransparentColor(DirectX::XMFLOAT4(0, 0.8f, 0.8f, enemyWingTransparency));
	gameContext.pMaterialManager->AddMaterial(enemyMaterial, 0);
	
	UberMaterial* bulletMat = new UberMaterial();
	bulletMat->EnableOpacityMap(true);
	bulletMat->SetDiffuseTexture(L"Resources/Textures/bullet_laser.png");
	bulletMat->SetOpacityTexture(L"Resources/Textures/bullet_laser.png"); 
	gameContext.pMaterialManager->AddMaterial(bulletMat, 6);

	UberMaterial* bulletMat2 = new UberMaterial();
	bulletMat2->EnableOpacityMap(true);
	bulletMat2->SetDiffuseTexture(L"Resources/Textures/bullet_laser2.png");
	bulletMat2->SetOpacityTexture(L"Resources/Textures/bullet_laser.png"); 
	gameContext.pMaterialManager->AddMaterial(bulletMat2, 7);

	UberMaterial* ringMat = new UberMaterial();
	ringMat->SetOpacity(1);
	ringMat->EnableOpacityMap(true);
	ringMat->SetDiffuseTexture(L"Resources/Textures/Ring.png");
	ringMat->SetOpacityTexture(L"Resources/Textures/Ring.png");
	gameContext.pMaterialManager->AddMaterial(ringMat, 20);

	DiffuseMaterial* astMat = new DiffuseMaterial();
	astMat->SetDiffuseTexture(L"Resources/Textures/Asteroids_BaseColor.png");
	gameContext.pMaterialManager->AddMaterial(astMat, 19);

	DiffuseMaterial* planetMat = new DiffuseMaterial();
	planetMat->SetDiffuseTexture(L"Resources/Textures/Planet.jpg");
	gameContext.pMaterialManager->AddMaterial(planetMat, 15);

	DiffuseMaterial* planetMat2 = new DiffuseMaterial();
	planetMat2->SetDiffuseTexture(L"Resources/Textures/Planet2.jpg");
	gameContext.pMaterialManager->AddMaterial(planetMat2, 16);

	UberMaterial* coinMat = new UberMaterial();
	coinMat->EnableNormalMapping(false);
	coinMat->SetNormalMapTexture(L"Resources/Textures/Coin/DL_ItemBell_Nrm.png");
	coinMat->SetDiffuseTexture(L"Resources/Textures/Coin/DL_ItemBell_Alb3.png");
	coinMat->SetDiffuseColor(DirectX::XMFLOAT4(1, 1, 0, 0));
	gameContext.pMaterialManager->AddMaterial(coinMat, 33);
}
#pragma endregion 
