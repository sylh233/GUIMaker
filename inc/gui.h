#ifndef GUI
#define GUI

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace gui {

// 内禀变量，用于注入用户设置
extern double WinWeight, WinHeight, TextRatio, TextRatioC; // 汉字和英文不同
extern std::string path, ttf_path, wav_path;
extern SDL_Renderer *gui_renderer;
extern SDL_Event *gui_event;
// 用于初始化内禀变量
void initVariable(double WinWeight, double WinHeight, double TextRatio,
                  double TextRatioC);
void initPath(std::string Path);
void initSDL(SDL_Renderer *renderer, SDL_Event *event);
void destroySDL();

struct Point { // 用于描述一个点
	double x, y;
};
typedef struct Point Point;

// 生成一个以p为左上角，宽为width的矩形，length为字长，ratio表示字体宽高比，width/lentgh*ratio就是高度
SDL_FRect genRect(gui::Point p, double width, size_t length,
                  double ratio = TextRatio);
// 与genRect类似，但这里p为矩形中心
SDL_FRect genRectCenter(gui::Point p, double width, size_t length,
                        double ratio = TextRatio);
// 获取字体材质
SDL_Texture *getFontTex(SDL_Renderer *renderer, std::string fontName,
                        SDL_Color color, std::string text, float size = 20);
// 检测点是否在矩形里（边界不算）
bool InRect(SDL_FRect *rect, gui::Point point);
// 获取bmp图片材质
SDL_Texture *getBMP(SDL_Renderer *renderer, std::string bmpName);
// 加载wav音频
bool loadWAV(std::string wav_name, SDL_AudioStream *&stream, Uint8 **wav_data,
             Uint32 *wav_len);
// 设置播放设备，将device_id赋值为id
bool addDevice(SDL_AudioDeviceID &device_id,
               SDL_AudioDeviceID id = SDL_OpenAudioDevice(
                   SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL));
// 绑定设备，将stream绑定到device_id
bool putStream(SDL_AudioDeviceID device_id, SDL_AudioStream *stream,
               Uint8 **wav_data, Uint32 *wav_len);

typedef std::map<std::string, SDL_Texture *> texMap;

void appendTex(texMap *tm, SDL_Texture *tex, std::string name);

void destroyTexMap(texMap *tm);

// 用于绘制的回调函数(现统一用一种脚本函数（现在改用function容器了
// typedef void (*drawScript)(SDL_Renderer *rend, texMap *fontsTex);
// using drawScript = void (*)(SDL_Renderer *, texMap *, void *);

// userdata 和 outdata 本质相同，只是显式的区分状态机内部信息和外部信息
// using stateScript = void (*)(void *userdata, void *outdata);
using stateScript = std::function<void(void *userdata, void *outdata)>;
using scriptSet = std::vector<stateScript>;

typedef uint32_t stateIndex;
typedef uint32_t eventIndex;

// typedef stateIndex (*stateHandler)(void *userdata, void *outdata);
using stateHandler = std::function<stateIndex(void *userdata, void *outdata)>;
typedef std::vector<std::vector<stateHandler>> stateHandleTable;

// typedef eventIndex (*getEvent)(void *userdata, void *outdata);
using getEvent = std::function<eventIndex(void *userdata, void *outdata)>;

using dst_script = void (*)(void *userdata);

class stateScene {
  private:
	stateScript main_script = nullptr; // 主脚本，每次都执行
	scriptSet script_set;              // 状态脚本库
	getEvent eventGeter;               // 事件获取回调函数
	stateHandleTable stateTable;       // 状态转移表
	stateIndex currentStateIndex;      // 自身中当前的状态索引
	stateIndex mainStateIndex;         // 主状态，保护用
	void *userdata = nullptr; // 放自定义数据，SDL给我的启发，其实使用回调也是
	void *outdata = nullptr;  // 外部数据接口，可用于在多个状态机之间传递信息
	dst_script destructor = nullptr; // 自定义析构函数，在对象析构函数中使用

	// 废弃设计
	// SDL_Renderer *rend;
	// std::vector<drawScript> scriptSet;
	// std::vector<stateIndex> outStates;
	// stateSceneIndex selfIndex;    // 自身对于状态树的索引
	// SDL_AudioStream *stream_ptr;
	// drawScript mainDraw = nullptr;
	// SDL_AudioDeviceID device_id{}; // 用于音频播放
	// resetFunc reset;

  public:
	stateScene(stateScript mainScript, getEvent eventGeter, void *userdata,
	           void *outdata, stateIndex mainStateIndex);
	void setStateScripts(scriptSet script_set);
	void event();
	void run();
	void setTable(stateHandleTable stateTable);
	void setDestructor(dst_script destructor);
	~stateScene();
};

// 现在用状态机自身嵌套，不再使用专门的树
// class stateTree {
//   private:
// 	std::vector<stateScene *> tree;
// 	stateSceneHandleTable stateSceneTable;
// 	stateSceneIndex currentState;

//   public:
// 	stateTree(stateScene *firstState, stateSceneIndex firstIndex);
// 	void appendState(stateScene *state, stateSceneIndex index);
// 	void setTable(stateSceneHandleTable stateTable);
// 	void event(SDL_Event &event);
// 	void run(SDL_Renderer *renderer);
// };

}; // namespace gui

#endif
