#include "Dungeons.h"
#include "DungeonsGameState.h"
#include "PlayerExperienceComponent.h"
#include "DungeonsGameInstance.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/HealthComponent.h"
#include <Net/UnrealNetwork.h>
#include "CharacterSerializeComponent.h"
#include "ClientEventHub.h"
#include "online/sessions/OnlineUtil.h"
#include "online/seasons/LiveOps.h"

UPlayerExperienceComponent::UPlayerExperienceComponent()
	: mXp(0)
	, mXpReceivedFromClient(false)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
}

void UPlayerExperienceComponent::BeginPlay() {
	Super::BeginPlay();
	ForceUpdateXP();
	SetComponentTickEnabled(false);
}

void UPlayerExperienceComponent::ForceUpdateXP()
{
	const auto player = Cast<APlayerCharacter>(GetOwner());

	if (player && player->IsLocallyControlled()) {
		//Transmit local state to server.
		mPreviousXP = LocalGetXp();
		ServerSetXp(mPreviousXP);
	}
}

namespace xputil { namespace internal {

int clampedAdd(int a, int b, int min, int max) {
	return FMath::Clamp<int64_t>(static_cast<int64_t>(a) + b, min, max);
}

}}
namespace xputil {

int safeXpAdd(int a, int b) {
	return internal::clampedAdd(a, b, 0, std::numeric_limits<int32>::max());
}

}

bool UPlayerExperienceComponent::ConsumeLevels_OnlyFromOwningClient(int32 amount) {
	const auto player = Cast<APlayerCharacter>(GetOwner());
	checkf(player && player->IsLocallyControlled(), TEXT("Consuming levels should only be done on the owning client."));
	if (!player || !player->IsLocallyControlled()) {
		return false;
	}
	checkf(HasBegunPlay(), TEXT("this should never happen before BeginPlay..that might mean we completely wipe player xp progress"));
	if (!HasBegunPlay()) {
		return false;
	}
	auto currentXPCharacterLevel = UDungeonsGameInstance::createCharacterLevel(mXp);
	int32 currentLevel = currentXPCharacterLevel.level;
	int32 targetLevel = currentXPCharacterLevel.level - amount;
	checkf(targetLevel > 0, TEXT("trying to consume more than we have, please guard for this earlier"));
	if (targetLevel <= 0) {
		return false;
	}
	auto currentCharacterLevel = UDungeonsGameInstance::createCharacterLevelForLevel(currentLevel);
	auto targetCharacterLevel = UDungeonsGameInstance::createCharacterLevelForLevel(targetLevel);
	int32 targetXpDelta = targetCharacterLevel.currentXp - currentCharacterLevel.currentXp;		
	checkf(targetXpDelta < 0, TEXT("this should result in a negative xp delta"));
	if (targetXpDelta >= 0) {
		return false;
	}
	int32 newExp = xputil::safeXpAdd(mXp, targetXpDelta);
	player->GetCharacterSerializeComponent()->SetXP(newExp);
	ServerSetXp(newExp);
	return true;	
}

void UPlayerExperienceComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Only replicate xp from server to client once we have received an initial value.
	DOREPLIFETIME_CONDITION(UPlayerExperienceComponent, mXp, COND_Custom);
}

void UPlayerExperienceComponent::PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker) {
	Super::PreReplication(ChangedPropertyTracker);

	DOREPLIFETIME_ACTIVE_OVERRIDE(UPlayerExperienceComponent, mXp, mXpReceivedFromClient);
}

void UPlayerExperienceComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	int32 unclaimedXPAmount = ClaimUnclaimedXp();

	if (unclaimedXPAmount > 0)
	{
		const auto levelBeforeXPAdded = CurrentLevel();

		InternalAddXpClamped(unclaimedXPAmount);
		OnRep_XpChanged(); //must be ran manually on server for offline games

		const auto levelAfterXPAdded = CurrentLevel();
		if (levelBeforeXPAdded < levelAfterXPAdded) {
			MulticastOnLevelUp(levelAfterXPAdded);
			if (auto gameInstance = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance())) {
				analytics::Analytics::GetInstance().FirePlayerLeveledUp(Cast<APlayerCharacter>(GetOwner()), levelAfterXPAdded);
			}
			if (const auto healthComponent = GetOwner()->FindComponentByClass<UHealthComponent>()) {
				healthComponent->ApplyHeal(healthComponent->GetMaximumHealth());
			}
		}
		if (levelAfterXPAdded != levelAfterXPAdded) {
			if (OnLevelChangedInternal.IsBound()) {
				OnLevelChangedInternal.Broadcast();
			}
		}
	}

	//disable tick until next XP feed
	SetComponentTickEnabled(false);
}

int32 UPlayerExperienceComponent::CurrentXp() const {
	return mXp;
}

int32 UPlayerExperienceComponent::CurrentLevel() const {
	return UDungeonsGameInstance::createCharacterLevel(mXp).level;
}

float UPlayerExperienceComponent::LevelProgress() const {
	return UDungeonsGameInstance::createCharacterLevel(mXp).getProgress();
}

int32 UPlayerExperienceComponent::LocalGetXp() const {
	if (UCharacterSerializeComponent* characterSerializer = Cast<APlayerCharacter>(GetOwner())->GetCharacterSerializeComponent()) {
		return (characterSerializer->HasProfile()) ? characterSerializer->ReadXP() : mXp;
	}
	else {
		return 0;
	}
}

void UPlayerExperienceComponent::InternalAddXpClamped(int32 xpToAdd) {
	mXp = xputil::safeXpAdd(mXp, xpToAdd);
}

void UPlayerExperienceComponent::InternalSetXpClamped(int32 newXpValue) {
	mXp = 0;
	InternalAddXpClamped(newXpValue);
}

UDungeonsGameInstance* UPlayerExperienceComponent::GetGameInstance() const {
	return Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance());
}

//Set experience counter on the server, called by the owning client.
void UPlayerExperienceComponent::ServerSetXp_Implementation(int32 xp) {
	InternalSetXpClamped(xp);
	mXpReceivedFromClient = true; //Replicated waits until this condition is true.
	OnRep_XpChanged(); //must be called for server to see its own changes
}

bool UPlayerExperienceComponent::ServerSetXp_Validate(int32 xp) {
	return true;
}

//Error fallback - see AddXP_OnlyFromServer
void UPlayerExperienceComponent::ClientRequestXp_Implementation() {
	ServerSetXp(LocalGetXp());
}

//Increase experience counter and trigger all related events. Logic only runs on server.
void UPlayerExperienceComponent::AddXP_OnlyFromServer(FVector location, int32 amount) {
	if (GetOwner()->HasAuthority()) {
		//Error handling fallback - this should never happen.
		if (!mXpReceivedFromClient) {
			//We have not yet received any initial xp value from client
			UE_LOG(LogTemp, Warning, TEXT("Xp trying to add on server before client has synchronized from its local save."))
			//Fallback here is to keep requesting the xp value until we get it.
			ClientRequestXp();
			//Save unclaimed xp for next time.
			mXpUnclaimed = xputil::safeXpAdd(mXpUnclaimed, amount);
			SetComponentTickEnabled(true);
			return;
		}

		mXpUnclaimed = xputil::safeXpAdd(mXpUnclaimed, amount);
		SetComponentTickEnabled(true);

		
	}	
}

int32 UPlayerExperienceComponent::GetLocalCurrentLevel() const {
	return UDungeonsGameInstance::createCharacterLevel(LocalGetXp()).level;
}

int32 UPlayerExperienceComponent::ClaimUnclaimedXp() {
	if (mXpUnclaimed > 0) {
		const auto claimedAmount = mXpUnclaimed;
		mXpUnclaimed = 0;
		return claimedAmount;
	}
	return 0;
}

void UPlayerExperienceComponent::MulticastOnLevelUp_Implementation(int32 newLevel) {
	//Ui,Effects delegate
	OnLevelUp.Broadcast(newLevel);

	if (auto player = Cast<APlayerCharacter>(GetOwner())) {
		if (auto pc = player->GetPlayerController()) {
			OnLevelUpWithPlayer.Broadcast(newLevel, UGameplayStatics::GetPlayerControllerID(pc), player);
		}
	}
}

void UPlayerExperienceComponent::OnRep_XpChanged() {
	//Are we the local player?
	const auto player = Cast<APlayerCharacter>(GetOwner());
	if (player && player->IsLocallyControlled()) {
		//save new value.
		LocalWriteXp(mXp);

		// D11.SSN
		if (auto* tracker = player->GetStatTracker()) {
			tracker->LevelUp(CurrentLevel());
		}
		player->GetClientEventHubComponent()->XPChanged(mXp - mPreviousXP);
	}
	mPreviousXP = mXp;
	OnDisplayXpChanged.Broadcast();
}

//Serialize
void UPlayerExperienceComponent::LocalWriteXp(const int32 xp) const {
	if (xp > LocalGetXp()) {
		Cast<APlayerCharacter>(GetOwner())->GetCharacterSerializeComponent()->SetXP(xp);
	}
}
