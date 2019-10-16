#pragma once

struct GameContext;

class GameSpecs final
{
public:
	static void Update(const GameContext& context);
	static int GetFPS() { return m_FPS; }
private:
	static int m_FPS;
	static float m_FpsElapsed;
	static int m_FpsFrames;
};

