#pragma once
#include "GameScene.h"
#include "Sound/SoundObject.h"
#include "Paths/PathHelper.h"
#include <map>
class FreeCamera;
class EnemyPrefab;
class SpaceShipPrefab;
class SpriteFont;
class TextObject;
class GalagaCamera;
class UIObject;

class GalagaWarScene final : public GameScene
{
public:

	GalagaWarScene();
	virtual ~GalagaWarScene();
	GalagaWarScene(const GalagaWarScene& other) = delete;
	GalagaWarScene(GalagaWarScene&& other) noexcept = delete;
	GalagaWarScene& operator=(const GalagaWarScene& other) = delete;
	GalagaWarScene& operator=(GalagaWarScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	PathHelper m_PathHelper;
	//SOUNDOBJECT
	SoundObject m_SoundObject;
	//GAMEOBJECTS
	GameObject* m_Planet;
	SpaceShipPrefab* m_SpaceShip;
	GalagaCamera* m_GalagaCamera;
	std::vector<GameObject*> m_Asteroids{};
	std::vector<GameObject*> m_PlanetAsteroids{};
	//ENEMIES
	std::vector<EnemyPrefab*> m_LeadEnemies{};
	//UIs
	SpriteFont* m_pFont;
	std::map<std::string, UIObject*> m_UIObjects;
	std::function<void()> m_ResetGameCallback;

	//----------------- FUNCTIONS ----------------
	void AddEnemies();
	void AddLeadEnemy(EnemyPrefab* obj, int followerNumber);
	void SetEnemyPath(EnemyPrefab* obj);

	void AddUI();
	void AddInputs();
	void AddSounds();
	void AddFixedCamera();
	void AddSpaceShip();
	void AddSceneProps();
	void AddMaterials();
	
	void UpdateSceneProps();
	void UpdateDynamicUI();
	void UpdateEnemyMovement();
	void HandleInputTrigger();

	void StartGame();
	void AddResetGameCallback();
};

