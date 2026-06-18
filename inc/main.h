#ifndef MAIN
#define MAIN

#include <SDL3/SDL.h>
#include <gui.h>

extern SDL_Renderer *rend;
extern SDL_Event eve;
extern std::string mainFontName;
extern std::string bigFontName;

// 函数原型
int init();
void event(SDL_Event &event);
SDL_FRect genRect(gui::Point p, double width, size_t length);
SDL_Texture *getFontTex(std::string fontName, SDL_Color color, std::string text,
                        float size = 20);

#endif
