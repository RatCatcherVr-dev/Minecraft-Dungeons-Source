// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/CurveSequence.h"
#include "Styling/SlateColor.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/CoreStyle.h"
#include <chrono>
#include <App.h>

class FPaintArgs;
class FSlateWindowElementList;

template<typename T>
struct FInterpolationData {
	T Start, End;

	FInterpolationData(T start, T end) :
		Start(start), End(end) {}
};

struct FTransformationCurve {
	FTransformationCurve(TWeakPtr<SWidget>, FInterpolationData<FLinearColor>, FInterpolationData<FVector2D>, FInterpolationData<float>, FInterpolationData<FBox2D>, float, ECurveEaseFunction);

	void Start();
	void Pause();
	void Resume();

	bool IsCompleted() const;

	FLinearColor GetColor() const;
	FVector2D GetPosition() const;
	float GetScale() const;
	// Gets the area of the widget to render from the widget's center position
	FBox2D GetClipBox() const;
private:
	FInterpolationData<FLinearColor> ColorInterpolation;
	FInterpolationData<FVector2D> PositionInterpolation;
	FInterpolationData<float> ScaleInterpolation;
	FInterpolationData<FBox2D> ClipBoxInterpolation;
	float DurationSeconds;
	TWeakPtr<SWidget> OwningWidget;

	FCurveSequence CurveSequence;
};

struct FTransformationSequence {
	FTransformationSequence() {}
	FTransformationSequence(TArray<FTransformationCurve> curves, bool loop);

	void Start();

	void Pause();

	void Resume();

	void Update(float deltaTime);

	const FTransformationCurve GetCurrentTransformationCurve() const;
private:
	int Index;

	TArray<FTransformationCurve> Curves;
	bool bShouldLoop;
};