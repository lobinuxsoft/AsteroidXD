#include "Button.h"

void Button::mouseCollision()
{
    hover = CheckCollisionPointRec(GetMousePosition(), rect);
    press = hover && IsMouseButtonDown(0);
    click = hover && IsMouseButtonReleased(0);
}

Button::Button(Vector2 position, std::string text, const char clickSfxUrl[], int fontSize, float hMargin, float vMargin, float roundness, int segment, int tickness, Color textColor, Color normalColor, Color pressColor) :
    Entity{position},
    text(text),
    clickSfx(LoadSound(clickSfxUrl)),
    fontSize(fontSize),
    hMargin(hMargin),
    vMargin(vMargin),
    roundness(roundness),
    segment(segment),
    tickness(tickness),
    textColor(textColor),
    normalColor(normalColor),
    pressColor(pressColor) { }

void Button::setPivot(Vector2 pivot)
{
    this->pivot = pivot;
}

void Button::setText(std::string text)
{
    this->text = text;
}

bool Button::isClick()
{
    if(click) PlaySound(clickSfx);
    return click;
}

void Button::update()
{
    mouseCollision();
}

void Button::draw()
{

    float deltaX = position.x - (hMargin * 2 + MeasureText(text.c_str(), fontSize)) * pivot.x;
    float deltaY = position.y - (vMargin * 2 + fontSize) * pivot.y;

    rect = Rectangle
    {
        deltaX,
        deltaY,
        hMargin * 2 + MeasureText(text.c_str(), fontSize),
        vMargin * 2 + fontSize
    };

    DrawRectangleRoundedLines(rect, roundness, segment, tickness, press ? normalColor : pressColor);
    DrawRectangleRounded(rect, roundness, segment, press ? pressColor : normalColor);
    DrawText(text.c_str(), rect.x + hMargin, rect.y + vMargin, fontSize, textColor);

#if _DEBUG
    DrawLine(position.x, 0, position.x, GetScreenHeight(), WHITE);
    DrawLine(0, position.y, GetScreenWidth(), position.y, WHITE);
    DrawCircle(position.x, position.y, 5.0f, WHITE);
#endif
}
