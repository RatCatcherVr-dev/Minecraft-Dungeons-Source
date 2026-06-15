#pragma once

class PerformanceTelemetry {
public:
	void Clear() {
		PlayerFrameTimeMS = 0.0f;
		GameThreadTimeMS = 0.0f;
		RenderThreadTimeMS = 0.0f;
		GPUFrameTimeMS = 0.0f;
		NumSamples = 0;
	}

	void Update(float GameThreadTime, float RenderThreadTime, float GPUFrameTime) {
		PlayerFrameTimeMS += 1000.0f * (FApp::GetCurrentTime() - FApp::GetLastTime());
		GameThreadTimeMS += GameThreadTime;
		RenderThreadTimeMS += RenderThreadTime;
		GPUFrameTimeMS += GPUFrameTime;
		++NumSamples;
	}

	float PlayerFrameTimeMS{ 0.0f };
	float GameThreadTimeMS{ 0.0f };
	float RenderThreadTimeMS{ 0.0f };
	float GPUFrameTimeMS{ 0.0f };
	int NumSamples{ 0 };
};
