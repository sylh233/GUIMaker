#ifndef GUI_TIMER
#define GUI_TIMER

#include <chrono>
#include <gui.h>

namespace gui {
class Timer {
  private:
	std::chrono::steady_clock::time_point point;
	std::chrono::milliseconds period;
	bool oneshot;
	// std::chrono::milliseconds elapsed = std::chrono::milliseconds(0);

  public:
	Timer(long long period, bool oneshot = false);
	bool elapse();
	~Timer() = default;
};
} // namespace gui

#endif
