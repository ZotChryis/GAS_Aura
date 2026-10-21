#include "AbilitySystem/Abilities/Passives/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent(const FObjectInitializer& Initializer)
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		AuraASC->PassiveAbilityActivated.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveAbilityActivated);
		AuraASC->PassiveAbilityDeactivated.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveAbilityDeactivated);
	}
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* ASC)
		{
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(ASC))
			{
				AuraASC->PassiveAbilityActivated.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveAbilityActivated);
				AuraASC->PassiveAbilityDeactivated.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveAbilityDeactivated);
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
