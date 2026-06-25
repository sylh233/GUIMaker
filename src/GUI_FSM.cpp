#include <GUI_FSM.h>

namespace GUI {

stateMachine::stateMachine(stateScript mss, getEvent geter, void *ud, void *od,
                           stateIndex mi) { // 一定不要忘记给script初始化为零
	eventGeter = geter;
	mainStateIndex = mi;
	currentStateIndex = mi;
	main_script = mss;
	userdata = ud;
	outdata = od;
}

void stateMachine::setStateScripts(scriptSet sS) { script_set = std::move(sS); }

void stateMachine::setTable(stateHandleTable table) {
	stateTable = std::move(table);
}

void stateMachine::event() {
	eventIndex eve = eventGeter(userdata, outdata);
	if (currentStateIndex >= stateTable.size() ||
	    eve >= stateTable[currentStateIndex].size() ||
	    stateTable[currentStateIndex][eve] == nullptr) {
		return;
	}
	stateIndex nextState =
	    stateTable[currentStateIndex][eve](userdata, outdata);
	if (nextState >= script_set.size()) {
		currentStateIndex = mainStateIndex;
	} else {
		currentStateIndex = nextState;
	}
	return;
}

void stateMachine::run() {
	if (script_set.size() > 0 && currentStateIndex < script_set.size()) {
		script_set[currentStateIndex](userdata, outdata);
	}
	if (main_script != nullptr) {
		main_script(userdata, outdata);
	}
}
void stateMachine::setDestructor(dst_script ds) { destructor = ds; }

stateMachine::~stateMachine() {
	if (destructor != nullptr) {
		destructor(userdata);
	}
}

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

// 已弃用
/*
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

void stateTree::run(SDL_Renderer *rend) { tree[currentState]->show(rend); }
*/

}; // namespace GUI
