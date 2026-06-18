#include <SDL3/SDL.h>
#include <gui.h>
#include <main.h>

namespace instance {
using namespace gui;

enum class mouseEvent {
	Null = 0,
	OnButton = 1,
};

enum class SceneIndexes {
	Main = 0,
};

enum class mainState {
	UnOn = 0,
	On = 1,
};

eventIndex mouseOnButton(SDL_Event &eve) {
	double x = -1, y = -1;

	x = eve.motion.x;
	y = eve.motion.y;
	// SDL_Log("%lf,%lf\n", x, y);

	if (x > 200 && x < 400 && y > 200 && y < 300) {
		return (eventIndex)mouseEvent::OnButton;
	} else {
		return (eventIndex)mouseEvent::Null;
	}
}

stateScene *mainScene = nullptr;
stateTree *mainTree = nullptr;

SDL_Color c1;
SDL_Color c2;
std::string t1;
SDL_Texture *f1;
SDL_Texture *f2;

void drawMainUnOn(SDL_Renderer *re, fontMap *mp) {
	SDL_SetRenderDrawColor(re, 0, 0, 0, 255);
	SDL_RenderClear(re);
	SDL_FRect textR1 = {200, 200, 200, 100};
	if (mp->count("UnOn")) {
		SDL_RenderTexture(re, (*mp)["UnOn"], NULL, &textR1);
	}
}

void drawMainOn(SDL_Renderer *re, fontMap *mp) {
	SDL_SetRenderDrawColor(re, 0, 0, 0, 255);
	SDL_RenderClear(re);
	SDL_FRect textR1 = {200, 200, 200, 100};
	if (mp->count("On")) {
		SDL_RenderTexture(re, (*mp)["On"], NULL, &textR1);
	}
}

stateIndex buttonUnonIdle() { return (stateIndex)mainState::UnOn; }
stateIndex buttonUnonToOn() {
	SDL_Log("The Button ON!");
	return (stateIndex)mainState::On;
}
stateIndex buttonOnToUnon() {
	SDL_Log("You UnOn it");
	return (stateIndex)mainState::UnOn;
}
stateIndex buttonOnIdle() { return (stateIndex)mainState::On; }

stateHandleTable table1 = {
    {buttonUnonIdle, buttonUnonToOn},
    {buttonOnToUnon, buttonOnIdle},
};

void stayMain() {};

stateSceneHandleTable table2 = {{stayMain}};

void initInstance() {
	mainScene = new stateScene(rend, mouseOnButton,
	                           (stateSceneIndex)SceneIndexes::Main);
	mainTree = new stateTree(mainScene, (stateSceneIndex)SceneIndexes::Main);
	c1 = {255, 255, 255, 255};
	c2 = {255, 255, 255, 255};
	t1 = "Hello";
	f1 = getFontTex(mainFontName, c1, t1, 26);
	f2 = getFontTex(bigFontName, c2, t1, 28);

	mainScene->appendFontTex(f1, "UnOn");
	mainScene->appendFontTex(f2, "On");
	mainScene->appendDrawScript(drawMainUnOn, (stateIndex)mainState::UnOn);
	mainScene->appendDrawScript(drawMainOn, (stateIndex)mainState::On);
	mainScene->setTable(table1);

	mainTree->setTable(table2);
}

void destroyInstance() {
	delete mainScene;
	delete mainTree;
}
}; // namespace instance
