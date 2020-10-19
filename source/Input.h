#ifndef INPUT_H
#define INPUT_H

bool MouseOverRectangle(rectangle2 rectangle)
{
    return IsInRectangle(rectangle, gameState->input.mousePosition);
}

bool ClickOverRectangle(rectangle2 rectangle, i32 button = 1)
{
    return gameState->input.mouseState[button] == KEY_PRESSED && MouseOverRectangle(rectangle);
}

bool ClickedOverRectangle(rectangle2 rectangle, i32 button = 1)
{
    return gameState->input.mouseState[button] == KEY_RELEASED && MouseOverRectangle(rectangle);
}

#endif