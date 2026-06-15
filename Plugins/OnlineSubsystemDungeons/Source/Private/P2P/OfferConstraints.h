//#pragma once
//
//#include "WebRTCCommon.h"
//
//class OfferConstraints : public webrtc::MediaConstraintsInterface
//{
//public:
//	OfferConstraints() { }
//	virtual ~OfferConstraints() { }
//
//	virtual const Constraints& GetMandatory() const {
//		return Mandatory;
//	}
//
//	virtual const Constraints& GetOptional() const {
//		return Optional;
//	}
//
//	template <class T>
//	void AddMandatory(const std::string& key, const T& value) {
//		Mandatory.push_back(Constraint(key, rtc::ToString<T>(value)));
//	}
//
//	template <class T>
//	void SetMandatory(const std::string& key, const T& value) {
//		std::string value_str;
//		if (Mandatory.FindFirst(key, &value_str)) {
//			for (Constraints::iterator iter = Mandatory.begin();
//				iter != Mandatory.end(); ++iter) {
//				if (iter->key == key) {
//					Mandatory.erase(iter);
//					break;
//				}
//			}
//		}
//		Mandatory.push_back(Constraint(key, rtc::ToString<T>(value)));
//	}
//
//	template <class T>
//	void AddOptional(const std::string& key, const T& value) {
//		Optional.push_back(Constraint(key, rtc::ToString<T>(value)));
//	}
//
//	void SetMandatoryMinAspectRatio(double ratio) {
//		SetMandatory(webrtc::MediaConstraintsInterface::kMinAspectRatio, ratio);
//	}
//
//	void SetMandatoryMinWidth(int width) {
//		SetMandatory(webrtc::MediaConstraintsInterface::kMinWidth, width);
//	}
//
//	void SetMandatoryMinHeight(int height) {
//		SetMandatory(webrtc::MediaConstraintsInterface::kMinHeight, height);
//	}
//
//	void SetOptionalMaxWidth(int width) {
//		AddOptional(webrtc::MediaConstraintsInterface::kMaxWidth, width);
//	}
//
//	void SetMandatoryMaxFrameRate(int frame_rate) {
//		SetMandatory(webrtc::MediaConstraintsInterface::kMaxFrameRate, frame_rate);
//	}
//
//	void SetMandatoryReceiveAudio(bool enable) {
//		SetMandatory(webrtc::MediaConstraintsInterface::kOfferToReceiveAudio, enable);
//	}
//
//	void SetMandatoryReceiveVideo(bool enable) {
//		SetMandatory(webrtc::MediaConstraintsInterface::kOfferToReceiveVideo, enable);
//	}
//
//	void SetMandatoryUseRtpMux(bool enable) {
//		SetMandatory(webrtc::MediaConstraintsInterface::kUseRtpMux, enable);
//	}
//
//	void SetMandatoryIceRestart(bool enable) {
//		SetMandatory(webrtc::MediaConstraintsInterface::kIceRestart, enable);
//	}
//
//	void SetAllowRtpDataChannels() {
//		SetMandatory(webrtc::MediaConstraintsInterface::kEnableRtpDataChannels, true);
//		SetMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, false);
//	}
//
//	void SetOptionalVAD(bool enable) {
//		AddOptional(webrtc::MediaConstraintsInterface::kVoiceActivityDetection, enable);
//	}
//
//	void SetAllowDtlsSctpDataChannels() {
//		SetMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, true);
//	}
//
//private:
//	Constraints Mandatory;
//	Constraints Optional;
//};