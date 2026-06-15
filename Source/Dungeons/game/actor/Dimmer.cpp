#include "Dungeons.h"
#include "game/level/environment/Environment.h"
#include "Runtime/Engine/Classes/Materials/MaterialParameterCollectionInstance.h"
#include "Dimmer.h"
#include "game/GameBP.h"
#include <LogMacros.h>

ADimmer::ADimmer(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	NetUpdateFrequency = 10.f;
}

void ADimmer::BeginPlay() {
	Super::BeginPlay();
}

float ADimmer::GetNightProgress() const {
	return 1.0f - dayFraction;
}

float ADimmer::GetNightIntensity() const {
	return 1.0f - FMath::Clamp(FMath::Pow(dayFraction, nightLightExponent), 0.0f, nightFractionThreshold) / nightFractionThreshold;
}

float ADimmer::GetLightIntensity() const {
	return FMath::Pow(dayFraction, nightLightExponent);
}

void ADimmer::Tick(float deltaTime) {
	Super::Tick(deltaTime);

	if (Game() && Game()->IsInitDone())
	{
		auto fading = false;

		if (sign > 0.f) {
			if (dayFraction < 1.f) {
				fading = true;
				dayFraction += deltaTime / fadeToDayDuration * sign;

				if (dayFraction >= 1.f) {
					dayFraction = 1.f;
				}
			}
		}
		else if (sign < 0.f) {
			if (dayFraction > 0.0f) {
				fading = true;
				dayFraction += deltaTime / fadeToNightDuration * sign;

				if (dayFraction <= 0.f) {
					dayFraction = 0.f;
					OnEnteredNight();
				}
			}
		}

		if (visualizedDayFraction != dayFraction) {

			const float lightValue = FMath::Pow(dayFraction, nightLightExponent);

			Game()->mEnvironment->dimLights(lightValue, sign < 0.f ? GetNightModeLight() : FNightColor());

			if (materialParameterCollectionInstance.IsValid()) {
				materialParameterCollectionInstance->SetScalarParameterValue(FName{ "DeathEmissive" }, FMath::Lerp(.5f, 1.f, lightValue));
			}
			else {
				materialParameterCollectionInstance = GetWorld()->GetParameterCollectionInstance(materialParameterCollection);
			}


			visualizedDayFraction = dayFraction;
		}
		else if (!fading) {
			SetActorTickEnabled(false);
		}
	}
}

void ADimmer::OnRep_Sign() {
	if (oldSign >= 0.0f) {
		if (sign < 0.f) {
			OnEnterNight();
			OnBecomingNightChanged.Broadcast();
		}
	}

	if (oldSign <= 0.0f)
	{
		if (sign > 0.f) {
			OnExitNight();
			OnBecomingNightChanged.Broadcast();
		}
	}

	if (sign != 0.0f) {
		SetActorTickEnabled(true);
	}

	oldSign = sign;
}

void ADimmer::OnRep_DayFraction() {
	SetActorTickEnabled(true);
}

void ADimmer::EnterNight() {
	ensure(HasAuthority());
	SetNetDormancy(ENetDormancy::DORM_Awake);

	if (const auto gameBp = actorquery::getFirstActor<AGameBP>(GetWorld())) {
		if (gameBp->GetGame()->canEnterNight()) {
			SetSign(-1.f);
		}
	}
}

void ADimmer::ExitNight() {
	ensure(HasAuthority());
	SetSign(1.f);
	SetNetDormancy(ENetDormancy::DORM_DormantAll);
}

void ADimmer::SetSign(float newSign) {
	sign = newSign;
	OnRep_Sign();
}

void ADimmer::PlayNightSound()
{
	if (NightSoundAudioComponent)
	{
		NightSoundAudioComponent->Stop();
	}

	if (NightSoundCue)
	{
		NightSoundAudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), NightSoundCue);

		if (NightSoundAudioComponent)
		{
			NightSoundAudioComponent->Play(0.0f);

			//sound mix manager
			if (!NightSoundMixSet && NightSoundMix)
			{
				auto* pMixManager = GetGameInstance<UDungeonsGameInstance>()->GetSoundMixManager();
				if (pMixManager)
				{
					pMixManager->PushSoundMix(NightSoundMix);
					NightSoundMixSet = true;
				}
			}
		}
	}
}

void ADimmer::StopNightSound()
{
	if (NightSoundAudioComponent)
	{
		NightSoundAudioComponent->FadeOut(1.0f, 0.0f);
		NightSoundAudioComponent = nullptr;
	}

	if (NightSoundMixSet && NightSoundMix)
	{
		auto* pMixManager = GetGameInstance<UDungeonsGameInstance>()->GetSoundMixManager();
		if (pMixManager)
		{
			pMixManager->PopSoundMix(NightSoundMix);
			NightSoundMixSet = false;
		}
	}


}

bool ADimmer::IsNight() const {
	return FMath::Pow(dayFraction, nightLightExponent) <= nightFractionThreshold;
}

void ADimmer::OnEnterNight()
{
	PlayNightSound();
	if (OnEnteringNight.IsBound())
	{
		OnEnteringNight.Broadcast();
	}
}

void ADimmer::OnEnteredNight()
{
	if (OnNightFallen.IsBound())
	{
		OnNightFallen.Broadcast();
	}
}

void ADimmer::OnExitNight()
{
	StopNightSound();
	if (OnExitingNight.IsBound())
	{
		OnExitingNight.Broadcast();
	}
}

bool ADimmer::IsBecomingNight() const {
	return sign < 0.0f;
}

bool ADimmer::IsBecomingDay() const {
	return sign > 0.0f;
}

bool ADimmer::isOceanNight()
{
	if (Game()) {
		auto mission = missions::get(Game()->settings().levelName);
		return mission.GetNightModeType() == ENightModeType::Oceans;
	}
	return false;
}

FNightColor ADimmer::GetNightModeLight()
{
	if (Game()) {
		auto mission = missions::get(Game()->settings().levelName);
		return mission.GetNightModeType() == ENightModeType::Oceans ? FNightColor(FColor::Red) : FNightColor();
	}
	return FNightColor();
}

void ADimmer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//This is animated locally - but represents a long duration animation - so we will want to sync the initial state
	DOREPLIFETIME_CONDITION(ADimmer, dayFraction, COND_InitialOnly);
	DOREPLIFETIME(ADimmer, sign);
}

const game::Game* ADimmer::Game() {
	if (!game) {
		game = actorquery::getGame(GetWorld());
	}
	return game;
}

FNightColor::FNightColor()
	:bCustomColor(false)
{
}

FNightColor::FNightColor(FLinearColor color)
	: bCustomColor(true),
	NightColor(color)
{
}
