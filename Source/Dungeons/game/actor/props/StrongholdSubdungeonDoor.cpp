
#include "Dungeons.h"
#include "StrongholdSubdungeonDoor.h"

#include "game/component/ReplicatedInteractableComponent.h"
#include <Net/UnrealNetwork.h>

AStrongholdSubdungeonDoor::AStrongholdSubdungeonDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;
	InteractableComponent = CreateDefaultSubobject<UReplicatedInteractableComponent>(TEXT("Interactable"));
	InteractableComponent->bOneTimeInteraction = false;
	InteractableComponent->bEditableWhenInherited = true;
	DefaultSceneRootInternal = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	DefaultSceneRootInternal->bEditableWhenInherited = true;
	RootComponent = DefaultSceneRootInternal;
	NetUpdateFrequency = 5.0f;
}

void AStrongholdSubdungeonDoor::BeginPlay() {
	Super::BeginPlay();
	SetActorTickEnabled(true);

}

void AStrongholdSubdungeonDoor::Tick(float DeltaSeconds) {
	if (mInitialized)
		return;

	auto gi = Cast<UDungeonsGameInstance>(GetGameInstance());
	auto pc = Cast<ABasePlayerController>(gi->GetUserManager()->GetInitialPlayerController());
	if (!pc->IsLocalPlayerController())
		return;

	SetupLockedBehavior(pc);
}

void AStrongholdSubdungeonDoor::SetupLockedBehavior(ABasePlayerController* playerController) {
	auto* characterSerialiseComponent = playerController->GetCharacterSerializeComponent();
	const auto& strongholdProgress = characterSerialiseComponent->GetStrongholdData();

	if (InteractableComponent) {
		InteractableComponent->ValidateCallback = [this, &strongholdProgress](const UReplicatedInteractableComponent& component, const AActor* instigator) {
			return strongholdProgress.EyesPlacedInPortalCount() >= this->EyeOfEnderUnlockCount;
		};

		mInitialized = true;
		SetActorTickEnabled(false);
	}
	else
		UE_LOG(LogDungeons, Error, TEXT("No InteractableComponent, check the configuration of the StrongholdSubdungeonDoor!"));
}
