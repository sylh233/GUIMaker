#ifndef INSTANCE
#define INSTANCE

#include <gui.h>

namespace instance {
extern gui::stateScene *mainScene;
extern gui::stateTree *mainTree;
void externVariable();
void initInstance();
void destroyInstance();
}; // namespace instance

#endif
