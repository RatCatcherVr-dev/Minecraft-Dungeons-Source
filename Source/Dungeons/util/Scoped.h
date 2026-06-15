#pragma once
#include <functional>

namespace scoped {

class Scoped
{
public:
	explicit Scoped(std::function<void()> onExit);
	Scoped(const Scoped&) = delete;
	Scoped& operator=(const Scoped&) = delete;
	Scoped(Scoped&&) = default;
	~Scoped();

	explicit operator bool() const { return true; }
private:
	std::function<void()> _onExit;
};

Scoped destroy(class AActor*);
Scoped measure(FString title, float* durationSeconds = nullptr);

struct ScopedEnterExit : private scoped::Scoped {
	ScopedEnterExit(const std::function<void()>& onEnter, std::function<void()> onExit);
};

}
