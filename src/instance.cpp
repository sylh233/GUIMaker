#include <SDL3/SDL.h>
#include <gui.h>
#include <main.h>

namespace instance {
using namespace gui;

enum class SceneIndexes {
	Main = 0,
	Hello = 1,
};

enum class mouseEvent {
	Null = 0,
	OnButton = 1,
	LeftDown = 2,
	LeftUp = 3,
};

enum class mainState {
	UnOn = 0,
	On = 1,
	Down = 2,
	toHello = 4, // 4-3 = 1
};

eventIndex mouseOnButton(SDL_Event &eve) {
	Point mouseP = {-1, -1};
	switch (eve.type) {
	case SDL_EVENT_MOUSE_MOTION:
		mouseP.x = eve.motion.x;
		mouseP.y = eve.motion.y;
		break;
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		switch (eve.button.button) {
		case SDL_BUTTON_LEFT:
			return (eventIndex)mouseEvent::LeftDown;
			break;
		default:
			break;
		}
		break;
	case SDL_EVENT_MOUSE_BUTTON_UP:
		switch (eve.button.button) {
		case SDL_BUTTON_LEFT:
			return (eventIndex)mouseEvent::LeftUp;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	// SDL_Log("%lf,%lf\n", x, y);

	Point p = {200, 200};
	SDL_FRect rect = genRectCenter(p, 200, 5);

	if (InRect(&rect, mouseP)) {
		return (eventIndex)mouseEvent::OnButton;
	} else {
		return (eventIndex)mouseEvent::Null;
	}
}

stateScene *mainScene = nullptr;
stateTree *mainTree = nullptr;

void drawMainUnOn(SDL_Renderer *re, fontMap *mp) {
	SDL_SetRenderDrawColor(re, 0, 0, 0, 255);
	SDL_RenderClear(re);
	Point p = {200, 200};
	SDL_FRect r1 = genRectCenter(p, 200, 5);
	SDL_SetRenderDrawColor(re, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderFillRect(re, &r1);
	if (mp->count("MainButton")) {
		SDL_RenderTexture(re, (*mp)["MainButton"], NULL, &r1);
	}
}

void drawMainOn(SDL_Renderer *re, fontMap *mp) {
	SDL_SetRenderDrawColor(re, 0, 0, 0, 255);
	SDL_RenderClear(re);
	Point p = {200, 200};
	SDL_FRect r1 = genRectCenter(p, 200, 5);
	SDL_SetRenderDrawColor(re, 0xff, 0, 0, 0xff);
	SDL_RenderFillRect(re, &r1);
	if (mp->count("MainWhite")) {
		SDL_RenderTexture(re, (*mp)["MainWhite"], NULL, &r1);
	}
}

void drawMainDown(SDL_Renderer *re, fontMap *mp) {
	SDL_SetRenderDrawColor(re, 0, 0, 0, 255);
	SDL_RenderClear(re);
	Point p = {200, 200};
	SDL_FRect r1 = genRectCenter(p, 200, 5);
	SDL_SetRenderDrawColor(re, 0xff, 0, 0, 128);
	SDL_RenderFillRect(re, &r1);
	if (mp->count("MainWhite")) {
		SDL_RenderTexture(re, (*mp)["MainWhite"], NULL, &r1);
	}
}

stateIndex buttonUnonIdle() { return (stateIndex)mainState::UnOn; }
stateIndex buttonUnonToOn() {
	SDL_Log("ON The Button!");
	return (stateIndex)mainState::On;
}
stateIndex buttonOnToUnon() {
	SDL_Log("You UnOn it");
	return (stateIndex)mainState::UnOn;
}
stateIndex buttonOnIdle() { return (stateIndex)mainState::On; }
stateIndex buttonOnToDown() {
	SDL_Log("You Click It!You do it!");
	return (stateIndex)mainState::Down;
}
stateIndex buttonDownToUnon() {
	return (stateIndex)mainState::UnOn;
} // 如果需要按下时鼠标移开仍然显示按下的样子，但不希望松开时跳转，则需要两种Up方法
stateIndex buttonDownToHello() { return (stateIndex)mainState::toHello; }

stateHandleTable table1 = {
    {buttonUnonIdle, buttonUnonToOn},
    {buttonOnToUnon, buttonOnIdle, buttonOnToDown},
    {buttonDownToUnon, nullptr, nullptr, buttonDownToHello}};

stateScene *HelloScene;
enum class HelloEventIndex {
	SayHello = 0,
};
enum class HelloStateIndex {
	Hello = 0,
};
eventIndex HelloEvent(SDL_Event &eve) {
	return (eventIndex)HelloEventIndex::SayHello;
}
void drawSayHello(SDL_Renderer *re, fontMap *mp) {
	SDL_SetRenderDrawColor(re, 0, 0, 0, 0xff);
	SDL_RenderClear(re);
	Point p = {(double)Ww / 2, (double)Wh / 2};
	SDL_FRect r1 = genRectCenter(p, 600, 5);
	if (mp->count("aBigHello")) {
		SDL_RenderTexture(re, (*mp)["aBigHello"], NULL, &r1);
	}
}

stateHandleTable table3 = {{}};

void stayMain() {};

stateSceneHandleTable table2 = {{stayMain}};

void initInstance() {
	mainScene = new stateScene(rend, mouseOnButton,
	                           (stateSceneIndex)SceneIndexes::Main);
	mainTree = new stateTree(mainScene, (stateSceneIndex)SceneIndexes::Main);
	SDL_Color c1;
	SDL_Color c2;
	std::string t1;
	SDL_Texture *f1;
	SDL_Texture *f2;
	c1 = {0, 0, 0, 255};
	c2 = {255, 255, 255, 255};
	t1 = "Hello";
	f1 = getFontTex(mainFontName, c1, t1, 40);
	f2 = getFontTex(mainFontName, c2, t1, 40);
	SDL_Color c3 = {255, 0, 0, 255};
	std::string t3 = "Hello";
	SDL_Texture *f3 = getFontTex(bigFontName, c3, t3, 100);
	HelloScene =
	    new stateScene(rend, HelloEvent, (stateSceneIndex)SceneIndexes::Hello);
	// if (!f1 || !f2) {
	// 	SDL_Log("Fuck");
	// }

	mainScene->appendFontTex(f1, "MainButton");
	mainScene->appendFontTex(f2, "MainWhite");
	mainScene->appendDrawScript(drawMainUnOn, (stateIndex)mainState::UnOn);
	mainScene->appendDrawScript(drawMainOn, (stateIndex)mainState::On);
	mainScene->appendDrawScript(drawMainDown, (stateIndex)mainState::Down);

	HelloScene->appendFontTex(f3, "aBigHello");
	HelloScene->appendDrawScript(drawSayHello,
	                             (stateIndex)HelloStateIndex::Hello);

	mainScene->setTable(table1);
	HelloScene->setTable(table3);

	mainTree->setTable(table2);
	mainTree->appendState(HelloScene, (stateSceneIndex)SceneIndexes::Hello);
}

void destroyInstance() {
	delete mainScene;
	delete mainTree;
}
}; // namespace instance
