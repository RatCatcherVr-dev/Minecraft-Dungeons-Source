#pragma once

#include <Array.h>

class USceneComponent;
class UTextRenderComponent;

namespace lovika {
	class Region;
}

class RegionRenderer {
public:
	RegionRenderer(USceneComponent& root, float textDegreesRotation = 0);
	~RegionRenderer();

	void add(const lovika::Region&);
	void clear();
private:
	UTextRenderComponent* createText(const FString&, float fontSize, FColor, FVector);

	TWeakObjectPtr<USceneComponent> mRoot;
	float mTextDegreesRotation;
	TArray<UTextRenderComponent*> mTexts;
};
