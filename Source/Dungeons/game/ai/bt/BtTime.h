#pragma once

#include "BtEvalState.h"
#include "util/NewType.h"
#include <chrono>

//#include "common_header.h"

namespace bt {
struct TimeStamp;

using Ticks = NewType<int>;

struct Seconds : public NewType<float> {
	using NewType<float>::NewType;

	template <typename T>
	Seconds(const std::chrono::duration<T>& duration) : NewType(duration.count()) {}
};
	
namespace internal {

template <int TemplateTicksPerSecond>
struct Duration {
	enum {
		TicksPerSecond = TemplateTicksPerSecond
	};

	template <typename T>
	Duration(const std::chrono::duration<T>& duration) : Duration(bt::Seconds(duration.count())) {}
	Duration(Ticks ticks) : mTicks(ticks) {}
	Duration(Seconds seconds) : mTicks(static_cast<Ticks::Raw>(Math::ceil(TemplateTicksPerSecond * seconds))) {}
	Duration(float duration) : mTicks(static_cast<Ticks::Raw>(Math::ceil(TemplateTicksPerSecond * duration))) {}

	Duration operator+(Duration rhs) const { return Ticks(mTicks + rhs.mTicks); }
	Duration operator-(Duration rhs) const { return Ticks(mTicks - rhs.mTicks); }
	Duration operator*(float k) const { return Ticks(static_cast<int>(k * mTicks)); }
	Duration operator/(float k) const { return Ticks(static_cast<int>(mTicks / k)); }
	Duration& operator+=(Duration rhs) { mTicks = mTicks + rhs.mTicks; return *this; }
	Duration& operator-=(Duration rhs) { mTicks = mTicks - rhs.mTicks; return *this; }

	bool operator<(Duration rhs) const { return mTicks < rhs.mTicks; }
	bool operator<=(Duration rhs) const { return mTicks <= rhs.mTicks; }
	bool operator>(Duration rhs) const { return mTicks > rhs.mTicks; }
	bool operator>=(Duration rhs) const { return mTicks >= rhs.mTicks; }

	float Seconds() const { return mTicks * (1.0f / TemplateTicksPerSecond); }
	bool isZero() const { return mTicks == 0; }
private:
	friend struct ::bt::TimeStamp;
	Ticks::Raw mTicks;
};
}

using Duration = internal::Duration<30>;

struct TimeStamp {
	bool IsPassed(StateRef state) const { return state.tick >= mTicks; }
	bool IsPassed(StateRef state, const bt::Duration& minDuration) const { return state.tick - minDuration.mTicks >= mTicks; }
	bool IsPassedAndNonZero(StateRef state) const { return mTicks != 0 && state.tick >= mTicks; }
	bool IsPassedOrZero(StateRef state, const bt::Duration& minDuration) const { return mTicks == 0 || state.tick - minDuration.mTicks >= mTicks; }
	Ticks::Raw Ticks() const { return mTicks; }

	TimeStamp operator+(Duration duration) { return TimeStamp(mTicks + duration.mTicks); }
	TimeStamp operator-(Duration duration) { return TimeStamp(mTicks - duration.mTicks); }
	TimeStamp& operator+=(Duration duration) { mTicks += duration.mTicks; return *this; }
	TimeStamp& operator-=(Duration duration) { mTicks -= duration.mTicks; return *this; }

	explicit operator bool() const { return mTicks != 0; }

	static TimeStamp Now(StateRef state) { return TimeStamp(state.tick); }
 	static TimeStamp FromNow(StateRef state, Duration duration) { return TimeStamp(state.tick) + duration; }
	static TimeStamp SecondsFromNow(StateRef state, float seconds) { return TimeStamp(state.tick) + Seconds(seconds); }

	TimeStamp() : mTicks(0) {}
private:
	TimeStamp(Ticks::Raw ticks) : mTicks(ticks) {}

	Ticks::Raw mTicks;
};

}

bt::Duration operator""_ticks(unsigned long long int t);
using namespace std::literals::chrono_literals;
