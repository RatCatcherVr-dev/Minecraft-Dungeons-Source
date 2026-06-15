#include "EndPillar.h"
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

AEndPillar::AEndPillar(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = .25f;

	DebugMessageIndex = 0;
	ActivatedOnce = false;

	bReplicates = true;
}

void AEndPillar::BeginPlay()
{
	Super::BeginPlay();
	ResetTimerDelay();
}

void AEndPillar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEndPillar, BossPillar)
	DOREPLIFETIME(AEndPillar, CurrentCharge)
	DOREPLIFETIME(AEndPillar, State)
}

void AEndPillar::Tick(float DeltaTime)
{
	if (!HasAuthority())
	{
		return;
	}

	Super::Tick(DeltaTime);

	switch (State)
	{
	case EEndPillarState::Inactive:
		break;
	case EEndPillarState::Active:
		UpdateState();
		break;
	case EEndPillarState::DrainCharge:
		DrainCharge();
		TickSpawner(DeltaTime);
		UpdateState();
		break;
	case EEndPillarState::RegenerateCharge:
		RegenerateCharge();
		UpdateState();
		break;
	case EEndPillarState::RapidRecharging:
		RapidRecharge();
		break;
	case EEndPillarState::TimedDelay:
		TimedDelay(DeltaTime);
		break;
	case EEndPillarState::Depleted:
		PushbackTimerHandle.Invalidate();
		SetPillarState(EEndPillarState::TimedDelay);
		break;
	case EEndPillarState::Defeated:
		ClearAllTracking();
		PushbackTimerHandle.Invalidate();
		SetActorTickEnabled(false);
		break;
	default:
		checkNoEntry();
	}

#if WITH_EDITOR || UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
	if (GetDebugMode())
	{
		DebugMessageIndex = GetUniqueID();
		TickDebugging(GetActorTickInterval());
	}
#endif
}

EEndPillarState AEndPillar::GetPillarState() const
{
	return State;
}

void AEndPillar::OnRep_PillarState()
{
	OnEndPillarStateChange.Broadcast(State);
}

void AEndPillar::SetPillarState(const EEndPillarState NewState)
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

void AEndPillar::StartRecharge()
{
	ResetTimerDelay();
	SetPillarState(EEndPillarState::RapidRecharging);
}

void AEndPillar::ResetTimerDelay()
{
	DelayTimer = RapidRechargeDelay;
}

void AEndPillar::ActivatePillar()
{
	if (!ActivatedOnce && (State == EEndPillarState::Inactive))
	{
		SetPillarState(EEndPillarState::RapidRecharging);
		ActivatedOnce = true;
		if (InitialWave)
		{
			SpawnInitialWave();
		}
	}
}

#if WITH_EDITOR || UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG

void AEndPillar::DebugMessage(const FString& Message, float TimeToDisplay, FColor TextColor)
{
	if (GEngine && GetDebugMode())
	{
		GEngine->AddOnScreenDebugMessage(DebugMessageIndex, TimeToDisplay, TextColor, FString::Printf(TEXT("[%s] %s"), *GetName(), *Message));
		DebugMessageIndex++;
	}
}

void AEndPillar::TickDebugging(float DeltaTime)
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
		if (State == EEndPillarState::TimedDelay)
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
		case EEndPillarState::Inactive:				return "Inactive";
		case EEndPillarState::Active:				return "Active";
		case EEndPillarState::DrainCharge:			return "DrainCharge";
		case EEndPillarState::RegenerateCharge:		return "RegenerateCharge";
		case EEndPillarState::RapidRecharging:		return "RapidRecharging";
		case EEndPillarState::TimedDelay:			return "TimedDelay";
		case EEndPillarState::Depleted:				return "Depleted";
		case EEndPillarState::Defeated:				return "Defeated";
		default:									
			checkNoEntry();
			return "ErrorState";
		}
	};
	DebugMessage(FString::Printf(TEXT("Pillar State { %s }"), *StateAsString()), DeltaTime);
}

#endif

bool AEndPillar::GetDebugMode() const
{
#if WITH_EDITOR || UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
	return DebugMode;
#endif
	return false;
}

bool AEndPillar::AddCharacterToTracking(ABaseCharacter* BaseCharacter)
{
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

bool AEndPillar::RemoveCharacterFromTracking(ABaseCharacter* BaseCharacter)
{
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

TArray<APlayerCharacter*> AEndPillar::GetTrackedPlayers()
{
	return TrackedPlayerCharacters;
}

void AEndPillar::ClearAllTracking()
{
	TrackedPlayerCharacters.Empty();
	TrackedMobCharacters.Empty();
}

bool AEndPillar::IsPlayerPresent() const
{
	return TrackedPlayerCharacters.Num() != 0;
}

bool AEndPillar::IsMobPresent() const
{
	return TrackedMobCharacters.Num() != 0;
}

bool AEndPillar::PillarHasCharge() const
{
	return CurrentCharge > 0.f;
}

void AEndPillar::DepleteCharge(bool ResetDelay)
{
	if (ResetDelay)
	{
		ResetTimerDelay();
	}
	SetChargeValue(0.f);
}

void AEndPillar::SetChargeValue(float NewCharge)
{
	CurrentCharge = NewCharge;
}

void AEndPillar::DisablePillar()
{
	SetPillarState(EEndPillarState::Defeated);
	SetChargeValue(0.f);
}

void AEndPillar::ApplyAreaPushback()
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

bool AEndPillar::AnyTrackedPlayerIsAlive() const
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

void AEndPillar::UpdateState()
{
	if (PillarHasCharge())
	{
		if (CurrentCharge == MaximumCharge && !IsPlayerPresent())
		{
			SetPillarState(EEndPillarState::Active);
		}
		else if (IsPlayerPresent() && AnyTrackedPlayerIsAlive())
		{
			SetPillarState(EEndPillarState::DrainCharge);
		}
		else
		{
			SetPillarState(EEndPillarState::RegenerateCharge);
		}
	}
	else
	{
		if (BossPillar)
		{
			SetPillarState(EEndPillarState::Depleted);
		}
		else
		{
			SetPillarState(EEndPillarState::Defeated);
		}
	}
}

void AEndPillar::DrainCharge()
{
	CurrentCharge = FMath::Max(0.f, (CurrentCharge -= 1.f * DrainChargeMultiplier));
}

void AEndPillar::RegenerateCharge()
{
	CurrentCharge = FMath::Min(MaximumCharge, (CurrentCharge += IsMobPresent() ?
		(1.f * RechargeMultiplier) * TrackedMobCharacters.Num() : 1.f * RechargeMultiplier));
}

void AEndPillar::RapidRecharge()
{
	if (CurrentCharge == MaximumCharge)
	{
		SetPillarState(EEndPillarState::Active);
	}
	else
	{
		CurrentCharge = FMath::Min(MaximumCharge, (CurrentCharge += 1.f * RapidRechargeMultiplier));
	}
}

void AEndPillar::TimedDelay(float DeltaTime)
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

void AEndPillar::SpawnInitialWave()
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

	TWeakObjectPtr<AEndPillar> weak_this = this;

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
