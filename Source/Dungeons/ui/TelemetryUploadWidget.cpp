// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "TelemetryUploadWidget.h"

#include "game/Game.h"
#include "util/telemetry/Analytics.h"
#include "game/util/ActorQuery.h"


void UTelemetryUploadWidget::LogGameSessionUserGrade(int grade)
{
#ifdef PLATFORM_WINDOWS  // D11.DJB
	// telemetry
	if (const auto game = actorquery::getGame(GetWorld()))
	{
		analytics::Analytics::GetInstance().FireEventRateMission(grade);
	}
#endif  // PLATFORM_WINDOWS
}

