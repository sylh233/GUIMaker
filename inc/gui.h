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

SDL_FRect genRect(gui::Point p, double width, size_t length,
                  double ratio = TextRatio);

SDL_FRect genRectCenter(gui::Point p, double width, size_t length,
                        double ratio = TextRatio);

SDL_Texture *getFontTex(SDL_Renderer *renderer, std::string fontName,
                        SDL_Color color, std::string text, float size = 20);

bool InRect(SDL_FRect *rect, gui::Point point);

SDL_Texture *getBMP(SDL_Renderer *renderer, std::string bmpName);

bool loadWAV(std::string wav_name, SDL_AudioStream *&stream, Uint8 **wav_data,
             Uint32 *wav_len);

bool addDevice(SDL_AudioDeviceID device_id, SDL_AudioDeviceID id);

bool putStream(SDL_AudioDeviceID device_id, SDL_AudioStream *stream,
               Uint8 **wav_data, Uint32 *wav_len);

typedef std::map<std::string, SDL_Texture *> texMap;

// 用于绘制的回调函数
// typedef void (*drawScript)(SDL_Renderer *rend, texMap *fontsTex);
using drawScript = void (*)(SDL_Renderer *, texMap *, void *);

using stateScript = void (*)(void *userdata);

typedef uint32_t stateIndex;
typedef uint32_t eventIndex;
typedef uint32_t stateSceneIndex;
typedef uint64_t doubleIndex;

typedef doubleIndex (*stateHandler)(SDL_AudioDeviceID device_id,
                                    void *userdata);
typedef std::vector<std::vector<stateHandler>> stateHandleTable;

typedef eventIndex (*getEvent)(SDL_Event &event, void *userdata);

typedef void (*stateSceneHandler)(void);
typedef std::vector<std::vector<stateSceneHandler>> stateSceneHandleTable;

using resetFunc = void (*)(void *);

class stateScene {
  private:
	texMap Texes;
	drawScript draw_script = nullptr;
	std::vector<stateScript> scriptSet;
	getEvent eventGeter;
	stateHandleTable stateTable;
	stateIndex currentStateIndex; // 自身中的状态索引
	stateIndex mainStateIndex;    // 主状态，决定如何返回
	stateSceneIndex selfIndex;    // 自身对于状态树的索引
	void *userdata = nullptr; // 放自定义数据，SDL给我的启发，其实使用回调也是
	SDL_AudioDeviceID device_id{}; // 用于音频播放
	resetFunc reset;
	// 废弃设计
	// SDL_Renderer *rend;
	// std::vector<drawScript> scriptSet;
	// std::vector<stateIndex> outStates;
	// SDL_AudioStream *stream_ptr;
	// drawScript mainDraw = nullptr;

  public:
	stateScene(drawScript mainDrawScript, getEvent eventGeter, void *userdata,
	           resetFunc reset, stateSceneIndex selfIndex,
	           stateScript mainStateScript, stateIndex mainStateIndex);
	void appendStateScript(stateScript callback, stateIndex index);
	void appendTex(SDL_Texture *tex, std::string texName);
	stateSceneIndex event(SDL_Event &event);
	void show(SDL_Renderer *renderer);
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
	void run(SDL_Renderer *renderer);
};

}; // namespace gui

#endif
