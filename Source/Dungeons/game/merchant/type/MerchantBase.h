#pragma once
#include <GameFramework/Info.h>
#include "game/merchant/slot/MerchantSlotBase.h"
#include "game/merchant/selection/MerchantSelectionBase.h"
#include "game/merchant/announcement/ItemAnnouncement.h"
#include "game/merchant/MerchantSaveData.h"
#include "game/item/ItemBulletPoint.h"
#include "game/merchant/focus/SelectionChain.h"
#include "util/Algo.hpp"
#include "MerchantBase.generated.h"

class UInventoryItem;
class USelectionChain;
class APlayerControllerBase;
class UCharacterSerializeComponent;


DECLARE_MULTICAST_DELEGATE(FOnAvailableTransactionsChanged);
DECLARE_MULTICAST_DELEGATE(FOnRequiredSelectionsChanged);
DECLARE_MULTICAST_DELEGATE(FOnSelectionFocusChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAnnouncement, const FItemAnnouncement&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMerchantTransactionExecuted, const UMerchantTransactionBase&);

class APlayerCharacter;
class UMerchantPricingComponent;
class UMerchantCurrencyComponent;
class AMerchantActor;

namespace merchant { namespace pred {

using ShowMarker = std::function<bool(const AMerchantBase*)>;

struct ShowMarkerContainer {
	void Add(const ShowMarker&);
	bool ShouldHideMarkers(const AMerchantBase*) const;

	TArray<ShowMarker> Preds;
};

ShowMarker IsWithinDistance(float distance);

}}


UCLASS(Abstract)
class DUNGEONS_API AMerchantBase : public AInfo {
	GENERATED_BODY()
	friend class UMerchantDefComponent;
	friend class UMerchantDef;

public:
	AMerchantBase();
	AMerchantBase(const FObjectInitializer& ObjectInitializer);

	FOnAvailableTransactionsChanged OnAvailableTransactionsChanged;
	FOnRequiredSelectionsChanged OnRequiredSelectionsChanged;	
	FOnSelectionFocusChanged OnSelectionFocusChanged;
	FOnItemAnnouncement OnItemAnnouncement;
	FOnMerchantTransactionExecuted OnMerchantTransactionExecuted;

	const merchant::Context& GetCurrentContext() const;
	const merchant::Session& GetCurrentSession() const;

	const FName& GetSaveName() const;

	const FText& GetDisplayName() const;
	const FText& GetDisplayDescription() const;
	const TArray<FItemBulletPoint>& GetDisplayBulletPoints() const;
	const FText& GetLockedSlotsHint() const;

	bool HasAnyLockedSlots() const;

	bool ShouldShowObjectiveMarker() const;

	bool ShouldShowEnchantmentPoints() const;

	TArray<FItemBulletPoint> GetRestockMethodsBulletPoints() const;

	AMerchantActor* GetMerchantActorOwner() const;

	void BroadcastItemAnnouncement(const FItemAnnouncement& announcement) const;

private:
	FName mSaveName;
	FText mDisplayName;

	TOptional<merchant::Context> mCurrentContext;
	TOptional<merchant::Session> mCurrentSession;

	UPROPERTY(Transient)
	TArray<UMerchantSubobjectBase*> mSubObjects;

	UPROPERTY(Transient)
	TArray<UMerchantSlotBase*> slots;

	UPROPERTY(Transient)
	TMap<TSubclassOf<UMerchantTransactionBase>, UMerchantTransactionBase*> transactions;

	UPROPERTY(Transient)
	TMap<TSubclassOf<UMerchantSelectionBase>, UMerchantSelectionBase*> selections;

	UPROPERTY(Transient)
	USelectionChain* mSelectionFocus;

	void OnSelectionChanged(const UMerchantSelectionBase&);
	void OnSelectionChangedAfterBlueprints(const UMerchantSelectionBase&);
	void OnSelectionCancelled(UMerchantSelectionBase&);
	void OnSelectionConfirmed(UMerchantSelectionBase&);
	void OnTransactionExecuted(const UMerchantTransactionBase&);

	TSubclassOf<UMerchantSelectionBase> GetSelectionClassForCancel(const TSubclassOf<UMerchantSelectionBase>& cancelledClass) const;
	TSubclassOf<UMerchantSelectionBase> GetSelectionClassForConfirm(const TSubclassOf<UMerchantSelectionBase>& confirmedClass) const;
	
	void AssignPlayerController(const APlayerControllerBase*, const FName& saveDataName, const FText& displayName);
	void ClearPlayerController();
		
	void EnsureSaveData();

	void CheckRestockQuest();

	merchant::Context CreateContext(UCharacterSerializeComponent*);
	merchant::Session CreateSession(APlayerCharacter*);

	TSet<TSubclassOf<UMerchantTransactionBase>> GatherAvailableTransactions();

	TSet<TSubclassOf<UMerchantSelectionBase>> GatherRequiredSelections();	

	template <class T>
	bool AnyDifference(TSet<T> SetA, TSet<T> SetB) const {
		return SetA.Difference(SetB).Num() > 0 || SetB.Difference(SetA).Num() > 0;
	}

	UPROPERTY(Transient)
	TSet<TSubclassOf<UMerchantTransactionBase>> mTransactionClasses;

	UPROPERTY(Transient)
	TSet<TSubclassOf<UMerchantSelectionBase>> mSelectionClasses;

	void Refresh();

	UMerchantSelectionBase* CreateSelectionByClass(const TSubclassOf<UMerchantSelectionBase>&);	
	void CreateTransactionByClass(const TSubclassOf<UMerchantTransactionBase>&);

	void OnFocusedSelectionClassChanged();

protected:	
	template <class T>
	T* CreateDefaultSlotSubobject(FName name) {
		auto slot = CreateDefaultSubobject<T>(name);
		return AddSlot(slot);
	}

	template <class T>
	T* AddSlot(T* slot) {		
		slots.Add(slot);
		return slot;
	}

	UMerchantTransactionBase* AddTransaction(UMerchantTransactionBase*);
	UMerchantSelectionBase* AddSelection(UMerchantSelectionBase*);

	UPROPERTY(Transient)
	FText mDisplayDescription;

	UPROPERTY(Transient)
	TArray<FItemBulletPoint> mDisplayBulletPoints;

	UPROPERTY(Transient)
	FText mLockedSlotsHint;

	UPROPERTY(Transient)
	UMerchantQuestBase* mRestockQuest;

	UPROPERTY()
	TSubclassOf<UMerchantSelectionBase> mPermanentSelectionClass;

	UPROPERTY()
	TSubclassOf<UMerchantSelectionBase> mAlwaysSelectSelectionClass;

	UPROPERTY(Transient)
	UMerchantPricingComponent* mMerchantPricingComponent;
	
	UPROPERTY(Transient)
	UMerchantCurrencyComponent* mMerchantCurrencyComponent;

	UPROPERTY(Transient)
	APlayerCharacter* mPlayerCharacterOwner;

	UPROPERTY(Transient)
	TSet<TSubclassOf<UMerchantTransactionBase>> mPermanentTransactionClasses;
	
	TArray<SelectionClassPicker> mSelectionClassFocusOrder;
	
	merchant::pred::ShowMarkerContainer mShowMarkerPredicates;
public:
	void BeginPlay() override;	
	void EndPlay(EEndPlayReason::Type) override;

	void RegisterSubObject(UMerchantSubobjectBase*);

	void OnOpeningUi();
	void OnBindingToWidget();

	virtual void OnPlayerInteraction(APlayerCharacter*);

	template <class T>
	const TArray<T*> GetSlots() const {
		return algo::map_if_as<TArray<T*>>(GetSlots(), RETLAMBDA(Cast<T>(it)), RETLAMBDA(Cast<T>(it)));
	}

	const TArray<UMerchantSlotBase*>& GetSlots() const;

	UMerchantCurrencyComponent* GetCurrencyComponent() const;
	const UMerchantPricingComponent* GetPricingComponent() const;

	bool IsTransactionClassAvailable(const TSubclassOf<UMerchantTransactionBase>&) const;
	bool IsSelectionClassRequired(const TSubclassOf<UMerchantSelectionBase>&) const;
	bool IsSelectionClassFocused(const TSubclassOf<UMerchantSelectionBase>&) const;
	bool CanConfirmSelection(const TSubclassOf<UMerchantSelectionBase>&) const;
	bool CanCancelSelection(const TSubclassOf<UMerchantSelectionBase>&) const;
	bool CanTransactionSelectAllRequiredSelections(const UMerchantTransactionBase*) const;
	USelectionChain* GetSelectionFocus() const;

	int GetCompletedSlotQuests() const;	

	void RestockSlots();

	APlayerCharacter* GetPlayerCharacterOwner() const;


	template <class T>
	T* GetSelection() const {
		if (TSubclassOf<UMerchantSelectionBase> selectionClass = T::StaticClass()) {
			return Cast<T>(GetSelectionByClass(selectionClass));
		}
		return nullptr;
	}

	template <class T>
	T* GetTransaction() const {
		if (TSubclassOf<UMerchantTransactionBase> transactionClass = T::StaticClass()) {
			return Cast<T>(GetTransactionByClass(transactionClass));
		}
		return nullptr;
	}

	UMerchantSelectionBase* GetOrCreateSelectionByClass(const TSubclassOf<UMerchantSelectionBase>&);
	UMerchantSelectionBase* GetSelectionByClass(const TSubclassOf<UMerchantSelectionBase>&) const;
	UMerchantTransactionBase* GetTransactionByClass(const TSubclassOf<UMerchantTransactionBase>&) const;	

};