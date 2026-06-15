// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Dungeons/LoadingScreen/STransformAnimationWidget.h"
#include "Rendering/DrawElements.h"
#include <Engine.h>

FTransformationCurve::FTransformationCurve(TWeakPtr<SWidget> owningWidget, FInterpolationData<FLinearColor> colorInterpolation, FInterpolationData<FVector2D> positionInterpolation, FInterpolationData<float> scaleInterpolation, FInterpolationData<FBox2D> clipBoxInterpolation, float duration, ECurveEaseFunction curveEaseFunction) :
	OwningWidget(owningWidget), ColorInterpolation(std::move(colorInterpolation)), PositionInterpolation(std::move(positionInterpolation)), ScaleInterpolation(std::move(scaleInterpolation)), ClipBoxInterpolation(std::move(clipBoxInterpolation)), DurationSeconds(duration) {
	CurveSequence = FCurveSequence();

	CurveSequence.AddCurve(0.f, DurationSeconds, curveEaseFunction);
} 

void FTransformationCurve::Start() {
	CurveSequence.Play(OwningWidget.Pin().ToSharedRef());
}

void FTransformationCurve::Pause() {
	CurveSequence.Pause();
}

void FTransformationCurve::Resume() {
	CurveSequence.Resume();
}

bool FTransformationCurve::IsCompleted() const {
	return CurveSequence.IsAtEnd();
}

FLinearColor FTransformationCurve::GetColor() const {
	return FMath::Lerp(ColorInterpolation.Start, ColorInterpolation.End, CurveSequence.GetLerp());
}

FVector2D FTransformationCurve::GetPosition() const {
	return FMath::Lerp(PositionInterpolation.Start, PositionInterpolation.End, CurveSequence.GetLerp());
}

float FTransformationCurve::GetScale() const {
	return FMath::Lerp(ScaleInterpolation.Start, ScaleInterpolation.End, CurveSequence.GetLerp());
}

FBox2D FTransformationCurve::GetClipBox() const {
	auto min = FMath::Lerp(ClipBoxInterpolation.Start.Min, ClipBoxInterpolation.End.Min, CurveSequence.GetLerp());
	auto max = FMath::Lerp(ClipBoxInterpolation.Start.Max, ClipBoxInterpolation.End.Max, CurveSequence.GetLerp());
	return FBox2D(min, max);
}

FTransformationSequence::FTransformationSequence(TArray<FTransformationCurve> curves, bool loop) : Curves(std::move(curves)), bShouldLoop(loop) {
	Index = 0;
}

void FTransformationSequence::Start() {
	if (Curves.Num()) {
		auto& currentCurve = Curves[Index];
		currentCurve.Start();
	}
}

void FTransformationSequence::Pause() {
	if (Curves.Num() && Index < Curves.Num()) {
		auto& currentCurve = Curves[Index];
		currentCurve.Pause();
	}
}

void FTransformationSequence::Resume() {
	if (Curves.Num() && Index < Curves.Num()) {
		auto& currentCurve = Curves[Index];
		currentCurve.Resume();
	}
}

void FTransformationSequence::Update(float deltaTime) {
	if (Curves.Num() && Index < Curves.Num()) {
		auto& currentCurve = Curves[Index];
		if (currentCurve.IsCompleted()) {
			if (Index + 1 < Curves.Num()) {
				Index++;
				auto& nextCurve = Curves[Index];
				nextCurve.Start();
			}
			else if (bShouldLoop) {
				Index = 0;
			}
		}
	}
}

const FTransformationCurve FTransformationSequence::GetCurrentTransformationCurve() const {
	FVector2D viewportSize;
	GEngine->GameViewport->GetViewportSize(viewportSize);
	return Curves.Num() ? Curves[Index] : FTransformationCurve(nullptr, FInterpolationData<FLinearColor>({ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }), FInterpolationData<FVector2D>({ 0.0f, 0.0f }, { 0.0f, 0.0f }), FInterpolationData<float>(1.0f, 1.0f), FInterpolationData<FBox2D>(FBox2D({-viewportSize.X, -viewportSize.Y}, { viewportSize.X, viewportSize.Y }), FBox2D({ -viewportSize.X, -viewportSize.Y }, { viewportSize.X, viewportSize.Y })), 0.0f, ECurveEaseFunction::Linear);
}