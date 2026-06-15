/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "Components/ActorComponent.h"
#include "ui/hints/HintManager.h"
#include "ObjectiveHintTargetComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UObjectiveHintTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	bool IsVisible() const;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	EUIHintType TriggerHintType = EUIHintType::None;

private:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	const UHintManager* GetHintManager() const;
};
