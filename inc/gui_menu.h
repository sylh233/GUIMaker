#ifndef MENU
#define MENU

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <gui.h>

namespace gui {

using buttonScript = void (*)(void *outdata);
using buttonTheme = struct {
	SDL_Color UnonColor, OnColor, DownColor, UnonTextColor, OnTextColor,
	    DownTextColor;
};

class Button {
  private:
	stateScene *buttonScene;
	buttonTheme theme;

  public:
	Button(SDL_FRect *position, const std::string text);
	void setTheme(SDL_Color UnonColor, SDL_Color OnColor, SDL_Color DownColor,
	              SDL_Color UnonTextColor, SDL_Color OnTextColor,
	              SDL_Color DownTextColor);
	void setScript(void *outdata, buttonScript script);
	~Button();
};
}; // namespace gui

#endif
