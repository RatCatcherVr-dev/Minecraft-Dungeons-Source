#include "Dungeons.h"
#include "Engine.h"
#include "CanvasUtility.h"
#include "hud/PlayerEmoticons.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/MapPinComponent.h"
#include "hud/PlayerIndicators.h"
#include "game/emotes/EmoteDefs.h"
#include "util/ClassUtil.h"


const float UPlayerEmoticons::ICON_PADDING = 10.0f;

const float UPlayerEmoticons::MASTER_SCALE = 0.5f;
const float UPlayerEmoticons::ICON_SCALE = 0.7f;
const float UPlayerEmoticons::BUBBLE_SCALE = 1.0f;

const float UPlayerEmoticons::EMOTE_DURATION = 10.0f;
const float UPlayerEmoticons::EMOTE_DETACHED_DURATION = 20.0f;
const float UPlayerEmoticons::EMOTE_FADE_IN_TIME = 0.5f;
const float UPlayerEmoticons::EMOTE_FADE_OUT_TIME = 2.0f;
const float UPlayerEmoticons::TEXTURE_PADDING = 0.0f;

//How much to scale it up when appearing
const float UPlayerEmoticons::EMOTE_JUICE_SCALE = 0.33f;
//How much to make it jump when appearing
const float UPlayerEmoticons::EMOTE_JUICE_JUMP = 2.0f;

const FVector2D UPlayerEmoticons::WORLD_CHARACTER_TALK_OFFSET = { 24.f, -60.f };

UPlayerEmoticons::UPlayerEmoticons(const FObjectInitializer& ObjectInitializer) {
	static const ConstructorHelpers::FObjectFinder<UTexture2D> PlayerEmoticonObj_Bubble(TEXT("/Game/UI/Materials/ChatWheel/New/T_TalkBubble"));
	
	static const ConstructorHelpers::FObjectFinder<UTexture2D> PlayerEmoticonObj_BubbleN(TEXT("/Game/UI/Materials/ChatWheel/bubble_edge_n"));
	static const ConstructorHelpers::FObjectFinder<UTexture2D> PlayerEmoticonObj_BubbleW(TEXT("/Game/UI/Materials/ChatWheel/bubble_edge_w"));
	static const ConstructorHelpers::FObjectFinder<UTexture2D> PlayerEmoticonObj_BubbleS(TEXT("/Game/UI/Materials/ChatWheel/bubble_edge_s"));
	static const ConstructorHelpers::FObjectFinder<UTexture2D> PlayerEmoticonObj_BubbleE(TEXT("/Game/UI/Materials/ChatWheel/bubble_edge_e"));

	static const ConstructorHelpers::FObjectFinder<UTexture2D> PlayerEmoticonObj_DetachedBubble(TEXT("/Game/UI/Materials/ChatWheel/New/T_TalkBubbleDetached"));
	static const ConstructorHelpers::FObjectFinder<UTexture2D> PlayerEmoticonObj_DetachedBubbleShadow(TEXT("/Game/UI/Materials/ChatWheel/New/T_DetachedBubbleShadow"));
	
	BubbleIcon = CanvasUtility::MakeIconFullTexture(PlayerEmoticonObj_Bubble.Object, TEXTURE_PADDING);
	BubbleIconN = CanvasUtility::MakeIconFullTexture(PlayerEmoticonObj_BubbleN.Object, TEXTURE_PADDING);
	BubbleIconW = CanvasUtility::MakeIconFullTexture(PlayerEmoticonObj_BubbleW.Object, TEXTURE_PADDING);
	BubbleIconS = CanvasUtility::MakeIconFullTexture(PlayerEmoticonObj_BubbleS.Object, TEXTURE_PADDING);
	BubbleIconE = CanvasUtility::MakeIconFullTexture(PlayerEmoticonObj_BubbleE.Object, TEXTURE_PADDING);
	DetachedBubbleIcon = CanvasUtility::MakeIconFullTexture(PlayerEmoticonObj_DetachedBubble.Object, TEXTURE_PADDING);
	DetachedBubbleShadow = CanvasUtility::MakeIconFullTexture(PlayerEmoticonObj_DetachedBubbleShadow.Object, TEXTURE_PADDING);

	TalkBubbleSize = FVector2D(PlayerEmoticonObj_Bubble.Object->GetSizeX(), PlayerEmoticonObj_Bubble.Object->GetSizeY());
	TalkBubbleOrientedSize = FVector2D(PlayerEmoticonObj_BubbleN.Object->GetSizeX(), PlayerEmoticonObj_BubbleN.Object->GetSizeY());
	DetachedBubbleSize = FVector2D(PlayerEmoticonObj_DetachedBubble.Object->GetSizeX(), PlayerEmoticonObj_DetachedBubble.Object->GetSizeY());
}

void UPlayerEmoticons::SetEmote(UWorld* world, const FVector& location, int32 playerId, EEmote emote) {
	const auto now = world->GetTimeSeconds();
	const auto emoteDef = emotes::getChecked(emote);
	if (!emoteDef) {
		//Trying to show an invalid emote
		return;
	}

	if (!EmoteCanvasIcons.Find(emote)) {
		EmoteCanvasIcons.Add(emote, UCanvas::MakeIcon(emoteDef->GetIconTexture()));
	}

	auto assignedDuration = 0.0f;
	bool followPlayer = false;

	if (emoteDef->IsDetachable()) {
		const auto until = now + EMOTE_DETACHED_DURATION;
		TalkBubbleMapping.Add(playerId, UPlayerEmoticons::EmoteState { emote, until, location });
		assignedDuration = EMOTE_DETACHED_DURATION;
	}
	else {
		const auto until = now + EMOTE_DURATION;
		TalkBubbleMapping.Add(playerId, UPlayerEmoticons::EmoteState{ emote, until, TOptional<FVector>() });
		assignedDuration = EMOTE_DURATION;
		followPlayer = true;
	}

	SpawnMapActor(playerId, emoteDef->GetMapPinWidgetClassPath(), followPlayer, world, location, assignedDuration);
}

void UPlayerEmoticons::SpawnMapActor(int32 playerId, const FSoftClassPath& classPath, bool followPlayer, UWorld* world, const FVector& location, float duration) {
	if (MapActorMapping.Contains(playerId)) {
		auto activeMapActor = *MapActorMapping.Find(playerId);
		if (activeMapActor.IsValid()) {
			activeMapActor->Destroy();
		}
	}

	if(auto BPClass = classPath.TryLoadClass<UMapPinWidget>()){
		FTransform trans;
		trans.SetLocation(location);
		auto newMapActor = world->SpawnActorDeferred<AChatMapActor>(AChatMapActor::StaticClass(), trans);
	
		newMapActor->SetMapPinWidget(BPClass);
		newMapActor->SetFollowPlayer(followPlayer ? GetPlayerFromId(world, playerId) : nullptr);
		newMapActor->SetLifeSpan(duration);
		UGameplayStatics::FinishSpawningActor(newMapActor, trans);
		MapActorMapping.Add(playerId, newMapActor);
	}
}

float UPlayerEmoticons::GetEmoticonWeight(const FVector2D& location, const FVector2D& viewportSize) const {
	if (
		location.X < 0 || location.X > viewportSize.X || 
		location.Y < 0 || location.Y > viewportSize.Y
	) {
		return 0.f;
	}

	const auto distMin = std::min({ location.X, location.Y, viewportSize.X - location.X, viewportSize.Y - location.Y });
	const auto lerpRadius = 35.f;

	return std::min(distMin / lerpRadius, 1.f);
}

void UPlayerEmoticons::DrawBubble(const EmoteState* iconState, const FVector2D& viewportSize, const APlayerCharacter* actor, UCanvas* canvas, APlayerController* playerOwner, float dpi_scale) const {
	const auto now = playerOwner->GetWorld()->GetTimeSeconds();

	if (now > iconState->until) {
		return;
	}

	bool isLocal = (playerOwner->GetPawn() == actor);

	bool isDetached = iconState->detachedWorldPosition.IsSet();

	const auto fadeInProgress = FMath::Min(1.0f, (now - iconState->until + (isDetached ? EMOTE_DETACHED_DURATION : EMOTE_DURATION)) / EMOTE_FADE_IN_TIME);
	const auto fadeOutProgress = FMath::Clamp((now - iconState->until + EMOTE_FADE_OUT_TIME) / EMOTE_FADE_OUT_TIME, 0.0f, 1.0f);
	const auto barLocation = actorquery::getActorHealthBarPosition(playerOwner, actor);

	const auto juiceScale = 1.0f + EMOTE_JUICE_SCALE * FMath::Min(1.0f, fadeInProgress*2.0f) * (1.0 - fadeInProgress);
	const auto juicePadding = 1.0f + EMOTE_JUICE_JUMP * FMath::Pow(fadeInProgress, 0.2) * (1.0f - fadeInProgress);
	const float juiceOpacity = (1.0f - FMath::Pow(fadeOutProgress, 2)) * FMath::Pow(FMath::Min(1.0f, fadeInProgress*3.0f), 0.25);

	const auto scale = MASTER_SCALE * dpi_scale * juiceScale;
	const float paddingScale = scale * juicePadding;
	const float iconScale = scale * ICON_SCALE;
	const float bubbleScale = scale * BUBBLE_SCALE;

	const FVector2D padding{ paddingScale * ICON_PADDING + UPlayerIndicators::MASTER_SCALE * UPlayerIndicators::EMOTE_PADDING.X * dpi_scale, paddingScale * ICON_PADDING + UPlayerIndicators::MASTER_SCALE * UPlayerIndicators::EMOTE_PADDING.Y * dpi_scale };

	const float scaledPadding = bubbleScale * FMath::Max(TalkBubbleOrientedSize.X, TalkBubbleOrientedSize.Y) * 0.5f;

	auto projectedPosition = CanvasUtility::ProjectLocationToCanvasViewport(isDetached ? iconState->detachedWorldPosition.GetValue() : barLocation, 0.f, canvas, viewportSize);

	auto talkBubbleOffset = isLocal ? FVector2D(0, 0) : WORLD_CHARACTER_TALK_OFFSET;
	auto detachedOffset = FVector2D(0.f, 10.f);

	const auto offsetedPosition = projectedPosition + (isDetached ? detachedOffset : talkBubbleOffset);

	const auto clampedPosition = CanvasUtility::ClampToViewportEdge(projectedPosition, viewportSize, padding + scaledPadding);


	const float opacity = GetEmoticonWeight(projectedPosition, viewportSize);
	const uint8 onScreenAlpha = 255 * juiceOpacity * opacity;
	const uint8 offScreenAlpha = 255 * juiceOpacity * (1 - opacity);

	if (onScreenAlpha > 0) {
		{
			const auto offsetedBubblePosition = offsetedPosition - (isDetached ? DetachedBubbleSize : TalkBubbleSize) * 0.5f * bubbleScale;
			canvas->SetDrawColor(FColor{ 255, 255, 255, onScreenAlpha });
			canvas->DrawIcon(isDetached ? DetachedBubbleIcon : BubbleIcon, offsetedBubblePosition.X, offsetedBubblePosition.Y, bubbleScale);
		}
		{
			const auto& icon = *EmoteCanvasIcons.Find(iconState->emote);
			const auto size = FVector2D(icon.Texture->GetSurfaceWidth(), icon.Texture->GetSurfaceHeight());
			// The arrow of the speech bubble prevents the icon from appearing in the middle of the bubble so we need an offset
			const auto offsetedIconPosition = offsetedPosition - FVector2D(size.X, size.Y + (isDetached ? 0.f : (size.Y * 0.25f))) * 0.5f * iconScale;
			canvas->SetDrawColor(FColor{ 255, 255, 255, onScreenAlpha });
			canvas->DrawIcon(icon, offsetedIconPosition.X, offsetedIconPosition.Y, iconScale);
		}

		if (isDetached)
		{
			float shadowAlpha = 0.2f;
			const auto& icon = DetachedBubbleShadow;
			const auto size = FVector2D(DetachedBubbleShadow.Texture->GetSurfaceWidth(), DetachedBubbleShadow.Texture->GetSurfaceHeight());
			const auto offsetedIconPosition = offsetedPosition - FVector2D(size.X, size.Y - 250.f) * 0.5f * bubbleScale;
			canvas->SetDrawColor(FColor{ 255, 255, 255, static_cast<uint8>(shadowAlpha * onScreenAlpha) });
			canvas->DrawIcon(icon, offsetedIconPosition.X, offsetedIconPosition.Y, bubbleScale);
		}
	}

	if (offScreenAlpha > 0) {
		{
			const auto offsetedClampedPosition = clampedPosition - TalkBubbleOrientedSize * 0.5f * bubbleScale;
			canvas->SetDrawColor(FColor{ 255, 255, 255, offScreenAlpha });

			const auto& bubbleOriented = projectedPosition.Y < padding.Y ? BubbleIconN
				: projectedPosition.X < padding.X ? BubbleIconW
				: projectedPosition.Y > viewportSize.Y - padding.Y ? BubbleIconS
				: BubbleIconE;
			canvas->DrawIcon(bubbleOriented, offsetedClampedPosition.X, offsetedClampedPosition.Y, bubbleScale);
		}

		{
			const auto& icon = *EmoteCanvasIcons.Find(iconState->emote);
			const auto size = FVector2D(icon.Texture->GetSurfaceWidth(), icon.Texture->GetSurfaceHeight());
			const auto offsetedBubblePosition = clampedPosition - size * 0.5f * iconScale;
			canvas->SetDrawColor(FColor{ 255, 255, 255, offScreenAlpha });
			canvas->DrawIcon(icon, offsetedBubblePosition.X, offsetedBubblePosition.Y, iconScale);
		}
	}
}

APlayerCharacter* UPlayerEmoticons::GetPlayerFromId(UWorld* world, int32 ID) const {
	for (APlayerCharacter* actor : InstanceTracker<APlayerCharacter>::GetList(world)) {
		if (ID == actor->GetPlayerId()) {
			return actor;
		}
	}
	return nullptr;
}

void UPlayerEmoticons::Draw(UCanvas* canvas, APlayerController* playerOwner, float dpi_scale) const {
	if (!playerOwner->GetPawn()) {
		return;
	}	

	FVector2D viewportSize;
	GEngine->GameViewport->GetViewportSize(viewportSize);

	const auto owner = playerOwner->GetPawn();
	const auto ownerLocation = owner->GetActorLocation();

	const auto now = playerOwner->GetWorld()->GetTimeSeconds();	

	for (const APlayerCharacter* actor : InstanceTracker<APlayerCharacter>::GetList(playerOwner->GetWorld())) {
		if (
			!actor->GetDungeonsBasePlayerState() ||
			actor->GetWorldState() != ECharacterWorldState::InWorld
		) {
			continue;
		}
		
		const auto* iconState = TalkBubbleMapping.Find(actor->GetPlayerId());

		if (iconState) {
			DrawBubble(iconState, viewportSize, actor, canvas, playerOwner, dpi_scale);
		}
	}
}

AChatMapActor::AChatMapActor() {
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	MapPinComp = CreateDefaultSubobject<UMapPinComponent>(TEXT("MapPinComponent"));
}

void AChatMapActor::Tick(float deltaTime) {
	Super::Tick(deltaTime);

	if (FollowPlayer.IsValid()) {
		SetActorLocation(FollowPlayer->GetActorLocation());
	}
}

void AChatMapActor::SetMapPinWidget(TSubclassOf<UMapPinWidget> mapPinClass) {
	MapPinComp->AddMapPinClass(EMapType::Fullscreen, mapPinClass);
	MapPinComp->AddMapPinClass(EMapType::Overlay, mapPinClass);
}

void AChatMapActor::SetFollowPlayer(APlayerCharacter* player) {
	FollowPlayer = player;
}
