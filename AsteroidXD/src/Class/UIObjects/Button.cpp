#include "Button.h"

void Button::mouseCollision()
{
    hover = CheckCollisionPointRec(GetMousePosition(), rect);
    press = hover && IsMouseButtonDown(0);
    click = hover && IsMouseButtonReleased(0);
}

Button::Button(Vector2 position, std::string text, int fontSize, Color textColor, Color normalColor, Color pressColor, float hMargin, float vMargin, float roundness, int segment) : 
    Entity{ position }, text(text), fontSize(fontSize), textColor(textColor), normalColor(normalColor), pressColor(pressColor), hMargin(hMargin), vMargin(vMargin), roundness(roundness), segment(segment) { }

bool Button::isClick()
{
    return click;
}

void Button::update()
{
    mouseCollision();
}

void Button::draw()
{
    rect = Rectangle
    {
        position.x,
        position.y,
        hMargin * 2 + MeasureText(text.c_str(), fontSize),
        vMargin * 2 + fontSize
    };

    DrawRectangleRounded(rect, 0.5f, 16, press ? pressColor : normalColor);
    DrawText(text.c_str(), position.x + hMargin, position.y + vMargin, fontSize, textColor);
}
