#ifndef LANGUAGE_H_
#define LANGUAGE_H_

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char* NormalizeVietnamese(const char* str, int** resolveTable);
int ConstructUnicodeChar(const char* codePoint);
int** ReadResolveTable(const char* path);

#endif