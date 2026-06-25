#ifndef GUI_TIMER_H
#define GUI_TIMER_H

#include <chrono>

namespace GUI {
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
} // namespace GUI

#endif
