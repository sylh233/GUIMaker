#include <gui_timer.h>

namespace gui {
Timer::Timer(long long p, bool oneshot) {
	point = std::chrono::steady_clock::now();
	period = std::chrono::milliseconds(p);
	this->oneshot = oneshot;
}

bool Timer::elapse() {
	auto nowp = std::chrono::steady_clock::now();
	auto dura = nowp - point;
	if (dura >= period) {
		if (!oneshot) {
			point = nowp;
		}
		return true;
	} else {
		return false;
	}
}
} // namespace gui
