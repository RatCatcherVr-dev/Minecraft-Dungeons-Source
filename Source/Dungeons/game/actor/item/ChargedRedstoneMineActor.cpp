#include "Dungeons.h"
#include "ChargedRedstoneMineActor.h"
#include "AIController.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "UnrealNetwork.h"
#include "game/util/Pushback.h"
#include "AbilitySystemComponent.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "DungeonsGameInstance.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/component/EquipmentComponent.h"
#include "game/item/instance/ChargedRedstoneMinesInstance.h"
#include "game/team/TeamQuery.h"
#include "util/CharacterQuery.h"
#include "world/entity/MobTags.h"

AChargedRedstoneMineActor::AChargedRedstoneMineActor(const class FObjectInitializer& OI) : Super(OI) {
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	MineCollision = CreateDefaultSubobject<USphereComponent>(TEXT("MineCollission"));
	MineCollision->SetCollisionProfileName(TEXT("PawnTrigger"));
	MineCollision->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	MineCollision->AttachTo(Root);
}

void AChargedRedstoneMineActor::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		MineCollision->OnComponentBeginOverlap.AddDynamic(this, &AChargedRedstoneMineActor::OnOverlapBegin);
	}
}

void AChargedRedstoneMineActor::OnOverlapBegin(UPrimitiveComponent * ThisComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	const AMobCharacter* mob = Cast<AMobCharacter>(OtherActor);
	const bool isMobCosmeticOrFloating = mob && (mob->HasTag(MobTags::HashTag_Cosmetic) || mob->HasTag(MobTags::HashTag_Floating));
	const bool isMobPlayerPet = mob && (mob->HasTag(MobTags::HashTag_Pet));

	if (OtherActor->IsA<ABaseCharacter>() && !isMobCosmeticOrFloating && !isMobPlayerPet) {
		Explode();
	}
}

void AChargedRedstoneMineActor::Explode() {
	if (HasAuthority()) {
		bShouldExpire = true;
		MineCollision->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		const auto pred = [this](const ABaseCharacter* character) { return character->IsAlive() && actorquery::getActorDistance(this, character) <= (ExplosionRadius + actorquery::getActorCapsuleRadius(character)); };
		ETeamName instigatorTeam = effects::GetDungeonsContextFromSpec(HostileSpec)->InstigatorTeam;

		for (auto actor : actorquery::getInstanceTrackedActors<ABaseCharacter>(GetWorld(), pred)) {
			FGameplayEffectSpec spec = teamquery::is::friendly(instigatorTeam, actor->GetCurrentTeam()) ? FriendlySpec : HostileSpec;
			spec.DuplicateEffectContext();

			FPushback targetPushback = Pushback;
			targetPushback.pushbackStrength *= 1.f - FMath::Clamp(GetDistanceTo(actor) / ExplosionRadius, 0.f, 1.f);
			effects::StorePushbackInNormal(spec, pushback::getLaunchVector(targetPushback, *this, *actor, 1.5f, 1.0f));
			pushback::pushback(targetPushback, *this, *actor);
			actor->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(spec);
		}

		MulticastExplode();
		SetLifeSpan(3.f);
	}
}

void AChargedRedstoneMineActor::Init(const FGameplayEffectSpec& hostile, const FGameplayEffectSpec& friendly) {
	HostileSpec = hostile;
	FriendlySpec = friendly;

	HostileSpec.DuplicateEffectContext();
	FriendlySpec.DuplicateEffectContext();

	HostileSpec.GetContext().AddOrigin(GetActorLocation());
	FriendlySpec.GetContext().AddOrigin(GetActorLocation());
}

void AChargedRedstoneMineActor::MulticastExplode_Implementation() {
	OnExplode();
}

void AChargedRedstoneMineActor::LifeSpanExpired() {
	if (HasAuthority() && !bShouldExpire) {
		Explode();
	} else {
		Super::LifeSpanExpired();	
	}
}