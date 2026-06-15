#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DebugList.h"
#include "Array.h"
#include "Engine/NetDriver.h"

template <typename T>
struct Statistic {

	void Update(T Current) {
		Value = Current;
	}
	T Value;
};

struct PeerStatistics
{
	FString XUID;

	Statistic<float> m_OutBandwidth;
	Statistic<int> m_OutPacket;
	Statistic<float> m_OutPacketLoss;
	Statistic<int> m_OutLatency;

	Statistic<float> m_InBandwidth;
	Statistic<int> m_InPacket;
	Statistic<float> m_InPacketLoss;
	Statistic<int> m_InLatency;
};

class DUNGEONS_API NetworkInfo {
	
	struct Table {
		Table(AHUD* hud, const FVector2D& offset, const FString& title);
		void Print(const FString& category, float value, FColor Color, const FString& unit);
		void Print(const FString& category, int value, FColor Color, const FString& unit);
		void Print(const FString& category, const FString& value);

		DebugList ColumnCategory;
		DebugList ColumnValue;
	};
public:

	NetworkInfo() = default;
	~NetworkInfo() = default;

	void Tick(AHUD* hud);
	void Draw(AHUD* hud);

private:
	UNetDriver * GetNetDriver(AHUD* hud);

	void ClientTick(UNetDriver& netdriver);
	void ServerTick(UNetDriver& netdriver);

	template<typename T>
	FColor Color(T v, T w, T e)
	{
		return ((v < w) ? FColor::White : ((v <= e) ? FColor::Orange : FColor::Red));
	}

	TMap<FString, PeerStatistics> m_Stats;
};