#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"


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

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddSpellPoints(GetAvatarActor(), -1);
		}
		
		const FAuraGameplayTags Tags = FAuraGameplayTags::Get(); 
		FGameplayTag Status = GetAbilityStatusFromSpec(*AbilitySpec);
		if (Status.MatchesTagExact(Tags.AbilityStatus_Eligible))
		{
			AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(Tags.AbilityStatus_Eligible);
			AbilitySpec->GetDynamicSpecSourceTags().AddTag(Tags.AbilityStatus_Unlocked);
			Status = Tags.AbilityStatus_Unlocked;
		}
		else if (Status.MatchesTagExact(Tags.AbilityStatus_Equipped) || Status.MatchesTagExact(Tags.AbilityStatus_Unlocked))
		{
			AbilitySpec->Level += 1;
		}
		
		MarkAbilitySpecDirty(*AbilitySpec);
		ClientUpdateAbilityStatus(AbilityTag, Status, AbilitySpec->Level);
	}
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag)
{
	if (FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FAuraGameplayTags Tags = FAuraGameplayTags::Get();
		const FGameplayTag PrevInputTag = GetAbilityInputTagFromSpec(*Spec);
		const FGameplayTag& Status = GetAbilityStatusFromSpec(*Spec);
		
		const bool ValidStatus = 
			Status.MatchesTagExact(Tags.AbilityStatus_Equipped) || 
			Status.MatchesTagExact(Tags.AbilityStatus_Unlocked);
		
		if (!ValidStatus)
		{
			return;
		}
		
		// There is an ability in the slot we're trying to equip to.
		if (!IsSlotEmpty(InputTag))
		{
			if (FGameplayAbilitySpec* EquippedAbilitySpec = GetSpecFromInputTag(InputTag))
			{
				const FGameplayTag& EquippedAbilityTag = GetAbilityTagFromSpec(*EquippedAbilitySpec);
				
				// We're equipping the same ability into the slot, do nothing except finish the equip flow
				if (AbilityTag.MatchesTagExact(EquippedAbilityTag))
				{
					ClientEquipAbility(AbilityTag, Tags.AbilityStatus_Equipped, InputTag, PrevInputTag);
					return;
				}
				
				// We're equipping over a passive, we have to deactivate it
				if (IsPassiveAbility(*EquippedAbilitySpec))
				{
					MulticastPassiveAbilityDeactivated(EquippedAbilityTag);
					PassiveAbilityDeactivated.Broadcast(EquippedAbilityTag);
				}
				
				ClearSlot(EquippedAbilitySpec);
			}
		}
		
		// This ability is not equipped/active
		if (!AbilityHasAnyInputTag(Spec))
		{
			if (IsPassiveAbility(*Spec))
			{
				TryActivateAbility(Spec->Handle);
				MulticastPassiveAbilityActivated(AbilityTag);
				PassiveAbilityActivated.Broadcast(AbilityTag);
			}
		}
		
		AssignSlotToAbility(*Spec, InputTag);
		MarkAbilitySpecDirty(*Spec);
		ClientEquipAbility(AbilityTag, Tags.AbilityStatus_Equipped, InputTag, PrevInputTag);
	}
}

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

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	
	FScopedAbilityListLock ScopedAbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag) || !AbilitySpec.IsActive())
		{
			continue;
		}

		AbilitySpecInputPressed(AbilitySpec);
		
		TArray<UGameplayAbility*> AbilityInstances = AbilitySpec.GetAbilityInstances();
		for (UGameplayAbility* AbilityInstance : AbilityInstances)
		{
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilityInstance->GetCurrentActivationInfo().GetActivationPredictionKey());
		}
	} 
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	
	for (FGameplayAbilitySpec& Ability : GetActivatableAbilities())
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

	FScopedAbilityListLock ScopedAbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag) || !AbilitySpec.IsActive())
		{
			continue;
		}

		AbilitySpecInputReleased(AbilitySpec);

		TArray<UGameplayAbility*> AbilityInstances = AbilitySpec.GetAbilityInstances();
		for (UGameplayAbility* AbilityInstance : AbilityInstances)
		{
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilityInstance->GetCurrentActivationInfo().GetActivationPredictionKey());
		}
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

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	if (Spec)
	{
		const FGameplayTag Slot = GetAbilityInputTagFromSpec(*Spec);
		if (Slot.IsValid())
		{
			Spec->GetDynamicSpecSourceTags().RemoveTag(Slot);
		}
	}
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilityHasInputTag(&Spec, InputTag))
		{
			ClearSlot(&Spec);
		}
	}
}

bool UAuraAbilitySystemComponent::AbilityHasInputTag(FGameplayAbilitySpec* Spec, const FGameplayTag& InputTag)
{
	if (Spec)
	{
		for (const FGameplayTag& Tag : Spec->GetDynamicSpecSourceTags())
		{
			if (Tag.MatchesTagExact(InputTag))
			{
				return true;
			}
		}
	}
	
	return false;
}

bool UAuraAbilitySystemComponent::AbilityHasAnyInputTag(FGameplayAbilitySpec* Spec)
{
	if (Spec)
	{
		return Spec->GetDynamicSpecSourceTags().HasTag(FGameplayTag::RequestGameplayTag(FName("Input")));
	}
	
	return false;
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

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFromAbilityTag(const FGameplayTag AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetAbilityStatusFromSpec(*Spec);
	}
	
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromAbilityTag(const FGameplayTag AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetAbilityInputTagFromSpec(*Spec);
	}
	
	return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& InAbilityTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (const FGameplayTag& AbilityTag : AbilitySpec.Ability->GetAssetTags())
		{
			if (AbilityTag.MatchesTag(InAbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}
	
	return nullptr;
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromInputTag(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (IsAbilityInSlot(AbilitySpec, InputTag))
		{
			return &AbilitySpec;
		}
	}
	
	return nullptr;
}

bool UAuraAbilitySystemComponent::IsSlotEmpty(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (IsAbilityInSlot(AbilitySpec, InputTag))
		{
			return false;
		}
	}
	
	return true;
}

bool UAuraAbilitySystemComponent::IsAbilityInSlot(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& InputTag)
{
	return AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag);
}

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec) const
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FGameplayTag& AbilityTag = GetAbilityTagFromSpec(AbilitySpec);
	const FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
	return Info.AbilityType.MatchesTagExact(FAuraGameplayTags::Get().AbilityType_Passive);
}

bool UAuraAbilitySystemComponent::IsOffensiveAbility(const FGameplayAbilitySpec& AbilitySpec) const
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FGameplayTag& AbilityTag = GetAbilityTagFromSpec(AbilitySpec);
	const FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
	return Info.AbilityType.MatchesTagExact(FAuraGameplayTags::Get().AbilityType_Offensive);
}

void UAuraAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& InputTag)
{
	ClearSlot(&AbilitySpec);
	AbilitySpec.GetDynamicSpecSourceTags().AddTag(InputTag);
}

void UAuraAbilitySystemComponent::MulticastPassiveAbilityActivated_Implementation(const FGameplayTag& AbilityTag)
{
	PassiveAbilityActivated.Broadcast(AbilityTag);
}

void UAuraAbilitySystemComponent::MulticastPassiveAbilityDeactivated_Implementation(const FGameplayTag& AbilityTag)
{
	PassiveAbilityDeactivated.Broadcast(AbilityTag);
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
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().AbilityStatus_Eligible);
		GiveAbility(AbilitySpec);
		MarkAbilitySpecDirty(AbilitySpec);
		ClientUpdateAbilityStatus(AbilityInformation.AbilityTag, FAuraGameplayTags::Get().AbilityStatus_Eligible, 1);
	}
}

bool UAuraAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription)
{
	OutDescription = FString();
	OutNextLevelDescription = FString();
	
	if (!AbilityTag.IsValid() ||  AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Ability_None))
	{
		return false;
	}
	
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (UAuraGameplayAbility* Ability = Cast<UAuraGameplayAbility>(Spec->Ability))
		{
			OutDescription = Ability->GetDescription(Spec->Level);
			OutNextLevelDescription = Ability->GetNextLevelDescription(Spec->Level + 1);
			return true;
		}
	}
	
	if (UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor()))
	{
		OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoByTag(AbilityTag).LevelRequirement);
		OutNextLevelDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoByTag(AbilityTag).LevelRequirement);
		return true;
	}
	
	return false;
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag StatusTag, const int32 Level)
{
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag, Level);
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag)
{
	AbilityEquipped.Broadcast(AbilityTag, StatusTag, InputTag, PrevInputTag);
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
