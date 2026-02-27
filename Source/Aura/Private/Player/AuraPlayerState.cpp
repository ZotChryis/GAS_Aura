#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	SetNetUpdateFrequency(100.f);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, Experience);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AAuraPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

void AAuraPlayerState::SetPlayerLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChanged.Broadcast(Level);
}

void AAuraPlayerState::AddPlayerLevel(int32 Amount)
{
	Level += Amount;
	OnLevelChanged.Broadcast(Level);
}

void AAuraPlayerState::SetExperience(int32 InExperience)
{
	Experience = InExperience;
	OnExperienceChanged.Broadcast(Experience);
}

void AAuraPlayerState::AddExperience(int32 Amount)
{
	Experience += Amount;
	OnExperienceChanged.Broadcast(Experience);
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChanged.Broadcast(Level);
}

void AAuraPlayerState::OnRep_Experience(int32 OldExperience)
{
	OnExperienceChanged.Broadcast(Experience);
}

