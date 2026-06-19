#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <gui.h>
#include <instance.h>
#include <main.h>
#include <string>

// 常量
const int Ww = 1600;
const int Wh = 900;
const double TEXTwhRATIO = 2.7;
// 全局变量
SDL_Window *win;
SDL_Renderer *rend;
SDL_Event eve;
std::string Path;
std::string ttfPath;
std::string mainFontName;
std::string bigFontName;

// 场景类
void state1(double mouse_x, double mouse_y);

//

//

int main(int argc, char *argv[]) {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		SDL_Log("%s", SDL_GetError());
		return 1;
	}
	if (!TTF_Init()) {
		SDL_Log("%s", SDL_GetError());
		return 1;
	}

	if (init()) {
		return 1;
	}

	//
	SDL_Log("The window COMPLETE!\n");
	event(eve);
	//
	instance::destroyInstance();
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);

	TTF_Quit();
	SDL_Quit();
	return 0;
}

void event(SDL_Event &event) {
	bool quitFlag = 1;
	while (quitFlag) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT:
				quitFlag = 0;
				break;
			case SDL_EVENT_MOUSE_MOTION:
				instance::mainTree->event(event);
				break;
			case SDL_EVENT_KEY_DOWN:
				switch (event.key.key) {
				case SDLK_ESCAPE:
					quitFlag = 0;
					break;
				default:
					instance::mainTree->event(event);
					break;
				}
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				switch (event.button.button) {
				default:
					instance::mainTree->event(event);
					break;
				}
				break;
			case SDL_EVENT_MOUSE_BUTTON_UP:
				switch (event.button.button) {
				default:
					instance::mainTree->event(event);
					break;
				}
				break;
			default:
				break;
			}
		}
		SDL_SetRenderDrawColor(rend, 0x00, 0xff, 0x00, 0xff);
		SDL_RenderClear(rend);
		//
		// state1(mx, my);
		instance::mainTree->run();
		//
		SDL_RenderPresent(rend);

		SDL_Delay(1000 / 24.0);
	}
}

SDL_FRect genRect(gui::Point p, double w, size_t l) {
	SDL_FRect r;
	r.x = p.x;
	r.y = p.y;
	r.w = w;
	r.h = w / l * TEXTwhRATIO;
	return r;
}

int init() {
	std::string WName = "GUI";
	win = SDL_CreateWindow(WName.c_str(), Ww, Wh, 0);
	if (!win) {
		SDL_Log("%s", SDL_GetError());
		return 1;
	}
	rend = SDL_CreateRenderer(win, NULL);
	if (!rend) {
		SDL_Log("%s", SDL_GetError());
		return 1;
	}
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

	Path = SDL_GetBasePath();
	Path = Path.substr(0, Path.size() - 4);
	ttfPath = Path + "ttf\\";

	mainFontName = "msyh.ttc";
	bigFontName = "msyhbd.ttc";

	//
	instance::initInstance();
	//

	return 0;
}

SDL_Texture *getFontTex(std::string fn, SDL_Color c, std::string t, float s) {
	TTF_Font *font = TTF_OpenFont((ttfPath + fn).c_str(), s);
	if (!font) {
		SDL_Log("%s", SDL_GetError());
		return nullptr;
	}

	SDL_Surface *text_surf =
	    TTF_RenderText_Blended(font, t.c_str(), t.size(), c);
	if (!text_surf) {
		SDL_Log("%s", SDL_GetError());
		return nullptr;
	}
	TTF_CloseFont(font);
	SDL_Texture *r;
	r = SDL_CreateTextureFromSurface(rend, text_surf);
	if (!r) {
		SDL_Log("%s", SDL_GetError());
		return nullptr;
	}
	SDL_DestroySurface(text_surf);
	return r;
}

void state1(double x, double y) {
	std::string text = "Hello 世界";
	gui::Point p{x, y};
	SDL_FRect r1 = genRect(p, 200, text.size());
	SDL_Color c = {0xff, 0x00, 0x00, 0xff};
	SDL_Texture *t1 = getFontTex(mainFontName, c, text, 28);
	SDL_RenderTexture(rend, t1, 0, &r1);
	SDL_DestroyTexture(t1);
}

SDL_FRect genRectCenter(gui::Point p, double w, size_t l) {
	double x = p.x, y = p.y, h = w / l * TEXTwhRATIO;
	x -= w / 2;
	y -= h / 2;
	SDL_FRect r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;
	return r;
}

bool InRect(SDL_FRect *r, gui::Point p) {
	if (p.x > r->x && p.x < r->x + r->w && p.y > r->y && p.y < r->y + r->h) {
		return true;
	} else {
		return false;
	}
}
