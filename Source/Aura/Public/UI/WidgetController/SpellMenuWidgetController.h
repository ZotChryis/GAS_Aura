#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnSpellGlobeSelectedChangedSignature, bool, bAllowSpend, bool, bAllowEquip, FString, Description, FString, NextLevelDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignedSignature, const FGameplayTag&, AbilityTag);

struct FSelectedAbility
{
	FGameplayTag AbilityTag = FGameplayTag();
	FGameplayTag Status= FGameplayTag();
};

UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
	
	void BroadcastSpellPoints() const;
	
	UFUNCTION(BlueprintCallable)
	void OnSpellGlobeSelected(const FGameplayTag& InAbilityTag);
	
	UFUNCTION(BlueprintCallable)
	void OnSpellGlobeDeselected();
	
	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();
	
	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();
	
	UFUNCTION(BlueprintCallable)
	void SpellRowGlobePressed(const FGameplayTag& Input, const FGameplayTag& AbilityType);
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnPlayerStatChangedSignature SpellPointsChangedDelegate;
	
	UPROPERTY(BlueprintAssignable, Category="UI|SpellMenu")
	FOnSpellGlobeSelectedChangedSignature SpellGlobeSelectedChangedDelegate;
	
	UPROPERTY(BlueprintAssignable, Category="UI|SpellMenu")
	FWaitForEquipSelectionSignature WaitForEquipSelectionDelegate;
	
	UPROPERTY(BlueprintAssignable, Category="UI|SpellMenu")
	FWaitForEquipSelectionSignature StopWaitForEquipSelectionDelegate;
	
	UPROPERTY(BlueprintAssignable, Category="UI|SpellMenu")
	FSpellGlobeReassignedSignature SpellGloveReassignedSignature;
	
protected:
	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag);
	
private:
	FSelectedAbility SelectedAbility = { FAuraGameplayTags::Get().Ability_None, FAuraGameplayTags::Get().AbilityStatus_Locked };
	void ShouldEnableButtons(const FGameplayTag& AbilityStatus, bool& bAllowSpend, bool& bAllowEquip);
		
	bool bWaitingForEquipSelection = false;
	FGameplayTag SelectedSlot;
};
