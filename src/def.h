#ifndef DEF_H_
#define DEF_H_
#include "raylib.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define TOP_LEFT_CORNER CLITERAL(Vector2){0, 0}
#define BOTTOM_RIGHT_CORNER CLITERAL(Vector2){SCREEN_WIDTH, SCREEN_HEIGHT}
#define TOP_RIGHT_CORNER CLITERAL(Vector2){SCREEN_WIDTH, 0}
#define BOTTOM_LEFT_CORNER CLITERAL(Vector2){0, SCREEN_HEIGHT}
#define SCREEN_CENTER CLITERAL(Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2}
#define FONT_SIZE_DEFAULT 10
#define FONT_SIZE_SMALL 10
#define TEXT_ALIGN_DEFAULT TEXT_ALIGN_LEFT

#define COLOR_LIGHTBLUE CLITERAL(Color){220, 250, 255, 255}
#define COLOR_LIGHTBLUE_ALPHA CLITERAL(Color){220, 250, 255, 100}


#endif