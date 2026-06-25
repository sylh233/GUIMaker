#ifndef GUI_FSM_H
#define GUI_FSM_H

#include <GUI_main.h>

namespace GUI {

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

class stateMachine {
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
	stateMachine(stateScript mainScript, getEvent eventGeter, void *userdata,
	             void *outdata, stateIndex mainStateIndex);
	void setStateScripts(scriptSet script_set);
	void event();
	void run();
	void setTable(stateHandleTable stateTable);
	void setDestructor(dst_script destructor);
	~stateMachine();
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

} // namespace GUI

#endif
