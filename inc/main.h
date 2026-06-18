#ifndef MAIN
#define MAIN

#include <SDL3/SDL.h>
#include <gui.h>

// 函数原型
int init();
void event(SDL_Event &event);
SDL_FRect genRect(gui::Point p, double width, size_t length);
SDL_Texture *getFontTex(std::string fontName, SDL_Color color, std::string text,
                        size_t size = 20);

#endif
