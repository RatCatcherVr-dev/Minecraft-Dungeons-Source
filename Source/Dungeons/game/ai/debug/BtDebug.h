#pragma once

#include "game/ai/bt/BtTypes.h"
#include "CommonTypes.h"
#include <LogVerbosity.h>

class UBtSequence;

namespace bt { namespace debug {

Action print(const FString&, ELogVerbosity::Type level = ELogVerbosity::Log);
Action print(const Provider<FString>&, ELogVerbosity::Type level = ELogVerbosity::Log);

Action above(const FString& text, int lineIndex = 0);

Unique<UBtSequence> printStartTickStop(const FString& title, ELogVerbosity::Type level = ELogVerbosity::Log);


}}
