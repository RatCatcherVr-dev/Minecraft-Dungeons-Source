#include "Dungeons.h"
#include "SplitComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "game/GameBP.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/mobspawn/MobSpawnTypes.h"
#include "EnchantmentComponent.h"
#include "Assets/DungeonsAssetManager.h"
#include "PlayerExperienceComponent.h"
#include "DungeonsGameInstance.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "world/entity/MobTags.h"

USplitComponent::USplitComponent() {
	PrimaryComponentTick.bCanEverTick = false;

	EntitiesToSpawnLevels.Add(99, EntityType::BabyZombie);
	EntitiesToSpawnLevels.Add(3, EntityType::SlimeLarge);
	EntitiesToSpawnLevels.Add(2, EntityType::SlimeMedium);
	EntitiesToSpawnLevels.Add(1, EntityType::SlimeSmall);
}

void USplitComponent::OnAttributeHealthChange(const FOnAttributeChangeData& data) {
	if (data.NewValue > 0.f || HasSplit == true)
		return;

	if (!data.GEModData)
		return;

	Split(effects::GetDungeonsDamageTypes(*data.GEModData));
	ApplyGameEffectsOnDeath();
}

void USplitComponent::Summon(EntityType type, const FVector& offset, const TArray<FEnchantmentData>& enchantments) {
	const auto owner = GetOwner();

	auto transform = owner->GetActorTransform();
	transform.SetTranslation(transform.GetTranslation() + offset);

	FVector ownerLocation = owner->GetActorLocation();


	AGameBP* gameBP = actorquery::getFirstActor<AGameBP>(owner->GetWorld());
	FPushback capturedPushBack = mPushback;

	gameBP->RequestMobGroupSpawn(game::mobspawn::SpawnGroup(type, 1, enchantments, 0.f), RETLAMBDA0(transform), game::mobspawn::configs::DefaultNoVariants(false), [](TArray<AMobCharacter*>& Mobs) {}, [capturedPushBack, ownerLocation](AMobCharacter* pMob) {
		if (pMob)
		{
			FVector direction = pMob->GetActorLocation() - ownerLocation;
			pushback::pushback(capturedPushBack, direction.GetUnsafeNormal(), *pMob);
		}
	});
}

void USplitComponent::ApplyGameEffectsOnDeath()
{
	if (EffectsToApplyOnDeath.Num() > 0)
	{
		const TArray<TEnumAsByte<EObjectTypeQuery>> collisionObjectTypes{ UEngineTypes::ConvertToObjectType(ECC_Pawn), UEngineTypes::ConvertToObjectType(static_cast<ECollisionChannel>(ECustomTraceChannels::PlayerPawn)) };
		const TArray<AActor*> actorsToIgnore;

		TArray<AActor*> targetDamageActors;
		UKismetSystemLibrary::SphereOverlapActors(
			GetWorld(),
			GetOwner()->GetActorLocation(),
			OnDeathRadiusToApplyEffects,
			collisionObjectTypes,
			ABaseCharacter::StaticClass(),
			actorsToIgnore,
			targetDamageActors
		);

		ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner());

		if (UAbilitySystemComponent* OwnerAbilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>())
		{
			for (const auto& targetActor : targetDamageActors) {
				const auto target = Cast<ABaseCharacter>(targetActor);
				const auto mobTarget = Cast<AMobCharacter>(targetActor);
				const bool isCosmetic = (mobTarget && hasMobTag(mobTarget->EntityType, MobTags::HashTag_Cosmetic));

				if (target && OwnerCharacter->IsHostileTowards(target) && !isCosmetic) {
					for (const FEffectTagMagnitude EffectToApply : EffectsToApplyOnDeath) {
						if (auto effectTemplate = Cast<UGameplayEffect>(EffectToApply.GameplayEffect->GetDefaultObject()))
						{
							ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(targetActor);
							UAbilitySystemComponent* TargetAbilitySystem = BaseCharacter ? BaseCharacter->GetAbilitySystemComponent() : targetActor->FindComponentByClass<UAbilitySystemComponent>();
							if (TargetAbilitySystem)
							{
								auto context = OwnerAbilitySystem->MakeEffectContext();
								context.AddInstigator(GetOwner(), GetOwner());
								context.AddSourceObject(GetOwner());
								FGameplayEffectSpec spec(effectTemplate, context, 1);
								for (const FTagMagnitude tagMagnitude : EffectToApply.TagMagnitude)
								{
									spec.SetSetByCallerMagnitude(tagMagnitude.TagName, tagMagnitude.Magnitude);
								}
								OwnerAbilitySystem->ApplyGameplayEffectSpecToTarget(spec, TargetAbilitySystem);
							}
						}
					}
				}
			}
		}
	}
}

void USplitComponent::Split(const FGameplayTagContainer& damageTypes) {
	if (level <= 0) {
		return;
	}

	TArray<FEnchantmentData> enchantments;

	if (auto enchantmentComponent = GetOwner()->FindComponentByClass<UEnchantmentComponent>()) {
		enchantments = enchantmentComponent->GetEnchantments();
	}

	const AMobCharacter* ownerCharacter = Cast<AMobCharacter>(GetOwner());
	const auto ownerType = ownerCharacter->EntityType;

	switch (ownerType) {
		case EntityType::ChickenJockeyTower: {
			const auto backward = ownerCharacter->GetActorForwardVector() * -distance;

			Summon(EntityType::BabyZombie, backward.RotateAngleAxis(offsetDegrees * -2.f, FVector::UpVector), enchantments);
			Summon(EntityType::BabyZombie, backward.RotateAngleAxis(offsetDegrees * -1.f, FVector::UpVector), enchantments);
			Summon(EntityType::BabyZombie, backward.RotateAngleAxis(0, FVector::UpVector), enchantments);
			Summon(EntityType::BabyZombie, backward.RotateAngleAxis(offsetDegrees * 1.f, FVector::UpVector), enchantments);
			Summon(EntityType::BabyZombie, backward.RotateAngleAxis(offsetDegrees * 2.f, FVector::UpVector), enchantments);

			break;
		}
		
		case EntityType::SlimeMedium:
		case EntityType::SlimeLarge: {
			if (damageTypes.HasTag(damageTag::explosion())) {
				if (level > 0) {
					const auto forward = GetOwner()->GetActorForwardVector() * level * distance;

					Summon(EntitiesToSpawnLevels[1], forward.RotateAngleAxis(90.f * 0, FVector::UpVector), enchantments);
					Summon(EntitiesToSpawnLevels[1], forward.RotateAngleAxis(90.f * 1, FVector::UpVector), enchantments);
					Summon(EntitiesToSpawnLevels[1], forward.RotateAngleAxis(90.f * 2, FVector::UpVector), enchantments);
					Summon(EntitiesToSpawnLevels[1], forward.RotateAngleAxis(90.f * 3, FVector::UpVector), enchantments);

					if (level > 1) {
						Summon(EntitiesToSpawnLevels[1], forward.RotateAngleAxis(90.f * 0 + 45.f, FVector::UpVector), enchantments);
						Summon(EntitiesToSpawnLevels[1], forward.RotateAngleAxis(90.f * 1 + 45.f, FVector::UpVector), enchantments);
						Summon(EntitiesToSpawnLevels[1], forward.RotateAngleAxis(90.f * 2 + 45.f, FVector::UpVector), enchantments);
						Summon(EntitiesToSpawnLevels[1], forward.RotateAngleAxis(90.f * 3 + 45.f, FVector::UpVector), enchantments);
					}
				}
			}
			else {
				NormalSlimeSplit(ownerCharacter, enchantments);
			}

			break;
		}
		case EntityType::TropicalSlimeLarge:
		case EntityType::TropicalSlimeMedium:
			NormalSlimeSplit(ownerCharacter, enchantments);
			break;
		case EntityType::MagmaCubeLarge:
		case EntityType::MagmaCubeMedium: {
			const auto type = ownerType == EntityType::MagmaCubeLarge
				? EntityType::MagmaCubeMedium
				: EntityType::MagmaCubeSmall;

			const auto forward = ownerCharacter->GetActorForwardVector() * level * distance;

			Summon(type, forward.RotateAngleAxis(-offsetDegrees, FVector::UpVector), enchantments);
			Summon(type, forward.RotateAngleAxis(offsetDegrees, FVector::UpVector), enchantments);

			break;
		}
	}

	HasSplit = true;
}

void USplitComponent::NormalSlimeSplit(const AMobCharacter* ownerCharacter, const TArray<FEnchantmentData>& enchantments)
{
	const auto type = level >= 3 ? EntitiesToSpawnLevels[3]
		: level == 2 ? EntitiesToSpawnLevels[2]
		: EntitiesToSpawnLevels[1];
	const auto forward = ownerCharacter->GetActorForwardVector() * level * distance;

	Summon(type, forward.RotateAngleAxis(-offsetDegrees, FVector::UpVector), enchantments);
	Summon(type, forward.RotateAngleAxis(offsetDegrees, FVector::UpVector), enchantments);
}
