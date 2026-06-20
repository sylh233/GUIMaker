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
	Point mouseP = {-1, -1};
	SDL_FRect backButtonR;
	SDL_FRect lowVolumeR;
	SDL_FRect highVolumeR;
	float Volume = 1.0f;

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
	SDL_DestroyTexture(tt);
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
	HighOn = 3,
	LowOn = 4,
	ButtonDown = 5,
	ButtonUp = 6,
};
enum class HelloStateIndex {
	Hello = 0,
	Played = 1,
	BackOn = 2,
	BackDown = 3,
	HighOn = 4,
	LowOn = 5,
	//
	BackMain = 6,
};

eventIndex HelloEvent(SDL_Event &eve, void *ud) {
	switch (eve.type) {
	case SDL_EVENT_MOUSE_MOTION:
		((HelloUDS *)ud)->mouseP.x = eve.motion.x;
		((HelloUDS *)ud)->mouseP.y = eve.motion.y;
		break;
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		if (eve.button.button == SDL_BUTTON_LEFT) {
			return (eventIndex)HelloEventIndex::ButtonDown;
		}
		break;
	case SDL_EVENT_MOUSE_BUTTON_UP:
		if (eve.button.button == SDL_BUTTON_LEFT) {
			return (eventIndex)HelloEventIndex::ButtonUp;
		}
		break;
	default:
		break;
	}
	if (InRect(&(((HelloUDS *)ud)->backButtonR), ((HelloUDS *)ud)->mouseP)) {
		return (eventIndex)HelloEventIndex::IWantBack;
	}
	if (InRect(&(((HelloUDS *)ud)->lowVolumeR), ((HelloUDS *)ud)->mouseP)) {
		return (eventIndex)HelloEventIndex::LowOn;
	}
	if (InRect(&(((HelloUDS *)ud)->highVolumeR), ((HelloUDS *)ud)->mouseP)) {
		return (eventIndex)HelloEventIndex::HighOn;
	}
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
	SDL_SetRenderDrawColor(re, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderFillRect(re, &(((HelloUDS *)ud)->backButtonR));
	if (mp->count("Back")) {
		SDL_RenderTexture(re, (*mp)["Back"], NULL,
		                  &(((HelloUDS *)ud)->backButtonR));
	}
	SDL_SetRenderDrawColor(re, 0xff, 0, 0, 0xff);
	SDL_RenderFillRect(re, &(((HelloUDS *)ud)->lowVolumeR));
	SDL_RenderFillRect(re, &(((HelloUDS *)ud)->highVolumeR));
	if (mp->count("H")) {
		SDL_RenderTexture(re, (*mp)["H"], NULL,
		                  &(((HelloUDS *)ud)->highVolumeR));
	}
	if (mp->count("L")) {
		SDL_RenderTexture(re, (*mp)["L"], NULL,
		                  &(((HelloUDS *)ud)->lowVolumeR));
	}
	SDL_Texture *V = getFontTex(
	    mainFontName, {0xff, 0, 0, 0xff},
	    std::to_string(((int)(100 * ((HelloUDS *)ud)->Volume))) + "%", 40);
	SDL_FRect vr = genRectCenter({WinWeight / 2, 100}, 100, 3);
	SDL_RenderTexture(re, V, NULL, &vr);
	SDL_DestroyTexture(V);
};

drawScript helloSilence = [](SDL_Renderer *re, texMap *mp, void *ud) -> void {
	SDL_SetRenderDrawColor(re, 0xff, 0, 0, 0xff);
	SDL_RenderClear(re);
};

drawScript drawBackOn = [](SDL_Renderer *re, texMap *mp, void *ud) -> void {
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
	SDL_SetRenderDrawColor(re, 0xff, 0, 0, 0xff);
	SDL_RenderFillRect(re, &(((HelloUDS *)ud)->backButtonR));
	if (mp->count("BackW")) {
		SDL_RenderTexture(re, (*mp)["BackW"], NULL,
		                  &(((HelloUDS *)ud)->backButtonR));
	}
	SDL_SetRenderDrawColor(re, 0xff, 0, 0, 0xff);
	SDL_RenderFillRect(re, &(((HelloUDS *)ud)->lowVolumeR));
	SDL_RenderFillRect(re, &(((HelloUDS *)ud)->highVolumeR));
	if (mp->count("H")) {
		SDL_RenderTexture(re, (*mp)["H"], NULL,
		                  &(((HelloUDS *)ud)->highVolumeR));
	}
	if (mp->count("L")) {
		SDL_RenderTexture(re, (*mp)["L"], NULL,
		                  &(((HelloUDS *)ud)->lowVolumeR));
	}
	SDL_Texture *V = getFontTex(
	    mainFontName, {0xff, 0, 0, 0xff},
	    std::to_string(((int)(100 * ((HelloUDS *)ud)->Volume))) + "%", 40);
	SDL_FRect vr = genRectCenter({WinWeight / 2, 100}, 100, 3);
	SDL_RenderTexture(re, V, NULL, &vr);
	SDL_DestroyTexture(V);
};

drawScript drawBackDown = [](SDL_Renderer *re, texMap *mp, void *ud) -> void {
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
	SDL_SetRenderDrawColor(re, 0xa5, 0, 0, 0xff);
	SDL_RenderFillRect(re, &(((HelloUDS *)ud)->backButtonR));
	if (mp->count("BackW")) {
		SDL_RenderTexture(re, (*mp)["BackW"], NULL,
		                  &(((HelloUDS *)ud)->backButtonR));
	}
	SDL_SetRenderDrawColor(re, 0xff, 0, 0, 0xff);
	SDL_RenderFillRect(re, &(((HelloUDS *)ud)->lowVolumeR));
	SDL_RenderFillRect(re, &(((HelloUDS *)ud)->highVolumeR));
	if (mp->count("H")) {
		SDL_RenderTexture(re, (*mp)["H"], NULL,
		                  &(((HelloUDS *)ud)->highVolumeR));
	}
	if (mp->count("L")) {
		SDL_RenderTexture(re, (*mp)["L"], NULL,
		                  &(((HelloUDS *)ud)->lowVolumeR));
	}
	SDL_Texture *V = getFontTex(
	    mainFontName, {0xff, 0, 0, 0xff},
	    std::to_string(((int)(100 * ((HelloUDS *)ud)->Volume))) + "%", 40);
	SDL_FRect vr = genRectCenter({WinWeight / 2, 100}, 100, 3);
	SDL_RenderTexture(re, V, NULL, &vr);
	SDL_DestroyTexture(V);
};

drawScript drawHighOn = [](SDL_Renderer *re, texMap *mp, void *ud) -> void {
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
	SDL_SetRenderDrawColor(re, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderFillRect(re, &(((HelloUDS *)ud)->backButtonR));
	if (mp->count("Back")) {
		SDL_RenderTexture(re, (*mp)["Back"], NULL,
		                  &(((HelloUDS *)ud)->backButtonR));
	}
	SDL_SetRenderDrawColor(re, 0xff, 0, 0, 0xff);
	SDL_RenderFillRect(re, &(((HelloUDS *)ud)->lowVolumeR));
	SDL_SetRenderDrawColor(re, 0xa5, 0, 0, 0xff);
	SDL_RenderFillRect(re, &(((HelloUDS *)ud)->highVolumeR));
	if (mp->count("H")) {
		SDL_RenderTexture(re, (*mp)["H"], NULL,
		                  &(((HelloUDS *)ud)->highVolumeR));
	}
	if (mp->count("L")) {
		SDL_RenderTexture(re, (*mp)["L"], NULL,
		                  &(((HelloUDS *)ud)->lowVolumeR));
	}
	SDL_Texture *V = getFontTex(
	    mainFontName, {0xff, 0, 0, 0xff},
	    std::to_string(((int)(100 * ((HelloUDS *)ud)->Volume))) + "%", 40);
	SDL_FRect vr = genRectCenter({WinWeight / 2, 100}, 100, 3);
	SDL_RenderTexture(re, V, NULL, &vr);
	SDL_DestroyTexture(V);
};

drawScript drawLowOn = [](SDL_Renderer *re, texMap *mp, void *ud) -> void {
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
	SDL_SetRenderDrawColor(re, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderFillRect(re, &(((HelloUDS *)ud)->backButtonR));
	if (mp->count("Back")) {
		SDL_RenderTexture(re, (*mp)["Back"], NULL,
		                  &(((HelloUDS *)ud)->backButtonR));
	}
	SDL_SetRenderDrawColor(re, 0xa5, 0, 0, 0xff);
	SDL_RenderFillRect(re, &(((HelloUDS *)ud)->lowVolumeR));
	SDL_SetRenderDrawColor(re, 0xff, 0, 0, 0xff);
	SDL_RenderFillRect(re, &(((HelloUDS *)ud)->highVolumeR));
	if (mp->count("H")) {
		SDL_RenderTexture(re, (*mp)["H"], NULL,
		                  &(((HelloUDS *)ud)->highVolumeR));
	}
	if (mp->count("L")) {
		SDL_RenderTexture(re, (*mp)["L"], NULL,
		                  &(((HelloUDS *)ud)->lowVolumeR));
	}
	SDL_Texture *V = getFontTex(
	    mainFontName, {0xff, 0, 0, 0xff},
	    std::to_string(((int)(100 * ((HelloUDS *)ud)->Volume))) + "%", 40);
	SDL_FRect vr = genRectCenter({WinWeight / 2, 100}, 100, 3);
	SDL_RenderTexture(re, V, NULL, &vr);
	SDL_DestroyTexture(V);
};

stateHandler playMusic = [](SDL_AudioDeviceID id, void *ud) -> stateIndex {
	SDL_ResumeAudioDevice(id);
	SDL_Log("Play!\n");
	return (stateIndex)HelloStateIndex::Played;
};
stateHandler hdIdle = [](SDL_AudioDeviceID id, void *ud) -> stateIndex {
	return (stateIndex)HelloStateIndex::Played;
};
stateHandler hdBackOn = [](SDL_AudioDeviceID id, void *ud) -> stateIndex {
	return (stateIndex)HelloStateIndex::BackOn;
};
stateHandler hdBackDown = [](SDL_AudioDeviceID id, void *ud) -> stateIndex {
	return (stateIndex)HelloStateIndex::BackDown;
};
stateHandler hdBackUp = [](SDL_AudioDeviceID id, void *ud) -> stateIndex {
	SDL_PauseAudioDevice(id);
	return (stateIndex)HelloStateIndex::BackMain;
};
stateHandler hdHighOn = [](SDL_AudioDeviceID id, void *ud) -> stateIndex {
	return (stateIndex)HelloStateIndex::HighOn;
};
stateHandler hdHighDown = [](SDL_AudioDeviceID id, void *ud) -> stateIndex {
	((HelloUDS *)ud)->Volume = ((HelloUDS *)ud)->Volume + 0.1 <= 2
	                               ? ((HelloUDS *)ud)->Volume + 0.1
	                               : 2;
	SDL_SetAudioDeviceGain(id, ((HelloUDS *)ud)->Volume);
	return (stateIndex)HelloStateIndex::Played;
};
stateHandler hdLowOn = [](SDL_AudioDeviceID id, void *ud) -> stateIndex {
	return (stateIndex)HelloStateIndex::LowOn;
};
stateHandler hdLowDown = [](SDL_AudioDeviceID id, void *ud) -> stateIndex {
	((HelloUDS *)ud)->Volume = ((HelloUDS *)ud)->Volume - 0.1 >= 0
	                               ? ((HelloUDS *)ud)->Volume - 0.1
	                               : 0;
	SDL_SetAudioDeviceGain(id, ((HelloUDS *)ud)->Volume);
	return (stateIndex)HelloStateIndex::Played;
};

stateHandleTable table3 = {
    {nullptr, playMusic, playMusic, playMusic, playMusic, playMusic, playMusic},
    {nullptr, nullptr, hdBackOn, hdHighOn, hdLowOn},
    {nullptr, hdIdle, nullptr, nullptr, nullptr, hdBackDown},
    {nullptr, hdIdle, nullptr, nullptr, nullptr, nullptr, hdBackUp},
    {nullptr, hdIdle, nullptr, nullptr, nullptr, hdHighDown},
    {nullptr, hdIdle, nullptr, nullptr, nullptr, hdLowDown}};

void stayMain() {};

stateSceneHandleTable table2 = {{stayMain}};

resetFunc MainRS = [](void *ud) -> void { ((mainUDS *)ud)->mouseP = {-1, -1}; };

resetFunc HelloRS = [](void *ud) -> void {
	((HelloUDS *)ud)->mouseP = {-1, -1};
};

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
	HelloUD.lowVolumeR = genRect({WinWeight - 200, WinHeight - 200}, 100, 1, 1);
	HelloUD.highVolumeR = genRect({WinWeight - 200, 100}, 100, 1, 1);
	std::string th = "大";
	std::string tl = "小";
	SDL_Texture *TH = getFontTex(mainFontName, c2, th, 20);
	SDL_Texture *TL = getFontTex(mainFontName, c2, tl, 20);

	mainScene =
	    new stateScene(rend, mouseOnButton, (stateSceneIndex)SceneIndexes::Main,
	                   drawMainUnOn, (stateIndex)mainState::UnOn);
	std::string t3 = "lr生日快乐！😄";
	std::string t3_d = "这是个笑脸但是显示不了";
	mainTree = new stateTree(mainScene, (stateSceneIndex)SceneIndexes::Main);
	SDL_Texture *f3 = getFontTex(bigFontName, c3, t3, 100);
	SDL_Texture *f3_d = getFontTex(mainFontName, c3, t3_d, 100);
	SDL_Color cr = {0xff, 0, 0, 0xff};
	SDL_Texture *f_back = getFontTex(bigFontName, cr, "BACK", 30);
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
	HelloScene->appendTex(TH, "H");
	HelloScene->appendTex(TL, "L");
	HelloScene->appendDrawScript(drawSayHello,
	                             (stateIndex)HelloStateIndex::Played);
	HelloScene->appendDrawScript(drawBackOn,
	                             (stateIndex)HelloStateIndex::BackOn);
	HelloScene->appendDrawScript(drawBackDown,
	                             (stateIndex)HelloStateIndex::BackDown);
	HelloScene->appendDrawScript(drawHighOn,
	                             (stateIndex)HelloStateIndex::HighOn);
	HelloScene->appendDrawScript(drawLowOn, (stateIndex)HelloStateIndex::LowOn);

	loadWAV("hello.wav", stream, &wav_data, &wav_len);
	HelloScene->addDevice(
	    SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL));
	HelloScene->putStream(stream, &wav_data, &wav_len);

	mainScene->setTable(table1);
	HelloScene->setTable(table3);

	mainTree->setTable(table2);
	mainTree->appendState(HelloScene, (stateSceneIndex)SceneIndexes::Hello);

	mainScene->setReset(MainRS);
	HelloScene->setReset(HelloRS);
}

void destroyInstance() {
	delete mainScene;
	delete mainTree;
	destroyMainRenderer();
	SDL_UnbindAudioStream(stream);
	SDL_DestroyAudioStream(stream);
}
}; // namespace instance
