#include "ConnectionIconWidget.h"

#include "LiveOpsClient.h"
#include "DungeonsWidgets/DungeonsSoftImage.h"
#include "ConnectionStatus.h"

void UConnectionIconWidget::NativePreConstruct() {
	Super::NativePreConstruct();
	Update(EMinecraftAPIConnectionStatus::NoConnection);
}

void UConnectionIconWidget::Update(EMinecraftAPIConnectionStatus status) {
	if (StatusIconMap.Contains(status)) {
		StatusIcon->SetBrushFromTexture(StatusIconMap[status]);
	}
	else if (StatusIconMap.Contains(EMinecraftAPIConnectionStatus::NoConnection)) {
		StatusIcon->SetBrushFromTexture(StatusIconMap[EMinecraftAPIConnectionStatus::NoConnection]);
	}
}
