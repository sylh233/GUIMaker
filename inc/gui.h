#ifndef GUI
#define GUI

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <map>
#include <string>
#include <vector>

namespace gui {

struct Point {
	double x, y;
};
typedef struct Point Point;

extern double WinWeight, WinHeight, TextRatio, TextRatioC; // 汉字和英文不同
extern std::string path, ttf_path, wav_path;
extern SDL_Renderer *mainRenderer;

void initVariable(double WinWeight, double WinHeight, double TextRatio,
                  double TextRatioC);

void initPath(std::string Path);

void initMainRenderer(SDL_Renderer *rend);

void destroyMainRenderer();

enum TEXT_TYPE {
	TEXT_TYPE_ONLY_LATIN,
	TEXT_TYPE_HAVE_CHINESE,
};
SDL_FRect genRect(gui::Point p, double width, size_t length,
                  TEXT_TYPE type = TEXT_TYPE_ONLY_LATIN);

SDL_FRect genRectCenter(gui::Point p, double width, size_t length,
                        TEXT_TYPE type = TEXT_TYPE_ONLY_LATIN);

SDL_Texture *getFontTex(std::string fontName, SDL_Color color, std::string text,
                        float size = 20);

bool InRect(SDL_FRect *rect, gui::Point point);

typedef std::map<std::string, SDL_Texture *> fontMap;

// 用于绘制的回调函数
// typedef void (*drawScript)(SDL_Renderer *rend, fontMap *fontsTex);
using drawScript = void (*)(SDL_Renderer *, fontMap *, void *);

typedef size_t stateIndex;
typedef size_t eventIndex;
typedef size_t stateSceneIndex;

typedef stateIndex (*stateHandler)(SDL_AudioDeviceID device_id);
typedef std::vector<std::vector<stateHandler>> stateHandleTable;

typedef eventIndex (*getEvent)(SDL_Event &event, void *userdata);

typedef void (*stateSceneHandler)(void);
typedef std::vector<std::vector<stateSceneHandler>> stateSceneHandleTable;

class stateScene {
  private:
	fontMap FontsTex;
	SDL_Renderer *rend;
	drawScript script = nullptr;
	std::vector<drawScript> scriptSet;
	std::vector<stateIndex> outStates;
	getEvent eventGeter;
	stateHandleTable stateTable;
	stateIndex currentStateIndex; // 自身中的状态索引
	stateIndex mainStateIndex;    // 主状态，决定如何返回
	stateSceneIndex selfIndex;    // 自身对于状态树的索引
	void *userdata = nullptr; // 放自定义数据，SDL给我的启发，其实使用回调也是
	// 用于音频播放
	SDL_AudioDeviceID device_id{};
	SDL_AudioStream *stream = nullptr;
	Uint8 *wav_data = nullptr;
	Uint32 wav_len = 0;

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
	void addUserData(void *userdata);
	void initAudio(std::string wav_name, SDL_AudioDeviceID device_id);
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
