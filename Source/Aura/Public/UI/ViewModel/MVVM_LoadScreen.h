#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

class UMVVM_LoadSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotSelected);

UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	
	void InitializeLoadSlots();
	void LoadData();
	
	UPROPERTY(BlueprintAssignable)
	FSlotSelected SlotSelected;
	
	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 SlotIndex);
	
	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 SlotIndex, const FString& Name);
	
	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 SlotIndex);
	
	UFUNCTION(BlueprintCallable)
	void PlayButtonPressed();
	
	UFUNCTION(BlueprintCallable)
	void DeleteSelectedSlot();
	
	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModel(int32 SlotIndex) const;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;
	
private:
	
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> SelectedSlot;
	
	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> LoadSlots;
	
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot0;
	
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot1;
	
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot2;
};
