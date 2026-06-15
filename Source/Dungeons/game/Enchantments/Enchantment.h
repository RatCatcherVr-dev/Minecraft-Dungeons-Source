#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/Enchantments/EnchantmentType.h"
#include "GameplayTagContainer.h"
#include "GameplayPrediction.h"
#include "game/component/MeleeAttackComponent.h"
#include <SharedPointer.h>
#include "game/util/ValueFormat.h"
#include "game/item/drop/ItemDropData.h"
#include "game/item/ItemBulletPoint.h"
#include <AbilitySystemInterface.h>
#include <GameplayEffectTypes.h>
#include "game/util/GearUtil.h"
#include "Enchantment.generated.h"

struct FRandomStream;
struct FDropCategoryDescription;
class ABaseProjectile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCurrentCounterValueChanged);

UCLASS(Blueprintable, BlueprintType)
class DUNGEONS_API UEnchantment : public UGearUtil
{
 	GENERATED_BODY()

public:	
	UEnchantment();

	void SetLevel(int level);

	int GetLevel() const;

	const EnchantmentType& GetType() const;

	FORCEINLINE void SetSource(AActor* actor) { Source = actor; }
	FORCEINLINE const AActor* GetSource() const { return Source; }

	FORCEINLINE void SetEnchantmentSource(EEnchantmentSource source) { EnchantmentSource = source; }
	FORCEINLINE const EEnchantmentSource GetEnchantmentSource() const { return EnchantmentSource; }

	FORCEINLINE void SetOwnerItemRarity(EItemRarity rarity) { OwnerItemRarity = rarity; }
	FORCEINLINE const EItemRarity GetOwnerItemRarity() const { return OwnerItemRarity; }

	FORCEINLINE EEnchantmentTypeID GetTypeId() const { return TypeId; }

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	float GetSourceItemPower() const;
	bool IsSourceItemMelee() const;
	bool IsSourceItemRanged() const;
	bool IsSourceItemArmor() const;
		
	class ABaseCharacter* GetCharacterOwner() const;

	bool IsOwnerMob() const;
	
	/** The item that spawned this enchantment. Will default to Owner. */
	AActor* Source = nullptr;

	static float DefaultLevelMultiplier(int level) {
		return (float)level;
	}

	static float DefaultItemPowerMultiplier(float itemPower) {
		return 1.0f;
	}

	static FString DefaultMultiplierFormatter(float multiplier) {
		return valueformat::asConstant(multiplier);
	}

	typedef std::function<float(int)> EnchantmentLevelToMultiplierFunction;
	typedef std::function<float(float)> EnchantmentItemPowerToMultiplierFunction;
	typedef std::function<FString(float)> EnchantmentMultiplierFormatterFunction;
	EnchantmentLevelToMultiplierFunction LevelMultiplier = DefaultLevelMultiplier;
	EnchantmentItemPowerToMultiplierFunction ItemPowerMultiplier = DefaultItemPowerMultiplier;
	EnchantmentMultiplierFormatterFunction MultiplierFormatter = DefaultMultiplierFormatter;

	FActiveGameplayEffectHandle SoulGatheringHandle;

public:
	FString CreateFormattedLevelEffectMultiplier(int forLevel, float forItemPower) const;

	virtual FText CreateDescription() const;	

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UFUNCTION(Client, Reliable)
	void BroadcastEnchantmentTriggeredEvent(bool canCombo = true) const;

	static TArray< UTexture2D* > s_IconTextures;
	static TArray< UMaterialInstance* > s_IconMaterials;
	static TArray< UClass* > s_EnchantmentClasses;

	static void PreloadIconTextures();
	
	//Tried using an in-outer to control garbage collection, did not work.
	//Currently using a work-around of not ever making the widget disappear completely in animations.
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static UTexture2D* GetIconTextureForEnchantmentType(UObject* ref, EEnchantmentTypeID type);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static UMaterialInstance* GetIconMaterialInstanceForEnchantmentType(UObject* ref, EEnchantmentTypeID type);

	UFUNCTION(BlueprintCallable, Category = "Dungeons", BlueprintPure)
	static FText GetNameForEnchantmentType(EEnchantmentTypeID type);

	UFUNCTION(BlueprintCallable, Category = "Dungeons", BlueprintPure)
	static FText GetDescriptionForEnchantmentType(EEnchantmentTypeID type);

	UFUNCTION(BlueprintCallable, Category = "Dungeons", BlueprintPure)
	static TArray<FItemBulletPoint> GetDisplayBulletPointsTextForEnchantmentType(EEnchantmentTypeID type);

	static UClass* GetEnchantmentTypeClass(EEnchantmentTypeID type);

	UFUNCTION(BlueprintCallable, Category = "Dungeons", BlueprintPure)
	static FText GetLevelEffectDescriptionForEnchantmentType(EEnchantmentTypeID type, int level, float itemPower);

	UFUNCTION(BlueprintCallable, Category = "Dungeons", BlueprintPure)
	static bool GetHasTagForEnchantmentType(EEnchantmentTypeID type, EEnchantmentTag tag);

	UFUNCTION(BlueprintCallable, Category = "Dungeons", BlueprintPure)
	static bool GetIsRarityForEnchantmentType(EEnchantmentTypeID type, EEnchantmentRarity rarity);

	UFUNCTION(BlueprintCallable, Category = "Dungeons", BlueprintPure)
	static EEnchantmentRarity GetEnchantmentTypeRarity(EEnchantmentTypeID type);

	UFUNCTION(BlueprintCallable, Category = "Dungeons", BlueprintPure)
	static EEnchantmentCategory GetEnchantmentTypeCategory(EEnchantmentTypeID type);

	UFUNCTION(BlueprintCallable, Category = "Dungeons", BlueprintPure)
	static int32 GetUpgradeCostForEnchantmentTypeLevel(EEnchantmentTypeID type, int32 level, bool isNetherite);

	UFUNCTION(BlueprintCallable, Category = "Dungeons", BlueprintPure)
	static bool GetCanBeUsedByMobsForEnchantmentType(EEnchantmentTypeID type);

	virtual void Start() final;
	virtual void End() final;

	virtual void OnStart() {};

	virtual void OnEnd() {};
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Dungeons")
	int Level = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	EEnchantmentTypeID TypeId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Dungeons")
	EEnchantmentSource EnchantmentSource;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Dungeons")
	EItemRarity OwnerItemRarity;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool bAlwaysTrigger;

	//Used for mobs. If set to true, this enchantment will not count towards this being an "Enchanted mob"
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool TreatAsInherrent = false;

	bool bIsOverriding = false;

	bool GivesHpBoost() const { return !bIsOverriding; }

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnCurrentCounterValueChanged OnCurrentCounterValueChanged;
protected:
	bool ShouldTriggerOnProjectile(const ABaseProjectile* Projectile) const override;

};
