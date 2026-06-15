#include "ConnectionDisplayBaseWidget.h"

#include "ConnectionIconWidget.h"
#include "LiveOpsClient.h"
#include "TextBlock.h"
#include "online/seasons/LiveOps.h"
#include "online/sessions/OnlineUtil.h"
#include "ui/util/ConnectionStatusToText.h"

void UConnectionDisplayBaseWidget::NativePreConstruct() {
	Super::NativePreConstruct();
	
	if (ConnectionDisplayLabel && ConnectionDisplayIcon) {
		SetState(EMinecraftAPIConnectionStatus::NoConnection);
	}
}

void UConnectionDisplayBaseWidget::NativeConstruct() {
	Super::NativeConstruct();

	check(ConnectionDisplayLabel);
	check(ConnectionDisplayIcon);

	if (const auto world = GetWorld()) {
		GameInstance = world->GetGameInstance();

		if (auto liveOps = online::getLiveOps(GameInstance)) {
			// Initial status
			SetState(liveOps->GetConnectionStatus());

			// Setup event listener
			OnStatusChangeHandle = liveOps->OnConnectionStatusChanged.AddLambda([&](EMinecraftAPIConnectionStatus status) {
				SetState(status);
			});
		}
	}
}

void UConnectionDisplayBaseWidget::BeginDestroy() {
	Super::BeginDestroy();

	if (GameInstance) {
		if (auto liveOps = online::getLiveOps(GameInstance)) {
			liveOps->OnConnectionStatusChanged.Remove(OnStatusChangeHandle);
		}
	}
}

FText UConnectionDisplayBaseWidget::GetTextToSpeech() const {
	return state.statusLabel;
}

void UConnectionDisplayBaseWidget::SetState(EMinecraftAPIConnectionStatus status) {
	state.statusLabel = connection::status::ConnectionStatusToTextLabel(status);
	state.status = status;
	UpdateWidgets();
	OnStateChanged(status);
}

void UConnectionDisplayBaseWidget::UpdateWidgets() {
	ConnectionDisplayLabel->SetText(state.statusLabel);
	ConnectionDisplayIcon->Update(state.status);
}
