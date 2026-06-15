#pragma once
#include "CoreMinimal.h"
#include <GameFramework/Actor.h>
#include "MerchantActor.generated.h"

class AMerchantBase;

UCLASS(Blueprintable, BlueprintType, Abstract)
class DUNGEONS_API AMerchantActor : public AActor {
	GENERATED_BODY()

public:
	AMerchantActor();

	void BeginPlay() override;

	using MerchantPredicate = std::function<bool(const AMerchantBase&)>;
	bool IsMerchantForMatching(class ACharacter*, MerchantPredicate) const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SimulateInteraction(class ACharacter* interactor);

	TSoftClassPtr<class UMerchantWidgetBase> GetMerchantWidgetClass() const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	class UAnimSequenceBase* transactionCompletedAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	class UAnimSequenceBase* interactEndAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	class UAnimSequenceBase* interactBeginAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	class UMerchantDefComponent* mMerchantDefComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	class UInteractableComponent* mInteractableComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSoftClassPtr<class UMerchantWidgetBase> mMerchantWidgetClass;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Dungeons")
	void SetCapture3DView(bool capture);

private:	
	UFUNCTION()
	void HandleInteraction(class ACharacter* interactor);

	UPROPERTY()
	class USceneComponent* mRootSceneComponent = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FName mAnimationSlot = FName(TEXT("DefaultSlot"));
		
	void PlayAnimation(UAnimSequenceBase* animation) const;

	UPROPERTY()
	class USkeletalMeshComponent* mMeshComponent;	

	class UMerchantDefComponent* GetMerchantDefComponent() const;
};

