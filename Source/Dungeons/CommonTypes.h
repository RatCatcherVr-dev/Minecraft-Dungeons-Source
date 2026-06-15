/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "legacy/Core/Utility/PrimitiveTypes.h"

#include "util/NewType.h"

#include "AsyncWork.h"

#include <functional>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>

#include "util/DataTracker.h"


class Entity;
class Player;
class Block;
class BlockSource;

template<typename T>
using Unique = std::unique_ptr<T>;

template<typename T>
using Shared = std::shared_ptr<T>;

template<typename T>
using Weak = std::weak_ptr<T>;

template<typename T>
using Ref = std::reference_wrapper<T>;

template<typename T>
using Pred = std::function<bool(T)>;

template<typename T1, typename T2>
using Pair = std::pair<T1, T2>;

template <typename T>
std::initializer_list<T> make_init(std::initializer_list<T> list) { return list; }


typedef std::vector<Entity*> EntityList;

typedef std::lock_guard<std::mutex> LockGuard;
typedef std::unique_lock<std::mutex> ScopedLock;

typedef uint32_t RandomSeed;

typedef int16_t Height; //don't use fast_t because it will cause all ChunkBlockPos to become 8 bytes if this is not 2 bytes

//D11.PS - D11.TODO investigate this, int_fast8_t is different on PS4 and causes a redeclaration
typedef signed char FacingID;
//typedef int_fast8_t FacingID;


enum class BlockProperty : unsigned int;

struct BlockID : public NewType<uint16_t> {
	static const BlockID AIR;

	BlockID() {}

	explicit BlockID(const uint16_t& v) :
		NewType(v) {
	}

	bool hasProperty(BlockProperty properties) const;

	const Block& getBlock() const;
};

struct Brightness : public NewType<uint8_t> {
	static const Brightness MAX, MIN, INVALID;

	explicit Brightness(const uint8_t& v) :
		NewType(v) {
	}

	Brightness() :
		Brightness(MIN) {
	}

	Brightness& operator -= (const Brightness& rhs) {
		value -= rhs.value;
		return *this;
	}

	Brightness& operator += (const Brightness& rhs) {
		value += rhs.value;
		return *this;
	}
};

struct BrightnessPair {
	Brightness sky, block;
};

typedef uint8_t DataID;

typedef unsigned int Index;

enum DimensionId : int {
	Overworld = 0,
	Nether = 1,
	TheEnd = 2,
	Count,
	Undefined = Count
};

struct FullBlock {
	static const FullBlock AIR;

	BlockID id;
	DataID data;

	FullBlock() :
		id(0)
		, data(0) {
	}

	FullBlock(BlockID id) :
		id(id)
		, data(0) {
	}

	FullBlock(const BlockID& id, const DataID& data) :
		id(id)
		, data(data) {
	}

	bool operator == (const FullBlock& other) const {
		return id == other.id && (data & 0xF) == (other.data & 0xF);
	}

	bool operator != (const FullBlock& other) const {
		return !(*this == other);
	}
	
	int toInt() const {
		return id + (data << 16);
	}

	std::size_t hashCode() const {
		return id << 8 | (data & 0xF);
	}

	bool isAir() const {
		return id == 0;
	}

	const Block& getBlock() const;
};

class UWorld;

template<typename T>
class InstanceTracker
{
protected:

	InstanceTracker() {}

	static DataTracker<T*> s_DataTracker;

public:
	
	static void AddInstance(UWorld* pWorld, T* pInst) { s_DataTracker.AddInstance(pWorld, pInst); };
	static void RemoveInstance(UWorld* pWorld, T* pInst) { s_DataTracker.RemoveInstance(pWorld, pInst); }
	static TArray<T*>& GetList(UWorld* pWorld ) { return s_DataTracker.GetList(pWorld);	};

};

template <typename T>
DataTracker<T*> InstanceTracker<T>::s_DataTracker;



using AsyncTaskCallback = std::function< void() >;
class FCallbackAsyncTask : public FNonAbandonableTask
{
public:
	friend class FAsyncTask<FCallbackAsyncTask>;
	friend class FAutoDeleteAsyncTask<FCallbackAsyncTask>;

	AsyncTaskCallback mCallbackFunc;

	FCallbackAsyncTask(AsyncTaskCallback callback)
		:
		mCallbackFunc(callback)
	{
	}

	void DoWork()
	{
		mCallbackFunc();
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FCallbackAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};





namespace std {
	///hash specialization for unordered_maps
	template<>
	struct hash<FullBlock> {
		// hash functor for vector
		size_t operator()(const FullBlock& _Keyval) const {
			return _Keyval.hashCode();
		}

	};

}

using std::make_shared;

#include "util/range.h"
