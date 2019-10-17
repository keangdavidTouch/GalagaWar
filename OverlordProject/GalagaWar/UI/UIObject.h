#pragma once
#include "GameObject.h"
#include "GameScene.h"

class SpriteComponent;
class SpriteFont;
class TextObject;

class UIObject : public GameObject
{
public:
	UIObject() = default;
	UIObject(const UIObject& other) = delete;
	UIObject(UIObject&& other) noexcept = delete;
	UIObject& operator=(const UIObject& other) = delete;
	UIObject& operator=(UIObject&& other) noexcept = delete;
	~UIObject() = default;
};