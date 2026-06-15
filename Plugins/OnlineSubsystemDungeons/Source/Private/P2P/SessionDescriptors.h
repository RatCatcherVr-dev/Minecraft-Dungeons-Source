#pragma once

#include "WebRTCCommon.h"

#include <functional>

typedef struct SessionDescriptionInfo
{
	rtc::scoped_refptr<webrtc::PeerConnectionInterface> PeerConnection;
	Peer2PeerManager *Peer2PeerMgr;
	FString LocalXUID;
	FString RemoteXUID;
} SessionDescriptionInfo;

class SetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver
{
//	friend class rtc;
public:
	SetSessionDescriptionObserver() : RefCount(0) { }

	template<class FUNC>
	static webrtc::SetSessionDescriptionObserver* Create(const FUNC& Func)
	{
		SetSessionDescriptionObserver* Observer = new SetSessionDescriptionObserver();
		Observer->Callback = Func;
		return Observer;
	}

	void AddRef(void) const
	{
		++RefCount;
	}

	rtc::RefCountReleaseStatus Release(void) const
	{
		int NewRef = --RefCount;

		if (NewRef == 0)
		{
			delete this;
			return rtc::RefCountReleaseStatus::kDroppedLastRef;
		}
		return rtc::RefCountReleaseStatus::kOtherRefsRemained;
	}

	void OnSuccess(void) override
	{
		Callback(true);
	}
	void OnFailure(const std::string &error) override
	{
		Callback(false);
	}
private:
	mutable std::atomic<int> RefCount;
	std::function<void(bool)> Callback;
};

class SessionDescriptionObserver : public webrtc::CreateSessionDescriptionObserver
{
public:

	SessionDescriptionObserver() : RefCount(0) { }

	template<class FUNC>
	static CreateSessionDescriptionObserver* Create(const FUNC& Func)
	{
		SessionDescriptionObserver* Observer = new SessionDescriptionObserver();
		Observer->Callback = Func;
		return Observer;
	}

	void AddRef(void) const override
	{
		++RefCount;
	}

	rtc::RefCountReleaseStatus Release(void) const override
	{
		int NewRef = --RefCount;

		if (NewRef == 0)
		{
			delete this;
			return rtc::RefCountReleaseStatus::kDroppedLastRef;
		}
		return rtc::RefCountReleaseStatus::kOtherRefsRemained;
	}

	void OnSuccess(webrtc::SessionDescriptionInterface* desc) override
	{
		Callback(desc, "");
	}
	void OnFailure(const std::string& error) override
	{
		Callback(nullptr, error);
	}
private:
	mutable std::atomic<int> RefCount;
	std::function<void(webrtc::SessionDescriptionInterface*, const std::string&)> Callback;
};

class SetRemoteDescriptionObserver : public webrtc::SetRemoteDescriptionObserverInterface
{
public:

	SetRemoteDescriptionObserver() : RefCount(0) { }

	template<class FUNC>
	static SetRemoteDescriptionObserver* Create(const FUNC& Func)
	{
		SetRemoteDescriptionObserver* Observer = new SetRemoteDescriptionObserver();
		Observer->Callback = Func;
		return Observer;
	}

	void AddRef(void) const override
	{
		++RefCount;
	}

	rtc::RefCountReleaseStatus Release(void) const override
	{
		int NewRef = --RefCount;

		if (NewRef == 0)
		{
			delete this;
			return rtc::RefCountReleaseStatus::kDroppedLastRef;
		}
		return rtc::RefCountReleaseStatus::kOtherRefsRemained;
	}

	void OnSetRemoteDescriptionComplete(webrtc::RTCError error) override
	{
		if (error.ok())
		{
			Callback(true, nullptr);
		}
		else
		{
			Callback(false, error.message());
		}
	}
private:
	mutable std::atomic<int> RefCount;
	std::function<void(bool, const char*)> Callback;
};