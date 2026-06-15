#pragma once
#include "game/progress/ProgressStat.h"
#include "MerchantSaveData.h"

class AMerchantBase;
class UCharacterSerializeComponent;

namespace merchant {
struct Context {		
private:
	AMerchantBase& mMerchant;
	UCharacterSerializeComponent& mSerializer;

	FName GetMerchantSaveName() const;
public:		
	Context(AMerchantBase&, UCharacterSerializeComponent& saveData);
	
	int GetFinishedObjectiveTagCount(const std::string& objectiveTag) const;
	int GetProgressStat(EProgressStat stat) const;
	void IncrementProgressStat(EProgressStat stat) const;

	const AMerchantBase& GetMerchant() const;	
	const UCharacterSerializeComponent& GetSerializer() const;
	
	bool HasSaveData() const;
	void CreateSaveData() const;
	const FMerchantSaveData& ReadSaveData() const;
	FMerchantSaveData& EditSaveData() const;
};
}