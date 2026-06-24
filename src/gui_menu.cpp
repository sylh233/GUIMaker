#include <gui_menu.h>

namespace gui {
Button::Button(SDL_FRect *pos, const std::string t, buttonMode mode,
               float size) {
	userdata.buttonPos = *(pos);
	userdata.text = t;
	userdata.textSize = size;
}

void Button::setTheme(buttonTheme *theme) { this->theme = *theme; }

void Button::setScript(void *od, buttonScript s) {
	odp = od;
	osp = s;
}

buttonTheme Button::getTheme() const { return theme; }

void Button::createButton() {
	std::string t = userdata.text;
	float size = userdata.textSize;

	userdata.currentName = "Unon";
	SDL_Texture *unonT =
	    getFontTex(gui_renderer, theme.UnonFont, theme.UnonTextColor, t, size);
	appendTex(&(userdata.fonts), unonT, "Unon");
	SDL_Texture *onT =
	    getFontTex(gui_renderer, theme.OnFont, theme.OnTextColor, t, size);
	appendTex(&(userdata.fonts), onT, "On");
	SDL_Texture *downT =
	    getFontTex(gui_renderer, theme.DownFont, theme.DownTextColor, t, size);
	appendTex(&(userdata.fonts), downT, "Down");
	userdata.colors["Unon"] = theme.UnonColor;
	userdata.colors["On"] = theme.OnColor;
	userdata.colors["Down"] = theme.DownColor;
	userdata.mouseP = {-1, -1};
	enum class EventIndex {
		Unon,
		On,
		Down,
		Up,
	};
	getEvent eve = [](void *ud, void *od) -> eventIndex {
		buttonData *udp = (buttonData *)ud;
		switch (gui_event->type) {
		case SDL_EVENT_MOUSE_MOTION:
			udp->mouseP.x = gui_event->motion.x;
			udp->mouseP.y = gui_event->motion.y;
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if (gui_event->button.button == SDL_BUTTON_LEFT) {
				return (eventIndex)EventIndex::Down;
			}
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			if (gui_event->button.button == SDL_BUTTON_LEFT) {
				return (eventIndex)EventIndex::Up;
			}
			break;
		default:
			break;
		}
		if (InRect(&(udp->buttonPos), udp->mouseP)) {
			return (eventIndex)EventIndex::On;
		} else {
			return (eventIndex)EventIndex::Unon;
		}
	};
	stateScript drawSc = [](void *ud, void *od) {
		buttonData *udp = (buttonData *)ud;
		SDL_Color tC = udp->colors[udp->currentName];
		// SDL_Log("help");
		SDL_SetRenderDrawColor(gui_renderer, tC.r, tC.g, tC.b, tC.a);
		SDL_RenderFillRect(gui_renderer, &(udp->buttonPos));
		SDL_RenderTexture(gui_renderer, udp->fonts[udp->currentName], NULL,
		                  &(udp->buttonPos));
	};

	enum class StateIndex {
		Unon,
		On,
		Down,
	};

	stateScript stateUnon = [](void *ud, void *od) {
		buttonData *udp = (buttonData *)ud;
		udp->currentName = "Unon";
	};

	stateScript stateOn = [](void *ud, void *od) {
		buttonData *udp = (buttonData *)ud;
		udp->currentName = "On";
	};

	stateScript stateDown = [](void *ud, void *od) {
		buttonData *udp = (buttonData *)ud;
		udp->currentName = "Down";
	};

	// stateScript stateUp = [this](void *ud, void *od) {
	// 	buttonData *udp = (buttonData *)ud;
	// 	this->osp(od);
	// };

	scriptSet sS = {stateUnon, stateOn, stateDown};

	// stateHandler 明天在写（来咯2026.6.24

	stateHandler idleToUnon = [](void *ud, void *od) -> stateIndex {
		return (stateIndex)StateIndex::Unon;
	};

	stateHandler UnonToOn = [](void *ud, void *od) -> stateIndex {
		return (stateIndex)StateIndex::On;
	};

	stateHandler OnToDown = [](void *ud, void *od) -> stateIndex {
		return (stateIndex)StateIndex::Down;
	};

	auto tUp = [](void *ud, void *od, buttonScript bs) -> stateIndex {
		if (od != nullptr && bs != nullptr) {
			bs(od);
		}
		return (stateIndex)StateIndex::Unon;
	};

	stateHandler buttonUp =
	    std::bind(tUp, std::placeholders::_1, std::placeholders::_2, osp);

	stateHandleTable sT = {{nullptr, UnonToOn, nullptr, nullptr},
	                       {idleToUnon, nullptr, OnToDown, nullptr},
	                       {idleToUnon, nullptr, nullptr, buttonUp}};

	buttonScene = new stateScene(drawSc, eve, (void *)&userdata, odp, 0);
	buttonScene->setStateScripts(sS);
	buttonScene->setTable(sT);
}

void Button::event() { buttonScene->event(); }

void Button::run() { buttonScene->run(); }

Button::~Button() {
	destroyTexMap(&(userdata.fonts));
	delete buttonScene;
}

}; // namespace gui
