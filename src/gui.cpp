#include <SDL3/SDL.h>
#include <gui.h>
#include <main.h>
#include <map>
#include <string>
#include <vector>

namespace gui {

stateScene::stateScene(SDL_Renderer *r, getEvent geter, stateSceneIndex self) {
	scriptSet.resize(1);
	eventGeter = geter;
	rend = r;
	selfIndex = self;
}

void stateScene::appendDrawScript(drawScript call, stateIndex i) {
	while (i >= scriptSet.size()) {
		scriptSet.resize(scriptSet.size() * 2);
	}
	if (script == NULL) {
		currentStateIndex = i;
		script = call;
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
	currentStateIndex = stateTable[currentStateIndex][eventGeter(e)]();
	if (currentStateIndex >= scriptSet.size()) {
		return currentStateIndex - scriptSet.size();
	}
	script = scriptSet[currentStateIndex];
	return selfIndex;
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
		tree.resize(tree.size() * 2);
	}
	tree[i] = s;
}

void stateTree::setTable(stateSceneHandleTable table) {
	stateSceneTable = std::move(table);
}

void stateTree::event(SDL_Event &eve) {
	if (currentState >= stateSceneTable.size()) {
		return;
	}
	stateSceneIndex nextIndex = tree[currentState]->event(eve);
	if (nextIndex >= tree.size()) {
		return;
	}
	if (nextIndex < stateSceneTable[currentState].size()) {
		stateSceneTable[currentState][nextIndex]();
	}
	currentState = nextIndex;
}

void stateTree::run() { tree[currentState]->show(); }

}; // namespace gui
