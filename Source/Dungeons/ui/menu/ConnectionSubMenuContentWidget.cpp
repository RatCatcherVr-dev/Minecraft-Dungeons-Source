#include "ConnectionSubMenuContentWidget.h"

#include "ConnectionSubMenuDisplayRowWidget.h"
#include "ConnectionSubMenuTextRowWidget.h"
#include "IDungeonsAuth.h"
#include "LiveOpsClient.h"
#include "VerticalBox.h"
#include "online/seasons/LiveOps.h"
#include "online/sessions/OnlineUtil.h"

namespace internal {
	auto LocKey = [](const FString& name) { return LocTableFromFile::Get("ConnectionStatusLabels.csv", name); };

	FText DefaultSignedOutLabel() { return LocKey("service_status_signedout"); }

	FText GetPlayfabAuthenticationStatusAsLabel(bool EditorPreview) {
		if (!EditorPreview && PlayfabServices::GetPlayfabPlayer()) {
			return LocKey("service_status_signedin");
		}

		return LocKey("service_status_signedout");
	}

	FText GetMinecraftAPIAuthenticationStatusAsLabel(bool EditorPreview) {
		if (!EditorPreview && IDungeonsAuth::IsAvailable() && IDungeonsAuth::Get().Auth() && IDungeonsAuth::Get().Auth()->GetClient()) {
			return LocKey("service_status_signedin");
		}

		return LocKey("service_status_signedout");
	}

	FText GetLiveOpsVersionAsLabel(bool EditorPreview, UGameInstance* GameInstance) {
		if (!EditorPreview) {
			if (const auto liveOps = online::getLiveOps(GameInstance)) {
				if (liveOps->GetConnectionStatus() == EMinecraftAPIConnectionStatus::GameClientTooOld) {
					return LocKey("service_status_liveops_version_outdated");
				}
			}
		}

		return LocKey("service_status_liveops_version_ok");
	}

	FText GetMinecraftAPIConnectionStatusAsLabel(bool EditorPreview, UGameInstance* GameInstance) {
		if (!EditorPreview) {
			if (const auto liveOps = online::getLiveOps(GameInstance)) {
				auto status = liveOps->GetConnectionStatus();
				if (status == EMinecraftAPIConnectionStatus::Connected || status == EMinecraftAPIConnectionStatus::GameClientTooOld) {
					return LocKey("connection_connected");
				}
			}
		}

		return LocKey("connection_noconnection");
	}
}

void UConnectionSubMenuContentWidget::NativePreConstruct() {
	Super::NativePreConstruct();

	if (Summary && ConnectionStatuses) {
		SetState(true);
	}
}

void UConnectionSubMenuContentWidget::NativeConstruct() {
	Super::NativeConstruct();

	check(Summary);
	check(ConnectionStatuses);

	if (const auto world = GetWorld()) {
		GameInstance = world->GetGameInstance();

		if (auto liveOps = online::getLiveOps(GameInstance)) {
			OnStatusChangeHandle = liveOps->OnConnectionStatusChanged.AddLambda([&](EMinecraftAPIConnectionStatus) {
				SetState(false);
			});
		}
	}

	SetState(false);
}

void UConnectionSubMenuContentWidget::BeginDestroy() {
	Super::BeginDestroy();

	if (GameInstance) {
		if (auto liveOps = online::getLiveOps(GameInstance)) {
			liveOps->OnConnectionStatusChanged.Remove(OnStatusChangeHandle);
		}
	}
}

FText UConnectionSubMenuContentWidget::GetOpenedText() const {
	FText connectionStatusesTTS;
	for (const auto& connection : state.connectionStatuses) {
		connectionStatusesTTS = FText::Format(FTextFormat::FromString("{0}{1} {2}. "), connectionStatusesTTS, connection.name, connection.status);
	}

	return FText::Format(FTextFormat::FromString("{0}. {1}"), Summary->GetTextToSpeech(), connectionStatusesTTS);
}

void UConnectionSubMenuContentWidget::SetState(bool EditorPreview) {
	state.connectionStatuses = {
		{ internal::LocKey("service_name_playfab"), internal::GetPlayfabAuthenticationStatusAsLabel(EditorPreview) },
		{ internal::LocKey("service_name_minecraftapi"), internal::GetMinecraftAPIAuthenticationStatusAsLabel(EditorPreview) },
		{ internal::LocKey("service_name_minecraftapi_connection"), internal::GetMinecraftAPIConnectionStatusAsLabel(EditorPreview, GameInstance) },
		{ internal::LocKey("service_name_liveops"), internal::GetLiveOpsVersionAsLabel(EditorPreview, GameInstance) },
	};

	UpdateWidgets();
}

void UConnectionSubMenuContentWidget::UpdateWidgets() {
	ConnectionStatuses->ClearChildren();
	for (auto& connection : state.connectionStatuses) {
		auto newRow = CreateWidget<UConnectionSubMenuTextRowWidget>(ConnectionStatuses, ConnectionSubMenuTextRowClass);
		newRow->Update(connection.name, connection.status);
		ConnectionStatuses->AddChildToVerticalBox(newRow);
	}
}
