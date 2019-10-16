#include "stdafx.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "OverlordGame.h"
#include "GameSpecs.h"
#include <algorithm>

SceneManager::SceneManager():
	m_pScenes(std::vector<GameScene*>()),
	m_IsInitialized(false),
	m_ActiveScene(nullptr),
	m_NewActiveScene(nullptr),
	m_pDevice(nullptr),
	m_pDeviceContext(nullptr),
	m_pGame(nullptr)
{
}

SceneManager::~SceneManager()
{
	for (GameScene* scene : m_pScenes)
	{
		SafeDelete(scene);
	}
}

void SceneManager::AddGameScene(GameScene* pScene)
{
	const auto it = find(m_pScenes.begin(), m_pScenes.end(), pScene);

	if (it == m_pScenes.end())
	{
		m_pScenes.push_back(pScene);

		if (m_IsInitialized)
			pScene->RootInitialize(m_pDevice, m_pDeviceContext);

		if (m_ActiveScene == nullptr && m_NewActiveScene == nullptr)
			m_NewActiveScene = pScene;
	}
}

void SceneManager::RemoveGameScene(GameScene* pScene)
{
	const auto it = find(m_pScenes.begin(), m_pScenes.end(), pScene);

	if (it != m_pScenes.end())
	{
		m_pScenes.erase(it);
	}
}

void SceneManager::NextScene()
{
	for (unsigned int i = 0; i < m_pScenes.size(); ++i)
	{
		if (m_pScenes[i] == m_ActiveScene)
		{
			const auto nextScene = ++i % m_pScenes.size();
			m_NewActiveScene = m_pScenes[nextScene];
			break;
		}
	}
}

void SceneManager::PreviousScene()
{
	for (unsigned int i = 0; i < m_pScenes.size(); ++i)
	{
		if (m_pScenes[i] == m_ActiveScene)
		{
			if (i == 0) i = m_pScenes.size();
			--i;
			m_NewActiveScene = m_pScenes[i];
			break;
		}
	}
}

void SceneManager::SetActiveGameScene(const std::wstring& sceneName)
{
	const auto it = find_if(m_pScenes.begin(), m_pScenes.end(), [sceneName](GameScene* scene)
	{
		return wcscmp(scene->m_SceneName.c_str(), sceneName.c_str()) == 0;
	});

	if (it != m_pScenes.end())
	{
		m_NewActiveScene = *it;
	}
}

void SceneManager::WindowStateChanged(int state, bool active) const
{
	if (state == 0 && m_ActiveScene)
	{
		m_ActiveScene->RootWindowStateChanged(state, active);
	}
}

void SceneManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, OverlordGame* pGame)
{
	if (m_IsInitialized)
		return;

	m_pGame = pGame;

	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	for (GameScene* scene : m_pScenes)
	{
		scene->RootInitialize(pDevice, pDeviceContext);
	}

	m_IsInitialized = true;
}

void SceneManager::Update()
{
	if (m_NewActiveScene != nullptr)
	{
		//Deactivate the current active scene
		if (m_ActiveScene != nullptr)
			m_ActiveScene->RootSceneDeactivated();

		//Set New Scene
		m_ActiveScene = m_NewActiveScene;
		m_NewActiveScene = nullptr;

		//Active the new scene and reset SceneTimer
		m_ActiveScene->RootSceneActivated();
	}

	if (m_ActiveScene != nullptr)
	{
		GameSpecs::Update(m_ActiveScene->m_GameContext);
		m_ActiveScene->RootUpdate();
	}
}

void SceneManager::Draw() const
{
	if (m_ActiveScene != nullptr)
		m_ActiveScene->RootDraw();
}

