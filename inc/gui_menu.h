#ifndef GUI_MENU_H
#define GUI_MENU_H

#include <GUI_FSM.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace GUI {

enum buttonMode {
	GUI_BUTTON_MODE_ACT_WHEN_UP,    // 抬起时响应
	GUI_BUTTON_MODE_ACT_WHEN_DOWN,  // 按下时响应
	GUI_BUTTON_MODE_CONTINUE_PRESS, // 长按一直执行
	GUI_BUTTON_MODE_LONG_PRESS,     // 长按才执行
	GUI_BUTTON_MODE_SWITCH,         // 开关
};

using buttonScript = std::function<void(void *outdata)>;
struct buttonData {
	Point mouseP;
	texMap fonts;
	std::map<std::string, SDL_Color> colors;
	SDL_FRect buttonPos;
	std::string text;
	float textSize;
	std::string currentName;
	buttonMode mode;
};
struct buttonTheme {
	SDL_Color UnonColor = {0xff, 0xff, 0xff, 0xff},
	          OnColor = {0xff, 0, 0, 0xff}, DownColor = {0xa5, 0, 0, 0xff},
	          UnonTextColor = {0, 0, 0, 0xff},
	          OnTextColor = {0xff, 0xff, 0xff, 0xff},
	          DownTextColor = {0xff, 0xff, 0xff, 0xff};
	std::string UnonFont = "msyh.ttc", OnFont = "msyh.ttc",
	            DownFont = "msyh.ttc";
};
typedef struct buttonTheme buttonTheme;

class Button {
  private:
	stateMachine *buttonScene;
	buttonData userdata;
	buttonTheme theme;
	void *odp = nullptr;        // outDataPointer
	buttonScript osp = nullptr; // outScriptPointer

  public:
	Button(SDL_FRect *position, const std::string text,
	       buttonMode mode = GUI_BUTTON_MODE_ACT_WHEN_UP, float textSize = 40);
	void setTheme(buttonTheme *theme);
	void setScript(void *outdata, buttonScript script);
	buttonTheme getTheme() const;
	void createButton();
	void event();
	void run();
	~Button();
};
}; // namespace GUI

#endif
