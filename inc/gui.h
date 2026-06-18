#ifndef GUI
#define GUI

#include <SDL3/SDL.h>
#include <map>
#include <string>
#include <vector>

namespace gui {
struct Point {
	double x, y;
};
typedef struct Point Point;

typedef std::map<std::string, SDL_Texture *> fontMap;

// 用于绘制的回调函数
typedef void (*drawScript)(SDL_Renderer *rend, fontMap *fontsTex);

typedef size_t stateIndex;
typedef size_t eventIndex;
typedef size_t stateSceneIndex;

typedef stateIndex (*stateHandler)(void);
typedef std::vector<std::vector<stateHandler>> stateHandleTable;

typedef eventIndex (*getEvent)(SDL_Event &event);

typedef void (*stateSceneHandler)(void);
typedef std::vector<std::vector<stateSceneHandler>> stateSceneHandleTable;

class stateScene {
  private:
	fontMap FontsTex;
	SDL_Renderer *rend;
	drawScript script;
	std::vector<drawScript> scriptSet;
	std::vector<stateIndex> outStates;
	getEvent eventGeter;
	stateHandleTable stateTable;
	stateIndex currentStateIndex; // 自身中的状态索引
	stateSceneIndex selfIndex;    // 自身对于状态树的索引

  public:
	stateScene(SDL_Renderer *rend, getEvent eventGeter,
	           stateSceneIndex selfIndex);
	void appendDrawScript(drawScript callbackDraw,
	                      stateIndex index); // 使用回调函数的方法绘制界面
	void appendFontTex(SDL_Texture *fontTex, std::string fontName);
	stateSceneIndex event(SDL_Event &event);
	void show();
	void setTable(stateHandleTable stateTable);
	~stateScene();
};

class stateTree {
  private:
	std::vector<stateScene *> tree;
	stateSceneHandleTable stateSceneTable;
	stateSceneIndex currentState;

  public:
	stateTree(stateScene *firstState, stateSceneIndex firstIndex);
	void appendState(stateScene *state, stateSceneIndex index);
	void setTable(stateSceneHandleTable stateTable);
	void event(SDL_Event &event);
	void run();
};

}; // namespace gui

#endif
