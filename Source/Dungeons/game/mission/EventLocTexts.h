#pragma once

#include "Dungeons.h"
#include "util/EnumUtil.h"
#include "game/mission/MissionDefs.h"
#include "StringTableCore.h"
#include <StringTableRegistry.h>
#include "game/actor/NotificationBroadcastActor.h"

namespace game { namespace objective {

#define LOCTEXT_NAMESPACE "Events"

static const FText EventTitle = LOCTEXT("event_title", "New Event");

static const TMap<EEventType, FText> EventMessages = {
	TPair<EEventType, FText>(EEventType::ArenaBattle, LOCTEXT("eventmsg_arena", "Arena Battle")),
	TPair<EEventType, FText>(EEventType::Boss, LOCTEXT("eventmsg_boss", "Boss Fight")),
	TPair<EEventType, FText>(EEventType::DoorOpened, LOCTEXT("eventmsg_door_open", "Door Opened"))
};

static const FText& GetEventTitle() {
	return EventTitle;
}

static const FText& GetEventMessage(EEventType eventType) {
	return EventMessages[eventType];
}



#undef LOCTEXT_NAMESPACE

}}