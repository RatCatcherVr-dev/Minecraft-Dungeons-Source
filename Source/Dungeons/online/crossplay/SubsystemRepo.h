#pragma once

#include "CoreMinimal.h"
#include "online/OnlineCommon.h"
#include "SubsystemRepo.generated.h"

UENUM(BlueprintType)
enum class SubsystemType : uint8 {
	Dungeons,
	PS4,
	Steam,
	Null
};
ENUM_NAME(SubsystemType);

namespace online {
namespace Crossplay {

class SubsystemRepo {
public:
	SubsystemRepo() = default;
	SubsystemRepo(const SubsystemRepo&) = delete;
	SubsystemRepo operator=(const SubsystemRepo&) = delete;
	void Register(SubsystemType, IOnlineSubsystem&);
	bool IsRegistered(SubsystemType) const;
	
	IOnlineSubsystem* GetActive(SubsystemType) const;
	IOnlineSubsystem* Get(SubsystemType type) const;
	int32 NumActive() const;
	bool IsActive(SubsystemType) const;
	IOnlineSubsystem* GetFirstActiveSubsystem() const;
	IOnlineSubsystem* GetFirstActiveSubsystem(const std::initializer_list<SubsystemType>&) const;

	SubsystemType GetFirstActiveType(const std::initializer_list<SubsystemType>&) const;
	
	void Activate(SubsystemType);
	void Deactivate(SubsystemType);

	TArray<IOnlineSubsystem*> GetAllActive() const;

	SubsystemType GetSubsystemType(FName) const;
	FName GetStringFromType(SubsystemType) const;

	void ForAllSubsystems(std::function<void(IOnlineSubsystem*)>) const;
	


private:
	struct SubsystemInfo {
		IOnlineSubsystem* Subsystem;
		bool IsActive = false;
	};
	TMap<SubsystemType, SubsystemInfo> Subsystems;


};

}
}
