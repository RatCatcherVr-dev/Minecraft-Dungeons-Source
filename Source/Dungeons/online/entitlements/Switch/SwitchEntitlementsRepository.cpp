#include "Dungeons.h"
#include "SwitchEntitlementsRepository.h"
#include <nn/aoc.h>

void USwitchEntitlementsRepository::RequestEntitlements() {
	RemoveAllEntitlements();
	//D11.PS - Add the base game entitlement
	AddEntitlementsForPlatform("game_dungeons", GetSource());

	const int MaxListCount = 256;
	nn::aoc::AddOnContentIndex ContentIndex[256];
	int NumAddons = nn::aoc::ListAddOnContent(ContentIndex, 0, MaxListCount);

	for (int i = 0; i < NumAddons; ++i)
	{
		TArray< FStringFormatArg > args;
		args.Add(FStringFormatArg(static_cast<int>(ContentIndex[i])));
		FString PackageName = FString::Format(TEXT("{0}"), args);
		AddEntitlementsForPlatform(PackageName, GetSource());
	}

	OnEntitlementsProvided.Broadcast(GetEntitlements());
}

void USwitchEntitlementsRepository::ConfigureForPlatform() {
	ensure(GetSource() == EEntitlementsSource::Nintendo);
}
