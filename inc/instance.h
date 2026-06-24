#ifndef INSTANCE
#define INSTANCE

#include <gui.h>
#include <gui_menu.h>

namespace instance {
extern gui::stateScene *mainScene;
extern gui::stateScene *HelloScene;
extern gui::stateScene *SceneController;
extern gui::Button b1;
extern gui::Button b2;
void externVariable();
void initInstance();
void destroyInstance();
}; // namespace instance

#endif
