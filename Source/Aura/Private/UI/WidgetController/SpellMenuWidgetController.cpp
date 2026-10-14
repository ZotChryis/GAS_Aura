#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();
	
	BroadcastAbilityInfo();
	BroadcastSpellPoints();
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->AbilityStatusChanged.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const int32 Level)
		{
			if (SelectedAbility.AbilityTag.MatchesTagExact(AbilityTag))
			{
				SelectedAbility.Status = StatusTag;
				
				bool bAllowSpend = false;
				bool bAllowEquip = false;
				ShouldEnableButtons(StatusTag, bAllowSpend, bAllowEquip);
				
				FString Description;
				FString NextLevelDescription;
				GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
				
				SpellGlobeSelectedChangedDelegate.Broadcast(bAllowSpend, bAllowEquip, Description, NextLevelDescription);
			}
			
			if (AbilityInfo)
			{
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
				Info.StatusTag = StatusTag;
				AbilityInfoDelegate.Broadcast(Info);
			}
		}	
	);
	
	GetAuraPlayerState()->OnSpellPointsChanged.AddLambda(
	[this](int32 SpellPoints)
		{
			SpellPointsChangedDelegate.Broadcast(SpellPoints);
		
			bool bAllowSpend = false;
			bool bAllowEquip = false;
			ShouldEnableButtons(SelectedAbility.Status, bAllowSpend, bAllowEquip);
		
			FString Description;
			FString NextLevelDescription;
			GetAuraASC()->GetDescriptionsByAbilityTag(SelectedAbility.AbilityTag, Description, NextLevelDescription);
			
			SpellGlobeSelectedChangedDelegate.Broadcast(bAllowSpend, bAllowEquip, Description, NextLevelDescription);
		}
	);
	
	GetAuraASC()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
}

void USpellMenuWidgetController::BroadcastSpellPoints() const
{
	AAuraPlayerState* PS = Cast<AAuraPlayerState>(PlayerState);
	SpellPointsChangedDelegate.Broadcast(PS->GetSpellPoints());
}

void USpellMenuWidgetController::OnSpellGlobeSelected(const FGameplayTag& InAbilityTag)
{
	const FGameplayTag AbiltyType = AbilityInfo->FindAbilityInfoByTag(InAbilityTag).AbilityType;
	StopWaitForEquipSelectionDelegate.Broadcast(AbiltyType);
	
	SelectedAbility.AbilityTag = InAbilityTag;
	SelectedAbility.Status = FAuraGameplayTags::Get().AbilityStatus_Locked;
	
	if (const FGameplayAbilitySpec* Spec = GetAuraASC()->GetSpecFromAbilityTag(InAbilityTag))
	{
		const FGameplayTag& AbilityStatus = GetAuraASC()->GetAbilityStatusFromSpec(*Spec);
		SelectedAbility.Status = AbilityStatus;
	}
	
	bool bAllowSpend = false;
	bool bAllowEquip = false;
	ShouldEnableButtons(SelectedAbility.Status, bAllowSpend, bAllowEquip);
	
	FString Description;
	FString NextLevelDescription;
	GetAuraASC()->GetDescriptionsByAbilityTag(InAbilityTag, Description, NextLevelDescription);
	
	SpellGlobeSelectedChangedDelegate.Broadcast(bAllowSpend, bAllowEquip, Description, NextLevelDescription);
}

void USpellMenuWidgetController::OnSpellGlobeDeselected()
{
	const FGameplayTag AbiltyType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag).AbilityType;
	StopWaitForEquipSelectionDelegate.Broadcast(AbiltyType);
	
	SelectedAbility.AbilityTag = FAuraGameplayTags::Get().Ability_None;
	SelectedAbility.Status = FAuraGameplayTags::Get().AbilityStatus_Locked;
	
	SpellGlobeSelectedChangedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (!SelectedAbility.AbilityTag.IsValid() || !SelectedAbility.Status.IsValid())
	{
		return;
	}
	
	if (SelectedAbility.Status.MatchesTagExact(FAuraGameplayTags::Get().AbilityStatus_Locked))
	{
		return;
	}
	
	GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.AbilityTag);
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag).AbilityType;
	WaitForEquipSelectionDelegate.Broadcast(AbilityType);

	bWaitingForEquipSelection = true;
	
	const FGameplayTag SelectedStatus = GetAuraASC()->GetStatusTagFromAbilityTag(SelectedAbility.AbilityTag);
	if (SelectedStatus.MatchesTagExact(FAuraGameplayTags::Get().AbilityStatus_Equipped))
	{
		SelectedSlot =  GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& Input, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquipSelection)
	{
		return;
	}
	
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType))
	{
		return;
	}
	
	GetAuraASC()->ServerEquipAbility(SelectedAbility.AbilityTag, Input);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag)
{
	bWaitingForEquipSelection = false;
	
	FAuraAbilityInfo PrevSlotInfo;
	PrevSlotInfo.StatusTag = FAuraGameplayTags::Get().AbilityStatus_Unlocked;
	PrevSlotInfo.AbilityTag = FAuraGameplayTags::Get().Ability_None;
	PrevSlotInfo.InputTag = PrevInputTag;
	AbilityInfoDelegate.Broadcast(PrevSlotInfo);
	
	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
	Info.StatusTag = StatusTag;
	Info.InputTag = InputTag;
	AbilityInfoDelegate.Broadcast(Info);
	
	StopWaitForEquipSelectionDelegate.Broadcast(Info.AbilityType);
	SpellGloveReassignedSignature.Broadcast(AbilityTag);
	OnSpellGlobeDeselected();
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, bool& bAllowSpend, bool& bAllowEquip)
{
	bAllowSpend = false;
	bAllowEquip = false;
	
	if (AbilityStatus.IsValid())
	{
		bAllowSpend = GetAuraPlayerState()->GetSpellPoints() > 0 && (
			AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().AbilityStatus_Eligible) ||
			AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().AbilityStatus_Unlocked) ||
			AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().AbilityStatus_Equipped)
		);
		bAllowEquip = 
			AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().AbilityStatus_Unlocked) || 
			AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().AbilityStatus_Equipped);
	}
}
