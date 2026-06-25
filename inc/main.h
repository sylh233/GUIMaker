#ifndef MAIN
#define MAIN

#include <GUI.h>

extern SDL_Renderer *rend;
extern SDL_Event eve;
extern const int Ww;
extern const int Wh;
extern const double TEXTwhRATIO;
extern const double TEXTwhRATIOC;
extern std::string Path;
extern std::string ttfPath;
extern std::string mainFontName;
extern std::string bigFontName;

// 函数原型
int init();
void event(SDL_Event &event);
void many_event();
void many_run();

#endif
