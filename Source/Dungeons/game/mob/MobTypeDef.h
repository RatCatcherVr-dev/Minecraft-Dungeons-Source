#pragma once

enum class EntityType : uint32;

struct MobTypeDef {
	MobTypeDef(EntityType);

	EntityType mobType() const {
		return mMobType;
	}

	bool IsDisabled() const;
	const FText& GetName() const;
	const FText& GetDescriptionText() const;

	FName GetIconRowName() const;
	static FName EntityTypeToFName(EntityType mobType);
protected:

	bool bDisabled = false;
	EntityType mMobType;
	FText mName;
	FText mDescriptionText;

	template <typename T>
	using Provider = std::function<T()>;
	Provider<FName> mIconRowNameProvider;
};
