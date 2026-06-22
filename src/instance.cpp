#include <SDL3/SDL_time.h>
#include <gui.h>
#include <main.h>

namespace instance {

using namespace gui;

void externVariable() {
	initVariable(Ww, Wh, TEXTwhRATIO, TEXTwhRATIOC);
	initPath(Path);
	initSDL(rend, &eve);
}

stateScene *mainScene = nullptr;

stateScene *HelloScene;

texMap tm1;

struct mainUDS {
	SDL_FRect rect;
	Point mouseP = {-1, -1};
	SDL_Renderer *rend;
	SDL_Event *eve;
	texMap *tm1p;
	std::string buttonTextName = "MainButton";
	SDL_Color buttonC = {255, 255, 255, 255};
} mainUD;

struct treeUDS {
	std::string info = "你好";
	std::vector<stateScene *> sv;
	size_t current = 0;
} treeUD;

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
	//	Ban = 4,
};

enum class mainState {
	UnOn = 0,
	On = 1,
	Down = 2,
	// Ban = 3,
	// toHello = 3,
};

eventIndex mouseOnButton(void *ud, void *od) {
	mainUDS *udp = (mainUDS *)ud;
	SDL_Event *eve = udp->eve;
	switch (eve->type) {
	case SDL_EVENT_MOUSE_MOTION:
		udp->mouseP.x = eve->motion.x;
		udp->mouseP.y = eve->motion.y;
		break;
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		switch (eve->button.button) {
		case SDL_BUTTON_LEFT:
			return (eventIndex)mouseEvent::LeftDown;
			break;
		default:
			break;
		}
		break;
	case SDL_EVENT_MOUSE_BUTTON_UP:
		switch (eve->button.button) {
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

	// SDL_Time time;
	// SDL_GetCurrentTime(&time);
	// SDL_DateTime dt;
	// SDL_TimeToDateTime(time, &dt, true);
	// if (dt.day < 21) {
	// 	return (eventIndex)mouseEvent::Ban;
	// }
	// SDL_Log("%lf,%lf\n", x, y);

	if (InRect(&(udp->rect), udp->mouseP)) {
		return (eventIndex)mouseEvent::OnButton;
	} else {
		return (eventIndex)mouseEvent::Null;
	}
}

// stateTree *mainTree = nullptr;

void mainMainScript(void *ud, void *od) {
	mainUDS *udp = (mainUDS *)ud;
	SDL_SetRenderDrawColor(udp->rend, 0, 0, 0, 255);
	SDL_RenderClear(udp->rend);
	SDL_Color c1 = udp->buttonC;
	SDL_SetRenderDrawColor(udp->rend, c1.r, c1.g, c1.b, c1.a);
	SDL_RenderFillRect(udp->rend, &((mainUDS *)ud)->rect);
	if (udp->tm1p->count(udp->buttonTextName)) {
		SDL_RenderTexture(udp->rend, (*(udp->tm1p))[udp->buttonTextName], NULL,
		                  &((mainUDS *)ud)->rect);
	}
}

stateScript drawMainUnon = [](void *ud, void *od) {
	mainUDS *udp = (mainUDS *)ud;
	udp->buttonC = {255, 255, 255, 255};
	udp->buttonTextName = "MainButton";
};

// void drawMainOn(SDL_Renderer *re, texMap *mp, void *ud) {
// 	SDL_SetRenderDrawColor(re, 0, 0, 0, 255);
// 	SDL_RenderClear(re);
// 	SDL_SetRenderDrawColor(re, 0xff, 0, 0, 0xff);
// 	SDL_RenderFillRect(re, &((mainUDS *)ud)->rect);
// 	if (mp->count("MainWhite")) {
// 		SDL_RenderTexture(re, (*mp)["MainWhite"], NULL, &((mainUDS *)ud)->rect);
// 	}
// }

stateScript drawMainOn = [](void *ud, void *od) {
	mainUDS *udp = (mainUDS *)ud;
	udp->buttonC = {255, 0, 0, 255};
	udp->buttonTextName = "MainWhite";
};

// void drawMainDown(SDL_Renderer *re, texMap *mp, void *ud) {
// 	SDL_SetRenderDrawColor(re, 0, 0, 0, 255);
// 	SDL_RenderClear(re);
// 	SDL_SetRenderDrawColor(re, 0xff, 0, 0, 128);
// 	SDL_RenderFillRect(re, &((mainUDS *)ud)->rect);
// 	if (mp->count("MainWhite")) {
// 		SDL_RenderTexture(re, (*mp)["MainWhite"], NULL, &((mainUDS *)ud)->rect);
// 	}
// }

stateScript drawMainDown = [](void *ud, void *od) {
	mainUDS *udp = (mainUDS *)ud;
	udp->buttonC = {255, 0, 0, 128};
	udp->buttonTextName = "MainWhite";
};

// drawScript drawMainBan = [](SDL_Renderer *re, texMap *mp, void *ud) {
// 	SDL_SetRenderDrawColor(re, 255, 0, 0, 255);
// 	SDL_RenderClear(re);
// 	SDL_Time time;
// 	SDL_GetCurrentTime(&time);
// 	SDL_DateTime dt;
// 	SDL_TimeToDateTime(time, &dt, true);
// 	SDL_Color c = {0xff, 0xff, 0xff, 0xff};
// 	std::string ttxt = "现在是" + std::to_string(dt.day) + "号，还不到点哦！";
// 	SDL_Texture *tt = getFontTex(mainFontName, c, ttxt.c_str(), 100);
// 	SDL_RenderTexture(re, tt, NULL, NULL);
// 	SDL_DestroyTexture(tt);
// };

stateIndex buttonUnonIdle(void *ud, void *od) {
	return (stateIndex)mainState::UnOn;
}
stateIndex buttonUnonToOn(void *ud, void *od) {
	SDL_Log("ON The Button!");
	return (stateIndex)mainState::On;
}
stateIndex buttonOnToUnon(void *ud, void *od) {
	SDL_Log("You UnOn it");
	return (stateIndex)mainState::UnOn;
}
stateIndex buttonOnIdle(void *ud, void *od) {
	return (stateIndex)mainState::On;
}
stateIndex buttonOnToDown(void *ud, void *od) {
	SDL_Log("You Click It!You do it!");
	return (stateIndex)mainState::Down;
}
stateIndex buttonDownToUnon(void *ud, void *od) {
	return (stateIndex)mainState::UnOn;
} // 如果需要按下时鼠标移开仍然显示按下的样子，但不希望松开时跳转，则需要两种Up方法
stateIndex buttonDownToHello(void *ud, void *od) {
	SDL_Log("Hello(Debugging):)");
	treeUDS *odp = (treeUDS *)od;
	mainUDS *udp = (mainUDS *)ud;
	odp->current = 1;
	udp->mouseP = {-1, -1};
	return (stateIndex)mainState::UnOn;
}
// stateHandler buttonBan = [](void *ud,void *od) {
// 	return (stateIndex)mainState::Ban;
// };
// stateHandler buttonBanToUnon = [](void *ud,void *od) {
// 	return (stateIndex)mainState::UnOn;
// };

stateHandleTable table1 = {
    {buttonUnonIdle, buttonUnonToOn, nullptr, nullptr},
    {buttonOnToUnon, buttonOnIdle, buttonOnToDown},
    {buttonDownToUnon, nullptr, nullptr, buttonDownToHello},
};

scriptSet ss1 = {drawMainUnon, drawMainOn, drawMainDown};

texMap tm2;
struct HelloUDS {
	Point mouseP = {-1, -1};
	SDL_FRect backButtonR;
	SDL_FRect lowVolumeR;
	SDL_FRect highVolumeR;
	SDL_FRect vr;
	SDL_AudioDeviceID id;
	int Volume = 100;
	SDL_Renderer *rend;
	SDL_Event *eve;
	texMap *tm2p;
	std::string backText;
	SDL_Color backBC, highC, lowC;
	SDL_Texture *volumeText;

} HelloUD;

enum class HelloEventIndex {
	SayHello = 0,
	IWantBack = 1,
	HighOn = 2,
	LowOn = 3,
	ButtonDown = 4,
	ButtonUp = 5,
};
enum class HelloStateIndex {
	Hello = 0,
	Played = 1,
	BackOn = 2,
	BackDown = 3,
	HighOn = 4,
	LowOn = 5,
	//
	// BackMain = 6,
};

eventIndex HelloEvent(void *ud, void *od) {
	HelloUDS *udp = (HelloUDS *)ud;
	SDL_Event *eve = udp->eve;
	switch (eve->type) {
	case SDL_EVENT_MOUSE_MOTION:
		udp->mouseP.x = eve->motion.x;
		udp->mouseP.y = eve->motion.y;
		break;
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		if (eve->button.button == SDL_BUTTON_LEFT) {
			return (eventIndex)HelloEventIndex::ButtonDown;
		}
		break;
	case SDL_EVENT_MOUSE_BUTTON_UP:
		if (eve->button.button == SDL_BUTTON_LEFT) {
			return (eventIndex)HelloEventIndex::ButtonUp;
		}
		break;
	default:
		break;
	}
	if (InRect(&(udp->backButtonR), udp->mouseP)) {
		return (eventIndex)HelloEventIndex::IWantBack;
	}
	if (InRect(&(udp->lowVolumeR), udp->mouseP)) {
		return (eventIndex)HelloEventIndex::LowOn;
	}
	if (InRect(&(udp->highVolumeR), udp->mouseP)) {
		return (eventIndex)HelloEventIndex::HighOn;
	}
	return (eventIndex)HelloEventIndex::SayHello;
}

stateScript drawHello = [](void *ud, void *od) {
	HelloUDS *udp = (HelloUDS *)ud;
	SDL_Renderer *re = udp->rend;
	texMap *mp = udp->tm2p;
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
	SDL_Color bc = udp->backBC;
	SDL_SetRenderDrawColor(re, bc.r, bc.g, bc.b, bc.a);
	SDL_RenderFillRect(re, &(udp->backButtonR));
	if (mp->count(udp->backText)) {
		SDL_RenderTexture(re, (*mp)[udp->backText], NULL, &(udp->backButtonR));
	}
	SDL_Color hc = udp->highC;
	SDL_SetRenderDrawColor(re, hc.r, hc.g, hc.b, hc.a);
	SDL_RenderFillRect(re, &(udp->highVolumeR));
	SDL_Color lc = udp->lowC;
	SDL_SetRenderDrawColor(re, lc.r, lc.g, lc.b, lc.a);
	SDL_RenderFillRect(re, &(udp->lowVolumeR));
	if (mp->count("H")) {
		SDL_RenderTexture(re, (*mp)["H"], NULL, &(udp->highVolumeR));
	}
	if (mp->count("L")) {
		SDL_RenderTexture(re, (*mp)["L"], NULL, &(udp->lowVolumeR));
	}
	SDL_RenderTexture(re, udp->volumeText, NULL, &(udp->vr));
};

stateScript drawSayHello = [](void *ud, void *od) -> void {
	HelloUDS *udp = (HelloUDS *)ud;
	udp->backText = "Back";
	udp->backBC = {255, 255, 255, 255};
	udp->highC = {255, 0, 0, 255};
	udp->lowC = {255, 0, 0, 255};
};

stateScript helloSilence = [](void *ud, void *od) -> void { return; };

stateScript drawBackOn = [](void *ud, void *od) -> void {
	HelloUDS *udp = (HelloUDS *)ud;
	udp->backText = "BackW";
	udp->backBC = {255, 0, 0, 255};
	udp->highC = {255, 0, 0, 255};
	udp->lowC = {255, 0, 0, 255};
};

stateScript drawBackDown = [](void *ud, void *od) -> void {
	HelloUDS *udp = (HelloUDS *)ud;
	udp->backText = "BackW";
	udp->backBC = {0xa5, 0, 0, 255};
	udp->highC = {255, 0, 0, 255};
	udp->lowC = {255, 0, 0, 255};
};

stateScript drawHighOn = [](void *ud, void *od) -> void {
	HelloUDS *udp = (HelloUDS *)ud;
	udp->backText = "Back";
	udp->backBC = {255, 255, 255, 255};
	udp->highC = {0xa5, 0, 0, 255};
	udp->lowC = {255, 0, 0, 255};
};

stateScript drawLowOn = [](void *ud, void *od) -> void {
	HelloUDS *udp = (HelloUDS *)ud;
	udp->backText = "Back";
	udp->backBC = {255, 255, 255, 255};
	udp->highC = {255, 0, 0, 255};
	udp->lowC = {0xa5, 0, 0, 255};
};

scriptSet ss2 = {helloSilence, drawSayHello, drawBackOn,
                 drawBackDown, drawHighOn,   drawLowOn};

stateHandler playMusic = [](void *ud, void *od) -> stateIndex {
	HelloUDS *udp = (HelloUDS *)ud;
	SDL_ResumeAudioDevice(udp->id);
	SDL_Log("Play!\n");
	return (stateIndex)HelloStateIndex::Played;
};
stateHandler hdIdle = [](void *ud, void *od) -> stateIndex {
	return (stateIndex)HelloStateIndex::Played;
};
stateHandler hdBackOn = [](void *ud, void *od) -> stateIndex {
	return (stateIndex)HelloStateIndex::BackOn;
};
stateHandler hdBackDown = [](void *ud, void *od) -> stateIndex {
	return (stateIndex)HelloStateIndex::BackDown;
};
stateHandler hdBackUp = [](void *ud, void *od) -> stateIndex {
	HelloUDS *udp = (HelloUDS *)ud;
	SDL_PauseAudioDevice(udp->id);
	treeUDS *odp = (treeUDS *)od;
	odp->current = 0;
	udp->mouseP = {-1, -1};
	return (stateIndex)HelloStateIndex::Hello;
};
stateHandler hdHighOn = [](void *ud, void *od) -> stateIndex {
	return (stateIndex)HelloStateIndex::HighOn;
};
stateHandler hdHighDown = [](void *ud, void *od) -> stateIndex {
	HelloUDS *udp = (HelloUDS *)ud;
	((HelloUDS *)ud)->Volume = ((HelloUDS *)ud)->Volume + 10 <= 100
	                               ? ((HelloUDS *)ud)->Volume + 10
	                               : 100;
	SDL_SetAudioDeviceGain(udp->id, (((HelloUDS *)ud)->Volume) / 100.0);
	SDL_DestroyTexture(udp->volumeText);
	udp->volumeText = getFontTex(udp->rend, mainFontName, {0xff, 0, 0, 0xff},
	                             std::to_string(udp->Volume) + "%", 40);
	return (stateIndex)HelloStateIndex::Played;
};
stateHandler hdLowOn = [](void *ud, void *od) -> stateIndex {
	return (stateIndex)HelloStateIndex::LowOn;
};
stateHandler hdLowDown = [](void *ud, void *od) -> stateIndex {
	HelloUDS *udp = (HelloUDS *)ud;
	((HelloUDS *)ud)->Volume =
	    ((HelloUDS *)ud)->Volume - 10 >= 0 ? ((HelloUDS *)ud)->Volume - 10 : 0;
	SDL_SetAudioDeviceGain(udp->id, (((HelloUDS *)ud)->Volume) / 100.0);
	SDL_DestroyTexture(udp->volumeText);
	udp->volumeText = getFontTex(udp->rend, mainFontName, {0xff, 0, 0, 0xff},
	                             std::to_string(udp->Volume) + "%", 40);
	return (stateIndex)HelloStateIndex::Played;
};

stateHandleTable table3 = {
    {playMusic, playMusic, playMusic, playMusic, playMusic, playMusic},
    {nullptr, hdBackOn, hdHighOn, hdLowOn},
    {hdIdle, nullptr, nullptr, nullptr, hdBackDown},
    {hdIdle, nullptr, nullptr, nullptr, nullptr, hdBackUp},
    {hdIdle, nullptr, nullptr, nullptr, hdHighDown},
    {hdIdle, nullptr, nullptr, nullptr, hdLowDown}};

// resetFunc MainRS = [](void *ud) -> void { ((mainUDS *)ud)->mouseP = {-1,
// -1};
// };

// resetFunc HelloRS = [](void *ud) -> void {
// 	((HelloUDS *)ud)->mouseP = {-1, -1};
// };

dst_script ds1 = [](void *ud) {
	mainUDS *udp = (mainUDS *)ud;
	destroyTexMap(udp->tm1p);
};

dst_script ds2 = [](void *ud) {
	HelloUDS *udp = (HelloUDS *)ud;
	destroyTexMap(udp->tm2p);
	if (udp->volumeText) {
		SDL_DestroyTexture(udp->volumeText);
	}
	SDL_CloseAudioDevice(udp->id);
};

stateScene *SceneController = nullptr;
getEvent treeEvent = [](void *ud, void *od) -> eventIndex {
	treeUDS *udp = (treeUDS *)ud;
	(udp->sv)[udp->current]->event();
	return 0;
};

stateScript sceneShow = [](void *ud, void *od) {
	treeUDS *udp = (treeUDS *)ud;
	(udp->sv)[udp->current]->run();
};

stateScript treeIdle = [](void *ud, void *od) { return; };

scriptSet treeSS = {treeIdle};

stateHandler treeHandlerIdle = [](void *ud, void *od) -> stateIndex {
	return 0;
};

stateHandleTable treeTable = {{treeHandlerIdle}};

void initInstance() {
	SDL_Color c1;
	SDL_Color c2;
	std::string t1;
	SDL_Texture *f1;
	SDL_Texture *f2;
	c1 = {0, 0, 0, 255};
	c2 = {255, 255, 255, 255};
	t1 = "      按这里看惊喜（      ";
	f1 = getFontTex(gui_renderer, bigFontName, c1, t1, 40);
	f2 = getFontTex(gui_renderer, bigFontName, c2, t1, 40);
	SDL_Color c3 = {255, 0, 0, 255};
	Point p = {100, 600};
	mainUD.rect = genRect(p, 700, t1.size(), TextRatioC);
	HelloUD.backButtonR = genRect({100, 100}, 200, 4);
	HelloUD.lowVolumeR = genRect({WinWeight - 200, WinHeight - 200}, 100, 1, 1);
	HelloUD.highVolumeR = genRect({WinWeight - 200, 100}, 100, 1, 1);
	HelloUD.vr = genRectCenter({WinWeight / 2, 100}, 100, 3);
	std::string th = "大";
	std::string tl = "小";
	SDL_Texture *TH = getFontTex(gui_renderer, mainFontName, c2, th, 20);
	SDL_Texture *TL = getFontTex(gui_renderer, mainFontName, c2, tl, 20);

	mainScene = new stateScene(mainMainScript, mouseOnButton, &mainUD, &treeUD,
	                           (stateIndex)mainState::UnOn);
	mainScene->setStateScripts(ss1);

	std::string t3 = "2333333333😄";
	std::string t3_d = "这是个笑脸但是显示不了";
	SDL_Texture *f3 = getFontTex(gui_renderer, bigFontName, c3, t3, 100);
	SDL_Texture *f3_d = getFontTex(gui_renderer, mainFontName, c3, t3_d, 100);
	SDL_Color cr = {0xff, 0, 0, 0xff};
	SDL_Texture *f_back = getFontTex(gui_renderer, bigFontName, cr, "BACK", 30);
	SDL_Texture *f_back_w =
	    getFontTex(gui_renderer, bigFontName, c2, "BACK", 30);

	mainUD.rend = gui_renderer;
	mainUD.eve = gui_event;
	appendTex(&tm1, f1, "MainButton");
	appendTex(&tm1, f2, "MainWhite");
	mainUD.tm1p = &tm1;

	HelloUD.rend = gui_renderer;
	HelloUD.eve = gui_event;

	HelloScene = new stateScene(drawHello, HelloEvent, &HelloUD, &treeUD,
	                            (stateIndex)HelloStateIndex::Hello);
	// if (!f1 || !f2) {
	// 	SDL_Log("Fuck");
	// }
	// mainScene->appendTex(f1, "MainButton");
	// mainScene->appendTex(f2, "MainWhite");
	// mainScene->appendDrawScript(drawMainOn, (stateIndex)mainState::On);
	// mainScene->appendDrawScript(drawMainDown, (stateIndex)mainState::Down);
	// mainScene->appendDrawScript(drawMainBan, (stateIndex)mainState::Ban);

	// mainScene->addUserData(&mainUD);
	// HelloScene->addUserData(&HelloUD);

	HelloUD.tm2p = &tm2;
	appendTex(&tm2, f3, "aBigHello");
	appendTex(&tm2, f3_d, "下面的小字");
	appendTex(&tm2, f_back, "Back");
	appendTex(&tm2, f_back_w, "BackW");
	appendTex(&tm2, TH, "H");
	appendTex(&tm2, TL, "L");
	// HelloScene->appendTex(f3, "aBigHello");
	// HelloScene->appendTex(f3_d, "下面的小字");
	// HelloScene->appendTex(f_back, "Back");
	// HelloScene->appendTex(f_back_w, "BackW");
	// HelloScene->appendTex(TH, "H");
	// HelloScene->appendTex(TL, "L");
	HelloScene->setStateScripts(ss2);
	HelloUD.volumeText =
	    getFontTex(gui_renderer, mainFontName, {0xff, 0, 0, 0xff},
	               std::to_string(HelloUD.Volume) + "%", 40);
	// HelloScene->appendDrawScript(drawSayHello,
	//                              (stateIndex)HelloStateIndex::Played);
	// HelloScene->appendDrawScript(drawBackOn,
	//                              (stateIndex)HelloStateIndex::BackOn);
	// HelloScene->appendDrawScript(drawBackDown,
	//                              (stateIndex)HelloStateIndex::BackDown);
	// HelloScene->appendDrawScript(drawHighOn,
	//                              (stateIndex)HelloStateIndex::HighOn);
	// HelloScene->appendDrawScript(drawLowOn,
	// (stateIndex)HelloStateIndex::LowOn);

	loadWAV("hello.wav", stream, &wav_data, &wav_len);
	addDevice(HelloUD.id);
	// HelloScene->addDevice(
	//     SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL));
	putStream(HelloUD.id, stream, &wav_data, &wav_len);

	mainScene->setTable(table1);
	mainScene->setDestructor(ds1);
	HelloScene->setTable(table3);
	HelloScene->setDestructor(ds2);

	// mainTree->setTable(table2);
	// mainTree->appendState(HelloScene, (stateSceneIndex)SceneIndexes::Hello);

	// mainScene->setReset(MainRS);
	// HelloScene->setReset(HelloRS);
	treeUD.sv = {mainScene, HelloScene};
	SceneController = new stateScene(sceneShow, treeEvent, &treeUD, nullptr, 0);
	SceneController->setTable(treeTable);
	SceneController->setStateScripts(treeSS);
}

void destroyInstance() {
	delete mainScene;
	delete HelloScene;
	delete SceneController;
	SDL_UnbindAudioStream(stream);
	SDL_DestroyAudioStream(stream);
}
}; // namespace instance
