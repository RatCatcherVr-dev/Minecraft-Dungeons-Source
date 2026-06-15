#include "Dungeons.h"
#include "game/component/HealthComponent.h"
#include "HealthBarComponent.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"

const float UHealthBarComponent::SmoothHealthState::INTERPOLATION_DURATION = 0.5f;
const float UHealthBarComponent::SmoothHealthState::INTERPOLATION_EXPONENT = 3.0f;

const float UHealthBarComponent::SmoothHealthState::FADE_OUT_DELAY = 2.0f;
const float UHealthBarComponent::SmoothHealthState::FADE_OUT_DURATION = 0.25f;

const float UHealthBarComponent::SmoothHealthState::HEALTHBAR_CHANGED_PERCENTAGE_THRESHOLD = 0.001f;
const float UHealthBarComponent::SHOW_HEALTHBAR_HEALTHPERCENTAGE_THRESHOLD = 0.999f;

UHealthBarComponent::UHealthBarComponent() : SmoothState(make_unique<SmoothHealthState>(1.0f, -FLT_MAX)) {
}

void UHealthBarComponent::BeginPlay() {
	Super::BeginPlay();

	if (auto abilitySystem = GetAbilitySystem()) {
		abilitySystem->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::MaxHealthAttribute()).AddUObject(this, &UHealthBarComponent::OnMaxHealthAttributeChanged);
		abilitySystem->RegisterAndCallGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Invisible")), FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &UHealthBarComponent::OnInvisibilityChanged));
	}

	if (auto hc = GetOwner()->FindComponentByClass<UHealthComponent>()) {
		healthComponent = hc;

		hc->OnChanged.AddUObject(this, &UHealthBarComponent::HealthChanged);
		RefreshScale();

		//This is an ugly hack because we depend on the health component to initialize before we can get a real value
		//This is a way to make sure that if this initializes before health component, we only update post-first data
		if(hc->GetCurrentHealthPercentage() > 0.0f){
			UpdateHealth();
		}
	}

	if (auto character = Cast<ABaseCharacter>(GetOwner())) {
		character->OnWorldStateChanged.AddUObject(this, &UHealthBarComponent::RefreshHidden);
	}	
	RefreshHidden();
}



UAbilitySystemComponent* UHealthBarComponent::GetAbilitySystem() const {
	ensure(GetOwner()->IsA<ABaseCharacter>());
	return Cast<ABaseCharacter>(GetOwner())->GetAbilitySystemComponent();
}


void UHealthBarComponent::OnMaxHealthAttributeChanged(const FOnAttributeChangeData& data) {
	RefreshScale();
}


void UHealthBarComponent::RefreshScale() {
	if (healthComponent.IsValid()) {
		const float DesignedMaxHealth = healthComponent->GetConstantDesignedMaximumHealth(); // We want the designed max health - no the attribute system current max health.
		const float ScaledMaxHealth = DesignedMaxHealth / 100.0f;
		const float SnappedMaxHealth = FMath::FloorToFloat(ScaledMaxHealth);
		const float ScaleBonus = SnappedMaxHealth * 0.1f;
		Scale = FMath::Min(2.0f, 1.0f + ScaleBonus);
	}
}

void UHealthBarComponent::OnInvisibilityChanged(FGameplayTag, int32 tagCount) {
	RefreshHidden();
}

void UHealthBarComponent::HealthChanged(const FOnAttributeChangeData& data) {
	UpdateHealth();
}

void UHealthBarComponent::HealthChanged(float amount) {
	UpdateHealth();
}

void UHealthBarComponent::RefreshHidden()
{
	if (auto character = Cast<ABaseCharacter>(GetOwner())) {
		if (!character->EnableHealthBar) {
			bHidden = true;
			return;
		}

		if (character->GetWorldState() == ECharacterWorldState::Disappeared) {
			bHidden = true;
			return;
		}
	}

	if (GetAbilitySystem()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StatusEffect.Invisible"))) {
		bHidden = true;
		return;
	}

	if (auto mob = Cast<AMobCharacter>(GetOwner())) {
		if (!mob->IsUnderling() && mob->IsEventMob()) {
			bHidden = true;
			return;
		}
	}
	if (bDisplayedGlobally) {
		bHidden = true;
		return;
	}

	bHidden = false;
}

void UHealthBarComponent::UpdateHealth() {
	if(healthComponent.IsValid()){
		float currentHealth = healthComponent.Get()->GetCurrentHealth();
		float maxHealth = healthComponent.Get()->GetMaximumHealth();		
		SmoothState->SetTargetPercentage(currentHealth/maxHealth, Now());
		MinOpacity = (currentHealth < maxHealth ? 1.0f : 0.0f);
	}
}

float UHealthBarComponent::GetSmoothHealthPercentage() const {
	return SmoothState->GetSmoothPercentage(Now());
}

float UHealthBarComponent::GetHealthPercentage() const {
	return SmoothState->GetTargetPercentage();
}

float UHealthBarComponent::GetOpacity() const {
	return bHidden ? 0.0f : FMath::Max(MinOpacity, SmoothState->GetOpacity(Now()));
}

float UHealthBarComponent::GetScale() const {
	return Scale;
}

void UHealthBarComponent::SetDisplayedGlobally(bool globally) {
	bDisplayedGlobally = globally;
	RefreshHidden();
}
