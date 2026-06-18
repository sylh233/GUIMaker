#include <SDL3/SDL.h>
#include <gui.h>
#include <map>
#include <string>
#include <vector>

namespace gui {

stateScene::stateScene(SDL_Renderer *r, getEvent geter) {
	eventGeter = geter;
	rend = r;
}

void stateScene::appendDrawScript(drawScript call, std::string name) {
	scriptSet[name] = call;
}

void stateScene::appendFontTex(SDL_Texture *font, std::string name) {
	FontsTex[name] = font;
}

void stateScene::setTable(stateHandleTable table) {
	stateTable = std::move(table);
}

stateIndex stateScene::event(SDL_Event &e) {
	stateBack sb;
	SDL_PollEvent(&e);
	sb = stateTable[currentStateIndex][eventGeter(e)]();
	
	
}

void stateScene::show() { script(rend, &FontsTex); }

stateTree::stateTree(SDL_Renderer *r, SDL_Event *e, stateScene *s,
                     std::string name) {
	rend = r;
	eve = e;
	appendState(s, name);
}

void stateTree::appendState(stateScene *s, std::string name) { tree[name] = s; }

void stateTree::setTable(stateHandleTable table) {
	stateTable = std::move(table);
}

void stateTree::event(SDL_Event &e) {}

void stateTree::run() {}

}; // namespace gui
