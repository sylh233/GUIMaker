#include <gui.h>

namespace gui {

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

SDL_FRect genRect(gui::Point p, double w, size_t l, double ratio) {
	SDL_FRect r;
	l = l == 0 ? 1 : l;
	r.x = p.x;
	r.y = p.y;
	r.w = w;
	r.h = w / l * ratio;
	return r;
}

SDL_FRect genRectCenter(gui::Point p, double w, size_t l, double ratio) {
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

bool InRect(SDL_FRect *r, gui::Point p) {
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

bool addDevice(SDL_AudioDeviceID device_id, SDL_AudioDeviceID id) {
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

stateScene::stateScene(stateScript mss, getEvent geter, void *ud, void *od,
                       stateIndex mi) { // 一定不要忘记给script初始化为零
	eventGeter = geter;
	mainStateIndex = mi;
	currentStateIndex = mi;
	main_script = mss;
	userdata = ud;
	outdata = od;
}

void stateScene::setStateScripts(scriptSet sS) { script_set = std::move(sS); }

void stateScene::setTable(stateHandleTable table) {
	stateTable = std::move(table);
}

void stateScene::event() {
	if (currentStateIndex >= stateTable.size() ||
	    eventGeter(userdata, outdata) >= stateTable[currentStateIndex].size() ||
	    stateTable[currentStateIndex][eventGeter(userdata, outdata)] ==
	        nullptr) {
		return;
	}
	stateIndex nextState =
	    stateTable[currentStateIndex][eventGeter(userdata, outdata)](userdata,
	                                                                 outdata);
	if (nextState >= script_set.size()) {
		currentStateIndex = mainStateIndex;
	} else {
		currentStateIndex = nextState;
	}
	return;
}

void stateScene::run() {
	script_set[currentStateIndex](userdata, outdata);
	if (main_script != nullptr) {
		main_script(userdata, outdata);
	}
}
void stateScene::setDestructor(dst_script ds) { destructor = ds; }

stateScene::~stateScene() { destructor(userdata); }

// 缅怀祖宗函数（
/*void stateScene::initAudio(std::string wn, SDL_AudioDeviceID id) {
device_id = id;
if (!device_id) { // 设置播放设备
    SDL_Log("%s", SDL_GetError());
}
SDL_AudioSpec spec;
if (!SDL_LoadWAV((wav_path + wn).c_str(), &spec, &wav_data,
                 &wav_len)) { // 加载wav
    SDL_Log("%s", SDL_GetError());
}
stream = SDL_CreateAudioStream(&spec, NULL);
if (!stream) {
    SDL_Log("%s", SDL_GetError());
}
// 先绑定再推流，错好几次
if (!SDL_BindAudioStream(device_id, stream)) { // 绑定设备
    SDL_Log("%s", SDL_GetError());
}
if (!SDL_PutAudioStreamData(stream, wav_data, wav_len)) { // 推流
    SDL_Log("%s", SDL_GetError());
}
SDL_PauseAudioDevice(device_id);
}*/

// 已弃用
/*
stateTree::stateTree(stateScene *s, stateSceneIndex i) {
tree.resize(1);
appendState(s, i);
currentState = i;
}

void stateTree::appendState(stateScene *s, stateSceneIndex i) {
while (i >= tree.size()) {
    tree.resize(tree.size() + 1);
}
tree[i] = s;
}

void stateTree::setTable(stateSceneHandleTable table) {
stateSceneTable = std::move(table);
}

void stateTree::event(SDL_Event &eve) {
if (currentState >= tree.size()) {
    return;
}
stateSceneIndex nextIndex = tree[currentState]->event(eve);
if (nextIndex >= tree.size()) {
    return;
}
if (currentState < stateSceneTable.size() &&
    nextIndex < stateSceneTable[currentState].size()) {
    stateSceneTable[currentState][nextIndex]();
}
currentState = nextIndex;
}

void stateTree::run(SDL_Renderer *rend) { tree[currentState]->show(rend); }
*/

}; // namespace gui
