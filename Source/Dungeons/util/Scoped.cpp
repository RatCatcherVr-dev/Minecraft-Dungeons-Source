#include "Dungeons.h"
#include "Scoped.h"
#include <LogMacros.h>

namespace scoped {

Scoped::Scoped(std::function<void()> onExit)
	: _onExit(std::move(onExit)) {
}

Scoped::~Scoped() {
	_onExit();
}

Scoped destroy(AActor* actor) {
	return Scoped([actor]{actor->Destroy();});
}

Scoped measure(FString title, float* durationSeconds /* = nullptr*/) {
	auto st = FPlatformTime::Seconds();
	return Scoped([st, durationSeconds, title = std::move(title)] {
		auto duration = FPlatformTime::Seconds() - st;
		if (durationSeconds) {
			*durationSeconds = duration;
		}
		UE_LOG(LogTemp, Display, TEXT("Measure <%s>: %f"), *title, duration);
	});
}

ScopedEnterExit::ScopedEnterExit(const std::function<void()>& onEnter, std::function<void()> onExit)
	: scoped::Scoped(std::move(onExit))
{
	onEnter();
}

}
