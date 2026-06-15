#include "Dungeons.h"
#include "AncientMobAudioComponent.h"

UAncientMobAudioComponent::UAncientMobAudioComponent(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	AncientAmbienceSound = CreateDefaultSubobject<UAudioComponent>(TEXT("AncientAmbienceSound"));
	AncientAmbienceSound->SetupAttachment(this);
	AncientAmbienceSound->bAutoActivate = false;
	AncientAmbienceSound->bAutoManageAttachment = true;

	AncientMobDeath = CreateDefaultSubobject<UAudioComponent>(TEXT("AncientMobDeath"));
	AncientMobDeath->SetupAttachment(this);
	AncientMobDeath->bAutoActivate = false;
	AncientMobDeath->bAutoManageAttachment = true;
	
}

void UAncientMobAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AMobCharacter* pMob = Cast<AMobCharacter>(GetOwner()))
	{
		pMob->AncientMobAudioComponent = this;
		AncientMobDeath->AutoAttachParent = pMob->GetRootComponent();
		AncientAmbienceSound->AutoAttachParent = AncientMobDeath->AutoAttachParent;
		AncientAmbienceSound->FadeIn(0.0f);
	}
}

void UAncientMobAudioComponent::OnMobDeath()
{
	AncientAmbienceSound->FadeOut(0.0f,0.0f);
	AncientMobDeath->FadeIn(0.0f);
}

