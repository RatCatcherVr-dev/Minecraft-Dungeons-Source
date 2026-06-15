#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SecretComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnSecretFound, AActor*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSecretFoundRecieved);

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DUNGEONS_API USecretComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USecretComponent();	

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SecretFound(AActor* SecretFinder);

	UFUNCTION(NetMulticast, Reliable)
	void ExecuteSecretFound(AActor* SecretFinder);

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnSecretFoundRecieved OnSecretFoundRecieved;

	FOnSecretFound OnSecretFound;

private:
	bool isFound = false;
};
