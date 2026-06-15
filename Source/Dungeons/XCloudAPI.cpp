#if PLATFORM_XBOXONE

#include "XCloudAPI.h"

DEFINE_LOG_CATEGORY(LogXCloud);

XCloud *XCloud::Instance = nullptr;
XGameStreamingConnectionStateChangedRegistrationToken XCloud::ConnectionToken = { 0u };

XCloud *XCloud::Create()
{
	check(!Instance);
	if (!Instance)
	{
		Instance = new XCloud();
	}

	return Instance;
}


void XCloud::Shutdown()
{
	if(Instance)
		delete Instance;

	Instance = nullptr;
}

XCloud* XCloud::Get() 
{
	if(Instance)
		return Instance; 
	return Create();
}

XCloud::XCloud()
	: Id(XGameStreamingNullClientId)
	, CurrentTouchControlLayout(nullptr)
	, Active(false)
	, TouchControlsEnabled(true)
{
	ConnectionToken.value = 0u;
	const HRESULT hrInit = XGameStreamingInitialize();
	UE_LOG(LogXCloud, Log, TEXT("XGameStreamingInitialized : %x GCT: %d"), hrInit, ConnectionToken.value);

	const HRESULT hrRegister = XGameStreamingRegisterConnectionStateChanged(this, OnConnectionStateChanged, &ConnectionToken);
	UE_LOG(LogXCloud, Log, TEXT("XGameStreamingRegisterConnectionStateChanged : %x"), hrRegister);
}

XCloud::~XCloud()
{
	XGameStreamingUnregisterConnectionStateChanged(ConnectionToken);
	ConnectionToken = { 0 };

	XGameStreamingUninitialize();

	UE_LOG(LogXCloud, Log, TEXT("XGameStreaming shutdown"));
}

const char* XCloud::ETakLayoutToCharArray(ETakLayout Layout)
{
	switch (Layout)
	{
		case ETakLayout::multiplayer_default:
			return "multiplayer_default";
		case ETakLayout::multiplayer_no_ranged:
			return "multiplayer_no_ranged";
		case ETakLayout::local_default:
			return "local_default";
		case ETakLayout::local_no_ranged:
			return "local_no_ranged";
		case ETakLayout::tutorial_default:
			return "tutorial_default";
		case ETakLayout::tutorial_default_ranged:
			return "tutorial_default_ranged";
		default:
			return nullptr;
	}
}

bool XCloud::IsStreaming() const
{
	return Active;
}

bool XCloud::IsTouchInputEnabled()
{
	bool OutVal = false;

	XGameStreamingIsTouchInputEnabled(Id, &OutVal);
	return OutVal;
}

void XCloud::ShowTouchControl(ETakLayout InLayout)
{
	const char* Layout = ETakLayoutToCharArray(InLayout);
	CurrentTouchControlLayout = Layout;
	if (TouchControlsEnabled) {
		XGameStreamingShowTouchControlLayoutOnClient(Id, Layout);
	}
}

void XCloud::HideTouchControl()
{
	CurrentTouchControlLayout = nullptr;
	if (TouchControlsEnabled) {
		XGameStreamingHideTouchControlsOnClient(Id);
	}
}

void XCloud::EnableTouchControls()
{
	//enable the previously attempted to set touch countrols
	if (CurrentTouchControlLayout)
	{
		XGameStreamingShowTouchControlLayoutOnClient(Id, CurrentTouchControlLayout);
	}
	else
	{
		XGameStreamingHideTouchControlsOnClient(Id);
	}
	TouchControlsEnabled = true;
}

void XCloud::DisableTouchControls()
{
	//Hide touch controls and set the flag
	XGameStreamingHideTouchControlsOnClient(Id);
	TouchControlsEnabled = false;
}

void CALLBACK XCloud::OnConnectionStateChanged(
	_In_opt_ void* context,
	_In_ XGameStreamingClientId client,
	_In_ XGameStreamingConnectionState state)
{
	XCloud *xCloud = static_cast<XCloud*>(context);


	if (state == XGameStreamingConnectionState::Connected)
	{
		xCloud->OnStreamingClientConnected(client);
	}
	else
	{
		check(state == XGameStreamingConnectionState::Disconnected);
		xCloud->OnStreamingClientDisconnected(client);
	}
}

void XCloud::OnStreamingClientConnected(XGameStreamingClientId client)
{
	UE_LOG(LogXCloud, Log, TEXT("Streaming client %llu connected"), client);
	Active = true;

	Id = client;

	if (CurrentTouchControlLayout)
	{
		XGameStreamingShowTouchControlLayoutOnClient(Id, CurrentTouchControlLayout);
	}
	else
	{
		XGameStreamingHideTouchControlsOnClient(Id);
	}
}

void XCloud::OnStreamingClientDisconnected(XGameStreamingClientId client)
{
	UE_LOG(LogXCloud, Log, TEXT("Streaming client %llu disconnected"), client);
	Active = false;
}


#endif