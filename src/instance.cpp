#include <SDL3/SDL_time.h>
#include <gui.h>
#include <main.h>

namespace instance {

using namespace gui;

void externVariable() {
	initVariable(Ww, Wh, TEXTwhRATIO, TEXTwhRATIOC);
	initPath(Path);
	initMainRenderer(rend);
}

struct mainUDS {
	SDL_FRect rect;
	Point mouseP = {-1, -1};
} mainUD;

struct HelloUDS {
	SDL_FRect backButtonR;
	SDL_FRect lowVolumeR;
	SDL_FRect highVolumeR;
} HelloUD;

SDL_AudioStream *stream = nullptr;
Uint8 *wav_data = nullptr;
Uint32 wav_len = 0;

enum class SceneIndexes {
	Main = 0,
	Hello = 1,
};

enum class mouseEvent {
	Null = 0,
	OnButton = 1,
	LeftDown = 2,
	LeftUp = 3,
	Ban = 4,
};

enum class mainState {
	UnOn = 0,
	On = 1,
	Down = 2,
	Ban = 3,
	toHello = 5,
};

eventIndex mouseOnButton(SDL_Event &eve, void *ud) {
	switch (eve.type) {
	case SDL_EVENT_MOUSE_MOTION:
		((mainUDS *)ud)->mouseP.x = eve.motion.x;
		((mainUDS *)ud)->mouseP.y = eve.motion.y;
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

	SDL_Time time;
	SDL_GetCurrentTime(&time);
	SDL_DateTime dt;
	SDL_TimeToDateTime(time, &dt, true);
	if (dt.day < 21) {
		return (eventIndex)mouseEvent::Ban;
	}
	// SDL_Log("%lf,%lf\n", x, y);

	if (InRect(&((mainUDS *)ud)->rect, ((mainUDS *)ud)->mouseP)) {
		return (eventIndex)mouseEvent::OnButton;
	} else {
		return (eventIndex)mouseEvent::Null;
	}
}

stateScene *mainScene = nullptr;
stateTree *mainTree = nullptr;

void drawMainUnOn(SDL_Renderer *re, texMap *mp, void *ud) {
	SDL_SetRenderDrawColor(re, 0, 0, 0, 255);
	SDL_RenderClear(re);
	SDL_SetRenderDrawColor(re, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderFillRect(re, &((mainUDS *)ud)->rect);
	if (mp->count("MainButton")) {
		SDL_RenderTexture(re, (*mp)["MainButton"], NULL,
		                  &((mainUDS *)ud)->rect);
	}
}

void drawMainOn(SDL_Renderer *re, texMap *mp, void *ud) {
	SDL_SetRenderDrawColor(re, 0, 0, 0, 255);
	SDL_RenderClear(re);
	SDL_SetRenderDrawColor(re, 0xff, 0, 0, 0xff);
	SDL_RenderFillRect(re, &((mainUDS *)ud)->rect);
	if (mp->count("MainWhite")) {
		SDL_RenderTexture(re, (*mp)["MainWhite"], NULL, &((mainUDS *)ud)->rect);
	}
}

void drawMainDown(SDL_Renderer *re, texMap *mp, void *ud) {
	SDL_SetRenderDrawColor(re, 0, 0, 0, 255);
	SDL_RenderClear(re);
	SDL_SetRenderDrawColor(re, 0xff, 0, 0, 128);
	SDL_RenderFillRect(re, &((mainUDS *)ud)->rect);
	if (mp->count("MainWhite")) {
		SDL_RenderTexture(re, (*mp)["MainWhite"], NULL, &((mainUDS *)ud)->rect);
	}
}

drawScript drawMainBan = [](SDL_Renderer *re, texMap *mp, void *ud) {
	SDL_SetRenderDrawColor(re, 255, 0, 0, 255);
	SDL_RenderClear(re);
	SDL_Time time;
	SDL_GetCurrentTime(&time);
	SDL_DateTime dt;
	SDL_TimeToDateTime(time, &dt, true);
	SDL_Color c = {0xff, 0xff, 0xff, 0xff};
	std::string ttxt = "现在是" + std::to_string(dt.day) + "号，还不到点哦！";
	SDL_Texture *tt = getFontTex(mainFontName, c, ttxt.c_str(), 100);
	SDL_RenderTexture(re, tt, NULL, NULL);
};

stateIndex buttonUnonIdle(SDL_AudioDeviceID id, void *ud) {
	return (stateIndex)mainState::UnOn;
}
stateIndex buttonUnonToOn(SDL_AudioDeviceID id, void *ud) {
	SDL_Log("ON The Button!");
	return (stateIndex)mainState::On;
}
stateIndex buttonOnToUnon(SDL_AudioDeviceID id, void *ud) {
	SDL_Log("You UnOn it");
	return (stateIndex)mainState::UnOn;
}
stateIndex buttonOnIdle(SDL_AudioDeviceID id, void *ud) {
	return (stateIndex)mainState::On;
}
stateIndex buttonOnToDown(SDL_AudioDeviceID id, void *ud) {
	SDL_Log("You Click It!You do it!");
	return (stateIndex)mainState::Down;
}
stateIndex buttonDownToUnon(SDL_AudioDeviceID id, void *ud) {
	return (stateIndex)mainState::UnOn;
} // 如果需要按下时鼠标移开仍然显示按下的样子，但不希望松开时跳转，则需要两种Up方法
stateIndex buttonDownToHello(SDL_AudioDeviceID id, void *ud) {
	return (stateIndex)mainState::toHello;
}
stateHandler buttonBan = [](SDL_AudioDeviceID id, void *ud) {
	return (stateIndex)mainState::Ban;
};
stateHandler buttonBanToUnon = [](SDL_AudioDeviceID id, void *ud) {
	return (stateIndex)mainState::UnOn;
};

stateHandleTable table1 = {
    {buttonUnonIdle, buttonUnonToOn, nullptr, nullptr, buttonBan},
    {buttonOnToUnon, buttonOnIdle, buttonOnToDown},
    {buttonDownToUnon, nullptr, nullptr, buttonDownToHello},
    {buttonBanToUnon}};

stateScene *HelloScene;
enum class HelloEventIndex {
	SayHello = 1,
	IWantBack = 2,
};
enum class HelloStateIndex {
	Hello = 0,
	Played = 1,
	BackMain = 2,
};
eventIndex HelloEvent(SDL_Event &eve, void *ud) {
	return (eventIndex)HelloEventIndex::SayHello;
}
drawScript drawSayHello = [](SDL_Renderer *re, texMap *mp, void *ud) -> void {
	SDL_SetRenderDrawColor(re, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderClear(re);
	Point p = {(double)Ww / 2, (double)Wh / 2};
	SDL_FRect r1 = genRectCenter(p, 1200, 10);
	if (mp->count("aBigHello")) {
		SDL_RenderTexture(re, (*mp)["aBigHello"], NULL, &r1);
	}
	SDL_FRect r3_d = genRectCenter({WinWeight / 2, 800}, 300, 22, TextRatioC);
	if (mp->count("下面的小字")) {
		SDL_RenderTexture(re, (*mp)["下面的小字"], NULL, &r3_d);
	}
};

drawScript helloSilence = [](SDL_Renderer *re, texMap *mp, void *ud) -> void {
	SDL_SetRenderDrawColor(re, 0xff, 0, 0, 0xff);
	SDL_RenderClear(re);
};

stateHandler playMusic = [](SDL_AudioDeviceID id, void *ud) -> stateIndex {
	SDL_ResumeAudioDevice(id);
	SDL_Log("Play!\n");
	return (stateIndex)HelloStateIndex::Played;
};

stateHandleTable table3 = {{nullptr, playMusic}};

void stayMain() {};

stateSceneHandleTable table2 = {{stayMain}};

void initInstance() {
	SDL_Color c1;
	SDL_Color c2;
	std::string t1;
	SDL_Texture *f1;
	SDL_Texture *f2;
	c1 = {0, 0, 0, 255};
	c2 = {255, 255, 255, 255};
	t1 = "      按这里看惊喜（      ";
	f1 = getFontTex(bigFontName, c1, t1, 40);
	f2 = getFontTex(bigFontName, c2, t1, 40);
	SDL_Color c3 = {255, 0, 0, 255};
	Point p = {100, 600};
	mainUD.rect = genRect(p, 700, t1.size(), TextRatioC);
	HelloUD.backButtonR = genRect({100, 100}, 200, 4);
	HelloUD.lowVolumeR = genRect({WinWeight - 200, 400}, 100, 1);
	HelloUD.highVolumeR = genRect({WinWeight - 200, 600}, 100, 1);

	mainScene =
	    new stateScene(rend, mouseOnButton, (stateSceneIndex)SceneIndexes::Main,
	                   drawMainUnOn, (stateIndex)mainState::UnOn);
	std::string t3 = "lr生日快乐！😄";
	std::string t3_d = "这是个笑脸但是显示不了";
	mainTree = new stateTree(mainScene, (stateSceneIndex)SceneIndexes::Main);
	SDL_Texture *f3 = getFontTex(bigFontName, c3, t3, 100);
	SDL_Texture *f3_d = getFontTex(mainFontName, c3, t3_d, 100);
	SDL_Texture *f_back = getFontTex(bigFontName, c1, "BACK", 30);
	SDL_Texture *f_back_w = getFontTex(bigFontName, c2, "BACK", 30);

	HelloScene =
	    new stateScene(rend, HelloEvent, (stateSceneIndex)SceneIndexes::Hello,
	                   helloSilence, (stateIndex)HelloStateIndex::Hello);
	// if (!f1 || !f2) {
	// 	SDL_Log("Fuck");
	// }

	mainScene->appendTex(f1, "MainButton");
	mainScene->appendTex(f2, "MainWhite");
	mainScene->appendDrawScript(drawMainOn, (stateIndex)mainState::On);
	mainScene->appendDrawScript(drawMainDown, (stateIndex)mainState::Down);
	mainScene->appendDrawScript(drawMainBan, (stateIndex)mainState::Ban);

	mainScene->addUserData(&mainUD);
	HelloScene->addUserData(&HelloUD);

	HelloScene->appendTex(f3, "aBigHello");
	HelloScene->appendTex(f3_d, "下面的小字");
	HelloScene->appendTex(f_back, "Back");
	HelloScene->appendTex(f_back_w, "BackW");
	HelloScene->appendDrawScript(drawSayHello,
	                             (stateIndex)HelloStateIndex::Played);

	loadWAV("hello.wav", stream, &wav_data, &wav_len);
	HelloScene->addDevice(
	    SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL));
	HelloScene->putStream(stream, &wav_data, &wav_len);

	mainScene->setTable(table1);
	HelloScene->setTable(table3);

	mainTree->setTable(table2);
	mainTree->appendState(HelloScene, (stateSceneIndex)SceneIndexes::Hello);
}

void destroyInstance() {
	delete mainScene;
	delete mainTree;
	destroyMainRenderer();
}
}; // namespace instance
