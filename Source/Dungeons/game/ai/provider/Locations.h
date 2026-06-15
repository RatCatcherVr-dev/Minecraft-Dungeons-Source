#pragma once

#include "game/ai/bt/BtEvalState.h"
#include "game/ai/bt/BtTypes.h"
#include "Vector.h"
#include "Locators.h"

class AActor;

namespace bt { 
namespace move { using Provider = bt::Provider<FAIMoveRequest>; }
namespace location {

using Provider = bt::Provider<FVector>;

struct AnchorGS { // @todo: figure out if we can do this cleaner (and keeping singleton-ness)
	FVector operator()(StateRef) const;
	void operator()(StateRef, FVector setAnchor) const;
};

Provider Actor(const bt::Provider<AActor*>&);

const Provider& StartPos();
const Provider& Self();
const AnchorGS& Anchor();

const Provider From( const locator::Provider& provider );
const Provider From( const move::Provider& provider );

}}
