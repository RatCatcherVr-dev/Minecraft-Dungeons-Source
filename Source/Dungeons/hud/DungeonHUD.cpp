#include "Dungeons.h"
#include "Engine.h"
#include "hud/DungeonHUD.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/ChatComponent.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/SuddenDeath.h"
#include "game/util/ActorQuery.h"
#include "ChartCreation.h"

//For dpi scaling
#include "Runtime/Engine/Classes/Engine/UserInterfaceSettings.h"
#include "CommonTypes.h"

ADungeonHUD::ADungeonHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), IsVisible(true), BoundPlayer(false)
{
	Indicators = CreateDefaultSubobject<UPlayerIndicators>(TEXT("PlayerIndicators"));
	HealthBars = CreateDefaultSubobject<UHealthBars>(TEXT("HealthBars"));
	Emoticons = CreateDefaultSubobject<UPlayerEmoticons>(TEXT("PlayerEmoticons"));
	Objectives = CreateDefaultSubobject<UObjectiveIndicators>(TEXT("ObjectiveIndicators"));
	Hotbar = CreateDefaultSubobject<UHotBarBackground>(TEXT("HotbarBackground"));
}

void ADungeonHUD::BeginPlay() {
	Super::BeginPlay();
	if (auto suddendeath = actorquery::getFirstActor<ASuddenDeath>(GetWorld())) {
		Indicators->SetSuddenDeathActor(suddendeath);
	}
}

void ADungeonHUD::EndPlay(EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	Objectives->ClearOxygenSource(); // This pointer can cause errors in editor unless cleared.
}

const APlayerCharacter* GetPlayerFromId(int32 id, UWorld* world) {
	for (const APlayerCharacter* actor : InstanceTracker<APlayerCharacter>::GetList(world)) {
		if (actor->GetPlayerId() == id) {
			return actor;
		}
	}

	return nullptr;
}

void ADungeonHUD::SetEmote(int32 playerId, EEmote emote) {
	auto player = GetPlayerFromId(playerId, GetWorld());
	Emoticons->SetEmote(PlayerOwner->GetWorld(), player ? player->GetActorLocation() : FVector::ZeroVector, playerId, emote);
}

float ADungeonHUD::GetUMG_DPI_Scale() const {
	FVector2D viewportSize;
	GEngine->GameViewport->GetViewportSize(viewportSize);
	return GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint((int)viewportSize.X, (int)viewportSize.Y));
}

void ADungeonHUD::SetHotbarBackgroundVisibility(bool visible) {
	Hotbar->SetVisible(visible);
}


void ADungeonHUD::SetHotbarBackgroundOffset(FVector2D vector) {
	Hotbar->SetOffset(vector);
}

void ADungeonHUD::SetCoopHotbarBackgroundOffset(int index, FVector2D vector) {
	Hotbar->SetCoopOffset(index, vector);
}

void ADungeonHUD::SetHotbarSize(FVector2D size) {
	Hotbar->SetHotbarSize(size);
}

void ADungeonHUD::SetHotbarBackgroundScaling(float scale)
{
	Hotbar->SetScale(scale);
}

void ADungeonHUD::SetIsVisible(bool visible) {
	IsVisible = visible;
}

void ADungeonHUD::DrawHUD() {
	if (!GEngine) {
		return;
	}

	if (!IsVisible) {
		return;
	}

	Super::DrawHUD();
		
	if (!BoundPlayer) {
		//Player is assumed to never change for player HUD, so it only does this once...
		if(auto playerController = Cast<ABasePlayerController>(PlayerOwner)){
			const auto chatComponent = PlayerOwner->FindComponentByClass<UChatComponent>();
			chatComponent->OnChatEmote.AddUObject(this, &ADungeonHUD::SetEmote);
			BoundPlayer = true;
		}
	}

	auto dpi_scale = GetUMG_DPI_Scale();

	//Mob health bars below hot bar.
	HealthBars->Draw(Canvas, PlayerOwner, dpi_scale, this);

	Hotbar->Draw(Canvas, PlayerOwner, dpi_scale, RenderDelta, this);

	//Indicators should be drawn on top of the hot bar background
	Indicators->Draw(Canvas, PlayerOwner, dpi_scale, this);

	
	Emoticons->Draw(Canvas, PlayerOwner, dpi_scale);
	
	Objectives->Draw(Canvas, PlayerOwner, dpi_scale, this);

	PlayerInfo.Draw(this, PlayerOwner);

	NetworkInfo.Tick(this);
	NetworkInfo.Draw(this);

	//FPSGraph Timer for QA
#if defined(FPS_GRAPH_TIMER) && !UE_BUILD_SHIPPING
	const float fpsGraphTimer = FPerformanceTrackingSystem::FPSChartElapsedTime();
	if (fpsGraphTimer > 0)
	{
		DrawText(FString::Printf(TEXT("%.3f"), fpsGraphTimer), FLinearColor::White, 0, 0, NULL, dpi_scale * 2.0f);
	}
#endif
}