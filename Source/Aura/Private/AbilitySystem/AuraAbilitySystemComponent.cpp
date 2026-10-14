#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
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
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().AbilityStatus_Equipped);
		GiveAbility(AbilitySpec);
	}

	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1.f);
		GiveAbilityAndActivateOnce(AbilitySpec);
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

void UAuraAbilitySystemComponent::ForEachAbility(FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (const UGameplayAbility* Ability = AbilitySpec.Ability.Get())
	{
		for (FGameplayTag Tag : Ability->GetAssetTags())
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability"))))
			{
				return Tag;
			}
		}
	}

	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (const UGameplayAbility* Ability = AbilitySpec.Ability.Get())
	{
		for (FGameplayTag Tag : Ability->GetAssetTags())
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Input"))))
			{
				return Tag;
			}
		}
	}

	for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Input"))))
		{
			return Tag;
		}
	}

	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability.Status"))))
		{
			return Tag;
		}
	}
	
	return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& InAbilityTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (const FGameplayTag& AbilityTag : AbilitySpec.Ability->AbilityTags)
		{
			if (AbilityTag.MatchesTag(InAbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}
	
	return nullptr;
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (AbilityInfo == nullptr)
	{
		return;
	}
	
	for (const FAuraAbilityInfo& AbilityInformation : AbilityInfo->AbilityInformation)
	{
		if (Level < AbilityInformation.LevelRequirement)
		{
			continue;
		}
		
		if (GetSpecFromAbilityTag(AbilityInformation.AbilityTag) != nullptr)
		{
			continue;
		}
		
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityInformation.Ability, 1);
		AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().AbilityStatus_Eligible);
		GiveAbility(AbilitySpec);
		MarkAbilitySpecDirty(AbilitySpec);
		ClientUpdateAbilityStatus(AbilityInformation.AbilityTag, FAuraGameplayTags::Get().AbilityStatus_Eligible);
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);;
	
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddAttributePoints(GetAvatarActor(), -1);
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag StatusTag)
{
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag);
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle EffectHandle) const
{
	FGameplayTagContainer Tags;
	EffectSpec.GetAllAssetTags(Tags);
	
	EffectAssetTags.Broadcast(Tags);
}
