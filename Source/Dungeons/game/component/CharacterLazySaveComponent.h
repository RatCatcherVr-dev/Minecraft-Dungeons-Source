#pragma once

#include "CoreMinimal.h"
#include "CharacterSerializeComponent.h"
#include "CharacterLazySaveComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSaveFailed, FString, filepath);

UCLASS()
class DUNGEONS_API UCharacterLazySaveComponent : public UCharacterSerializeComponent {
	GENERATED_BODY()

public:
	UCharacterLazySaveComponent();
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void FlushCommand();	

	void DirtyItems(int32 inValue);

	UPROPERTY(BlueprintAssignable, Category = "CharacterSaveData")
	FOnCharacterSaveFailed OnCharacterSaveFailed;
	
protected:
	void IncrementDirtyFlag(int32 inValue) override;
	void OnCharacterAssigned() override;

private:	
	// dirty state and flushing:
	int32 mDirty = 0;
	float mTime = 0.0f;

	bool mInitialized = false;
	
	// D11.SSN
	FTimerHandle saveTimerHandle;
	void Save();
	bool saveTimerSet = false;


	UFUNCTION()
	void SaveFailed(FString message);
};
