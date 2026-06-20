#include <gui.h>

namespace gui {

double WinWeight, WinHeight, TextRatio, TextRatioC;
std::string path, ttf_path, wav_path;
SDL_Renderer *mainRenderer;

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

void initMainRenderer(SDL_Renderer *rend) { mainRenderer = rend; }

void destroyMainRenderer() { SDL_DestroyRenderer(mainRenderer); }

SDL_FRect genRect(gui::Point p, double w, size_t l, double ratio) {
	SDL_FRect r;
	l = l == 0 ? 1 : l;
	r.x = p.x;
	r.y = p.y;
	r.w = w;
	r.h = w / l * ratio;
	return r;
}

SDL_Texture *getFontTex(std::string fn, SDL_Color c, std::string t, float s) {
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
	r = SDL_CreateTextureFromSurface(mainRenderer, text_surf);
	if (!r) {
		SDL_Log("%s", SDL_GetError());
		return nullptr;
	}
	SDL_DestroySurface(text_surf);
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

SDL_Texture *getBMP(std::string bmpN) {
	SDL_Surface *ts;
	ts = SDL_LoadBMP((path + bmpN).c_str());
	if (!ts) {
		SDL_Log("%s", SDL_GetError());
	}
	SDL_Texture *r = SDL_CreateTextureFromSurface(mainRenderer, ts);
	if (!r) {
		SDL_Log("%s", SDL_GetError());
	}
	SDL_DestroySurface(ts);
	return r;
}

void loadWAV(std::string wn, SDL_AudioStream *&stream, Uint8 **wav_data,
             Uint32 *wav_len) {
	SDL_AudioSpec spec;
	if (!SDL_LoadWAV((wav_path + wn).c_str(), &spec, wav_data,
	                 wav_len)) { // 加载wav
		SDL_Log("%s", SDL_GetError());
		return;
	}
	// 注意stream是指针引用，因为我们要给他赋值
	stream = SDL_CreateAudioStream(&spec, NULL);
	if (!stream) {
		SDL_Log("%s", SDL_GetError());
		return;
	}
}

stateScene::stateScene(SDL_Renderer *r, getEvent geter, stateSceneIndex self,
                       drawScript mds, stateIndex mi)
    : script(nullptr) { // 一定不要忘记给script初始化为零，要不然随机报错
	scriptSet.resize(1);
	eventGeter = geter;
	rend = r;
	selfIndex = self;
	mainStateIndex = mi;
	currentStateIndex = mi;
	appendDrawScript(mds, mi);
	script = scriptSet[mi];
}

void stateScene::appendDrawScript(drawScript call, stateIndex i) {
	while (i >= scriptSet.size()) {
		scriptSet.resize(scriptSet.size() + 1);
	}
	// if (script == nullptr) {
	// 	currentStateIndex = i;
	// 	script = call;
	// 	mainStateIndex = i;
	// }
	scriptSet[i] = call;
}

void stateScene::appendTex(SDL_Texture *tex, std::string name) {
	Texes[name] = tex;
}

void stateScene::setTable(stateHandleTable table) {
	stateTable = std::move(table);
}

stateSceneIndex stateScene::event(SDL_Event &e) {
	if (currentStateIndex >= stateTable.size() ||
	    eventGeter(e, userdata) >= stateTable[currentStateIndex].size() ||
	    stateTable[currentStateIndex][eventGeter(e, userdata)] == NULL) {
		return selfIndex;
	}
	stateIndex nextState =
	    stateTable[currentStateIndex][eventGeter(e, userdata)](device_id,
	                                                           userdata);
	if (nextState >= scriptSet.size()) {
		currentStateIndex = mainStateIndex;
		script = scriptSet[currentStateIndex];
		reset(userdata);
		return nextState - scriptSet.size();
	} else {
		currentStateIndex = nextState;
		script = scriptSet[currentStateIndex];
		return selfIndex;
	}
}

void stateScene::show() {
	if (script != nullptr) {
		script(rend, &Texes, userdata);
	}
	if (mainDraw != nullptr) {
		mainDraw(rend, &Texes, userdata);
	}
}

stateScene::~stateScene() {
	for (auto p = Texes.begin(); p != Texes.end(); p++) {
		SDL_DestroyTexture(p->second);
	}
}

void stateScene::addUserData(void *ud) { userdata = ud; }

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

void stateScene::addDevice(SDL_AudioDeviceID id) {
	if (!id) {
		SDL_Log("%s", SDL_GetError());
		return;
	}
	device_id = id;
}

void stateScene::putStream(SDL_AudioStream *stream, Uint8 **wav_data,
                           Uint32 *wav_len) {
	stream_ptr = stream; // 保存该stream，用于调节音量
	// 先绑定再推流，错好几次
	if (!SDL_BindAudioStream(device_id, stream)) { // 绑定设备
		SDL_Log("%s", SDL_GetError());
		return;
	}
	if (!SDL_PutAudioStreamData(stream, *wav_data, *wav_len)) { // 推流
		SDL_Log("%s", SDL_GetError());
		return;
	}
	SDL_PauseAudioDevice(device_id);
}

void stateScene::setReset(resetFunc rs) { reset = rs; }

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

void stateTree::run() { tree[currentState]->show(); }

}; // namespace gui
