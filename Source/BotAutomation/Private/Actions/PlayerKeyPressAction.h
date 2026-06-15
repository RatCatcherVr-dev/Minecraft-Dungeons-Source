#pragma once

#include "PlayerBotActionBase.h"
#include "InputCoreTypes.h"

class BOTAUTOMATION_API PlayerKeyPressAction : public PlayerBotActionBase
{
private:
	typedef PlayerBotActionBase Super;
public:
	PlayerKeyPressAction(FString Name, FKey Key, bool Viewport = false)
		: Super(2.0, Name)
		, CurrentKey(Key)
		, Viewport(Viewport)
		, Pressed(false)
	{
	}

protected:
	bool ExecuteAction(float DeltaSeconds) override;

private:
	FKey CurrentKey;
	bool Viewport;
	bool Pressed;
};
