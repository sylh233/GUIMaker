#ifndef MAIN
#define MAIN

#include <SDL3/SDL.h>
#include <gui.h>

extern SDL_Renderer *rend;
extern SDL_Event eve;
extern const int Ww;
extern const int Wh;
extern std::string Path;
extern std::string ttfPath;
extern std::string mainFontName;
extern std::string bigFontName;

// 函数原型
int init();
void event(SDL_Event &event);
SDL_FRect genRect(gui::Point p, double width, size_t length);
SDL_FRect genRectCenter(gui::Point p, double width, size_t length);
SDL_Texture *getFontTex(std::string fontName, SDL_Color color, std::string text,
                        float size = 20);
bool InRect(SDL_FRect *rect, gui::Point point);

#endif
