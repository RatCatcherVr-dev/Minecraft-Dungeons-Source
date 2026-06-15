#pragma once

#include <type_traits>

class ABaseCharacter;

namespace game { namespace mobspawn {

template <typename T>
MobAction AddComponent(TOptional<bool> setReplicates /*= {}*/) {
#if !PLATFORM_PS4 && !PLATFORM_SWITCH //D11.PS std::is_base_of_v not defined on these platfoms
	static_assert(std::is_base_of_v<UActorComponent, T>, "T needs to be a UActorComponent");
#endif
	return[setReplicates](AMobCharacter& mob) {
		auto component = NewObject<T>(&mob, T::StaticClass());
		component->RegisterComponent();
		component->SetIsReplicated(setReplicates.Get(component->GetIsReplicated()));
	};
}

}}
