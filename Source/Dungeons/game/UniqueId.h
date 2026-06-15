#pragma once



template <typename KeyT, typename ValueT, typename SizeT = uint16>
class TIdRegistry {
	 
	TIdRegistry(TIdRegistry& other) = delete;
	TIdRegistry(TIdRegistry&& other) = delete;
	
public:

	TIdRegistry() = default;
	using KeyType = KeyT;
	using ValueType = ValueT;
	using SizeType = SizeT;

	
	class TId {

		KeyT key;

		TId(KeyT k): key(k) {};
		TId(KeyT&& k): key(std::move(k)) {};
		TId() = delete;
	public:
		TId(const TId& other) = default;
		TId& operator=(const TId& other) = default;

		bool operator==(const TId& other) const { return key == other.key; };
		bool operator!=(const TId& other) const { return key != other.key; };
		bool operator<(const TId& other) const { return key < other.key; };

		const KeyT& GetBackingType() const { return key; }

		friend uint32 GetTypeHash(const TId& id) { return GetTypeHash(id.key); };
		friend class TIdRegistry;
	};
	
	TId Register(KeyT key, TUniquePtr<ValueT> value) {
		ensureAlwaysMsgf(!_Registry.Contains(key), TEXT("Registry already contains specified key, replacing mapping."));

		_Registry.Add(key, MoveTemp(value));
		_IndexLookup.Add(key, _ValueLookup.Emplace(_Registry[key].Get()));
		return TId(key);
	}
	
	TOptional<TId> Request(const KeyT& key) {
		return _Registry.Contains(key) ? TId(key) : TOptional<TId>();
	}

	const ValueT& Get(const TId& key) {
		return *_Registry[key.key];
	}

	const TArray<const ValueT*>& GetValues() {
		return _ValueLookup;
	}
	
	SizeT IndexOf(const TId& key) {
		return _IndexLookup[key.key];
	}
private:
	
	TMap<KeyT, TUniquePtr<ValueT>> _Registry;
	TMap<KeyT, SizeT> _IndexLookup;
	TArray<const ValueT*> _ValueLookup;
};

