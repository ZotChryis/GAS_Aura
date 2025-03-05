#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1.f);
		const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability);
		if (!AuraAbility)
		{
			continue;
		}
		
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraAbility->StartupInputTag);
		GiveAbility(AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	TArray<FGameplayAbilitySpec> Abilities = GetActivatableAbilities();
	for (FGameplayAbilitySpec& Ability : Abilities)
	{
		if (!Ability.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			continue;
		}

		AbilitySpecInputPressed(Ability);
		
		if (Ability.IsActive())
		{
			continue;
		}

		TryActivateAbility(Ability.Handle);
	} 
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	TArray<FGameplayAbilitySpec> Abilities = GetActivatableAbilities();
	for (FGameplayAbilitySpec& Ability : Abilities)
	{
		if (!Ability.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			continue;
		}

		AbilitySpecInputReleased(Ability);
	} 
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle EffectHandle) const
{
	FGameplayTagContainer Tags;
	EffectSpec.GetAllAssetTags(Tags);
	
	EffectAssetTags.Broadcast(Tags);
}
