#include "Dungeons.h"
#include "InteractableComponent.h"
#include "WorldspaceHUDComponent.h"
#include <GameFramework/PlayerController.h>
#include "game/input/InputController.h"
#include "game/actor/item/StorableItem.h"
#include "game/actor/character/player/BasePlayerController.h"

namespace {
	template <typename T, typename P> TArray<T> FilterArrayByType(const TArray<P>& inArray) {
		static_assert(std::is_base_of<typename std::remove_pointer<P>::type, typename std::remove_pointer<T>::type>::value, "T must be a descendant of P");

		TArray<T> retVal;
		TArray<P> tmp = inArray.FilterByPredicate([](const P& v) { return v->template IsA<std::remove_pointer<T>::type>();  });
		retVal.Reserve(tmp.Num());

		for (auto v : tmp) {
			retVal.Emplace(Cast<std::remove_pointer<T>::type>(v));
		}

		return retVal;
	}
}

TMap< AActor*, UInteractableComponent* > UInteractableComponent::s_ActorToInteractableComponentMap;

UInteractableComponent::UInteractableComponent() 
	: Type(EClickyEnum::CE_Interactable)
    , BalloonTipBehavior(EBalloonTipBehavior::Hidden)
	, bAddedToWorldHud(false) {
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractableComponent::BeginPlay() {
	Super::BeginPlay();

	if (bInteractionEnabled) {
		EnableInteraction();
	}

	AActor* pOwner = GetOwner();

	//track owner
	if (pOwner)
	{
		CachedOwner = pOwner;

		if(!pOwner->IsA(APlayerCharacter::StaticClass())) //dont bother tracking player owned ones, they have a specific use and only end up being filtered out
		{
			InstanceTracker< UInteractableComponent >::AddInstance(pOwner->GetWorld(), this);
			s_ActorToInteractableComponentMap.Add(pOwner, this);

			if (auto item = Cast<AStorableItem>(pOwner))
			{
				if (item->lockItemToOwner && item->GetOwner() && item->GetItemType().shouldShowLockedOwnerOutline())
				{
					if (auto itemPlayerCharacter = Cast<APlayerCharacter>(item->GetOwner()))
					{
						if (AHighlightController* highlightController = AHighlightController::CheckHighlightControllerExists(GetWorld())) {
							highlightController->BeginOutlineActor(itemPlayerCharacter, GetOwner(), this, true);
						}
					}
				}
			}
		}
		else
		{
			s_ActorToInteractableComponentMap.Add(pOwner, this);
		}
	}	
}

void UInteractableComponent::EndPlay(const EEndPlayReason::Type type) {
	Super::EndPlay(type);
	RemoveWorldHud();
	AActor* pOwner = GetOwner();
	if (!pOwner->IsA(APlayerCharacter::StaticClass()))
	{
		s_ActorToInteractableComponentMap.Remove(pOwner);
		InstanceTracker< UInteractableComponent >::RemoveInstance(pOwner->GetWorld(), this);
	}

	PurgeAllInteractors();
}

void UInteractableComponent::PurgeAllInteractors() {
	// #D11.CM	- Parse our cached players, adding only valid ones to our remove list.
	//			- We do this as this function can be called on game end, where players could possibly be nullptr.
	TArray<APlayerCharacter*> validRemove = CachedInteractingPlayers.FilterByPredicate([](APlayerCharacter* character) {return character && character->GetPlayerController(); });

	// End highlights with valid players.
	for (auto* removedCharacter : validRemove) {
		EndHighlight(removedCharacter);
	}

	// Empty our arrays.
	validRemove.Empty();
	CachedInteractingPlayers.Empty();
}

//This is an Optimization - not game logic!
bool UInteractableComponent::DoesPlatformWantClickyWorldspaceHud() const {
	//On Windows - We have both mouse and game-pad and should always show the world-space hud.
	//D11.SC We only want clickies on windows (console uses something else)
	//However, always visible ones still need to be showed.
	return PLATFORM_WINDOWS || BalloonTipBehavior == EBalloonTipBehavior::AlwaysVisible;
}

//This is done because we add/remove these during play
//BeginPlay does not catch all entry points for this component
void UInteractableComponent::EnsureWorldHud() {
	if (DoesPlatformWantClickyWorldspaceHud() && !bAddedToWorldHud && GetWorld()) {
		for (auto it = GetWorld()->GetPlayerControllerIterator(); it; ++it) {
			if ((*it)->IsLocalController()) {
				UWorldspaceHUDComponent* component = (*it)->FindComponentByClass<UWorldspaceHUDComponent>();
				if(component) {
					component->AddClicky(this);
					bAddedToWorldHud = true;
					break;
				}
			}
		}
	}
}

//This is done because we add/remove these during play
//BeginPlay does not catch all entry points for this component
void UInteractableComponent::RemoveWorldHud() {	
	if (DoesPlatformWantClickyWorldspaceHud()){
		if (auto world = GetWorld()) {
			for (auto it = world->GetPlayerControllerIterator(); it; ++it) {
				if ((*it)->IsLocalController()) {
					if (UWorldspaceHUDComponent* component = (*it)->FindComponentByClass<UWorldspaceHUDComponent>()) {
						component->RemoveClicky(this);
						bAddedToWorldHud = false;
						break;
					}
				}
			}
		}
	}
}


void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// #D11.CM - Remove all stale pointers
	CachedInteractingPlayers.RemoveAllSwap([](const APlayerCharacter* ptr) {
		return ptr == nullptr;
	});
}

EClickyEnum UInteractableComponent::GetType() const {
	return Type;
}

const FText& UInteractableComponent::GetBalloontipText() const {
	return BalloontipText;
}

EBalloonTipBehavior UInteractableComponent::GetBalloontipBehavior() const {
	return BalloonTipBehavior;
}

void UInteractableComponent::SetType(EClickyEnum clickyType) {
	Type = clickyType;
}

void UInteractableComponent::SetBalloontipBehavior(EBalloonTipBehavior behaviour) {
	BalloonTipBehavior = behaviour;
}

void UInteractableComponent::SetText(const FText& text) {
	BalloontipText = text;
}

void UInteractableComponent::SetTextDesc(const FText& text, const FText& description) {
	BalloontipText = text;
	BalloontipDescription = description;
}

const FText& UInteractableComponent::GetBalloonTipDescription() const {
	return BalloontipDescription;
}

const FInventoryItemData& UInteractableComponent::GetBalloonItemData() const {
	if(auto* storable = Cast<AStorableItem>(GetOwner())) {
		return storable->ItemData;
	}

	//Return empty struct for non storables.
	static FInventoryItemData dummy;
	return dummy;
}

void UInteractableComponent::OnInteractingPlayerDestroyed(AActor* actor) {
	if(auto player = Cast<APlayerCharacter>(actor)) EndHighlight(player);
}

void UInteractableComponent::BeginHighlight_Implementation(APlayerCharacter* interactingPlayer) {
	auto wasHighlighted = IsHighlighted();

	if(!CachedInteractingPlayers.Contains(interactingPlayer)) {
		interactingPlayer->OnDestroyed.AddDynamic(this, &UInteractableComponent::OnInteractingPlayerDestroyed);
	}
	CachedInteractingPlayers.Add(interactingPlayer);
	

	HighlightCount++;

	ABasePlayerController* controller = Cast<ABasePlayerController>(interactingPlayer->GetPlayerController());
	check(controller && "Must have base player controller!");
	if (AHighlightController* highlightController = AHighlightController::CheckHighlightControllerExists(GetWorld())){
		highlightController->BeginOutlineActor(interactingPlayer, GetOwner(), this);
	}

	if (OnHighlightBegin.IsBound() && !wasHighlighted) {
		OnHighlightBegin.Broadcast();
	}

	// #D11.CM - Keeping interact prompts tied to the primary players worldspace.
	if (controller->GetGamepadActive() && !ActionInformation.ActionText.IsEmpty()) {
		if (auto* primaryPlayer = GetWorld()->GetFirstPlayerController()) {
			if (auto* worldHUD = primaryPlayer->FindComponentByClass<UWorldspaceHUDComponent>()) {
				worldHUD->UpdateContextPrompt(this);
			}
		}
	}
}

void UInteractableComponent::EndHighlight_Implementation(APlayerCharacter* interactingPlayer) {
	auto wasHighlighted = IsHighlighted();

	HighlightCount = FMath::Max(0, HighlightCount - 1);

	if (AHighlightController* highlightController = AHighlightController::CheckHighlightControllerExists(GetWorld())){
		highlightController->EndOutlineActor(GetOwner());
	}

	if (OnHighlightEnd.IsBound() && wasHighlighted && !IsHighlighted()) {
		OnHighlightEnd.Broadcast();
	}

	// #D11.CM - We don't need to check if a controller is active here, handles if someone moves to an interactable with a gamepad and leaves its radius with the mouse.
	if (interactingPlayer != nullptr) {
		CachedInteractingPlayers.RemoveSingle(interactingPlayer);
		if(!CachedInteractingPlayers.Contains(interactingPlayer)) {
			interactingPlayer->OnDestroyed.RemoveDynamic(this, &UInteractableComponent::OnInteractingPlayerDestroyed);
		}
	}

	// #D11.CM - If we don't have any more interactors, broadcast as such.
	if (CachedInteractingPlayers.Num() <= 0) {
		OnNoMoreInteractors.Broadcast();
	}

	// #D11.CM - Keeping interact prompts tied to the primary players worldspace.
	if(!IsHighlighted()) {
		if (auto* primaryPlayer = GetWorld()->GetFirstPlayerController()) {
			if (auto* worldHUD = primaryPlayer->FindComponentByClass<UWorldspaceHUDComponent>()) {
				worldHUD->UpdateContextPrompt(this);
			}
		}
	}
}

bool UInteractableComponent::IsHighlighted() const {
	for (auto interactor : CachedInteractingPlayers) {

		//D11.KS - Safety check for dropping out.
		if (interactor)
		{
			const AHighlightController* highlightController = AHighlightController::CheckHighlightControllerExists(GetWorld());

			if (highlightController && highlightController->IsActorHighlighted(GetOwner())){
				return true;
			}
		}
	}

	return false;
}

EBalloonItemDescriptionBehavior UInteractableComponent::GetBalloonItemDescriptionBehavior(const FInventoryItemData& balloonItemData) {
	const auto& item = GetItemRegistry().Get(balloonItemData.GetItemId());
	if (item.isGear() || item.isPermanent()) {
		return EBalloonItemDescriptionBehavior::Detailed;
	}
	return EBalloonItemDescriptionBehavior::Collapsed;
}

float UInteractableComponent::GetRadius() const {
	return Radius;
}


float UInteractableComponent::GetRadiusGamepadModifier() const {
	return RadiusGamepadModifier;
}

void UInteractableComponent::Interacted(ACharacter* instigator) {
	OnInteracted.Broadcast(instigator);

	// #D11.CM - Clear our our instigators inputs so they aren't rooted.
	if (auto* controller = instigator->GetController()) {
		if (auto* baseController = Cast<ABasePlayerController>(controller)) {
			baseController->CancelCurrentInputActions();
		}
	}

	if (bOneInteractionPerPlayer) {
		InteractedPlayers.AddUnique(Cast<APlayerCharacter>(instigator));
	}

	if (bOneTimeInteraction) {
		DisableInteraction();
	}
}

bool UInteractableComponent::IsInteractionEnalbed() const {
	return bInteractionEnabled;
}

UInteractableComponent* UInteractableComponent::GetComponentFromActor(const AActor* pActor)
{
	if (auto ppComp = s_ActorToInteractableComponentMap.Find(pActor)) { return *ppComp; }
	return nullptr;
}

void UInteractableComponent::EnableInteraction() {
	EnsureWorldHud();
	bInteractionEnabled = true;
}

void UInteractableComponent::DisableInteraction() {
	RemoveWorldHud();
	PurgeAllInteractors();
	bInteractionEnabled = false;
}