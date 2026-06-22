#ifndef INSTANCE
#define INSTANCE

#include <gui.h>

namespace instance {
extern gui::stateScene *mainScene;
extern gui::stateScene *HelloScene;
extern gui::stateScene *SceneController;
void externVariable();
void initInstance();
void destroyInstance();
}; // namespace instance

#endif
