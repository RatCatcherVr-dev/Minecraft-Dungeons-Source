#include "Dungeons.h"
#include "HealthActions.h"
#include "game/ai/provider/Actors.h"
#include "game/component/RagdollOnDeathComponent.h"
#include "game/actor/character/mob/MobCharacter.h"

namespace bt { namespace health {

Action Kill(actor::Provider prov, FGameplayTag damageType, float impulseMagnitude) {
	return [provider = std::move(prov), damageType, impulseMagnitude](StateRef state) {
		if(const AActor* target = provider(state)) {
			if (UHealthComponent* healthComponent = target->FindComponentByClass<UHealthComponent>()) {
				if (impulseMagnitude > 0.f) {
					if (const auto ragdollDeathComponent = target->FindComponentByClass<URagdollOnDeathComponent>()) {
						//Set any pushback to be applied to ragdolls (modified with a bonus amount and extra strength)
						//Hack: This vector make sense since it doesn't go via pushback::getLaunchVector, but on the other hand, we're not really using this code anywhere...
						ragdollDeathComponent->LaunchOrStoreRagdollImpulse(FVector::UpVector * impulseMagnitude);
					}
				}
				healthComponent->Kill();
			}
		}
	};
}

// D11.DB
bt::Action Heal(actor::Provider prov, float factor) {
	return[provider = std::move(prov), factor](StateRef state) {
		if (const AActor* target = provider(state)) {
			if (UHealthComponent* healthComponent = target->FindComponentByClass<UHealthComponent>()) {
				float healAmount = healthComponent->GetMaximumHealth() * factor;
				healthComponent->ApplyHeal(healAmount);
			}
		}
	};
}

}}
