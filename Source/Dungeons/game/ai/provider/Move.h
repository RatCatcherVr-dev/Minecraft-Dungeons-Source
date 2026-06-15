#pragma once

#include "AITypes.h"
#include "game/ai/bt/BtTypes.h"
#include "Locators.h"
#include "Locations.h"
#include "Actors.h"

namespace bt { namespace move {

using Provider = bt::Provider<FAIMoveRequest>;

FAIMoveRequest defaultRequest();

Provider withSettings(const Provider&, const FAIMoveRequest& requestTemplateWithoutGoal);

Provider Actor(const actor::Provider&);
Provider Actor(const actor::Provider&, const FAIMoveRequest&);
Provider Location(const location::Provider&);
Provider Forward(const bt::Provider<float>&);
Provider RandomLocationAround(const locator::Provider&, float radius);
Provider LocationAwayFrom(const locator::Provider&, float distance = 500.0f, float radius = 200.0f);
Provider From(const locator::Provider& provider);
}}
