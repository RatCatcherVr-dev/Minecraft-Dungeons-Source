
#include "Dungeons.h"
#include "MapPinWidget.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/avatar/PlayerAvatarComponent.h"
#include "game/component/MapPinComponent.h"
#include "game/actor/item/StorableItem.h"


bool UMapPinWidget::IsLocalPlayer() const
{
	if (Initialized)
		return !IsRemotePlayer;

	if (!ParentComponent)
		return false;

	if (const auto playerCharacter = Cast<APlayerCharacter>(ParentComponent->GetOwner()))
	{
		return playerCharacter->IsLocallyControlled();
	}
	return false;
}

void UMapPinWidget::InitMapPinWidget(UMapPinComponent* parent)
{
	if (!parent)
		return;
	
	ParentComponent = parent;

	if (auto playerCharacter = Cast<APlayerCharacter>(parent->GetOwner()))
	{
		IsRemotePlayer = !playerCharacter->IsLocallyControlled();
		if (IsRemotePlayer)
		{
			//////// why is the player color not updated (for server) on client here?! replication ?
			Color = playerCharacter->GetPlayerColor();
			DoSetPinColor(Color);
		}
		
		setPinAngle(playerCharacter->GetActorRotation().Yaw + ParentComponent->PIN_ROTATION_OFFSET);
		setIsDead(!playerCharacter->IsAlive());
		
		DoSetPlayerCharacter(playerCharacter);
	}

	if (auto storable = Cast<AStorableItem>(parent->GetOwner()))
	{
		bool isGearOrPermanent = storable->GetItemType().isGear() ||
			storable->GetItemType().isPermanent();
		DoUpdatePinRarity(storable->GetRarity(), isGearOrPermanent, storable->ItemData.IsNetherite());
	}

	Initialized = true;
}

void UMapPinWidget::setVisible(bool visible)
{
	if (visible != IsVisible)
	{
		IsVisible = visible;
		ESlateVisibility visibility = IsVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden;
		SetVisibility(visibility);
	}
}

// Pin Widget update calls:
void UMapPinWidget::setPinAngle(float angle)
{
	if (!FMath::IsNearlyEqual(CurrentAngle, angle, 1.0f))
	{
		CurrentAngle = angle;
		DoUpdateDirection(angle);
	}
}

// note; single call -send in init-call
void UMapPinWidget::setAspectScale(FVector2D AspectScale)
{
	DoSetAspectScale(AspectScale);
}

void UMapPinWidget::setIsDead(bool dead)
{
	if (PlayerDead != dead)
	{
		PlayerDead = dead;
		DoUpdateDownedState(PlayerDead);
	
	}
}
