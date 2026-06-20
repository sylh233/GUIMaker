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

SDL_FRect genRect(gui::Point p, double w, size_t l, TEXT_TYPE type) {
	SDL_FRect r;
	r.x = p.x;
	r.y = p.y;
	r.w = w;
	switch (type) {
	case TEXT_TYPE_HAVE_CHINESE:
		r.h = w / l * TextRatioC;
		break;
	case TEXT_TYPE_ONLY_LATIN:
	default:
		r.h = w / l * TextRatio;
		break;
	}

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

SDL_FRect genRectCenter(gui::Point p, double w, size_t l, TEXT_TYPE type) {
	double x = p.x, y = p.y, h;
	switch (type) {
	case TEXT_TYPE_HAVE_CHINESE:
		h = w / l * TextRatioC;
		break;
	case TEXT_TYPE_ONLY_LATIN:
	default:
		h = w / l * TextRatio;
		break;
	}
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

stateScene::stateScene(SDL_Renderer *r, getEvent geter, stateSceneIndex self)
    : script(nullptr) { // 一定不要忘记给script初始化为零，要不然随机报错
	scriptSet.resize(1);
	eventGeter = geter;
	rend = r;
	selfIndex = self;
}

void stateScene::appendDrawScript(drawScript call, stateIndex i) {
	while (i >= scriptSet.size()) {
		scriptSet.resize(scriptSet.size() + 1);
	}
	if (script == nullptr) {
		currentStateIndex = i;
		script = call;
		mainStateIndex = i;
	}
	scriptSet[i] = call;
}

void stateScene::appendFontTex(SDL_Texture *font, std::string name) {
	FontsTex[name] = font;
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
	    stateTable[currentStateIndex][eventGeter(e, userdata)]();
	if (nextState >= scriptSet.size()) {
		currentStateIndex = mainStateIndex;
		script = scriptSet[currentStateIndex];
		return nextState - scriptSet.size();
	} else {
		currentStateIndex = nextState;
		script = scriptSet[currentStateIndex];
		return selfIndex;
	}
}

void stateScene::show() {
	if (script != NULL) {
		script(rend, &FontsTex, userdata);
	}
}

stateScene::~stateScene() {
	for (auto p = FontsTex.begin(); p != FontsTex.end(); p++) {
		SDL_DestroyTexture(p->second);
	}
}

void stateScene::addUserData(void *ud) { userdata = ud; }

void stateScene::initAudio(std::string wn, SDL_AudioDeviceID id) {
	device_id = id;
	if (!device_id) { // 设置播放设备
		SDL_Log("%s", SDL_GetError());
	}
	SDL_AudioSpec spec;
	if (!SDL_LoadWAV(wav_path.c_str(), &spec, &wav_data,
	                 &wav_len)) { // 加载wav
		SDL_Log("%s", SDL_GetError());
	}
	stream = SDL_CreateAudioStream(&spec, NULL);
	if (!stream) {
		SDL_Log("%s", SDL_GetError());
	}
	if (!SDL_PutAudioStreamData(stream, wav_data, wav_len)) { // 推流
		SDL_Log("%s", SDL_GetError());
	}
	if (!SDL_BindAudioStream(device_id, stream)) { // 绑定设备
		SDL_Log("%s", SDL_GetError());
	}
	SDL_PauseAudioDevice(device_id);
}

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
