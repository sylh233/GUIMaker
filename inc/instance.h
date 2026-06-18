#ifndef INSTANCE
#define INSTANCE

#include <gui.h>

namespace instance {
extern gui::stateScene *mainScene;
extern gui::stateTree *mainTree;
void initInstance();
void destroyInstance();
}; // namespace instance

#endif
