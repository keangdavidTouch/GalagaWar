#pragma once
#include "OverlordGame.h"

struct GameContext;

class MainGame: public OverlordGame
{
public:
	MainGame(void);
	~MainGame(void);

protected:

	virtual void OnGamePreparing(GameSettings& gameSettings);
	virtual void Initialize();
	virtual LRESULT WindowProcedureHook(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//virtual void Update(GameContext context);
	//virtual void Draw(GameContext context);
};

