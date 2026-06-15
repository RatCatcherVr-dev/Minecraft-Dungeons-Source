// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/util/Tags.h"
#include "IncreasedMobTargeting.h"


UIncreasedMobTargeting::UIncreasedMobTargeting() {
	TypeID = EArmorPropertyID::IncreasedMobTargeting;
}

void UIncreasedMobTargeting::BeginPlay() {
	Super::BeginPlay();
	const auto owner = GetOwner();

	if (owner->HasAuthority()) {
		owner->Tags.Add(tags::isAttractive);
	}
}

void UIncreasedMobTargeting::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	const auto owner = GetOwner();

	if (owner->HasAuthority()) {
		if (owner->Tags.Find(tags::isAttractive) != INDEX_NONE) {
			owner->Tags.Remove(tags::isAttractive);
		}
	}
}
