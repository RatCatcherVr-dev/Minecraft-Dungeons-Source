#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#if PLATFORM_XBOXONE

DECLARE_LOG_CATEGORY_EXTERN(LogXCloud, Verbose, All);

#include "XGameStreaming.h"

#endif

UENUM(BlueprintType)
enum class ETakLayout : uint8 {
	multiplayer_default = 0u UMETA(DisplayName = "multiplayer_default"),
	multiplayer_no_ranged = 1u UMETA(DisplayName = "multiplayer_no_ranged"),
	local_default = 2u UMETA(DisplayName = "local_default"),
	local_no_ranged = 3u UMETA(DisplayName = "local_no_ranged"),
	tutorial_default = 4u UMETA(DisplayName = "tutorial_default"),
	tutorial_default_ranged = 5u UMETA(DisplayName = "tutorial_default_ranged")
};

class DUNGEONS_API XCloud
{
#if PLATFORM_XBOXONE

public:
	static XCloud* Get();
	static XCloud* Create();
	static void Shutdown();

	const char* ETakLayoutToCharArray(ETakLayout Layout);
	bool IsStreaming() const;
	bool IsTouchInputEnabled();
	void ShowTouchControl(ETakLayout InLayout);
	void HideTouchControl();

	void EnableTouchControls();
	void DisableTouchControls();

private:
	XCloud();
	~XCloud();

	static void CALLBACK OnConnectionStateChanged(_In_opt_ void* context, _In_ XGameStreamingClientId client, _In_ XGameStreamingConnectionState state);
	void OnStreamingClientConnected(XGameStreamingClientId client);
	void OnStreamingClientDisconnected(XGameStreamingClientId client);

	XGameStreamingClientId Id;
	const char* CurrentTouchControlLayout;
	bool Active;
	bool TouchControlsEnabled;
	static XGameStreamingConnectionStateChangedRegistrationToken ConnectionToken;
	static XCloud *Instance;

#endif

};