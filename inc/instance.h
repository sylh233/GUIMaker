#ifndef INSTANCE
#define INSTANCE

#include <GUI.h>

namespace instance {
using namespace GUI;
	
extern GUI::stateMachine *mainScene;
extern GUI::stateMachine *HelloScene;
extern GUI::stateMachine *SceneController;
extern GUI::Button b1;
extern GUI::Button b2;
void externVariable();
void initInstance();
void destroyInstance();
}; // namespace instance

#endif
