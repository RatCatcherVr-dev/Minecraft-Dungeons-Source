/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

template <typename EntityType, typename ConditionType, typename RuntimeType>
inline bool _tryInitializeComponent(EntityType *e, ConditionType && condition, Unique<RuntimeType> &component) {
	// Check whether the component should be initialized
	if (bool(condition)) {
		if (!component) {
			component = make_unique<RuntimeType>(*e);
		}
		// Run initialization code
		component->initFromDefinition();
		return true;
	}
	else {
		component.reset();
		return false;
	}
}
