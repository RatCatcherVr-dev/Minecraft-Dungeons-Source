#pragma once

#include "Components/ActorComponent.h"
#include "DoorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOpen, bool, instant);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClose, bool, instant);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UDoorComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UDoorComponent();

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void Open(bool instant = false);
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void Close(bool instant = false);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;

	UFUNCTION(BlueprintCallable)
	bool IsDoorOpen() const { return IsOpen; };

protected:
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnOpen OnOpen;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnClose OnClose;

	UFUNCTION()
	void OnRep_IsOpen();

	UFUNCTION()
	void OnRep_StartOpen();

	UFUNCTION()
	void OnReppedVar();

	void UpdateIsOpenOnServer(bool);

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_IsOpen)
	bool IsOpen;

	//Have to have this to solve replication of instance editable variables.
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Replicated, ReplicatedUsing = OnRep_StartOpen)
	bool StartOpen;
};
