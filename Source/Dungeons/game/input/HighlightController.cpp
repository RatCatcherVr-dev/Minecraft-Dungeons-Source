
#include "Dungeons.h"
#include "HighlightController.h"

#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerState.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/util/actorquery.h"
#include "MouseMeleeState.h"
#include "game/component/HealthComponent.h"
#include "game/actor/item/StorableItem.h"
#include "DungeonsGameInstance.h"
#include "GameSettingsFunctionLibrary.h"
#include "game/actor/TickInterleaving.h"
#include "game/team/TeamQuery.h"
#include "game/actor/FallingIceActor.h"
#include "game/inventory/InventoryItemUtil.h"

DECLARE_CYCLE_STAT(TEXT("STAT_HighlightController_Update"), STAT_HighlightController_Update, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_HighlightController_Update_PlayeHL"), STAT_HighlightController_Update_PlayeHL, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_HighlightController_Update_HighlighObscuredCharacters"), STAT_HighlightController_Update_HighlighObscuredCharacters, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_HighlightController_Update_UpdateHighlightedActorInfo"), STAT_HighlightController_Update_UpdateHighlightedActorInfo, STATGROUP_PlayerController);

DECLARE_CYCLE_STAT(TEXT("STAT_Process_LineTraceSingleByChannel"), STAT_Process_LineTraceSingleByChannel, STATGROUP_PlayerController);

namespace {
	InputController::EOutlineState GetOutlineForActor(const APlayerCharacter* player, const AActor* actor) {

		bool useLocalMultiplayerColours = (player && player->GetGameInstance() && player->GetGameInstance()->GetNumLocalPlayers() > 1);

		if (auto mob = Cast<AMobCharacter>(actor)) {
			if (mob->cachedTargetees.Num() > 0) {
				if (APlayerCharacter* mobTargeter = mob->cachedTargetees[0].Get()) {
					if (mobTargeter && mob->IsHostileTowards(mobTargeter)) {
						if (useLocalMultiplayerColours) {
							return mob->cachedTargetees.Num() < 2 ?
								InputController::EOutlineState(mobTargeter->GetPlayerNumber() + static_cast<uint8>(InputController::EOutlineState::LocalP1TargetOutline)) :
								InputController::EOutlineState::Interactable;
						}

						return UGameSettingsFunctionLibrary::GetEnemyHighlightColour(mobTargeter->GetPlayerController());
					}
				}
			}
		}

		if (useLocalMultiplayerColours)
		{
			//D11.KS - Check if this actor is storable, because local coop can lock them, thus needing special colours.
			if (auto item = Cast<AStorableItem>(actor))
			{
				if (item->lockItemToOwner && item->GetOwner())
				{
					//D11.KS - Check if it's locked by someone on our side, else we don't need to treat it differently.
					if (auto playerCharacter = Cast<APlayerCharacter>(item->GetOwner()))
					{
						if (playerCharacter->GetPlayerState())
						{
							InputController::EOutlineState outlineIndex = InputController::EOutlineState(playerCharacter->GetPlayerNumber() + (uint8)InputController::EOutlineState::LocalP1Outline);

							return outlineIndex;
						}
					}
				}
			}
			else
			{
				//If is interactable we can get their highlight count and determine whether more than 1 person is highlighting.
				if(auto interactable = UInteractableComponent::GetComponentFromActor(actor))
				{
					if (interactable->GetHighlightCount() > 1)
					{
						return InputController::EOutlineState::Interactable;
					}			
				}

				InputController::EOutlineState outlineIndex = InputController::EOutlineState(player->GetPlayerNumber() + (uint8)InputController::EOutlineState::LocalP1Outline);

				return outlineIndex;
			}
		}

		return InputController::EOutlineState::Interactable;
	}

	InputController::ESilhouetteState GetSilhouetteForActor(const APlayerCharacter* player, const AActor* actor) {
		bool useLocalMultiplayerColours = (player && player->GetGameInstance() && player->GetGameInstance()->GetNumLocalPlayers() > 1);

		if (auto mob = Cast<AMobCharacter>(actor)) {
			if (player && mob->IsHostileTowards(player)) return InputController::ESilhouetteState::OccludedUnfriendly;
		}
		else if (auto item = Cast<AStorableItem>(actor)) {

			//D11.KS - Beams are now overridden by local coop colour, we need to override the occluded rarity colour now.
			if(useLocalMultiplayerColours && item->lockItemToOwner)
			{
				if (auto playerCharacter = Cast<APlayerCharacter>(item->GetOwner()))
				{
					return InputController::ESilhouetteState(playerCharacter->GetPlayerNumber() * 0x10 + (uint8)InputController::ESilhouetteState::OccludedLocalP1);
				}
			}

			switch (UInventoryItemUtil::GetDisplayRarity(item->ItemData))
			{
				case EItemRarity::Common: return InputController::ESilhouetteState::OccludedCommonItem;
				case EItemRarity::Rare: return InputController::ESilhouetteState::OccludedRareItem;
				case EItemRarity::Unique: return InputController::ESilhouetteState::OccludedUniqueItem;
			}
		}

		if(useLocalMultiplayerColours)
		{
			if(auto playerCharacter = Cast<APlayerCharacter>(actor))
			{
				if (playerCharacter->GetPlayerState())
				{
					return InputController::ESilhouetteState(playerCharacter->GetPlayerNumber() * 0x10 + (uint8)InputController::ESilhouetteState::OccludedLocalP1);
				}
			}
		}

		return InputController::ESilhouetteState::OccludedFriendy;
	}

	bool GetConstantOutlineForActor(const APlayerCharacter* player, const AActor* actor)
	{
		if (player && player->GetGameInstance() && player->GetGameInstance()->GetNumLocalPlayers() > 1)
		{
			if (auto item = Cast<AStorableItem>(actor)) {				
				if (!item->GetItemType().shouldShowLockedOwnerOutline()){
					return item->lockItemToOwner;
				}
			}
		}

		return false;
	}

	bool IsOccluded(UWorld* world, const FVector& begin, const FVector& end, FHitResult& result, FCollisionQueryParams params = FCollisionQueryParams::DefaultQueryParam) {
		TArray<FHitResult> results;
		world->LineTraceMultiByChannel(results, begin, end, (ECollisionChannel)ECustomTraceChannels::TerrainAndPlayerOnly);

		if(results.Num()) {
			result = *results.begin();
			return true;
		}

		return false;
	}
	
}
AHighlightController::AHighlightController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AHighlightController::BeginPlay()
{
	Super::BeginPlay();
	InstanceTracker< AHighlightController >::AddInstance(GetWorld(), this);
}

void AHighlightController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	InstanceTracker< AHighlightController >::RemoveInstance(GetWorld(), this);
	Super::EndPlay(EndPlayReason);
}

AHighlightController* AHighlightController::CheckHighlightControllerExists(UWorld* pWorld)
{
	if (pWorld)
	{
		if (InstanceTracker< AHighlightController >::GetList(pWorld).Num() != 1)
		{
			return nullptr;
		}

		return InstanceTracker< AHighlightController >::GetList(pWorld)[0];
	}

	return nullptr;
}


void AHighlightController::Tick(float DeltaTime) {
	SCOPE_CYCLE_COUNTER(STAT_HighlightController_Update);
	
	//If for whatever reason our player controller has been lost, just disable ourselves
	if(!PlayerController) {
		Enable(false);
		return;
	}
	if (MainPlayerOcclusionSilhouettingTimer > 0) {
		MainPlayerOcclusionSilhouettingTimer -= DeltaTime;
	}

	if(Ticker->ShouldTick(ETargetingTickStage::HighlightingUpdate)) {
		if (bDisableHighlightsOneFrame) {
			bDisableHighlightsOneFrame = false;
			return;
		}

		UpdateOcclusionSilhouetting();
		UpdateHighlightedActorInfo();
	}
}

static TAutoConsoleVariable<int> CVarTracesPerFrame(TEXT("Dungeons.obscured.maxtracesperframe"), 2, TEXT("Sets how many occlusion traces per frame (player excluded) will be performed."), ECVF_Default);

void AHighlightController::HighlighObscuredCharacters(const FVector& sourcePoint, const FVector& cameraLocation) {

	SCOPE_CYCLE_COUNTER(STAT_HighlightController_Update_HighlighObscuredCharacters);

	if (ActorsToCheck.Num() == 0) {
		for (const auto& actor : InstanceTracker<ABaseCharacter>::GetList(GetWorld())) {
			if (actor->EnableOcclusionSilhouetting) {
				ActorsToCheck.Add(actor);
			}
		}
		for (const auto& actor : InstanceTracker<AStorableItem>::GetList(GetWorld())) {
			ActorsToCheck.Add(actor);
		}
		for (const auto& actor : InstanceTracker<AFallingIceActor>::GetList(GetWorld())) {
			ActorsToCheck.Add(actor);
		}
	}

	auto IsNearbyPredicate = [sourcePoint](AActor* actor) {
		const float squareDistance(1800*1800);
		return actorquery::getActorDistanceSquared(sourcePoint, actor) < squareDistance;
	};

	int numTracesLeft = CVarTracesPerFrame.GetValueOnGameThread();
	while (numTracesLeft > 0 && ActorsToCheck.Num() > 0) {
		AActor* actor = ActorsToCheck.Pop(false).Get();
		if (actor && IsNearbyPredicate(actor)) {
			numTracesLeft--;

			SCOPE_CYCLE_COUNTER(STAT_Process_LineTraceSingleByChannel);

			FVector end = actor->GetActorLocation();
			FHitResult hitResult;

			bool needsSilhouette = false;
			if (IsOccluded(GetWorld(), cameraLocation, end, hitResult)) {
				auto target = hitResult.GetActor();

				// Highlight the enemy if the trace didn't reach it (ie hit terrain or something)
				if (!target->IsA<ABaseCharacter>() && target != actor) {
					needsSilhouette = true;
				}
			}
			
			if (needsSilhouette) {
				BeginSilhouetteActor(actor);
			}
			else {
				EndSilhouetteActor(actor);
			}
		}
	}
}

void AHighlightController::ApplyHighlightedActorInfo(const HighlightedActorInfo& info, uint8 newState) {
	TArray<UPrimitiveComponent*, TInlineAllocator<256> > componentsToOutline;
	AActor* actor = info.Actor.Get();
	actor->GetComponents(componentsToOutline, true);
		
	for (auto& component : componentsToOutline) {
		if (component->ComponentTags.Contains("no-highlight") || component->IsA<UParticleSystemComponent>()) 
		{
			continue;
		}

		if (!info.currentState || !newState) component->SetRenderCustomDepth(newState != 0);
		if (newState) component->SetCustomDepthStencilValue(newState);
	}

	info.currentState = newState;
}

void AHighlightController::UpdateHighlightedActorInfo() {

	SCOPE_CYCLE_COUNTER(STAT_HighlightController_Update_UpdateHighlightedActorInfo);
	
	for (int i = 0; i < HighlightedActors.Num(); ++i) {
		HighlightedActorInfo& entry = HighlightedActors[i];
		//Clean up dead actors
		if (!entry.Actor.IsValid()) {
			HighlightedActors.RemoveAtSwap(i);
			--i;
			continue;
		}

		if(entry.sticky)
		{
			entry.outline = GetOutlineForActor(Cast<APlayerCharacter>(entry.Actor.Get()->GetOwner()), entry.Actor.Get());
		}
		else if (entry.outline.IsSet()) {
			if (entry.interactableComponent.IsValid() && entry.interactableComponent->IsInteractionEnalbed())
			{
				const TArray<APlayerCharacter*> interactingPlayers = entry.interactableComponent->GetInteractingPlayers();

				if (interactingPlayers.Num() > 0)
				{
					entry.outline = GetOutlineForActor(interactingPlayers[0], entry.Actor.Get());
				}
			}
			else
			{
				entry.outline = GetOutlineForActor(Cast<APlayerCharacter>(PlayerController->GetCharacter()), entry.Actor.Get());
			}
		}


		if (entry.silhouette.IsSet()) {
			entry.silhouette = GetSilhouetteForActor(Cast<APlayerCharacter>(PlayerController->GetCharacter()), entry.Actor.Get());
		}

		uint8 newState = InputController::CombinedState(entry.outline.Get(InputController::EOutlineState::None), entry.silhouette.Get(InputController::ESilhouetteState::None));

		if (newState != entry.currentState) {
			ApplyHighlightedActorInfo(entry, newState);
			if (!newState) {
				HighlightedActors.RemoveAtSwap(i);
				--i;
			}
		}
		
	}
}

void AHighlightController::BeginOutlineActor(APlayerCharacter* playerCharacter, AActor* actor, UInteractableComponent* interactableComponent, bool sticky /* = false */) {
	HighlightedActorInfo& entry = GetOrCreateForActor(actor);
	entry.outlineCount++;
	if(!entry.sticky) {
		entry.outline = GetOutlineForActor(playerCharacter, actor);
		entry.sticky = sticky;
		entry.interactableComponent = interactableComponent;	
	}
}

void AHighlightController::EndOutlineActor(AActor* actor) {
	if (auto entry = HighlightedActors.FindByKey(actor)) {
		if (entry->outlineCount == 0) return;

		if (--entry->outlineCount == 0 && !entry->sticky) {
			entry->outline.Reset();
		}
	}
}

void AHighlightController::BeginSilhouetteActor(AActor* actor) {

	HighlightedActorInfo& entry = GetOrCreateForActor(actor);
	entry.silhouette = GetSilhouetteForActor(Cast<APlayerCharacter>(PlayerController->GetCharacter()), actor);

	//This needs to retain even with silhouetting.
	entry.sticky = GetConstantOutlineForActor(Cast<APlayerCharacter>(PlayerController->GetCharacter()), actor);
}

void AHighlightController::EndSilhouetteActor(AActor* actor) {
	if (auto entry = HighlightedActors.FindByKey(actor)) {
		entry->silhouette.Reset();
	}
}

void AHighlightController::UpdateOcclusionSilhouetting()
{
	auto player = Cast<APlayerCharacter>(PlayerController->GetCharacter());
	auto world = GetWorld();


	// D11.DH 
	// Fix for crash when joining a game that has just ended caused by attempting to highlight a null player
	if(player && PlayerController->PlayerCameraManager)
	{
		FVector cameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();

		SCOPE_CYCLE_COUNTER(STAT_HighlightController_Update_PlayeHL);
		
		if (player->IsAlive() && MainPlayerOcclusionSilhouettingTimer <= 0.0f)
		{
			FHitResult hitResult;
			FVector end =  player->GetMesh()->GetBoneLocation(FName("J_Head"));

			if (end == FVector::ZeroVector) {
				end = player->GetActorLocation();
			}

			FCollisionQueryParams params;

			params.bTraceComplex = true;
			
			if(IsOccluded(world, cameraLocation, end, hitResult, params))
			{
				bool playerObscured = hitResult.GetActor() != nullptr && !hitResult.GetActor()->GetClass()->IsChildOf(ABaseCharacter::StaticClass());
				if (playerObscured && !PlayerObscured) {
					BeginSilhouetteActor(player);
				}
				else if (!playerObscured && PlayerObscured) {
					EndSilhouetteActor(player);
				}

				PlayerObscured = playerObscured;
			}

			MainPlayerOcclusionSilhouettingTimer = 0.1f;
		}
		
		auto sourcePoint = player->GetActorLocation();
		auto gameInstance = Cast<UDungeonsGameInstance>(player->GetGameInstance());
		if (gameInstance && gameInstance->GetNumLocalPlayers() > 1) {
			sourcePoint = gameInstance->CoopCameraLookAtCache;
		}

		HighlighObscuredCharacters(sourcePoint, cameraLocation);
	}
}

HighlightedActorInfo& AHighlightController::GetOrCreateForActor(AActor* actor) {
	if (auto entry = HighlightedActors.FindByKey(actor)) return *entry;

	HighlightedActors.Emplace(actor);
	return HighlightedActors.Last();
}

void AHighlightController::DisableHighlightsForOneFrame()
{
	bDisableHighlightsOneFrame = true;
	ClearAllHighlights();
}

void AHighlightController::Enable(bool enabled) {
	if (enabled == IsActorTickEnabled()) return;

	if (!enabled) {
		ClearAllHighlights();	
	}

	SetActorTickEnabled(enabled);
}

void AHighlightController::Claim(ABasePlayerController* controller, UTargetingTickStageComponent* ticker)
{
	AddTickPrerequisiteActor(controller);
	PlayerController = controller;
	Ticker = ticker;
}

void AHighlightController::ClearAllHighlights() {
	for (auto& entry : HighlightedActors) {
		if (entry.Actor.IsValid() && !entry.sticky) {
			ApplyHighlightedActorInfo(entry, 0);
		}
	}

	// #D11.CM - Don't remove constant outline actors.
	HighlightedActors.RemoveAll([](HighlightedActorInfo info) { return !info.sticky; });
}
