#include "Dungeons.h"
#include "MapPinComponent.h"

#include "Components/CanvasPanel.h"
#include "game/Conversion.h"
#include "game/GameBP.h"
#include "game/MapTexture.h"
#include "game/actor/item/StorableItem.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/loot/LootActor.h"
#include "game/level/doors/InstantTravelActor.h"
#include "game/util/ActorQuery.h"
#include "game/util/PlayerQuery.h"
#include "game/item/ItemType.h"
#include "game/item/ItemTypeDefs.h"

UMapPinComponent::UMapPinComponent()
{
	bWantsInitializeComponent = true;
	mapPinWidgets.SetNum((int32)EMapType::Max);
}

void UMapPinComponent::InitializeComponent()
{
	Super::InitializeComponent();
	Activate(true);
}

void UMapPinComponent::Activate(bool reset)
{
	Super::Activate(reset);
}

void UMapPinComponent::Deactivate()
{
	Super::Deactivate();
}

void UMapPinComponent::BeginPlay()
{
	if (mapPinClasses.Num() > 0)
	{
		//only track the ones we care about
		InstanceTracker<UMapPinComponent>::AddInstance(GetWorld(), this);
	}

	Super::BeginPlay();
	
}

void UMapPinComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{	
	if (mapPinClasses.Num() > 0)
	{
		InstanceTracker<UMapPinComponent>::RemoveInstance(GetWorld(), this);
	}

	for (const TWeakObjectPtr<UMapPinWidget>& p : mapPinWidgets)
	{
		if (p.IsValid())
			p.Get(false)->RemoveFromParent();
	}

	if(UGameViewportClient* gameViewportClient = GetWorld()->GetGameViewport())
	{
		gameViewportClient->OnPlayerAdded().Remove(OnPlayerAddedDelegateHandle);
		gameViewportClient->OnPlayerRemoved().Remove(OnPlayerRemovedDelegateHandle);
	}

	Super::EndPlay(EndPlayReason);
}

const float UMapPinComponent::PIN_ROTATION_OFFSET = -40.0f;

void UMapPinComponent::AddMapPinClass(EMapType mapType, TSubclassOf<UMapPinWidget> pinClass) {
	mapPinClasses.Add(mapType, pinClass);
}

void UMapPinComponent::UpdateLocalCoopColours(int32 playerIndex)
{
	if (auto playerCharacter = Cast<APlayerCharacter>(GetOwner()))
	{
		const FColor newPinColour = playerCharacter->GetGameInstance()->GetNumLocalPlayers() > 1 ? playerCharacter->GetPlayerColor() : FColor::White;

		if (UMapPinWidget* overlayPin = GetMapPin(EMapType::Overlay))
		{
			overlayPin->SetColorAndOpacity(newPinColour);

		}
		if (UMapPinWidget* fullscreenPin = GetMapPin(EMapType::Fullscreen))
		{
			fullscreenPin->SetColorAndOpacity(newPinColour);
		}
	}
}

void UMapPinComponent::Update(EMapType MapType, APlayerCharacter* primaryPlayer)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_UMapPinComponent_Update)
	// note: out of map clipping behaviour is handled on the Map Pin Widget in BP
	if (auto mapPin = GetMapPin(MapType))
	{
		UWorld* world = GetWorld();
		AActor* owner = GetOwner();
		APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(owner);

		
		//Primary player map pin is in the center so always visible
		if (primaryPlayer && playerCharacter && primaryPlayer == playerCharacter)
		{
			mapPin->setPinAngle(playerCharacter->GetActorRotation().Yaw + PIN_ROTATION_OFFSET);
			mapPin->setIsDead(!playerCharacter->IsAlive());
			mapPin->setVisible(true);
			return;
		}

		bool visible = IsActive();
		if (visible && mapPin->OutsideBoundBehavior < EOutsideBoundBehavior::RevealAlways)
		{
			// handle hidden in fog of war
			TerrainPos pos = conversion::ueToTerrain(owner->GetActorLocation());

			AGameBP* gameBp = actorquery::getFirstActor<AGameBP>(GetWorld());
			const auto terrain = gameBp->GetTerrain();

			auto revealed = terrain->getReveal(pos);
			visible = revealed >= RevealCell::FullReveal;

			// handle being above/below the player
			if (visible)
			{
				FVector pinLocation = owner->GetActorLocation();
				if (FMath::Abs(pinLocation.Z - PlayerCenter.Z) > LevelThreshhold)
				{
					visible = false;
				}
			}
		}

		if (visible)
		{
			//if we aren't visible, bypass doing the expensive stuff
			if (playerCharacter)
			{
				if (primaryPlayer)
				{
					visible = playerCharacter->GetCurrentDungeonInstanceId() == primaryPlayer->GetCurrentDungeonInstanceId();
				}

				if (visible)
				{
					// rotate this type of pin
					mapPin->setPinAngle(playerCharacter->GetActorRotation().Yaw + PIN_ROTATION_OFFSET);
					mapPin->setIsDead(!playerCharacter->IsAlive());
				}
			}
			else if (ABaseCharacter* character = Cast<ABaseCharacter>(owner))
			{
				visible = character->IsAlive();
				if (visible)
				{
					mapPin->setPinAngle(character->GetActorRotation().Yaw + PIN_ROTATION_OFFSET);
				}
			}
			else if (auto lootActor = Cast<ALootActor>(owner))
			{
				if (lootActor->InteractableComp) {
					visible = !lootActor->IsOpen() && lootActor->InteractableComp->IsInteractionEnalbed();
				} else {
					visible = !lootActor->IsOpen();
				}
			}
			else if (auto travelActor = Cast<AInstantTravelActor>(owner)) {
				if (const auto* interactableComponent = travelActor->GetInteractableComponent()) {
					if (!interactableComponent->IsInteractionEnalbed()) {
						visible = false;
					}
				}
			}
		}
		mapPin->setVisible(visible);
	}
}

void UMapPinComponent::SetMapPin(UMapPinWidget * widget, EMapType MapType)
{
	ensure(widget);
	if (!widget || MapType == EMapType::UNSET)
		return;


	if (!mapPinWidgets[(uint8)MapType].IsValid())
		mapPinWidgets[(uint8)MapType] = TWeakObjectPtr<UMapPinWidget>(widget);

}

UMapPinWidget* UMapPinComponent::GetMapPin(EMapType MapType) const
{
	return mapPinWidgets[(uint8)MapType].Get(false);

}

FVector2D UMapPinComponent::Get2DPosition(const UMapTexture2D* map) const
{
	const auto* owner = GetOwner();
	ensure(owner);
	ensure(map);

	return map->worldToMap(owner->GetActorLocation(), PlayerCenter);
}

void UMapPinComponent::setCenterPos(const FVector& center)
{
	PlayerCenter = center;
}
	
void UMapPinComponent::addToCanvas(UWidget* Content, UCanvasPanel* Panel)
{
	if (!Content || !Panel)
		return;

	Panel->AddChildToCanvas(Content);
	
}

void UMapPinComponent::Initialize(UMapPinWidget* Content, UCanvasPanel* Panel, float inOpacity, FVector2D AspectScale)
{
	// Initialization steps from BP
	if (!Content || !Panel)
		return;

	Content->SetRenderOpacity(inOpacity);
	Content->InitMapPinWidget(this);

	Content->setAspectScale(AspectScale);

	Panel->AddChildToCanvas(Content);	


	if (auto playerCharacter = Cast<APlayerCharacter>(GetOwner()))
	{
		if (UGameViewportClient* gameViewportClient = GetWorld()->GetGameViewport())
		{
			OnPlayerAddedDelegateHandle = gameViewportClient->OnPlayerAdded().AddUObject(this, &UMapPinComponent::UpdateLocalCoopColours);
			OnPlayerRemovedDelegateHandle = gameViewportClient->OnPlayerRemoved().AddUObject(this, &UMapPinComponent::UpdateLocalCoopColours);
		}

		UpdateLocalCoopColours();
	}
}
TSubclassOf<UMapPinWidget> UMapPinComponent::getMapPinClass(EMapType MapType) const
{
	if (mapPinClasses.Contains(MapType)) {
		return mapPinClasses[MapType];
	}
	return nullptr;
}