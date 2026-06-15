#include "Dungeons.h"
#include "RegionRenderer.h"
#include "game/Conversion.h"
#include "lovika/Region.h"
#include <DrawDebugHelpers.h>
#include <Components/TextRenderComponent.h>
#include <UObjectGlobals.h>

RegionRenderer::RegionRenderer(USceneComponent& root, float textDegreesRotation)
	: mRoot(&root)
	, mTextDegreesRotation(textDegreesRotation) {
}

RegionRenderer::~RegionRenderer() {
	clear();
}

void RegionRenderer::add(const lovika::Region& region) {
	const auto box = conversion::blockCuboidToUe(region.area());

	DrawDebugBox(mRoot->GetWorld(), box.GetCenter(), box.GetExtent(), region.type().color, true, 60 * 60 * 24 * 365, 0, 5.0f);

	const auto textColor = region.type().color;
	const auto textPos = box.GetCenter() + box.GetExtent().Z / 2.f + 1.0f;

	mTexts.Emplace(createText(region.type().name.c_str(), 100, textColor, textPos));

	const auto text = (region.lowerName() == region.lowerTagString()) ? region.name() : region.name() + '(' + region.tagString() + ')';
	if (!text.empty()) {
		mTexts.Emplace(createText(text.c_str(), 50, textColor, textPos + FVector(-50, 0, 0)));
	}
}

void RegionRenderer::clear() {
	if (mRoot == nullptr) {
		return;
	}
	FlushPersistentDebugLines(mRoot->GetWorld());

	for (auto* text : mTexts) {
		text->DestroyComponent();
	}
	mTexts.Empty();
}

UTextRenderComponent* RegionRenderer::createText(const FString& text, float fontSize, FColor color, FVector pos) {
	const auto obj = NewObject<UTextRenderComponent>(mRoot->GetOwner(), UTextRenderComponent::StaticClass());
	obj->SetWorldSize(fontSize);
	obj->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	obj->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	obj->SetText(FText::FromString(text));
	obj->AttachTo(mRoot.Get());
	obj->SetRelativeLocation(pos);
	obj->AddRelativeRotation(FRotator(90, mTextDegreesRotation, 0));
	obj->RegisterComponent();
	obj->SetTextRenderColor(color);
	return obj;
}
