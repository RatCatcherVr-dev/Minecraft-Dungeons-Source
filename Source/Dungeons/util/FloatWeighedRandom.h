/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "util/Algo.h"
#include "util/SharedRandom.h"

class FloatWeighedRandom {
public:

	class WeighedRandomItem {
	public:
		WeighedRandomItem()
			:   mRandomWeight(-128){
		}

		WeighedRandomItem(float randomWeight)
			:   mRandomWeight(randomWeight){
		}

		bool isValid() const {
			return mRandomWeight >= 0;
		}

		float getWeight() const {
			return mRandomWeight;
		}
	protected:
		float mRandomWeight;
	};

	class MutableWeighedRandomItem : public WeighedRandomItem {
	public:
		MutableWeighedRandomItem() {};
		MutableWeighedRandomItem(float randomWeight) :WeighedRandomItem(randomWeight) {};
		void setWeight(float weight) { mRandomWeight = weight; }
	};

	template<typename T>
	static float getTotalWeight(const T& items) {
		return algo::sum(items, RETLAMBDA(it.getWeight()));
	}

	template<typename T>
	static float getTotalWeightFromPointers(const T& items) {
		return algo::sum(items, RETLAMBDA(it->getWeight()));
	}

	template<typename T>
	static const T* getRandomItem(Random* random, const std::vector<T>& items, float totalWeight) {
		if (!items.empty() && totalWeight > 0) {
			auto selection = random->nextFloat(totalWeight);

			for (typename std::vector<T>::const_iterator it = items.begin(); it != items.end(); ++it) {
				selection -= it->getWeight();
				if (selection <= 0) {
					return &(*it);
				}
			}
		}
		return nullptr;
	}

	template<typename T>
	static T* getRandomItem(Random* random, std::vector<T>& items, float totalWeight) {
		if (!items.empty() && totalWeight > 0) {
			auto selection = random->nextFloat(totalWeight);

			for (typename std::vector<T>::iterator it = items.begin(); it != items.end(); ++it) {
				selection -= it->getWeight();
				if (selection <= 0) {
					return &(*it);
				}
			}
		}
		return nullptr;
	}

	template<typename T>
	static const T* getRandomItem(Random* random, const std::vector<T>& items) {
		return getRandomItem(random, items, getTotalWeight(items));
	}

	template<typename T>
	static T* getRandomItem(Random* random, std::vector<T>& items) {
		return getRandomItem(random, items, getTotalWeight(items));
	}

	template<typename T>
	static int getRandomItemIndex(Random* random, const T& items, float totalWeight) {
		if (totalWeight > 0) {
			float selection = random->nextFloat(totalWeight);

			unsigned int i = 0;
			for (const auto& item : items) {
				selection -= item.getWeight();
				if (selection <= 0) {
					return i;
				}
				++i;
			}
		}
		return -1;
	}

	template<typename T>
	static int getRandomItemIndex(Random* random, const T& items) {
		return getRandomItemIndex(random, items, getTotalWeight(items));
	}

	template<typename T>
	static int getRandomItemIndexFromPointers(Random* random, const T& items, float totalWeight) {
		if (totalWeight > 0) {
			float selection = random->nextFloat(totalWeight);

			unsigned int i = 0;
			for (const auto* item : items) {
				selection -= item->getWeight();
				if (selection <= 0) {
					return i;
				}
				++i;
			}
		}
		return -1;
	}

	template<typename T>
	static int getRandomItemIndexFromPointers(Random* random, const T& items) {
		return getRandomItemIndexFromPointers(random, items, getTotalWeightFromPointers(items));
	}

	template <typename Collection>
	static auto getWeightedRandomItemOrEmpty(const Collection& weightedItems, Random* random = nullptr) {
		using T = std::remove_reference_t<decltype(*algo::copyable_begin(weightedItems))>;
		const auto index = getRandomItemIndex(Util::thisOrSharedRandom(random), weightedItems);
		return index >= 0 ? TOptional<T>(weightedItems[index]) : TOptional<T>{};
	}
};
