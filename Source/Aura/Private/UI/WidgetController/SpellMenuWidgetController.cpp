#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
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
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
		{
			if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
			{
				SelectedAbility.Status = StatusTag;
				
				bool bAllowSpend = false;
				bool bAllowEquip = false;
				ShouldEnableButtons(StatusTag, bAllowSpend, bAllowEquip);
				SpellGlobeSelectedChangedDelegate.Broadcast(bAllowSpend, bAllowEquip);
			}
			
			if (AbilityInfo)
			{
				FAuraAbilityInfo Info = AbilityInfo->GetAbilityInfoByTag(AbilityTag);
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
			SpellGlobeSelectedChangedDelegate.Broadcast(bAllowSpend, bAllowEquip);
		}
	);
}

void USpellMenuWidgetController::BroadcastSpellPoints() const
{
	AAuraPlayerState* PS = Cast<AAuraPlayerState>(PlayerState);
	SpellPointsChangedDelegate.Broadcast(PS->GetSpellPoints());
}

void USpellMenuWidgetController::OnSpellGlobeSelected(const FGameplayTag& InAbilityTag)
{
	bool bAllowSpend = false;
	bool bAllowEquip = false;
	
	SelectedAbility.Ability = InAbilityTag;
	SelectedAbility.Status = FAuraGameplayTags::Get().AbilityStatus_Locked;
	
	if (const FGameplayAbilitySpec* Spec = GetAuraASC()->GetSpecFromAbilityTag(InAbilityTag))
	{
		const FGameplayTag& AbilityStatus = GetAuraASC()->GetAbilityStatusFromSpec(*Spec);
		SelectedAbility.Status = AbilityStatus;
	}
	
	ShouldEnableButtons(SelectedAbility.Status, bAllowSpend, bAllowEquip);
	SpellGlobeSelectedChangedDelegate.Broadcast(bAllowSpend, bAllowEquip);
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
		bAllowEquip = AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().AbilityStatus_Unlocked);
	}
}
