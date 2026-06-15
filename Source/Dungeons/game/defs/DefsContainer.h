#pragma once
#include "CommonTypes.h"
#include "util/CollectionUtils.h"

template <typename MutableDefTypeT, typename DefTypeT, typename EnumTypeT>
class DefsContainer {
	TArray<Unique<MutableDefTypeT>> mDefs;
	TOptional<EnumTypeT> mInvalid;

public:
	DefsContainer(int numDefs, TOptional<EnumTypeT> invalidEnumValue = {}) {
		mInvalid = invalidEnumValue;
		mDefs = Util::createDefaultedTArrayOfSize<Unique<MutableDefTypeT>>(numDefs);
	}

	MutableDefTypeT& create(EnumTypeT name) {
		const int index = enum_cast(name);
		check(!mDefs[index]);
		mDefs[index] = make_unique<MutableDefTypeT>(name);
		return *mDefs[index];
	}

	const DefTypeT& get(EnumTypeT name) const {
		if (auto dlc = getChecked(name)) {
			return *dlc;
		}
		DEBUG_ASSERT(mInvalid.IsSet(), "cannot return reference without invalid def defined");
		return *mDefs[static_cast<int>(mInvalid.GetValue())].get();		
	}

	const DefTypeT* getChecked(EnumTypeT name) const {
		const int index = static_cast<int>(name);
		if (mInvalid.IsSet() && mInvalid.GetValue() == name) {
			return nullptr;
		}
		if (!mDefs.IsValidIndex(index)) {
			return nullptr;
		}		
		return mDefs[index].get();		
	}

	const TArray<Unique<MutableDefTypeT>>& getAllMutable() const {
		return mDefs;
	}

};
