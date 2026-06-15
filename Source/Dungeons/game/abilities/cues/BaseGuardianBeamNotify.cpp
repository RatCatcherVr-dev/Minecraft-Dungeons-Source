#include "BaseGuardianBeamNotify.h"
#include "game/util/ActorQuery.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/mob/MobCharacter.h"

ABaseGuardianBeamNotify::ABaseGuardianBeamNotify() {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bAutoAttachToOwner = true;
	bAutoDestroyOnRemove = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	BeamVFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BeamVFXComponent"));
	BeamVFX->SetupAttachment(RootSceneComponent);
}

void ABaseGuardianBeamNotify::Tick(const float DeltaTime) {
	if (ABaseCharacter* characterOwner = Cast<ABaseCharacter>(GetOwner())) {
		const FVector beamOrigin = BeamVFX->GetComponentLocation();
		FVector beamDirection = characterOwner->GetActorForwardVector();
		FVector beamEndLocation = beamDirection * BeamRange + beamOrigin;

		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
		if (!Piercing) {
			ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
			ObjectParams.AddObjectTypesToQuery(ECC_GameTraceChannel11); // Player Pawn
		}

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(characterOwner);

		TArray<AMobCharacter*> friendlyMobs;
		UActorQuery::GetFriendlyMobCharactersInRange(this, characterOwner, FVector::Distance(beamOrigin, beamEndLocation), friendlyMobs);
		for (AMobCharacter* friendlyMob : friendlyMobs) {
			QueryParams.AddIgnoredActor(friendlyMob);
		}

		while (true) {
			FHitResult res;
			if (GetWorld()->LineTraceSingleByObjectType(res, beamOrigin, beamEndLocation, ObjectParams, QueryParams)) {
				if (const ABaseCharacter* hitCharacter = Cast<ABaseCharacter>(res.GetActor())) {
					const UHealthComponent* healthComponent = res.GetActor()->FindComponentByClass<UHealthComponent>();
					if (!healthComponent || !healthComponent->ShouldImpactProjectile(FGameplayTag::RequestGameplayTag(TEXT("Damage.Medium")))) {
						QueryParams.AddIgnoredActor(res.GetActor());
						continue;
					}
				}
				BeamVFX->SetBeamTargetPoint(0, res.ImpactPoint, 0);
				return;
			}
			break;
		}
		BeamVFX->SetBeamTargetPoint(0, beamEndLocation, 0);
	}
}