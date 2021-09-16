#include "ProgressBar.h"

ProgressBar::ProgressBar(Vector2 position, float width, float height, float roundness, int segment, int lineTick, Color front, Color back):
	Entity{ position }, width(width), height(height), roundness(roundness), segment(segment), lineTick(lineTick), front(front), back(back) { }

void ProgressBar::setProgressValue(float value)
{
	this->value = value;
}

void ProgressBar::draw()
{
	// Shield Bar
	DrawRectangleRoundedLines(Rectangle{ position.x,position.y, width, height }, roundness, segment, lineTick, back);
	DrawRectangleRounded(Rectangle{ position.x, position.y, value * width, height }, roundness, segment, front);
}
