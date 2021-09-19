#pragma once
#include "Class/Entity.h"
#include <string>

class Button : public Entity
{
private:
	std::string text = "";
	Sound clickSfx;
	int fontSize = 10;
	Color textColor = WHITE;
	Color normalColor = BLUE;
	Color pressColor = DARKBLUE;
	float hMargin = 5;
	float vMargin = 5;
	float roundness = 0.5f;
	int segment = 16;
	int tickness = 1;
	Rectangle rect{ 0,0,0,0 };
	Vector2 pivot = {0.0f, 0.0f};
	bool hover = false;
	bool press = false;
	bool click = false;

private:
	void mouseCollision();

public:
	Button(
		Vector2 position,
		std::string text,
		const char clickSfxUrl[],
		int fontSize = 10,
		float hMargin = 5,
		float vMargin = 5,
		float roundness = 0.5f,
		int segment = 16,
		int tickness = 1,
		Color textColor = WHITE,
		Color normalColor = BLUE,
		Color pressColor = DARKBLUE
	);

	void setPivot(Vector2 pivot);
	void setText(std::string text);
	bool isClick();
	void update();
	void draw() override;
};

