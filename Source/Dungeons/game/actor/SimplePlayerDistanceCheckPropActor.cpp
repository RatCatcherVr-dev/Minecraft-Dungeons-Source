#include "Dungeons.h"
#include "SimplePlayerDistanceCheckPropActor.h"
#include "DungeonsGameState.h"

ASimplePlayerDistanceCheckPropActor::ASimplePlayerDistanceCheckPropActor() {
 	PrimaryActorTick.bCanEverTick = true;	
}

ASimplePlayerDistanceCheckPropActor::ASimplePlayerDistanceCheckPropActor(const class FObjectInitializer& OI) : Super(OI) {
	PrimaryActorTick.bCanEverTick = true;
}

void ASimplePlayerDistanceCheckPropActor::BeginPlay() {

	mCurrentPlayerCheckTimer = mPlayerCheckTimer + FMath::FRandRange(0.025f, 0.1f); //just a teeny bit of initial varience, just to reduce the amount of checks polled together

	Super::BeginPlay();	
}

void ASimplePlayerDistanceCheckPropActor::Tick(float DeltaSeconds)
{
	if (auto world = GetWorld())
	{
		mCurrentPlayerCheckTimer -= DeltaSeconds;

		if (mCurrentPlayerCheckTimer <= 0.0f)
		{
			mCurrentPlayerCheckTimer = mPlayerCheckTimer;

			TArray< APlayerCharacter* > CurrentPlayerList;
			CurrentPlayerList.Reserve(8);

			UActorQuery::GetPlayerCharactersInRange(world,	GetActorLocation(), mMaxPlayerDistance, CurrentPlayerList, false);

			if (CurrentPlayerList != mCurrentPlayerCharacterList) //this should be fine, as the list is pulled from a statically stored base and its order/composition rarely changes
			{
				mCurrentPlayerCharacterList = CurrentPlayerList;
				PlayersInRangeChanged(mCurrentPlayerCharacterList);
			}
		}
	}

	Super::Tick(DeltaSeconds);
}

void ASimplePlayerDistanceCheckPropActor::EndPlay(EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
}

void ASimplePlayerDistanceCheckPropActor::PlayersInRangeChanged(const TArray< APlayerCharacter* >& PlayerCharacterList)
{
	PlayersInRangeChangedEvent(PlayerCharacterList);
}

