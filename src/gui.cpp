#include <SDL3/SDL.h>
#include <gui.h>
#include <main.h>
#include <map>
#include <string>
#include <vector>

namespace gui {

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
	    eventGeter(e) >= stateTable[currentStateIndex].size() ||
	    stateTable[currentStateIndex][eventGeter(e)] == NULL) {
		return selfIndex;
	}
	stateIndex nextState = stateTable[currentStateIndex][eventGeter(e)]();
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
		script(rend, &FontsTex);
	}
}

stateScene::~stateScene() {
	for (auto p = FontsTex.begin(); p != FontsTex.end(); p++) {
		SDL_DestroyTexture(p->second);
	}
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
