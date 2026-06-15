#pragma once
#include "game/merchant/MerchantSubobjectBase.h"
#include "game/merchant/selection/MerchantSelectionBase.h"
#include "SelectionChain.generated.h"

class AMerchantBase;

struct SelectionClassPicker {
public:
	using Provider = std::function<TSubclassOf<UMerchantSelectionBase>(const UMerchantSubobjectBase*)>;

	SelectionClassPicker(const Provider& provider);
	SelectionClassPicker(TSubclassOf<UMerchantSelectionBase> selectionClass);

	TSubclassOf<UMerchantSelectionBase> getSelectionClass(const UMerchantSubobjectBase*) const;
private:
	Provider mProvider;
};

DECLARE_MULTICAST_DELEGATE(FOnFocusedSelectionClassChangedInternal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFocusedSelectionClassChanged);

UCLASS(BlueprintType)
class DUNGEONS_API USelectionChain : public UMerchantSubobjectBase {
	GENERATED_BODY()
public:

	static USelectionChain* CreateObject(AMerchantBase* merchant, const FName& name, const TArray<SelectionClassPicker>&);

	FOnFocusedSelectionClassChangedInternal OnFocusedSelectionClassChangedInternal;
private:	
	TArray<SelectionClassPicker> mFocusedSelectionClassOrder;

protected:
	UPROPERTY(Transient)
	TSubclassOf<UMerchantSelectionBase> mFocusedSelectionClass;

	TOptional<int> FindSelectionClassIndex(const TSubclassOf<UMerchantSelectionBase>& selectionClass) const;

	void NavigateRelative(int direction);
public:

	using SelectionClassFilter = std::function<bool(const TSubclassOf<UMerchantSelectionBase>& selectionClass)>;
	TSubclassOf<UMerchantSelectionBase> GetLastSelectionClass(SelectionClassFilter filter) const;
	TSubclassOf<UMerchantSelectionBase> GetFirstSelectionClass(SelectionClassFilter filter) const;
	
	void SetFocusedSelectionClass(const TSubclassOf<UMerchantSelectionBase>& focusedSelectionClass);

	bool ContainsSelectionClass(const TSubclassOf<UMerchantSelectionBase>&) const;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnFocusedSelectionClassChanged OnFocusedSelectionClassChanged;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool CanNavigateNext() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool CanNavigatePrevious() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void NavigatePrevious();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void NavigateNext();	

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const TSubclassOf<UMerchantSelectionBase>& GetSelectionClass() const;

	bool IsSelectionClassInFocusTrail(const TSubclassOf<UMerchantSelectionBase>&) const;
};

