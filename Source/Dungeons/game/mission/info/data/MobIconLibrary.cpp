#include "Dungeons.h"
#include <ConstructorHelpers.h>
#include "MobIconLibrary.h"

const FString UMobIconLibrary::ContextString = FString(TEXT("MobIconLibrary"));

UMobIconLibrary::UMobIconLibrary() {
	static const auto objectFinder = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("DataTable'/Game/Content_DLC4/UI/Mobs/MobIcons.MobIcons'"));
	mInfoDataTable = objectFinder.Object;
}

const FMobIconRow* UMobIconLibrary::findRow(const FName& row) const {
	return mInfoDataTable ? mInfoDataTable->FindRow<FMobIconRow>(row, ContextString) : nullptr;
}