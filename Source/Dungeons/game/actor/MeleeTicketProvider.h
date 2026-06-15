#pragma once

#include "character/player/PlayerCharacter.h"
#include "MeleeTicketProvider.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FTicketEntry {
	GENERATED_USTRUCT_BODY()
	
	TWeakObjectPtr<AActor> Requester;

	float ExpiryTime;
};

UCLASS()
class DUNGEONS_API AMeleeTicketProvider : public AActor {
	GENERATED_BODY()

public:
	AMeleeTicketProvider(const FObjectInitializer& objectInitializer);	

	bool CanAttack(AActor* requester, AActor* target, float duration, int maxCount = 3);

	void RequestTicket(AActor* requester, AActor* target, float duration);

	void PostRegisterAllComponents() override;

	void PostUnregisterAllComponents() override;

	

private:
	TMap<uint32, TArray<FTicketEntry>> map;

	FCriticalSection mutex;
};