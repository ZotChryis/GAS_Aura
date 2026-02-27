#include "UI/WidgetController/OverlayWidgetController.h"
#include "Engine/Engine.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();

	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	OnPlayerLevelChangedDelegate.Broadcast(AuraPlayerState->GetPlayerLevel());
	OnExperiencePercentChangedDelegate.Broadcast(AuraPlayerState->GetExperience());
	
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->OnExperienceChanged.AddUObject(this, &UOverlayWidgetController::OnExperienceChanged);
	AuraPlayerState->OnLevelChanged.AddLambda([&](const int32 NewLevel)
	{
		OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
	});
	
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
		[&](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
		[&](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda(
		[&](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
		[&](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);

	UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraAbilitySystemComponent->EffectAssetTags.AddLambda(
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

	if (AuraAbilitySystemComponent->bStartupAbilitiesGiven)
	{
		OnStartupAbilitiesInitialized(AuraAbilitySystemComponent);
	}
	else
	{
		AuraAbilitySystemComponent->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnStartupAbilitiesInitialized);
	}
}

void UOverlayWidgetController::OnStartupAbilitiesInitialized(UAuraAbilitySystemComponent* AuraASC)
{
	if (!AuraASC || !AuraASC->bStartupAbilitiesGiven)
	{
		return;
	}

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda(
		[this](const FGameplayAbilitySpec& AbilitySpec){
			FAuraAbilityInfo Info = AbilityInfo->GetAbilityInfoByTag(UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec));
			Info.InputTag = UAuraAbilitySystemComponent::GetAbilityInputTagFromSpec(AbilitySpec);

			AbilityInfoDelegate.Broadcast(Info);
		}
	);

	AuraASC->ForEachAbility(BroadcastDelegate);
}

void UOverlayWidgetController::OnExperienceChanged(int32 NewExperience) const
{
	const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	const ULevelUpInfo* LevelUpInfo = AuraPlayerState->LevelUpData;
	checkf(LevelUpInfo, TEXT("Player State did not have LevelUpInfo. Fill that out!"));

	const int32 Level = LevelUpInfo->FindLevelForExperience(AuraPlayerState->GetExperience());
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
