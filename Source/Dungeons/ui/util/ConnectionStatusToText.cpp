#include "CoreMinimal.h"
#include "ConnectionStatusToText.h"
#include "ConnectionStatus.h"
#include "locale/LocTableFromFile.h"

namespace connection { namespace status {

FText ConnectionStatusToTextLabel(EMinecraftAPIConnectionStatus status) {
	switch (status) {
	case EMinecraftAPIConnectionStatus::Connected:
		return LocTableFromFile::Get("ConnectionStatusLabels.csv", "connection_connected");
	case EMinecraftAPIConnectionStatus::TimingOut:
		return LocTableFromFile::Get("ConnectionStatusLabels.csv", "connection_timingout");
	default:
		return LocTableFromFile::Get("ConnectionStatusLabels.csv", "connection_noconnection");
	}
}

}}
