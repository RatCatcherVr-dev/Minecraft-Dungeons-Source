#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "util/DataTracker.h"
#include "game/actor/PropActor.h"
#include "SimplePlayerDistanceCheckPropActor.generated.h"

UCLASS()
class DUNGEONS_API ASimplePlayerDistanceCheckPropActor : public APropActor
{
	GENERATED_BODY()
	
public:	
	
	// Sets default values for this actor's properties
	ASimplePlayerDistanceCheckPropActor();
	ASimplePlayerDistanceCheckPropActor(const class FObjectInitializer& OI);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	
	virtual void PlayersInRangeChanged(const TArray< APlayerCharacter* >& PlayerCharacterList);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void PlayersInRangeChangedEvent(const TArray< APlayerCharacter* >& PlayerCharacterList);

	//maximum distance from  actor that a player must be to be counted as within activation range
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float mMaxPlayerDistance = 3500.0f; 

	//count down between player distance checks (in seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float mPlayerCheckTimer = 0.25f;
	
protected:
	
	float mCurrentPlayerCheckTimer = 0.0f;
	TArray< APlayerCharacter* > mCurrentPlayerCharacterList;
	
};

