#include "Dungeons.h"
#include "MerchantBase.h"
#include "util/Algo.hpp"
#include "game/component/ShopperComponent.h"
#include "game/component/CharacterSerializeComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/merchant/pricing/MerchantPricingComponent.h"
#include "game/merchant/quest/MerchantQuestBase.h"
#include "game/merchant/focus/SelectionChain.h"
#include "game/actor/character/merchant/MerchantActor.h"
#include "game/actor/character/player/PlayerControllerBase.h"

namespace merchant { namespace pred {

ShowMarker IsWithinDistance(float distance) {
	return [distance](const AMerchantBase* merchant) {
		const auto visibleDistanceSquared = FMath::Pow(distance, 2);
		const auto* merchantActor = merchant->GetMerchantActorOwner();
		const auto* player = merchant->GetPlayerCharacterOwner();
		const auto distanceSquared = FVector::DistSquared(merchantActor->GetActorLocation(), player->GetActorLocation());
		return distanceSquared < visibleDistanceSquared;
	};
}

void ShowMarkerContainer::Add(const ShowMarker& pred) {
	Preds.Add(pred);
}

bool ShowMarkerContainer::ShouldHideMarkers(const AMerchantBase* merchant) const {
	return !algo::all_of(Preds, RETLAMBDA(it(merchant)));
}

}}


AMerchantBase::AMerchantBase() : mCurrentContext() {
	mShowMarkerPredicates.Add(merchant::pred::IsWithinDistance(13000.f));
}
AMerchantBase::AMerchantBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer) {
}

void AMerchantBase::AssignPlayerController(const APlayerControllerBase* controller, const FName& saveDataName, const FText& displayName) {
	check(controller != nullptr && "assigning null player");
	check(controller->GetSaveData() != nullptr && "controller have no save data");
	mSaveName = saveDataName;
	mDisplayName = displayName;

	auto pawn = Cast<APlayerCharacter>(controller->GetPawn());
	if (pawn && GetClass()->GetDefaultObject(false) != this) {
		mPlayerCharacterOwner = pawn;
		mCurrentContext = CreateContext(pawn->GetCharacterSerializeComponent());
		mCurrentSession = CreateSession(pawn);
		for (auto subObject : mSubObjects) {
			subObject->OnSetupSession();
		}
	} else {
		mCurrentContext = CreateContext(controller->FindComponentByClass<UCharacterSerializeComponent>());
	}	
}



void AMerchantBase::ClearPlayerController() {
	mSaveName = NAME_None;
	mDisplayName = FText::GetEmpty();
	mPlayerCharacterOwner = nullptr;
	mCurrentContext.Reset();
	if (mCurrentSession.IsSet()) {
		if(GetWorld()){ //If we don't have a world, we don't really need to clean anything up. This catched endplay in editor and similar.
			check(GetClass()->GetDefaultObject(false) != this && "Sessions should never exist on class default objects");
			for (auto subObject : mSubObjects) {
				subObject->OnCleanupSession();
			}
		}
		mCurrentSession.Reset();
	}
}

void AMerchantBase::EnsureSaveData() {
	check(mCurrentSession.IsSet() && "Trying to ensure save data without a session");
	auto& context = GetCurrentContext();
	if (!context.HasSaveData()) {
		context.CreateSaveData();
	}
	for (auto& slot : GetSlots()) {
		slot->EnsureSaveData();
	}
	if (mMerchantPricingComponent) {
		mMerchantPricingComponent->EnsureSaveData();
	}
	CheckRestockQuest();
	Refresh();
}

void AMerchantBase::CheckRestockQuest() {
	if (mRestockQuest) {
		if (mRestockQuest->EnsureSaveData() || mRestockQuest->IsCompleted()) {
			mRestockQuest->ResetProgress();
			RestockSlots();
			if (mMerchantPricingComponent) {
				mMerchantPricingComponent->OnRestockQuestCompleted();
			}
		}
	}
}

void AMerchantBase::RestockSlots() {
	for (auto& slot : GetSlots()) {
		slot->Restock();
	}
	GetCurrentContext().EditSaveData().EditPricing().IncrementRestocked();
	Refresh();
}


APlayerCharacter* AMerchantBase::GetPlayerCharacterOwner() const {
	return mPlayerCharacterOwner;
}

const merchant::Context& AMerchantBase::GetCurrentContext() const {
	check(mCurrentContext.IsSet() && "cannot get context when it doesn't exists... duh");
	return mCurrentContext.GetValue();
}

const merchant::Session& AMerchantBase::GetCurrentSession() const {
	check(mCurrentSession.IsSet() && "cannot get session when it doesn't exists... duh");
	return mCurrentSession.GetValue();
}

const FName& AMerchantBase::GetSaveName() const {
	return mSaveName;
}

const FText& AMerchantBase::GetDisplayName() const {
	return mDisplayName;
}

const FText& AMerchantBase::GetDisplayDescription() const {
	return mDisplayDescription;
}

const TArray<FItemBulletPoint>& AMerchantBase::GetDisplayBulletPoints() const {
	return mDisplayBulletPoints;
}


const FText& AMerchantBase::GetLockedSlotsHint() const {
	return mLockedSlotsHint;
}

bool AMerchantBase::HasAnyLockedSlots() const {
	return algo::any_of(slots, RETLAMBDA(it->GetActiveUnlockQuest() != nullptr));
}

bool AMerchantBase::ShouldShowObjectiveMarker() const {
	if (mShowMarkerPredicates.ShouldHideMarkers(this)) {
		return false;
	}
	if (!GetCurrentContext().ReadSaveData().HasInteracted()) {
		return true;
	}
	return algo::any_of(slots, RETLAMBDA(it->ShouldShowObjectiveMarker()));
}

bool AMerchantBase::ShouldShowEnchantmentPoints() const {
	for (const auto& pair : selections) {
		if (pair.Value->ShouldShowEnchantmentPoints()) {
			return true;
		}
	}
	return false;
}

TArray<FItemBulletPoint> AMerchantBase::GetRestockMethodsBulletPoints() const {
	TArray<FItemBulletPoint> points;
	if (mRestockQuest != nullptr) {
		points.Add({ mRestockQuest->GetExplainerText(), EBulletIcon::Common });
	}
	for (const auto& pair : transactions) {
		if (pair.Value->GetSuccessStatusReason() == EMerchantTransactionStatusReason::RESTOCK) {
			points.Add({ NSLOCTEXT("Merchants","Manually_pay_to_restock_the_store", "Pay to restock the store"), EBulletIcon::Common });
			break;
		}
	}
	return points;
}

AMerchantActor* AMerchantBase::GetMerchantActorOwner() const {
	return Cast<AMerchantActor>(GetOwner());
}

void AMerchantBase::BroadcastItemAnnouncement(const FItemAnnouncement& announcement) const {
	OnItemAnnouncement.Broadcast(announcement);
}

merchant::Context AMerchantBase::CreateContext(UCharacterSerializeComponent* serializer) {
	return { *this, *serializer };
}

merchant::Session AMerchantBase::CreateSession(APlayerCharacter* character) {
	return { *this, *character };
}

TSet<TSubclassOf<UMerchantTransactionBase>> AMerchantBase::GatherAvailableTransactions() {
	TSet<TSubclassOf<UMerchantTransactionBase>> transactionClasses;

	transactionClasses.Append(mPermanentTransactionClasses);

	if (mPermanentSelectionClass) {
		if (auto selection = GetSelectionByClass(mPermanentSelectionClass)) {
			transactionClasses.Append(selection->GetAvailableTransactions());
		}
	}

	return transactionClasses;
}

TSet<TSubclassOf<UMerchantSelectionBase>> AMerchantBase::GatherRequiredSelections() {
	TSet<TSubclassOf<UMerchantSelectionBase>> selectionClasses;
	if (mPermanentSelectionClass) {
		selectionClasses.Add(mPermanentSelectionClass);
	}

	auto transactionClasses = GatherAvailableTransactions();
	for (auto& transactionClass : transactionClasses) {
		if (auto transaction = GetTransactionByClass(transactionClass)) {
			selectionClasses.Append(transaction->GetRequiredSelectionClasses());
		}
	}

	return selectionClasses;
}

bool AMerchantBase::CanTransactionSelectAllRequiredSelections(const UMerchantTransactionBase* transaction) const {
	auto selectionClasses = transaction->GetRequiredSelectionClasses();
	return (selectionClasses.Num() == 0) || algo::all_of(selectionClasses.Array(), RETLAMBDA( GetSelectionByClass(it) ? GetSelectionByClass(it)->CanSelectAny() : false ));
}

USelectionChain* AMerchantBase::GetSelectionFocus() const {
	return mSelectionFocus;
}

void AMerchantBase::Refresh() {
	auto transactionClasses = GatherAvailableTransactions();

	if (AnyDifference(mTransactionClasses,transactionClasses)){
		mTransactionClasses = transactionClasses;
		for (auto& transactionClass : transactionClasses) {
			if (!GetTransactionByClass(transactionClass)) {
				CreateTransactionByClass(transactionClass);
			}
		}
		OnAvailableTransactionsChanged.Broadcast();
	}

	auto selectionClasses = GatherRequiredSelections();
	if (AnyDifference(mSelectionClasses, selectionClasses)){
		mSelectionClasses = selectionClasses;
		for (auto& selectionClass : selectionClasses) {
			if (!GetSelectionByClass(selectionClass)) {
				CreateSelectionByClass(selectionClass);
			}
		}
		OnRequiredSelectionsChanged.Broadcast();
	}
	
	
	if(mAlwaysSelectSelectionClass){
		if(auto selection = GetSelectionByClass(mAlwaysSelectSelectionClass)){
			selection->EnsureSelection();
		}
	}

	for (auto transaction : transactions) {
		transaction.Value->Refresh();
	}

	for (auto selection : selections) {
		selection.Value->Refresh();
	}
}

UMerchantSelectionBase* AMerchantBase::CreateSelectionByClass(const TSubclassOf<UMerchantSelectionBase>& selectionClass) {
	auto selection = NewObject<UMerchantSelectionBase>(this, selectionClass, selectionClass->GetDefaultObjectName());
	return AddSelection(selection);	
}

UMerchantSelectionBase* AMerchantBase::AddSelection(UMerchantSelectionBase* selection) {
	selection->OnSelectionChangedInternal.AddUObject(this, &AMerchantBase::OnSelectionChanged);
	selection->OnSelectionChangedInternalAfterBlueprints.AddUObject(this, &AMerchantBase::OnSelectionChangedAfterBlueprints);
	selection->OnSelectionConfirmedInternal.AddUObject(this, &AMerchantBase::OnSelectionConfirmed);
	selection->OnSelectionCancelledInternal.AddUObject(this, &AMerchantBase::OnSelectionCancelled);	
	return selections.Add(selection->GetClass(), selection);
}

void AMerchantBase::CreateTransactionByClass(const TSubclassOf<UMerchantTransactionBase>& transactionClass) {
	auto transaction = NewObject<UMerchantTransactionBase>(this, transactionClass, transactionClass->GetDefaultObjectName());
	AddTransaction(transaction);
}

UMerchantTransactionBase* AMerchantBase::AddTransaction(UMerchantTransactionBase* transaction) {
	transaction->OnMerchantTransactionExecutedInternal.AddUObject(this, &AMerchantBase::OnTransactionExecuted);	
	return transactions.Add(transaction->GetClass(), transaction);
}


void AMerchantBase::OnSelectionChanged(const UMerchantSelectionBase& selectionChanged) {
	Refresh();
}

void AMerchantBase::OnSelectionChangedAfterBlueprints(const UMerchantSelectionBase& selectionChanged) {
	if (mSelectionFocus && selectionChanged.HasSelectedAny() && mSelectionFocus->ContainsSelectionClass(selectionChanged.GetClass())) {
		mSelectionFocus->SetFocusedSelectionClass(selectionChanged.GetClass());
	}
}

TSubclassOf<UMerchantSelectionBase> AMerchantBase::GetSelectionClassForCancel(const TSubclassOf<UMerchantSelectionBase>& cancelledClass) const {
	return mSelectionFocus->GetLastSelectionClass([this, cancelledClass](const TSubclassOf<UMerchantSelectionBase>& selectionClass) {
		return selectionClass != cancelledClass && IsSelectionClassRequired(selectionClass) && GetSelectionByClass(selectionClass)->HasSelectedAny();
	});
}

TSubclassOf<UMerchantSelectionBase> AMerchantBase::GetSelectionClassForConfirm(const TSubclassOf<UMerchantSelectionBase>& confirmedClass) const {
	return mSelectionFocus->GetLastSelectionClass([this, confirmedClass](const TSubclassOf<UMerchantSelectionBase>& selectionClass) {
		return selectionClass != confirmedClass && IsSelectionClassRequired(selectionClass) && !GetSelectionByClass(selectionClass)->HasSelectedAny();
	});
}

void AMerchantBase::OnSelectionCancelled(UMerchantSelectionBase& selection) {
	if(mSelectionFocus){
		if (auto bestSelectionClass = GetSelectionClassForCancel(selection.GetClass()))	{
			mSelectionFocus->SetFocusedSelectionClass(bestSelectionClass);		
		}
	}
}

void AMerchantBase::OnSelectionConfirmed(UMerchantSelectionBase& selection) {
	if (mSelectionFocus) {
		if(auto bestSelectionClass = GetSelectionClassForConfirm(selection.GetClass())) {
			mSelectionFocus->SetFocusedSelectionClass(bestSelectionClass);		
		}		
	}
}

void AMerchantBase::OnTransactionExecuted(const UMerchantTransactionBase& transaction) {
	OnMerchantTransactionExecuted.Broadcast(transaction);
	Refresh();
}

void AMerchantBase::BeginPlay() {
	check(mPlayerCharacterOwner != nullptr && "no controller assigned");
	check(mCurrentSession.IsSet() && "trying to create a merchant without a session");
	if(mPermanentSelectionClass){
		CreateSelectionByClass(mPermanentSelectionClass);
	}
	Refresh();
}

void AMerchantBase::EndPlay(EEndPlayReason::Type reason) {
	ClearPlayerController();
}

void AMerchantBase::RegisterSubObject(UMerchantSubobjectBase* subObject) {
	mSubObjects.Add(subObject);
	if (mCurrentSession.IsSet()) {
		subObject->OnSetupSession();
	}
}

void AMerchantBase::OnFocusedSelectionClassChanged() {	
	TArray<TSubclassOf<UMerchantSelectionBase>> selectionClasses;
	selections.GenerateKeyArray(selectionClasses);
	for (auto selectionClass : selectionClasses) {
		if(auto selection = GetSelectionByClass(selectionClass)){
			if (mSelectionFocus->IsSelectionClassInFocusTrail(*selectionClass)) {
				selection->EnsureSelection();
			} else if(mSelectionFocus->ContainsSelectionClass(*selectionClass))  {
				selection->ClearSelection();
			}
		}
	}
	OnSelectionFocusChanged.Broadcast();
}

void AMerchantBase::OnPlayerInteraction(APlayerCharacter* player) {
	if (auto controller = player->GetController()) {
		if (auto shopper = controller->FindComponentByClass<UShopperComponent>()) {
			shopper->OpenMerchantWidgetFor(this);
		}
	}
}

void AMerchantBase::OnOpeningUi() {
	CheckRestockQuest();
	Refresh();
	OnFocusedSelectionClassChanged();
	if (!GetCurrentContext().ReadSaveData().HasInteracted()) {
		GetCurrentContext().EditSaveData().SetInteracted(true);
	}
}

void AMerchantBase::OnBindingToWidget()
{
	if (!mSelectionFocus) {
		mSelectionFocus = USelectionChain::CreateObject(this, TEXT("Focus"), mSelectionClassFocusOrder);
		mSelectionFocus->OnFocusedSelectionClassChangedInternal.AddUObject(this, &AMerchantBase::OnFocusedSelectionClassChanged);
	}	
}

UMerchantCurrencyComponent* AMerchantBase::GetCurrencyComponent() const {
	return mMerchantCurrencyComponent;
}

const UMerchantPricingComponent* AMerchantBase::GetPricingComponent() const {
	return mMerchantPricingComponent;
}

bool AMerchantBase::IsTransactionClassAvailable(const TSubclassOf<UMerchantTransactionBase>& transactionClass) const {
	return mTransactionClasses.Contains(transactionClass);
}

bool AMerchantBase::IsSelectionClassRequired(const TSubclassOf<UMerchantSelectionBase>& selectionClass) const {
	return mSelectionClasses.Contains(selectionClass);
}

bool AMerchantBase::IsSelectionClassFocused(const TSubclassOf<UMerchantSelectionBase>& selectionClass) const {
	if (mSelectionFocus) {
		return mSelectionFocus->GetSelectionClass() == selectionClass;
	}
	return false;
}

bool AMerchantBase::CanConfirmSelection(const TSubclassOf<UMerchantSelectionBase>& selectionClass) const {
	return GetSelectionClassForConfirm(selectionClass) != nullptr;
}

bool AMerchantBase::CanCancelSelection(const TSubclassOf<UMerchantSelectionBase>& selectionClass) const {
	return GetSelectionClassForCancel(selectionClass) != nullptr;
}

UMerchantSelectionBase* AMerchantBase::GetOrCreateSelectionByClass(const TSubclassOf<UMerchantSelectionBase>& selectionClass) {
	auto selection = GetSelectionByClass(selectionClass);
	if (!selection) {
		return CreateSelectionByClass(selectionClass);
	}
	return selection;
}

UMerchantSelectionBase* AMerchantBase::GetSelectionByClass(const TSubclassOf<UMerchantSelectionBase>& selectionClass) const {
	if (auto selectionPtr = selections.Find(selectionClass)) {
		return *selectionPtr;
	}
	return nullptr;
}

UMerchantTransactionBase* AMerchantBase::GetTransactionByClass(const TSubclassOf<UMerchantTransactionBase>& transactionClass) const {
	if (auto transactionPtr = transactions.Find(transactionClass)) {
		return *transactionPtr;
	}
	return nullptr;
}

const TArray<UMerchantSlotBase*>& AMerchantBase::GetSlots() const {
	return slots;
}

int AMerchantBase::GetCompletedSlotQuests() const {
	if (!GetCurrentContext().HasSaveData()) { return 0; }
	return algo::count_if(GetSlots(), RETLAMBDA(it->HasCompletedUnlockQuest()));
}
