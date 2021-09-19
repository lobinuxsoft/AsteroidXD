#pragma once
#include "Class/Entity.h"
class ProgressBar : public Entity
{
private:
	float width;
	float height;
	float roundness;
	int segment;
	int lineTick;
	Color front;
	Color back;
	Vector2 pivot = { 0,0 };
	float value;

public:
	ProgressBar(Vector2 position, float width = 300, float height = 10, float roundness = 1, int segment = 12, int lineTick = 5, Color front = BLUE, Color back = DARKBLUE);
	void setPivot(Vector2 pivot);
	void setProgressValue(float value = 1);
	void draw() override;
};

