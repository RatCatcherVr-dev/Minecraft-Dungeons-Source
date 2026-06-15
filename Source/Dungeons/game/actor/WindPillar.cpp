#include "WindPillar.h"
#include "Engine/Engine.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/util/DungeonsEffectLibrary.h"
#include "game/GameBP.h"
#include "game/component/DistancelimitComponent.h"
#include "game/difficulty/Difficulty.h"
#include "game/mobspawn/MobAction.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/util/ActorQuery.h"
#include "game/util/Tags.h"
#include "game/util/LocationQuery.h"
#include <NavigationSystem.h>
#include "Assets/DungeonsAssetManager.h"
#include "game/component/PlayerExperienceComponent.h"
#include "DungeonsGameInstance.h"
#include "EffectsActor.h"
#include "game/util/DungeonsEffectLibrary.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/WorldDamageGameplayEffect.h"


AWindPillar::AWindPillar(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = .25f;

	DebugMessageIndex = 0;
	ActivatedOnce = false;

	bReplicates = true;
}

void AWindPillar::BeginPlay()
{
	Super::BeginPlay();
	ResetTimerDelay();
}

void AWindPillar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWindPillar, BossPillar)
	DOREPLIFETIME(AWindPillar, CurrentCharge)
	DOREPLIFETIME(AWindPillar, State)
}

void AWindPillar::Tick(float DeltaTime)
{
	if (!HasAuthority())
	{
		return;
	}

	Super::Tick(DeltaTime);

	switch (State)
	{
	case EPillarState::Inactive:
		break;
	case EPillarState::Active:
		UpdateState();
		break;
	case EPillarState::DrainCharge:
		DrainCharge();
		TickSpawner(DeltaTime);
		UpdateState();
		break;
	case EPillarState::RegenerateCharge:
		RegenerateCharge();
		UpdateState();
		break;
	case EPillarState::RapidRecharging:
		RapidRecharge();
		break;
	case EPillarState::TimedDelay:
		TimedDelay(DeltaTime);
		break;
	case EPillarState::Depleted:
		PushbackTimerHandle.Invalidate();
		SetPillarState(EPillarState::TimedDelay);
		break;
	case EPillarState::Defeated:
		ClearAllTracking();
		PushbackTimerHandle.Invalidate();
		SetActorTickEnabled(false);
		break;
	default:
		checkNoEntry();
	}

	if (GetDebugMode())
	{
		DebugMessageIndex = GetUniqueID();
		TickDebugging(GetActorTickInterval());
	}
}

EPillarState AWindPillar::GetPillarState() const
{
	return State;
}

void AWindPillar::OnRep_PillarState()
{
	OnPillarStateChange.Broadcast(State);
}

void AWindPillar::SetPillarState(const EPillarState NewState)
{
	if(!HasAuthority())
	{
		return;
	}

	if (NewState != State)
	{
		State = NewState;
		OnRep_PillarState();
	}
}

void AWindPillar::StartRecharge()
{
	ResetTimerDelay();
	SetPillarState(EPillarState::RapidRecharging);
}

void AWindPillar::ResetTimerDelay()
{
	DelayTimer = RapidRechargeDelay;
}

void AWindPillar::ActivatePillar()
{
	if (!ActivatedOnce && (State == EPillarState::Inactive))
	{
		SetPillarState(EPillarState::RapidRecharging);
		ActivatedOnce = true;
		if (InitialWave)
		{
			SpawnInitialWave();
		}
	}
}

void AWindPillar::DebugMessage(const FString& Message, float TimeToDisplay, FColor TextColor)
{
	if (GEngine && GetDebugMode())
	{
		GEngine->AddOnScreenDebugMessage(DebugMessageIndex, TimeToDisplay, TextColor, FString::Printf(TEXT("[%s] %s"), *GetName(), *Message));
		DebugMessageIndex++;
	}
}

void AWindPillar::TickDebugging(float DeltaTime)
{
	if (DebugSpawning)
	{
		const TArray<float> simResults{
			currentWaveSize * ((1.0f - waveSizePerPlayerScalingFactor) + waveSizePerPlayerScalingFactor * 1),
			currentWaveSize * ((1.0f - waveSizePerPlayerScalingFactor) + waveSizePerPlayerScalingFactor * 2),
			currentWaveSize * ((1.0f - waveSizePerPlayerScalingFactor) + waveSizePerPlayerScalingFactor * 3),
			currentWaveSize * ((1.0f - waveSizePerPlayerScalingFactor) + waveSizePerPlayerScalingFactor * 4)
		};

		DebugMessage(FString::Printf(TEXT("Sim Scaled Wave: 1P { %f } | 2P { %f } | 3P { %f } | 4P { %f }"), simResults[0], simResults[1], simResults[2], simResults[3]), DeltaTime, FColor::Cyan);
		DebugMessage(FString::Printf(TEXT("Fraction { %f } | Scaled Wave Size { %f } | Spawn Request Count { %i }"), fraction, debugScaledWaveSize, debugSpawnRequestCount), DeltaTime, FColor::Cyan);
		DebugMessage(FString::Printf(TEXT("Wave Count { %i } | Wave Max { %i }"), mobs.Num(), countMax), DeltaTime, FColor::Cyan);
	}
	
	auto DebugProperties = [&]() -> FString
	{
		FString out = FString::Printf(TEXT("Remaining Charge { %f }"), CurrentCharge);
		if (State == EPillarState::TimedDelay)
		{
			out.Append(FString::Printf(TEXT(" | Timer { %fs }"), DelayTimer));
		}
		if (TrackedPlayerCharacters.Num() > 0)
		{
			out.Append(FString::Printf(TEXT(" | Players { %i }"), TrackedPlayerCharacters.Num()));
		}
		if (TrackedMobCharacters.Num() > 0)
		{
			out.Append(FString::Printf(TEXT(" | Mobs { %i }"), TrackedMobCharacters.Num()));
		}
		if (PushbackTimerHandle.IsValid() && GEngine)
		{
			if (UWorld* World = GEngine->GameViewport->GetWorld())
			{
				out.Append(FString::Printf(TEXT("Pushback Timer { %f }"), World->GetTimerManager().GetTimerRemaining(PushbackTimerHandle)));
			}
		}
		return out;
	};
	DebugMessage(DebugProperties(), DeltaTime);

	auto StateAsString = [&]() -> FString
	{
		switch (State)
		{
		case EPillarState::Inactive:				return "Inactive";
		case EPillarState::Active:					return "Active";
		case EPillarState::DrainCharge:				return "DrainCharge";
		case EPillarState::RegenerateCharge:		return "RegenerateCharge";
		case EPillarState::RapidRecharging:			return "RapidRecharging";
		case EPillarState::TimedDelay:				return "TimedDelay";
		case EPillarState::Depleted:				return "Depleted";
		case EPillarState::Defeated:				return "Defeated";
		default:									
			checkNoEntry();
			return "ErrorState";
		}
	};
	DebugMessage(FString::Printf(TEXT("Pillar State { %s }"), *StateAsString()), DeltaTime);
}

bool AWindPillar::GetDebugMode() const
{
#if WITH_EDITOR || UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
	return DebugMode;
#endif
	return false;
}

bool AWindPillar::AddCharacterToTracking(ABaseCharacter* BaseCharacter)
{
	if (State == EPillarState::Inactive)
	{
		return false;
	}

	int32 res = INDEX_NONE;
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(BaseCharacter))
	{
		res = TrackedPlayerCharacters.AddUnique(PlayerCharacter);
	}
	else if (AMobCharacter* MobCharacter = Cast<AMobCharacter>(BaseCharacter))
	{
		res = TrackedMobCharacters.AddUnique(MobCharacter);
	}
	return res != INDEX_NONE;
}

bool AWindPillar::RemoveCharacterFromTracking(ABaseCharacter* BaseCharacter)
{
	if (State == EPillarState::Inactive)
	{
		return false;
	}

	int32 res = 0;
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(BaseCharacter))
	{
		res = TrackedPlayerCharacters.RemoveSingle(PlayerCharacter);
	}
	else if (AMobCharacter* MobCharacter = Cast<AMobCharacter>(BaseCharacter))
	{
		res = TrackedMobCharacters.RemoveSingle(MobCharacter);
	}
	return res != 0;
}

TArray<APlayerCharacter*> AWindPillar::GetTrackedPlayers()
{
	return TrackedPlayerCharacters;
}


void AWindPillar::ClearAllTracking()
{
	TrackedPlayerCharacters.Empty();
	TrackedMobCharacters.Empty();
}

bool AWindPillar::IsPlayerPresent() const
{
	return TrackedPlayerCharacters.Num() != 0;
}

bool AWindPillar::IsMobPresent() const
{
	return TrackedMobCharacters.Num() != 0;
}

bool AWindPillar::PillarHasCharge() const
{
	return CurrentCharge > 0.f;
}

void AWindPillar::DepleteCharge(bool ResetDelay)
{
	if (ResetDelay)
	{
		ResetTimerDelay();
	}
	SetChargeValue(0.f);
}

void AWindPillar::SetChargeValue(float NewCharge)
{
	CurrentCharge = NewCharge;
}

void AWindPillar::DisablePillar()
{
	SetPillarState(EPillarState::Defeated);
	SetChargeValue(0.f);
}

void AWindPillar::ApplyAreaPushback()
{
	auto DamageTargetCharacter = [&](const ABaseCharacter* TargetCharacter) -> void
	{
		if (const auto targetAbilitySystem = TargetCharacter->GetAbilitySystemComponent())
		{
			auto targetDamageSpec = effects::CreateGameplayEffectSpec<UWorldDamageGameplayEffect>(targetAbilitySystem, effects::HealthName, -PushbackDamage, this, this, GetActorLocation(), FGameplayEffectConstants::INSTANT_APPLICATION);
			targetAbilitySystem->ApplyGameplayEffectSpecToSelf(targetDamageSpec);
		}
	};

	if (PillarHasCharge())
	{
		for (const APlayerCharacter* player : TrackedPlayerCharacters)
		{
			if (player->IsAlive()) {
				UDungeonsEffectLibrary::PushBackOnActor(PushbackValues, this, Cast<AActor>(player));
				DamageTargetCharacter(player);
			}
		}
		for (const AMobCharacter* mob : TrackedMobCharacters)
		{
			if (mob->IsAlive())
			{
				UDungeonsEffectLibrary::PushBackOnActor(PushbackValues, this, Cast<AActor>(mob));
				if (mob->IsFriendlyTowardsPlayers())
				{
					DamageTargetCharacter(mob);
				}
			}
		}
	}
	PushbackTimerHandle.Invalidate();
}

bool AWindPillar::AnyTrackedPlayerIsAlive() const
{
	for (const APlayerCharacter* player : TrackedPlayerCharacters)
	{
		if (player->IsAlive())
		{
			return true;
		}
	}
	return false;
}

void AWindPillar::UpdateState()
{
	if (PillarHasCharge())
	{
		if (CurrentCharge == MaximumCharge && !IsPlayerPresent())
		{
			SetPillarState(EPillarState::Active);
		}
		else if (IsPlayerPresent() && AnyTrackedPlayerIsAlive())
		{
			SetPillarState(EPillarState::DrainCharge);
		}
		else
		{
			SetPillarState(EPillarState::RegenerateCharge);
		}
	}
	else
	{
		if (BossPillar)
		{
			SetPillarState(EPillarState::Depleted);
		}
		else
		{
			SetPillarState(EPillarState::Defeated);
		}
	}
}

void AWindPillar::DrainCharge()
{
	CurrentCharge = FMath::Max(0.f, (CurrentCharge -= 1.f * DrainChargeMultiplier));
}

void AWindPillar::RegenerateCharge()
{
	CurrentCharge = FMath::Min(MaximumCharge, (CurrentCharge += IsMobPresent() ?
		(1.f * RechargeMultiplier) * TrackedMobCharacters.Num() : 1.f * RechargeMultiplier));
}

void AWindPillar::RapidRecharge()
{
	if (CurrentCharge == MaximumCharge)
	{
		SetPillarState(EPillarState::Active);
	}
	else
	{
		CurrentCharge = FMath::Min(MaximumCharge, (CurrentCharge += 1.f * RapidRechargeMultiplier));
	}
}

void AWindPillar::TimedDelay(float DeltaTime)
{
	if (DelayTimer > 0.f)
	{
		DelayTimer -= DeltaTime;
	}
	else
	{
		StartRecharge();
	}
}

void AWindPillar::SpawnInitialWave()
{
	if (!HasAuthority())
	{
		return;
	}

	const auto world = GetWorld();

	const auto playersAlive = actorquery::getActors<APlayerCharacter>(world).FilterByPredicate([](APlayerCharacter* player) { return player->IsAlive(); });

	NumPlayerAlive = playersAlive.Num();

	if (NumPlayerAlive <= 0)
	{
		return;
	}

	const int countCapped = InitialWaveSize - mobs.Num() - requestedSpawnCount;

	if (!game.IsValid())
	{
		game = actorquery::getFirstActor<AGameBP>(world);
	}

	TWeakObjectPtr<AWindPillar> weak_this = this;

	for (auto entityType : game::mobspawn::calculateMobsWithoutDifficultyLimitation({ mobGroup }, countCapped))
	{
		if (const auto maybeTransform = locationquery::retry([&] { return GenerateTransform(world, playersAlive); }))
		{
			const auto& difficulty = game->GetGame() ? game->GetGame()->settings().difficulty : game::FDifficulty::DEFAULT;
			const auto spawnConfig = game::mobspawn::configs::Default(difficulty, false)
				.Actions({ game::mobspawn::AddTag(tags::noDrop), game::mobspawn::AddComponent<UDistancelimitComponent>() });

			FTransform MobTransform;

			if (!game::mobspawn::getSpawnData(*world, RETLAMBDA0(maybeTransform.GetValue()), spawnConfig, entityType, MobTransform))
			{
				continue;
			}

			++requestedSpawnCount;

			// request a spawn with callback
			game->RequestMobSpawn(entityType, MobTransform, {}, spawnConfig, [weak_this](AMobCharacter* pMob) {
				if (weak_this.IsValid())
				{
					if (pMob)
					{
						pMob->SpawnEffectsAtLocation(weak_this->spawnParticles, weak_this->spawnSound, true);
						weak_this->mobs.Add(pMob);
						weak_this->OnSuccessfulMobSpawn.Broadcast();
					}
					weak_this->requestedSpawnCount--;
				}
			});
		}
	}
}
