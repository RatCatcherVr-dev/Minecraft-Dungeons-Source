#pragma once

#include "Dungeons.h"
#include "DungeonsDefsMinimal.h"


namespace util {
	namespace autodetect {

#define LOCTEXT_NAMESPACE "AutoDetect"

static const TMap<ELowSpecNotifyType, FText> NotifyMessages = {
	TPair<ELowSpecNotifyType, FText>(ELowSpecNotifyType::OnBatteryPower, LOCTEXT("OnAcMessage","Battery power supply detected, reduced graphics settings to conserve power. These options can be changed in Advanced Graphics settings.")),
	TPair<ELowSpecNotifyType, FText>(ELowSpecNotifyType::LowMemory, LOCTEXT("LowMemMessage", "Below recommended VRAM of {0}GB. You may experience poor performance.")),
	TPair<ELowSpecNotifyType, FText>(ELowSpecNotifyType::LowPerformance, LOCTEXT("LowPerfMessage", "Below recommended CPU or GPU performance detected. You may experience poor performance."))
};

static const FText& GetNotifyMessage(ELowSpecNotifyType type) {
	if (NotifyMessages.Contains(type)) {
		return NotifyMessages[type];
	}
	return FText::GetEmpty();
}

#undef LOCTEXT_NAMESPACE
}
}