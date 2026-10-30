#include "AbilitySystem/Abilities/Passives/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent(const FObjectInitializer& Initializer)
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::ActivateIfEquipped(UAuraAbilitySystemComponent* AuraASC)
{
	if (AuraASC->bStartupAbilitiesGiven)
	{
		if (AuraASC->GetStatusTagFromAbilityTag(PassiveAbilityTag) == FAuraGameplayTags::Get().AbilityStatus_Equipped)
		{
			Activate();
		}
	}
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		AuraASC->PassiveAbilityActivated.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveAbilityActivated);
		AuraASC->PassiveAbilityDeactivated.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveAbilityDeactivated);

		ActivateIfEquipped(AuraASC);
	}
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* ASC)
		{
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(ASC))
			{
				AuraASC->PassiveAbilityActivated.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveAbilityActivated);
				AuraASC->PassiveAbilityDeactivated.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveAbilityDeactivated);
				
				ActivateIfEquipped(AuraASC);
			}
		});
	}
}

void UPassiveNiagaraComponent::OnPassiveAbilityActivated(const FGameplayTag& AbilityTag)
{
	if (!PassiveAbilityTag.MatchesTagExact(AbilityTag) || IsActive())
	{
		return;
	}
	
	Activate();
}

void UPassiveNiagaraComponent::OnPassiveAbilityDeactivated(const FGameplayTag& AbilityTag)
{
	if (!PassiveAbilityTag.MatchesTagExact(AbilityTag))
	{
		return;
	}
	
	Deactivate();
}
