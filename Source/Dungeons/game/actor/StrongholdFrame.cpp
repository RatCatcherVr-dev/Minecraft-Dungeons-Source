// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#include "StrongholdFrame.h"
#include "game/component/ReplicatedInteractableComponent.h"
#include "game/component/drop/EyeOfEnderDefines.h"
#include "game/item/ItemUtil.h"
#include "item/StorableItem.h"
#include "DungeonsGameInstance.h"
#include "DungeonsUserManagement.h"
#include "ui/hints/HintManager.h"
#include "game/component/WalletComponent.h"

ABasePlayerController* getLocalPlayerController(UWorld* world) {
	auto* gi = world->GetGameInstance<UDungeonsGameInstance>();
	auto* pc = Cast<ABasePlayerController>(gi->GetUserManager()->GetInitialPlayerController());
	if (!pc->IsLocalPlayerController())
		return nullptr;
	return pc;
}

AStrongholdFrame::AStrongholdFrame() {
	bReplicates = true;
	Interactable = CreateDefaultSubobject<UReplicatedInteractableComponent>(TEXT("Interactable"));
	Interactable->bOneTimeInteraction = false;
	Interactable->bEditableWhenInherited = true;
	DefaultSceneRootInternal = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	DefaultSceneRootInternal->bEditableWhenInherited = true;
	RootComponent = DefaultSceneRootInternal;
	NetUpdateFrequency = 5.0f;
}

void AStrongholdFrame::BeginPlay()
{
	Super::BeginPlay();	

	Interactable->OnReplicatedInteract.AddDynamic(this, &AStrongholdFrame::InteractionBegin);

	UnlockCompletedMissions();
}

void AStrongholdFrame::PlaceEyesInFrame_Implementation(ACharacter* interactor) {
	auto* localPlayerController = getLocalPlayerController(GetWorld());
	if (!localPlayerController)
		return;

	auto* characterSerialiseComponent = localPlayerController->GetCharacterSerializeComponent();
	const auto strongholdData = characterSerialiseComponent->GetStrongholdData();

	TArray<EEyeOfEnderType> heldEyes;
	for (const auto type : eyeofenderquery::AllEyeOfEnderTypes) {
		if (strongholdData.HasEye(type)) {
			heldEyes.Add(type);
		}
	}

	if (heldEyes.Num() > 0) {
		localPlayerController->ServerSpendEye(this, heldEyes, localPlayerController);
	}
	else if (interactor && interactor->IsLocallyControlled()) {
			OnUnlockFailed.Broadcast();
			OnNoEffectEndPortalInteraction.Broadcast(&strongholdData);
	}
}

void AStrongholdFrame::InteractionBegin() {
	if (!HasAuthority()) {
		return;
	}

	LastInteractor = Interactable->GetLastInstigator();
	ForceNetUpdate();
	PlaceEyesInFrame(LastInteractor);
}

void AStrongholdFrame::TrySpendEyes(const TArray<EEyeOfEnderType>& eyeTypes, AActor* spender) {
	if (!HasAuthority()) {
		return;
	}

	auto* localPlayerController = getLocalPlayerController(GetWorld());
	if (!localPlayerController)
		return;

	if (auto* character = Cast<ABasePlayerController>(localPlayerController)->GetControlledPlayerCharacter()) {
		TArray<EEyeOfEnderType> validEyes;
		auto* characterSerialiseComponent = localPlayerController->GetCharacterSerializeComponent();
		auto strongholdData = characterSerialiseComponent->GetStrongholdData();

		for (auto eyeType : eyeTypes) {
			if (!strongholdData.IsUnlocked(eyeType)) {
				validEyes.Add(eyeType);
			}
		}

		// Notify success or failure
		if (validEyes.Num() > 0) {
			SpendEyesSuccess(eyeTypes);
		}
		else if(auto* spenderController = Cast<ABasePlayerController>(spender)) {
			spenderController->ClientFailSpendEye(this);
		}
	}
}

void AStrongholdFrame::SpendEyesSuccess_Implementation(const TArray<EEyeOfEnderType>& eyesSpent) {
	auto* gi = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance());
	auto pcs = gi->GetUserManager()->GetAllLocalPlayerControllers();

	for (auto* pc : pcs) {
		if (auto* character = Cast<ABasePlayerController>(pc)->GetControlledPlayerCharacter()) {
			auto* characterSerialiseComponent = character->GetCharacterSerializeComponent();
			auto strongholdData = characterSerialiseComponent->GetStrongholdData();

			for (auto eyeType : eyesSpent) {
				if (eyeofenderquery::isValid(eyeType)) {
					if (strongholdData.HasEye(eyeType)) {
						character->GetWalletComponent()->Deduct(game::item::type::EyeOfEnder.getId(), 1);
					}

					strongholdData.UnlockAndRemoveEye(eyeType);
					characterSerialiseComponent->AddUnlockKey(eyeofenderquery::unlockString(eyeType).GetValue());
					GetDelegateForEyeOfEnderType(eyeType).Broadcast(true);
				}
			}

			characterSerialiseComponent->SetStrongholdData(strongholdData);
		}
	}

	if (HasAuthority()) {
		auto* localPlayerController = getLocalPlayerController(GetWorld());
		if (!localPlayerController)
			return;

		auto* characterSerialiseComponent = localPlayerController->GetCharacterSerializeComponent();
		const auto strongholdData = characterSerialiseComponent->GetStrongholdData();

		EyesInFrameInstance = strongholdData.EyesPlacedInPortalCount();
		OnRep_EyesInFrameInstance();
	}
}

void AStrongholdFrame::SpendEyesFailure() {
	auto* localPlayerController = getLocalPlayerController(GetWorld());
	if (!localPlayerController)
		return;

	if (LastInteractor && LastInteractor->IsLocallyControlled()) {
		if (auto* character = Cast<APlayerCharacter>(LastInteractor)) {
			if (character->GetWalletComponent()->Balance(game::item::type::EyeOfEnder.getId()) > 0) {
				auto* characterSerialiseComponent = localPlayerController->GetCharacterSerializeComponent();
				auto strongholdData = characterSerialiseComponent->GetStrongholdData();

				OnUnlockFailed.Broadcast();
				OnNoEffectEndPortalInteraction.Broadcast(&strongholdData);
			}
		}
	}
}

bool AStrongholdFrame::IsFirstPortalUse()
{
	auto* gi = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance());
	auto* pc = Cast<ABasePlayerController>(gi->GetUserManager()->GetInitialPlayerController());
	auto* characterSerialiseComponent = pc->GetCharacterSerializeComponent();

	auto strongholdData = characterSerialiseComponent->GetStrongholdData();

	return !strongholdData.usedPortal;
}

void AStrongholdFrame::OnRep_EyesInFrameInstance() {
	OnEyesPlacedUpdated.Broadcast(TOTAL_EYES_OF_ENDER - EyesInFrameInstance);

	if (IsFrameFilled()) {
		bool firstPortalUse = false;

		if (auto* localPlayerController = getLocalPlayerController(GetWorld())) {
			if (auto* characterSerialiseComponent = localPlayerController->GetCharacterSerializeComponent()) {
				auto& strongholdData = characterSerialiseComponent->GetStrongholdData();
				firstPortalUse = !strongholdData.usedPortal;
				characterSerialiseComponent->SetStrongholdUsedPortal();
			}
		}

		OnFrameFilled.Broadcast(firstPortalUse);
	}
}

void AStrongholdFrame::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AStrongholdFrame, EyesInFrameInstance);
	DOREPLIFETIME(AStrongholdFrame, LastInteractor);
}

void AStrongholdFrame::UnlockCompletedMissions()
{
	auto* gi = GetWorld()->GetGameInstance<UDungeonsGameInstance>();
	auto* pc = Cast<ABasePlayerController>(gi->GetUserManager()->GetInitialPlayerController());
	auto* characterSerialiseComponent = pc->GetCharacterSerializeComponent();

	const auto& strongholdData = characterSerialiseComponent->GetStrongholdData();
	for (const auto type : eyeofenderquery::AllEyeOfEnderTypes) {
		if (strongholdData.IsUnlocked(type)) {
			GetDelegateForEyeOfEnderType(type).Broadcast(false);
		}
	}

	if (HasAuthority()) {
		EyesInFrameInstance = strongholdData.EyesPlacedInPortalCount();
		OnRep_EyesInFrameInstance();
	}
}

bool AStrongholdFrame::IsFrameFilled()
{
	return EyesInFrameInstance == TOTAL_EYES_OF_ENDER;
}

const FOnSubDungeonUnlocked& AStrongholdFrame::GetDelegateForEyeOfEnderType(EEyeOfEnderType type) const {
	switch (type) {
	case EEyeOfEnderType::DEEPWOOD_BROOK:            return OnDeepwoodBrookUnlocked;
	case EEyeOfEnderType::OLD_TOWN_HALL:             return OnOldTownHallUnlocked;
	case EEyeOfEnderType::SUNKEN_WATCHTOWER:         return OnSunkenWatchtowerUnlocked;
	case EEyeOfEnderType::THE_UNDERCROFT:            return OnTheUndercroftUnlocked;
	case EEyeOfEnderType::THE_TRIAL_OF_THE_NAMELESS: return OnTheTrialOfTheNamelessUnlocked;
	case EEyeOfEnderType::HIGHBLOCK_HIDEAWAY:        return OnHighblockHideawayUnlocked;
	default:                                         return OnDummyUnlock;
	}
}
