#include "Dungeons.h"
#include "BtDebug.h"
#include "CommonTypes.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/component/DebugComponent.h"

namespace bt { namespace debug {

Action print(const FString& text, ELogVerbosity::Type level /*= ELogVerbosity::Log*/) {
	return print(bt::value(text), level);
}

Action print(const Provider<FString>& provider, ELogVerbosity::Type level /*= ELogVerbosity::Log*/) {
	// Due to UE being idiotic and macro-stitching UE_LOG calls together, we need to do this.
	// @todo: extract logging into a real function once and for all?

	if (level == ELogVerbosity::Fatal) {
		return [=](bt::StateRef state) { UE_LOG(LogTemp, Fatal, TEXT("%d : %s"), state.tick, *provider(state)); };
	}
	if (level == ELogVerbosity::Error) {
		return [=](bt::StateRef state) { UE_LOG(LogTemp, Error, TEXT("%d : %s"), state.tick, *provider(state)); };
	}
	if (level == ELogVerbosity::Warning) {
		return [=](bt::StateRef state) { UE_LOG(LogTemp, Warning, TEXT("%d : %s"), state.tick, *provider(state)); };
	}
	if (level == ELogVerbosity::Log) {
		return [=](bt::StateRef state) { UE_LOG(LogTemp, Log, TEXT("%d : %s"), state.tick, *provider(state)); };
	}
	return [=](bt::StateRef state) { UE_LOG(LogTemp, Verbose, TEXT("%d : %s"), state.tick, *provider(state)); };
}

Action above(const FString& text, int lineIndex/* = 0*/) {
	return [=](bt::StateRef state) {
		const auto debugComponent = state.owner->FindComponentByClass<UDebugComponent>();
		debugComponent->SetText(text, lineIndex);
	};
}

Unique<UBtSequence> printStartTickStop(const FString& title, ELogVerbosity::Type level /*= ELogVerbosity::Log*/) {
	return sequence(
		onStart(print(FString("Start: " + title))),
		onTick (print(FString("Tick : " + title))),
		onStop (print(FString("Stop : " + title)))
	);
}

}}
