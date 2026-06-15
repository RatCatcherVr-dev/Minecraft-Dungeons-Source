#include "Dungeons.h"
#include "NetworkInfo.h"

#include "IDungeonsAuth.h"
#include "LiveOpsClient.h"
#include "PlayfabPlayer.h"
#include "Engine/NetConnection.h"
#include "online/crossplay/Identity.h"
#include "online/seasons/LiveOps.h"

static TAutoConsoleVariable<int32> CVarDebugNetworkStatistics(
	TEXT("Dungeons.Network.Statistics"),
	0,
	TEXT("Enables statistics for network traffic.\n")
	TEXT("v == 1: on.\n")
	TEXT("v != 1: off.\n"),
	ECVF_Default);

NetworkInfo::Table::Table(AHUD* hud, const FVector2D& offset, const FString& title)
	: ColumnCategory(hud, offset)
	, ColumnValue(hud, { offset.X + 100, offset.Y }) {
	ColumnCategory.Draw(title);
	ColumnValue.NewLine();
	ColumnCategory.NewLine();
	ColumnValue.NewLine();
}

void NetworkInfo::Table::Print(const FString& category, float value, FColor Color, const FString& unit)
{
	ColumnCategory.Draw(*category, Color);
	ColumnValue.Draw(FString::Printf(TEXT("%.02f %s"), value, *unit), Color);
}

void NetworkInfo::Table::Print(const FString& category, int value, FColor Color, const FString& unit)
{
	ColumnCategory.Draw(*category, Color);
	ColumnValue.Draw(FString::Printf(TEXT("%d %s"), value, *unit), Color);
}

void NetworkInfo::Table::Print(const FString& category, const FString& value) {
	ColumnCategory.Draw(*category, FColor::White);
	ColumnValue.Draw(*value, FColor::White);
}

UNetDriver* NetworkInfo::GetNetDriver(AHUD* hud) {
	if(hud && hud->GetWorld() && hud->GetWorld()->GetNetDriver()) {
		return hud->GetWorld()->GetNetDriver();
	}
	return nullptr;
}

void NetworkInfo::Tick(AHUD* hud) {
	if (UNetDriver* netdriver = GetNetDriver(hud)) {
		if (netdriver->ServerConnection) {
			ClientTick(*netdriver);
		}

		if (netdriver->ClientConnections.Num() > 0) {
			ServerTick(*netdriver);
		}
	}
}

void UpdateStats(PeerStatistics& PeerStats, UNetConnection* connection)
{
	PeerStats.XUID = connection->GetInternetAddr()->ToString(false);

	PeerStats.m_OutBandwidth.Update(connection->OutBytesPerSecond);
	PeerStats.m_OutPacket.Update(connection->OutPacketsPerSecond);
	PeerStats.m_OutPacketLoss.Update(connection->OutPacketsLost);
	PeerStats.m_OutLatency.Update(connection->AvgLag * 1000);
	PeerStats.m_InBandwidth.Update(connection->InBytesPerSecond);
	PeerStats.m_InPacket.Update(connection->InPacketsPerSecond);
	PeerStats.m_InPacketLoss.Update(connection->InPacketsLost / connection->StatUpdateTime);
	PeerStats.m_InLatency.Update(connection->AvgLag * 1000);
}

void NetworkInfo::ClientTick(UNetDriver& netdriver) {

	if (m_Stats.Num() > 1)
	{
		//When client, the m_stats should only hold one element (this might occur when going from host->client)
		m_Stats.Empty();
	}

	if (UNetConnection* connection = netdriver.ServerConnection) {
		auto& ConnStat = m_Stats.FindOrAdd(connection->GetInternetAddr()->ToString(false));
		UpdateStats(ConnStat, connection);
	}
}

void NetworkInfo::ServerTick(UNetDriver& netdriver) {

	if (m_Stats.Num() != netdriver.ClientConnections.Num())
	{
		m_Stats.Empty();
	}
	
	for (UNetConnection* connection : netdriver.ClientConnections) {
		auto& ConnStat = m_Stats.FindOrAdd(connection->GetInternetAddr()->ToString(false));
		UpdateStats(ConnStat, connection);
	}
}

void NetworkInfo::Draw(AHUD* hud) {
	if (CVarDebugNetworkStatistics.GetValueOnGameThread() != 1)
		return;
	{
		NetworkInfo::Table playfab(hud, { 200, 50}, "Playfab");
		PlayFabPlayer* PfPlayer = nullptr;
		PfPlayer = PlayfabServices::GetPlayfabPlayer();
		if (PfPlayer) {
			playfab.Print(TEXT("Signed in"), (!PfPlayer->GetSessionTicket().IsEmpty() ? *FString("true") : *FString("false")));
		} else {
			playfab.Print(TEXT("Signed in"), *FString("false"));
		}
		NetworkInfo::Table minecraftAPI(hud, { 200, 100}, "MinecraftAPI");
		minecraftAPI.Print(TEXT("Signed in"), (IDungeonsAuth::IsAvailable() && IDungeonsAuth::Get().Auth() && IDungeonsAuth::Get().Auth()->GetClient() ? *FString("true") : *FString("false")));
		if (auto world = hud->GetWorld()) {
			switch (online::getLiveOps(world->GetGameInstance())->GetConnectionStatus()) {
				case EMinecraftAPIConnectionStatus::Connected:
					minecraftAPI.Print(TEXT("Status"), TEXT("Connected"));
					break;
				case EMinecraftAPIConnectionStatus::TimingOut:
					minecraftAPI.Print(TEXT("Status"), TEXT("TimingOut"));
					break;
				case EMinecraftAPIConnectionStatus::GameClientTooOld:
					minecraftAPI.Print(TEXT("Status"), TEXT("GameClientTooOld"));
					break;
				case EMinecraftAPIConnectionStatus::NoConnection:
				default:
					minecraftAPI.Print(TEXT("Status"), TEXT("NoConnection"));
					break;
			}
		}
	}

	int i = 0;
	for (const auto& PeerStat : m_Stats)
	{
		const auto& stat = PeerStat.Value;
		FString XUID = "Peer: " + stat.XUID;

		float InitialIndent = i == 0 ? 20 : 0; //Add 20 pixels x-wise on first peer

		NetworkInfo::Table peer(hud, { 100, InitialIndent + 150 * (float)i}, XUID);
		peer.Print("Up Latency", stat.m_OutLatency.Value, Color(stat.m_OutLatency.Value, 40, 70), "ms");
		peer.Print("Up Bandwidth", stat.m_OutBandwidth.Value / 1024.0f, Color(stat.m_OutBandwidth.Value / 1024.0f, 10.0f, 20.0f), "KB/s");
		peer.Print("Up Traffic", stat.m_OutPacket.Value, Color(stat.m_OutPacket.Value, 60, 120), "pkt/s");
		peer.Print("Down Latency", stat.m_InLatency.Value, Color(stat.m_InLatency.Value, 40, 70), "ms");
		peer.Print("Down Bandwidth"
			, stat.m_InBandwidth.Value / 1024.0f
			, Color(stat.m_InBandwidth.Value / 1024.0f, 10.0f, 20.0f)
			, "KB/s");
		peer.Print("Down Traffic", stat.m_InPacket.Value, Color(stat.m_InPacket.Value, 60, 120), "pkt/s");
	
		i++;
	}
}
