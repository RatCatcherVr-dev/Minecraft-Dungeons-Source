#include "Dungeons.h"
#include "SummonFormationTask.h"
#include "game/GameBP.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/SummonedMobGameplayEffect.h"
#include "game/actor/EffectsActor.h"
#include "game/ai/bt/BtEvalState.h"
#include "game/component/BaseParticleAssetsComponent.h"
#include "game/item/enchantment/EnchantmentData.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/util/LocationQuery.h"
#include "game/util/Tags.h"
#include <AbilitySystemComponent.h>
#include <GameplayPrediction.h>
#include "DungeonsGameInstance.h"
#include "Assets/DungeonsAssetManager.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

USummonFormationTask::USummonFormationTask(
	UAnimSequenceBase* sequence,
	bt::Duration animationDuration,
	bt::Duration summonStart,
	bt::Duration summonCooldown,
	bt::Duration disappearDelay,
	bt::Duration appearDelay,
	const std::function<void(bt::StateRef, AddEntryRef)>& generateEntries,
	SummonRoutine& summon,
	bt::Duration emergeDuration/* = 0s*/
)
	: sequence { sequence }
	, animationDuration { animationDuration }
	, summonStart { summonStart }
	, summonCooldown { summonCooldown }
	, disappearDelay { disappearDelay }
	, appearDelay { appearDelay }
	, generateEntries { generateEntries }
	, summon { summon }
	, emergeDuration { emergeDuration }
{
	name = "summon-formation-task";
}

bool USummonFormationTask::OnCanRun(bt::StateRef state) {
	return earliestStartTime.IsPassed(state);
}

bool USummonFormationTask::OnCanContinue(bt::StateRef state) {
	if (!summonStartTime.IsPassed(state)) {
		return true;
	}
	
	//finished summoning and animating?
	if (hasSummoned && summonQueue.empty() && animationEndTime.IsPassed(state))
	{
		earliestStartTime = bt::TimeStamp::FromNow(state, summonCooldown);
		return false;
	}	
	
	return true;
}

void USummonFormationTask::Init(bt::StateRef state) {
	effectsActor = AEffectsActor::GetInstance(&state.world());
}

void USummonFormationTask::OnStart(bt::StateRef state) {
	hasSummoned = false;
	//startTime = bt::TimeStamp::Now(state);
	startTime = state.world().GetTimeSeconds();
	summonStartTime = bt::TimeStamp::FromNow(state, summonStart);
	animationEndTime = bt::TimeStamp::FromNow(state, animationDuration);

	if (!disappearDelay.isZero()) {
		disappearTime = bt::TimeStamp::FromNow(state, disappearDelay);
		disappearDone = false;

		appearTime = bt::TimeStamp::FromNow(state, appearDelay);
		appearDone = false;
	}
	
	{
		decltype(summonQueue) empty;
		std::swap(summonQueue, empty);
	}
	{
		decltype(particleQueue) empty;
		std::swap(particleQueue, empty);
	}
	
	if (sequence != nullptr) {
		state.owner->MulticastPlayAnimationAsDynamicMontage(sequence, FName(TEXT("FullBody")), 0.f, 0.2f, 1.f, 1, 0, 0, FPredictionKey());
		state.owner->RemoveInvisibility();
	}
}

void USummonFormationTask::OnTick(bt::StateRef state) {
	if (!disappearDelay.isZero()) {
		if (!disappearDone && disappearTime.IsPassed(state)) {
			disappearDone = true;
			state.owner->SetTargetable(false);
			state.owner->SetWorldState(ECharacterWorldState::Disappeared);
		}

		if (!appearDone && appearTime.IsPassed(state)) {
			appearDone = true;
			state.owner->SetTargetable(true);
			state.owner->SetWorldState(ECharacterWorldState::InWorld);
		}
	}

	//D11.SC May distribute this process across ticks
	if (!hasSummoned && summonStartTime.IsPassed(state)) {
		std::vector<SummonEntry> entries;

		const auto ownerCharacter = Cast<ABaseCharacter>(state.owner);

		generateEntries(state, [&entries, &ownerCharacter, &state](const auto& delay, const auto& entityType, const auto& subclass, const auto& transform, bool ignorePlayers, bool transferEnchantments) {
			const auto translation = transform.GetLocation();
			if (const auto ground = locationquery::findGround(state.world(), translation, ignorePlayers)) {
				const FVector groundedTranslation{ translation.X, translation.Y, ground.GetValue() + 1.f };
				if (ownerCharacter->IsLocationReachable(groundedTranslation)) {
					entries.push_back({
						delay,
						entityType,
						subclass,
						{ transform.GetRotation(), groundedTranslation, transform.GetScale3D() },
						ignorePlayers,
						transferEnchantments
					});
				}
			}
		});

		std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) { return a.delay < b.delay; });
		for (auto&& entry : entries) {
			if (!(emergeDuration <= 0s)) {
				particleQueue.push({ entry.delay, entry.transform });

				// not all pre-summon particle systems should be grounded
				//if (const auto ground = actorquery::findGround(state.world(), translation, entry.ignorePlayers)) {
					//particleQueue.push({
						//entry.delay, 
						//{ entry.transform.GetRotation(), { translation.X, translation.Y, ground.GetValue() + 1.f }, FVector::OneVector }
					//});
				//}
			}
			summonQueue.push({ entry.delay + emergeDuration, entry.type, entry.subclass, entry.transform, entry.ignorePlayers, entry.transferEnchantments });
		}

		state.params().totalAttacks.all++;
		state.params().successfulAttacks.all++;
		state.params().lastAttackTime = bt::TimeStamp::Now(state);
		hasSummoned = true;
	}


	while(!particleQueue.empty()) 
	{
		const auto entry = particleQueue.front();

		if (!(summonStartTime + entry.delay).IsPassed(state))
		{
			break;
		}
		
		particleQueue.pop();

		if (effectsActor.IsValid())
		{
			effectsActor->SpawnEffectsAtLocation(
				state.owner->Particles->PreSummon,
				state.owner->preSummonSound,
				entry.transform.GetTranslation()
			);
		}
		
	}

	while(!summonQueue.empty()) 
	{
		const auto entry = summonQueue.front();

		if (!(summonStartTime + entry.delay).IsPassed(state))
		{
			break;
		}
		
		summonQueue.pop();
		summon(state, entry);
		
	}

	if (summonQueue.empty() && !disappearDelay.isZero()) {
		state.owner->SetWorldState(ECharacterWorldState::InWorld);
		state.owner->SetTargetable(true);
	}
}

FGameplayEffectSpecHandle USummonFormationTask::GenerateTransferSpec(bt::StateRef state) {
	auto ownerAbilitySystem = state.owner->GetAbilitySystemComponent();

	const auto meleeDamageMultiplier = ownerAbilitySystem->GetNumericAttribute(UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute());
	const auto rangedDamageMultiplier = ownerAbilitySystem->GetNumericAttribute(URangedAttributeSet::RangedAttackDamageMultiplerAttribute());

	auto spec = ownerAbilitySystem->MakeOutgoingSpec(USummonedMobGameplayEffect::StaticClass(), 1.f, ownerAbilitySystem->MakeEffectContext());
	spec.Data->SetSetByCallerMagnitude("SummonEffectMeleeDamage", meleeDamageMultiplier);
	spec.Data->SetSetByCallerMagnitude("SummonEffectRangedDamage", rangedDamageMultiplier);

	return spec;
}

static FCriticalSection GSpawningCS;

static void QueueMobSpawn(game::mobspawn::ConfigBuilder spawnConfig, bt::StateRef state, const USummonFormationTask::SummonEntry& entry) {
	EntityType entityType = entry.type;
	FTransform MobTransform;

	if (!game::mobspawn::getSpawnData(*state.owner->GetWorld(), RETLAMBDA0(entry.transform), spawnConfig, entityType, MobTransform))
	{
		return;
	}

	AGameBP* game = actorquery::getFirstActor<AGameBP>(state.owner->GetWorld());
	TWeakObjectPtr< AMobCharacter > weak_Owner = state.owner;
	
	auto enchantments = entry.transferEnchantments ? state.owner->GetEnchantmentComponent()->GetEnchantments() : TArray<FEnchantmentData>();

	auto spec = USummonFormationTask::GenerateTransferSpec(state);

	//request a spawn with callback
	game->RequestMobSpawn(entityType, MobTransform, enchantments, spawnConfig, [weak_Owner, spec](AMobCharacter* pMob) {
		if (pMob)
		{
			pMob->GetAbilitySystemComponent()->BP_ApplyGameplayEffectSpecToSelf(spec);
			
			if (weak_Owner.IsValid() && weak_Owner->Particles && weak_Owner->Particles->Summon)
			{
				pMob->SpawnAttachedParticles(weak_Owner->Particles->Summon);
			}
		}
	});
}

void USummonFormationTask::SummonMob(bt::StateRef state, const USummonFormationTask::SummonEntry& entry) {

	//D11.SC Scope lock this to prevent race conditions for now
	FScopeLock ScopeLock(&GSpawningCS);

	const auto spawnConfig = game::mobspawn::configs::Default(state.difficulty(), entry.ignorePlayers).AddTag(tags::noDrop);

	QueueMobSpawn(spawnConfig, state, entry);
}

void USummonFormationTask::SummonMobNoAdjustPosition(bt::StateRef state, const USummonFormationTask::SummonEntry& entry) {

	//D11.SC Scope lock this to prevent race conditions for now
	FScopeLock ScopeLock(&GSpawningCS);

	const auto spawnConfig = game::mobspawn::configs::DefaultNoAdjustPosition(state.difficulty(), entry.ignorePlayers).AddTag(tags::noDrop);

	QueueMobSpawn(spawnConfig, state, entry);
}

void USummonFormationTask::SummonMobWithDrops(bt::StateRef state, const SummonEntry& entry) {

	//D11.SC Scope lock this to prevent race conditions for now
	FScopeLock ScopeLock(&GSpawningCS);

	const auto spawnConfig = game::mobspawn::configs::Default(state.difficulty(), entry.ignorePlayers);

	QueueMobSpawn(spawnConfig, state, entry);
}

void USummonFormationTask::SummonMobUnderling(bt::StateRef state, const USummonFormationTask::SummonEntry& entry) {

	//D11.SC Scope lock this to prevent race conditions for now
	FScopeLock ScopeLock(&GSpawningCS);

	const auto spawnConfig = game::mobspawn::configs::Default(state.difficulty(), entry.ignorePlayers)
		.AddTag(tags::noDrop)
		.SpawnAsUnderling();

	QueueMobSpawn(spawnConfig, state, entry);
}


void USummonFormationTask::SummonActor(bt::StateRef state, const USummonFormationTask::SummonEntry& entry) {

	//D11.SC Scope lock this to prevent race conditions for now
	FScopeLock ScopeLock(&GSpawningCS);

	const auto& positionCorrector = game::mobspawn::positioncorrectors::ByLevelTrace(entry.ignorePlayers);

	if (const auto transform = positionCorrector(state.world(), EntityType::Undefined, entry.transform)) {
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = state.owner;
		{
			state.world().SpawnActor<AActor>(entry.subclass, transform.GetValue(), spawnParams);
		}
	}
}
