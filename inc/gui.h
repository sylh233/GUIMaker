#ifndef DATASTRUCT
#define DATASTRUCT

#include <SDL3/SDL.h>
#include <map>
#include <string>
#include <vector>

extern SDL_Renderer *rend;
extern SDL_Event eve;
extern std::string mainFontName;
extern std::string bigFontName;

namespace gui {
struct Point {
	double x, y;
};
typedef struct Point Point;

typedef void (*drawScript)(SDL_Renderer *rend,
                           std::map<std::string, SDL_Texture *> *fontsTex);

typedef size_t stateIndex;

struct stateBack {
	std::string name;
	stateIndex index;
};
typedef struct stateBack stateBack;

typedef stateBack (*stateHandler)(void);

typedef stateIndex (*getEvent)(SDL_Event &event);

typedef std::vector<std::vector<stateHandler>> stateHandleTable;

class stateScene {
  private:
	std::map<std::string, SDL_Texture *> FontsTex;
	SDL_Renderer *rend;
	drawScript script;
	std::map<std::string, drawScript> scriptSet;
	getEvent eventGeter;
	stateHandleTable stateTable;
	stateIndex currentStateIndex;

  public:
	stateScene(SDL_Renderer *rend, getEvent eventGeter);
	// 使用回调函数的方法绘制界面
	void appendDrawScript(drawScript callbackDraw, std::string callName);
	void appendFontTex(SDL_Texture *fontTex, std::string fontName);
	stateIndex event(SDL_Event &event);
	void show();
	void setTable(stateHandleTable stateTable);
};

class stateTree {
  private:
	std::map<std::string, stateScene *> tree;
	SDL_Renderer *rend;
	SDL_Event *eve;
	stateHandleTable stateTable;
	std::string currentState;

  public:
	stateTree(SDL_Renderer *rend, SDL_Event *eve, stateScene *firstState,
	          std::string stateName);
	void appendState(stateScene *state, std::string stateName);
	void setTable(stateHandleTable stateTable);
	void event();
	void run();
};

}; // namespace gui

#endif
