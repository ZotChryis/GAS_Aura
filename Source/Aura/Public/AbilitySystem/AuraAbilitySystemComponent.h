#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags */);
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag& /* AbilityTag */, const FGameplayTag& /* StatusTag */, const int32 /*Level*/);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag& /* AbilityTag */, const FGameplayTag& /* StatusTag */, const FGameplayTag& /* InputTag */, const FGameplayTag& /* PrevInputTag */)
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /* AbilityTag */);

UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);
	
	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);
	
	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag);
	
	void AbilityActorInfoSet();
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void ForEachAbility(FForEachAbility& Delegate);
	
	void ClearSlot(FGameplayAbilitySpec* Spec);
	void ClearAbilitiesOfSlot(const FGameplayTag& InputTag);
	static bool AbilityHasInputTag(FGameplayAbilitySpec* Spec, const FGameplayTag& InputTag);
	
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetAbilityInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetAbilityStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	FGameplayTag GetStatusTagFromAbilityTag(const FGameplayTag AbilityTag);
	FGameplayTag GetInputTagFromAbilityTag(const FGameplayTag AbilityTag);
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& InAbilityTag);
	
	void UpgradeAttribute(const FGameplayTag& AttributeTag);
	void UpdateAbilityStatuses(int32 Level);
	
	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription);
	
	bool bStartupAbilitiesGiven = false;
	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;
	FAbilityStatusChanged AbilityStatusChanged;
	FAbilityEquipped AbilityEquipped;
	FDeactivatePassiveAbility PassiveAbilityDeactivated;
	
protected:
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle EffectHandle) const;

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag StatusTag, const int32 Level);
	
	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag);
	
	virtual void OnRep_ActivateAbilities() override;
};
