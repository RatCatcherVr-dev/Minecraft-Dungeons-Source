// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "ConnectionStatusToasterWidget.h"
#include "online/sessions/OnlineUtil.h"
#include "online/seasons/LiveOps.h"
#include "LiveOpsClient.h"
#include "TextBlock.h"
#include "ui/menu/ConnectionIconWidget.h"
#include "ConnectionStatus.h"
#include "TextToSpeechBPLibrary.h"

namespace internal {
	TMap<EMinecraftAPIConnectionStatus, FText> ToasterTextMap = {
		{ EMinecraftAPIConnectionStatus::Connected, LocTableFromFile::Get("ConnectionStatusLabels.csv", "connection_toaster_returned") },
		{ EMinecraftAPIConnectionStatus::NoConnection, LocTableFromFile::Get("ConnectionStatusLabels.csv", "connection_toaster_failed") },
		{ EMinecraftAPIConnectionStatus::GameClientTooOld, LocTableFromFile::Get("ConnectionStatusLabels.csv", "connection_toaster_versionfail") },
	};
}

void UConnectionStatusToasterWidget::NativePreConstruct() {
	Super::NativePreConstruct();
	ConnectionStatusIcon->Update(EMinecraftAPIConnectionStatus::NoConnection);
	ConnectionStatusNotification->SetText(GetStatusText(EMinecraftAPIConnectionStatus::NoConnection));
}

void UConnectionStatusToasterWidget::NativeConstruct() {
	Super::NativeConstruct();
	GameInstance = GetGameInstance();
	if (GameInstance) {
		auto liveOps = online::getLiveOps(GameInstance);
		StatusChangeHandle = liveOps->OnConnectionStatusChanged.AddUObject(this, &UConnectionStatusToasterWidget::OnConnectionStatusChanged);
	}
}

void UConnectionStatusToasterWidget::NativeDestruct() {
	Super::NativeDestruct();
	if (GameInstance) {
		auto liveOps = online::getLiveOps(GameInstance);
		liveOps->OnConnectionStatusChanged.Remove(StatusChangeHandle);
	}
}

void UConnectionStatusToasterWidget::OnConnectionStatusChanged(EMinecraftAPIConnectionStatus status) {
	auto showPopup = [&]() {
		if (status == EMinecraftAPIConnectionStatus::Connected) {
			return bConnectionWasLost;
		}
		if ((status == EMinecraftAPIConnectionStatus::NoConnection) || (status == EMinecraftAPIConnectionStatus::GameClientTooOld)) {
			return true;
		}
		return false;
	}();

	if (showPopup) {
		ConnectionStatusIcon->Update(status);
		const auto statusText = GetStatusText(status);
		ConnectionStatusNotification->SetText(statusText);
		SetVisibility(ESlateVisibility::HitTestInvisible);
		PlayAnimation(PopIn);
		UTextToSpeechBPLibrary::PlayTextToSpeech(statusText, true);
	}
	bConnectionWasLost = status == EMinecraftAPIConnectionStatus::NoConnection;
}

FText UConnectionStatusToasterWidget::GetStatusText(EMinecraftAPIConnectionStatus status) {
	if (internal::ToasterTextMap.Contains(status)) {
		return internal::ToasterTextMap[status];
	}
	return FText::GetEmpty();
}
