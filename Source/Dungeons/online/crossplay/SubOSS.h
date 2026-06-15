#pragma once
#include "online/OnlineCommon.h"
#include "CoreMinimal.h"
#include "SubsystemRepo.h"

#include <functional>

namespace online {
namespace Crossplay {
class SubsystemRepo;

bool IsCompatibleNetId(const IOnlineSubsystem*, const FUniqueNetId&);

template <typename T>
struct Child {
	Child(const IOnlineSubsystem * s, T c) : Subsystem(s), obj(c), Success(false), ErrorMessage(FString()) {}
	const IOnlineSubsystem* Subsystem;
	T obj;
	bool Success;
	FString ErrorMessage;
};

template <typename T>
struct Link {
	Link(const T* p, int32 cnr) : Parent(p), ChildrenNotReturned(cnr), Children({}) {}
	const T* Parent;
	int32 ChildrenNotReturned;
	TArray<TSharedRef<Child<T>>> Children;
};

class SubOSS {
public:
	SubOSS(const SubsystemRepo&);
	virtual ~SubOSS() {}
	virtual bool OnDeactivate(IOnlineSubsystem*) { return true; }
	virtual bool OnActivate(IOnlineSubsystem*) { return true; }

	TSharedPtr<const FUniqueNetId> GetUniqueNetIdFromName(const FUniqueNetId& Requestor, FName name) const;
	TSharedPtr<const FUniqueNetId> GetUniqueNetIdFromType(const FUniqueNetId& Requestor, SubsystemType type) const;
	TSharedPtr<const FUniqueNetId> GetUniqueNetIdFromType(const FUniqueNetId& Requestor, const std::initializer_list<SubsystemType>& pickOrder) const;

protected:
	IOnlineSubsystem* GetDungeonsSubsystem() const;
	IOnlineSubsystem* GetPS4Subsystem() const;
	IOnlineSubsystem* GetFirstActiveSubsystem() const;
	IOnlineSubsystem* GetFirstActiveSubsystem(const std::initializer_list<SubsystemType>&) const;
	IOnlineSubsystem* GetSubsystemFromUniqueNetId(const FUniqueNetId&) const;
	//Returns the subsystem depending on the crossplay toggle (true=Dungeons, false=PS4 (Null is -lan))
	IOnlineSubsystem* GetSessionSubsystem() const;
	IOnlineSubsystem* GetSubsystemFromName(const FName&) const;

	SubsystemType GetFirstActiveType(const std::initializer_list<SubsystemType>&) const;

	void ClearDelegateHandles(const FString& key, std::function<void(TSharedRef<Child<FDelegateHandle>>&)> callback);
	void AddDelegateHandle(const FString &key, const TSharedRef<Link<FDelegateHandle>>& link);

	const SubsystemRepo& Subsystems;
private:
	TMap<FString, TSharedRef<Link<FDelegateHandle>>> DelegateHandles;
};
}
}
