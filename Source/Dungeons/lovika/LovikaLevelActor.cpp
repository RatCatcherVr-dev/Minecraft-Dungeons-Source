#include "Dungeons.h"

#include "LovikaLevelActor.h"
#include "GameplayCueManager.h"
#include "AbilitySystemGlobals.h"
#include "builder/LovikaGeneratorBuilder.h"
#include "client/renderer/texture/TextureAtlas.h"
#include "game/Conversion.h"
#include "game/GameTypes.h"
#include "game/level/instancing/InstancedLeavesActor.h"
#include "world/phys/HitResult.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "world/level/ChunkBlockSource.h"
#include "LoadingScreen/LoadingScreenInitializer.h"

#include "game/util/EnvironmentUtils.h"
#include "FXSystem.h"
#include "client/resource/Resource.h"


ALovikaLevelActor::ALovikaLevelActor() {
	UE_LOG(LogTemp, Display, TEXT("Running ALovikaLevelActor::ALovikaLevelActor constructor %x"), this);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	RootComponent->SetMobility(EComponentMobility::Static);

	static ConstructorHelpers::FClassFinder<AActor> fireBPClassFinder(*(game::PrefabPath("Decor/Prefabs/Blueprints/BP_Fire")));
	if (fireBPClassFinder.Class != nullptr)
	{
		fireBPClass = fireBPClassFinder.Class;
	}
	
	mBuilder = CreateDefaultSubobject<ULovikaGeneratorBuilder>(TEXT("GeneratorBuilder"));
	mResourcePacks = { ResourcePack::DEFAULT };
}

void ALovikaLevelActor::rebuildTextures() {
	if (doRefresh && mBuilder) {
		mBuilder->init(mResourcePacks, mGame);
		DebugAtlasTexture = mBuilder->getMostRecentResourcePackTextureAtlas().atlasTexture;
		DebugAtlasTextureEmissive = mBuilder->getMostRecentResourcePackTextureAtlas().atlasTextureEmissive;
		RegisterAllComponents();
		doRefresh = false;
	}
}

void ALovikaLevelActor::buildInstancedLeaves(const game::Tiles& tiles) {
	if (auto* instancedLeavesActor = actorquery::getFirstActor<AInstancedLeavesActor>(GetWorld())) {
		for (auto* tile : tiles.getTiles()) {
			instancedLeavesActor->Place(*tile);
		}
	}
}

void ALovikaLevelActor::OnConstruction(const FTransform& transform) {
	Super::OnConstruction(transform);

	if (!IsRunningCommandlet()) {
		rebuildTextures(); //@todo: remove a few of these
		UE_LOG(LogTemp, Display, TEXT("Running ALovikaLevelActor::OnConstruction"));
	}
}

void ALovikaLevelActor::PreInitializeComponents() {
	UE_LOG(LogTemp, Display, TEXT("Running ALovikaLevelActor::PreInitializeComponents"));

	/// Check for Dungeons level override param:
	if (FParse::Param(FCommandLine::Get(), TEXT("DungeonsLevel")))
	{
		FString value = "";
		FParse::Value(FCommandLine::Get(), TEXT("DungeonsLevel"), value, false);

		UE_LOG(LogDungeons, Display, TEXT("override level to load with: '%s' in params[]"), *value);
		TOptional<ELevelNames> level = EnumValueFromString(ELevelNames, value);
		if (level.IsSet())
			developmentLevelId = level.GetValue();
	}

 	// rebuildTextures(); //@todo: remove a few of these
}

namespace {

void StopCharacterMovements(const TArray<ABaseCharacter*>& characters) {
	for (auto* character : characters) {
		if (UCharacterMovementComponent* movementComponent = character->FindComponentByClass<UCharacterMovementComponent>()) {
			movementComponent->DisableMovement();
		}
	}
}

void ResumeCharacterMovements(const TArray<ABaseCharacter*>& characters) {
	for (auto* character : characters) {
		if (UCharacterMovementComponent* characterMovement = character->FindComponentByClass<UCharacterMovementComponent>()) {
			characterMovement->SetDefaultMovementMode();

			// hack/fix mobs walking in place; this resets their state
			characterMovement->Launch(FVector::UpVector);
		}
	}
}

}

void ALovikaLevelActor::Tick(float DeltaSeconds)
{
	StopCharacterMovements(InstanceTracker<ABaseCharacter>::GetList(GetWorld()));
	
	const auto game = actorquery::getFirstActor<AGameBP>(GetWorld());	

	// Wait until all asynchronous local loading tasks are finished.
	if (game && mBuilder->HasGeneratedWorldMeshes() &&
		mGame->HasLoadedSublevels() &&
		game->IsInitialisationComplete() &&
		mGame->LoadAsyncAndPollIntroCinematic()) {

		// Tell the server we're ready!
		CharactersReadyToPlay();

		// Let the server start adding players to the level
		if (game->HasAuthority() && !game->IsReadyForPlayers()) {
			game->SetReadyForPlayers_ServerOnly();
		}

		// Mobs spawn around players, so we must wait until players are added and mobs have started spawning.
		// This is at most two ticks (ADungeonsGameMode to add players and AGameBP to spawn mobs), but adding one for some margin.
		if (game->HasPendingMobSpawns()) {
			mFramesWithoutPendingMobs = 0;
		}
		else {
			mFramesWithoutPendingMobs++;
		}

		// Now that everything is loaded locally, wait for the server to add us to the game and start
		if (AreAllLocalPlayersLoaded() && mFramesWithoutPendingMobs > 2) {
			if (game->HasAuthority()) {
				game->StartGame_ServerOnly();
			}

			if (game->IsStarted()) {
				
				auto GameInstance = GetGameInstance<UDungeonsGameInstance>();
				GameInstance->GetLoadingScreenInitializer()->TearDownLoadingScreen(GetWorld(), { 1.0f,1.0f });
				GameInstance->LevelStarted(); //D11.PS added this for xcloud delegate
				ResumeCharacterMovements(InstanceTracker<ABaseCharacter>::GetList(GetWorld()));

				game->StartGameLocally(ShouldPlayIntro());

				SetActorTickEnabled(false);
				PrimaryActorTick.bCanEverTick = false;
#if !WITH_EDITOR
				FXConsoleVariables::bFreezeParticleSimulation = false;
#endif
			}
		}
	}
}

bool ALovikaLevelActor::AreAllLocalPlayersLoaded() const {
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController->IsLocalController()) {
			if (APlayerCharacter* character = Cast<APlayerCharacter>(PlayerController->GetCharacter())) {
				if (!character->IsLoadedInLevel()) {
					return false;
				}
			}
		}
	}

	return true;
}
void ALovikaLevelActor::CharactersReadyToPlay() {
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController->IsLocalController()) {
			if (APlayerCharacter* character = Cast<APlayerCharacter>(PlayerController->GetCharacter())) {
				character->SetReadyToPlay();
			}
		}
	}
}

bool ALovikaLevelActor::ShouldPlayIntro() const {
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController->IsLocalController()) {
			if (APlayerCharacter* character = Cast<APlayerCharacter>(PlayerController->GetCharacter())) {
				if (!character->IsAllowedToSeeIntro()) {
					return false;
				}
			}
		}
	}
	return true;
}

void ALovikaLevelActor::setupGameAndBuildMesh(game::Game& game, ChunkBlockSource& region, const std::vector<ResourcePack>& resourcePacks) {
	mGame = &game;
	mRegion = &region;
	mResourcePacks = resourcePacks;
	doRefresh = true;
	rebuildTextures(); //@todo: remove a few of these (also: hack)
	
	mBuilder->build(useAmbientOcclusion, &region, game.tiles(), game.missionDef().isHyperMission());
	SetActorTickEnabled(true);

	buildInstancedLeaves(game.tiles());
}

FLevelHitResult ALovikaLevelActor::levelTrace(FVector from, FVector to, bool liquid, bool solidOnly) {
	if (mRegion) {
		FVector levelOrigin = GetActorLocation();
		HitResult hitResult = mRegion->clip(conversion::ueToPos(from - levelOrigin), conversion::ueToPos(to - levelOrigin), liquid, solidOnly);
		if (hitResult.isTile()) {
			return FLevelHitResult(true, hitResult.isHitLiquid(), static_cast<EMaterialTypeEnum>(mRegion->getMaterial(hitResult.getBlock()).getType()));
		}
	}
	return FLevelHitResult();
}

void ALovikaLevelActor::BeginPlay() {
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("Running ALovikaLevelActor::BeginPlay"));

	// Need to init Singleton GameplayCueManager to avoid Lazy loading stuttering (async or not)
	UGameplayCueManager* gameplayCueManager = UAbilitySystemGlobals::Get().GetGameplayCueManager();
	mRegionRenderer = std::make_unique<RegionRenderer>(*GetRootComponent(), 180);
}


void ALovikaLevelActor::AbortMeshGeneration()
{
	if (mBuilder) {
		mBuilder->AbortMeshGeneration();
	}
}

void ALovikaLevelActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	AbortMeshGeneration();
}

const TOptional<FLevelSettings>& ALovikaLevelActor::getGeneratedLevelSettings() const
{
	return generatedLevelSettings;
}

//Blueprint access
FLevelSettings ALovikaLevelActor::getGeneratedLevelSettingsOrEmpty() const
{
	return generatedLevelSettings.Get(FLevelSettings());
}

void ALovikaLevelActor::setGeneratedLevelSettings(const FLevelSettings& levelSettings)
{
	generatedLevelSettings = levelSettings;
}

bool ALovikaLevelActor::IsShowingRegions() const {
	return isShowingRegions;
}

void ALovikaLevelActor::SetShowingRegions(bool show) {
	if (isShowingRegions == show) {
		return;
	}
	if (!mRegionRenderer || !mGame) {
		return;
	}
	isShowingRegions = show;
	mRegionRenderer->clear();

	if (isShowingRegions) {
		for (const auto tile : mGame->tiles().getTiles()) {
			for (const auto& region : tile->tilePlacement().regions()) {
				mRegionRenderer->add(region);
			}
		}
	}
}

UActorComponent* ALovikaLevelActor::FindComponentByClass(const TSubclassOf<UActorComponent> ComponentClass) const
{
	if (ComponentClass.Get() == fireBPClass)
	{
		return Super::FindComponentByClass(ComponentClass);
	}
	//D11.SC - Things are searching the level actor's 4000+ components for something that doest exist
	return nullptr;
}

bool ALovikaLevelActor::IsReady() const
{
	return (mBuilder && mBuilder->HaveMeshGenTasksCompleted());
}


//
// Console commands
//

static void CMD_ShowRegions(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (commands.Num() == 0) {
		return;
	}
	if (auto level = actorquery::getFirstActor<ALovikaLevelActor>(world)) {
		const bool show = commands[0] != "0";
		level->SetShowingRegions(show);
	}
}

static const FAutoConsoleCommand ToggleDebugDrawRegions(TEXT("Dungeons.DebugDraw.Regions")
	, TEXT("Toggles debug rendering of regions.")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&CMD_ShowRegions)
	, ECVF_Cheat);
