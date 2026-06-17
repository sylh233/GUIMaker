#ifndef DATASTRUCT
#define DATASTRUCT

#include <SDL3/SDL.h>
#include <map>
#include <string>

extern std::string mainFontName;
extern std::string bigFontName;

namespace ds {
struct Point {
	double x, y;
};
typedef struct Point Point;

class stateScene {
  private:
	std::map<std::string, SDL_Texture *> FontsTex;

  public:
	stateScene();
	void event(SDL_Event &event);
	void show();
};
	
class stateTree {
  private:
	std::map<std::string,stateScene*> tree;
  public:
	
};
}; // namespace ds

#endif
