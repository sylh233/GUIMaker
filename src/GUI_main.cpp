#include <GUI_main.h>

namespace GUI {

double WinWeight, WinHeight, TextRatio, TextRatioC;
std::string path, ttf_path, wav_path;
SDL_Renderer *gui_renderer;
SDL_Event *gui_event;

void initVariable(double w, double h, double tr, double trc) {
	WinWeight = w;
	WinHeight = h;
	TextRatio = tr;
	TextRatioC = trc;
}

void initPath(std::string p) {
	path = p;
	ttf_path = p + "ttf\\";
	wav_path = p + "wav\\";
}

void initSDL(SDL_Renderer *r, SDL_Event *e) {
	gui_renderer = r;
	gui_event = e;
}
// 一般在main中都会释放，不需要手动执行这个函数，不然会崩溃
void destroySDL() { SDL_DestroyRenderer(gui_renderer); }

SDL_Texture *getFontTex(SDL_Renderer *rend, std::string fn, SDL_Color c,
                        std::string t, float s) {
	TTF_Font *font = TTF_OpenFont((ttf_path + fn).c_str(), s);
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

SDL_FRect genRect(GUI::Point p, double w, size_t l, double ratio) {
	SDL_FRect r;
	l = l == 0 ? 1 : l;
	r.x = p.x;
	r.y = p.y;
	r.w = w;
	r.h = w / l * ratio;
	return r;
}

SDL_FRect genRectCenter(GUI::Point p, double w, size_t l, double ratio) {
	double x = p.x, y = p.y, h;
	l = l == 0 ? 1 : l;
	h = w / l * ratio;
	x -= w / 2;
	y -= h / 2;
	SDL_FRect r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;
	return r;
}

bool InRect(SDL_FRect *r, GUI::Point p) {
	if (p.x > r->x && p.x < r->x + r->w && p.y > r->y && p.y < r->y + r->h) {
		return true;
	} else {
		return false;
	}
}

SDL_Texture *getBMP(SDL_Renderer *rend, std::string bmpN) {
	SDL_Surface *ts;
	ts = SDL_LoadBMP((path + bmpN).c_str());
	if (!ts) {
		SDL_Log("%s", SDL_GetError());
	}
	SDL_Texture *r = SDL_CreateTextureFromSurface(rend, ts);
	if (!r) {
		SDL_Log("%s", SDL_GetError());
	}
	SDL_DestroySurface(ts);
	return r;
}

bool loadWAV(std::string wn, SDL_AudioStream *&stream, Uint8 **wav_data,
             Uint32 *wav_len) {
	SDL_AudioSpec spec;
	if (!SDL_LoadWAV((wav_path + wn).c_str(), &spec, wav_data,
	                 wav_len)) { // 加载wav
		SDL_Log("%s", SDL_GetError());
		return false;
	}
	// 注意stream是指针引用，因为我们要给他赋值
	stream = SDL_CreateAudioStream(&spec, NULL);
	if (!stream) {
		SDL_Log("%s", SDL_GetError());
		return false;
	}
	return true;
}

bool addDevice(SDL_AudioDeviceID &device_id, SDL_AudioDeviceID id) {
	device_id = id;
	if (!device_id) {
		SDL_Log("%s", SDL_GetError());
		return false;
	}
	return true;
}

bool putStream(SDL_AudioDeviceID device_id, SDL_AudioStream *stream,
               Uint8 **wav_data, Uint32 *wav_len) {
	// 先绑定再推流，错好几次
	if (!SDL_BindAudioStream(device_id, stream)) { // 绑定设备
		SDL_Log("%s", SDL_GetError());
		return false;
	}
	if (!SDL_PutAudioStreamData(stream, *wav_data, *wav_len)) { // 推流
		SDL_Log("%s", SDL_GetError());
		return false;
	}
	SDL_PauseAudioDevice(device_id);
	return true;
}

void appendTex(texMap *tm, SDL_Texture *tex, std::string name) {
	(*tm)[name] = tex;
}

void destroyTexMap(texMap *tm) {
	for (auto p = tm->begin(); p != tm->end(); p++) {
		SDL_DestroyTexture(p->second);
	}
}

}; // namespace GUI
