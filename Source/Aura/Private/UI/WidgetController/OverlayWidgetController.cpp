#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "Engine/Engine.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();

	OnPlayerLevelChangedDelegate.Broadcast(GetAuraPlayerState()->GetPlayerLevel());
	OnExperiencePercentChangedDelegate.Broadcast(GetAuraPlayerState()->GetExperience());
	
	OnHealthChanged.Broadcast(GetAuraAttributeSet()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAttributeSet()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAttributeSet()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAttributeSet()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	GetAuraPlayerState()->OnExperienceChanged.AddUObject(this, &UOverlayWidgetController::OnExperienceChanged);
	GetAuraPlayerState()->OnLevelChanged.AddLambda([&](const int32 NewLevel)
	{
		OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
	});
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetHealthAttribute()).AddLambda(
		[&](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetMaxHealthAttribute()).AddLambda(
		[&](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetManaAttribute()).AddLambda(
		[&](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetMaxManaAttribute()).AddLambda(
		[&](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);

	GetAuraASC()->EffectAssetTags.AddLambda(
		[&](const FGameplayTagContainer& AssetTags)
		{
			const FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
			for (const FGameplayTag& Tag : AssetTags)
			{
				if (!Tag.MatchesTag(MessageTag))
				{
					continue;
				}

				const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
				MessageWidgetRowDelegate.Broadcast(*Row);
			}
		}
	);
	
	GetAuraASC()->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);

	if (GetAuraASC()->bStartupAbilitiesGiven)
	{
		BroadcastAbilityInfo();
	}
	else
	{
		GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
	}
}

void UOverlayWidgetController::OnExperienceChanged(int32 NewExperience)
{
	const ULevelUpInfo* LevelUpInfo = GetAuraPlayerState()->LevelUpData;
	checkf(LevelUpInfo, TEXT("Player State did not have LevelUpInfo. Fill that out!"));

	const int32 Level = LevelUpInfo->FindLevelForExperience(GetAuraPlayerState()->GetExperience());
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

	if (Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PrevLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;
		const int32 DeltaLevelUpRequirement = LevelUpRequirement - PrevLevelUpRequirement;
		const int32 ExperienceInCurrentLevel = NewExperience - PrevLevelUpRequirement;

		const float ExperiencePercentage = static_cast<float>(ExperienceInCurrentLevel) / static_cast<float>(DeltaLevelUpRequirement);
		OnExperiencePercentChangedDelegate.Broadcast(ExperiencePercentage);
	}
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag) const
{
	FAuraAbilityInfo PrevSlotInfo;
	PrevSlotInfo.StatusTag = FAuraGameplayTags::Get().AbilityStatus_Unlocked;
	PrevSlotInfo.AbilityTag = FAuraGameplayTags::Get().Ability_None;
	PrevSlotInfo.InputTag = PrevInputTag;
	AbilityInfoDelegate.Broadcast(PrevSlotInfo);
	
	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
	Info.StatusTag = StatusTag;
	Info.InputTag = InputTag;
	AbilityInfoDelegate.Broadcast(Info);
}
