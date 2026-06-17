#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

const int Ww = 1600;
const int Wh = 900;

void event(SDL_Event *event);

int main(int argc, char *argv[]) {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		SDL_Log("%s", SDL_GetError());
		return 1;
	}
	if (!TTF_Init()) {
		SDL_Log("%s", SDL_GetError());
		return 1;
	}

	std::string WName = "GUI";
	SDL_Window *win = SDL_CreateWindow(WName.c_str(), Ww, Wh, 0);
	if (!win) {
		SDL_Log("%s", SDL_GetError());
		return 1;
	}
	SDL_Renderer *rend = SDL_CreateRenderer(win, NULL);
	if (!rend) {
		SDL_Log("%s", SDL_GetError());
		return 1;
	}

	std::string Path = SDL_GetBasePath();
	Path = Path.substr(0, Path.size() - 4);

	std::string ttfPath = Path + "ttf\\";

	std::string fontName = "msyh.ttc";
	TTF_Font *font = TTF_OpenFont((ttfPath + fontName).c_str(), 14);
	if (!font) {
		SDL_Log("%s", SDL_GetError());
		return 1;
	}

	std::string text = "Hello 世界";

	SDL_Color color = {0, 0, 0, 255};
	SDL_Surface *text_surf =
	    TTF_RenderText_Blended(font, text.c_str(), text.size(), color);
	SDL_Texture *text_tex = SDL_CreateTextureFromSurface(rend, text_surf);
	SDL_DestroySurface(text_surf);

	SDL_FRect r1 = {100, 100, 600, 600};
	SDL_RenderTexture(rend, text_tex, 0, &r1);
	SDL_RenderPresent(rend);

	SDL_Event eve;
	event(&eve);

	SDL_DestroyTexture(text_tex);
	TTF_CloseFont(font);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);

	TTF_Quit();
	SDL_Quit();
	return 0;
}

void event(SDL_Event *event) {
	bool quitFlag = 1;

	while (quitFlag) {
		while (SDL_PollEvent(event)) {
			switch (event->type) {
			case SDL_EVENT_QUIT:
				quitFlag = 0;
				break;
			default:
				break;
			}
		}
	}

	SDL_Delay(1000 / 24.0);
}
