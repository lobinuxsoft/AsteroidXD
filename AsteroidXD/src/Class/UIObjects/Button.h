#pragma once
#include "../Entity.h"
#include <string>

class Button : public Entity
{
private:
	std::string text = "";
	int fontSize = 10;
	Color textColor = WHITE;
	Color normalColor = BLUE;
	Color pressColor = DARKBLUE;
	float hMargin = 5;
	float vMargin = 5;
	float roundness = 0.5f;
	int segment = 16;
	Rectangle rect{ 0,0,0,0 };
	bool hover = false;
	bool press = false;
	bool click = false;

private:
	void mouseCollision();

public:
	Button(Vector2 position, std::string text, int fontSize = 10, Color textColor = WHITE, Color normalColor = BLUE, Color pressColor = DARKBLUE, float hMargin = 5, float vMargin = 5, float roundness = 0.5f, int segment = 16);
	bool isClick();
	void update();
	void draw() override;
};

