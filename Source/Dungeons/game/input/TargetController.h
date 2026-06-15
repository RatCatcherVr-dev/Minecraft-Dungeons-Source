#pragma once

#include "HighlightController.h"
#include "game/component/InteractableComponent.h"
#include "game/util/Raycasting.h"

class APlayerCharacter;
class ABasePlayerController;
class UInteractableComponent;

class DUNGEONS_API TargetController : public InputController
{
public:
	enum class ETargetType : uint8 {
		None = 0,
		TargetOnly,
		ClickyOnly,
		All,
	};

	enum class ETargetMode : uint8 {
		/** Any valid target hovered is targeted. */
		Default,
		/** Current stays, no new target is acquired. */
		Sticky,
		/** Target cannot be replaced with null, but new valid targets replaces current target. */
		OnlyNew,
		/** Replaces empty target, but not existing target */
		StickyReplaceEmpty
	};

	void Update();
	void UpdateCursorTargetSelection(UWorld* world, ABasePlayerController* playerController, const FVector2D& screenSpaceOrigin, float maxDistance, float radius = 0.0f);
	void UpdateRadialTargetSelection(APlayerCharacter* player, float DeltaTime);
	ETargetType GetTargetType() { return TargetType; }
	void SetTargetType(ETargetType type);
	void SetTargetMode(ETargetMode mode);

	void ResetTarget();
	void ResetTargetCandidate();
	void ForceSetTarget(AActor* actor);

	FVector GetClosestTerrain() const;

	TWeakObjectPtr<AActor> GetAttackTarget() const { return CurrentTarget.IsValid() && !IsClickyTarget(CurrentTarget.Get()) ? CurrentTarget : nullptr; }
	TWeakObjectPtr<UInteractableComponent> GetClickyTarget() const { return CurrentTarget.IsValid() && IsClickyTarget(CurrentTarget.Get()) ? CurrentTarget->FindComponentByClass<UInteractableComponent>() : nullptr; }

private:
	ETargetType TargetType = ETargetType::All;
	ETargetMode TargetMode = ETargetMode::Default;

	static bool ShouldHighlightMobs(ETargetType mode) { return mode == ETargetType::TargetOnly || mode == ETargetType::All; }
	static bool ShouldHighlightClickies(ETargetType mode) { return mode == ETargetType::ClickyOnly || mode == ETargetType::All; }

	void HighlightTarget(AActor* actor);
	void UnhighlightTarget(AActor* actor);

	bool IsClickyTarget(const AActor* actor) const;
	bool IsTargetTargetable(const AActor* actor) const;
	bool IsTargetClickyTargetable(UInteractableComponent* Clicky, APlayerCharacter* playerCharacter) const;
	void UpdateCurrentTaget();

	void UpdateCurrentTarget(AActor* newTarget = nullptr);

	bool CanPlayerInteractWithClickies(APlayerCharacter* playerCharacter);

	TWeakObjectPtr<class AActor> CurrentTarget;
	TWeakObjectPtr<class AActor> TargetCandidate;
	TWeakObjectPtr<ABasePlayerController> CachedPlayerController;

	TargetSelection mTargetSelection;
	float fUpdateDelay = 0.0f;
};



