#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpellGlobeSelectedChangedSignature, bool, bAllowSpend, bool, bAllowEquip);

struct FSelectedAbility
{
	FGameplayTag Ability = FGameplayTag();
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
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnPlayerStatChangedSignature SpellPointsChangedDelegate;
	
	UPROPERTY(BlueprintAssignable, Category="UI|SpellMenu")
	FOnSpellGlobeSelectedChangedSignature SpellGlobeSelectedChangedDelegate;
	
private:
	FSelectedAbility SelectedAbility = { FAuraGameplayTags::Get().Ability_None, FAuraGameplayTags::Get().AbilityStatus_Locked };
	void ShouldEnableButtons(const FGameplayTag& AbilityStatus, bool& bAllowSpend, bool& bAllowEquip);
		
};
