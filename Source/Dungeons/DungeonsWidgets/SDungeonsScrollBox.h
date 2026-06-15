#pragma once

#include "CoreMinimal.h"
#include <SScrollBox.h>

class DUNGEONS_API SDungeonsScrollBox : public SScrollBox {
public:
	void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;
private:
};
