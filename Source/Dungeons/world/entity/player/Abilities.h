/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

class Abilities {
public:
	Abilities()
		: mInvulnerable(false)
		, mInstabuild(false) {
	}

	bool isWorldBuilder() const { return false; }

	//Stock abilities
	bool mInvulnerable;
	bool mInstabuild;
};
