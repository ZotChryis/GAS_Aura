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
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
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
	OnLevelChanged.Broadcast(Level, false);
}

void AAuraPlayerState::AddPlayerLevel(int32 Amount)
{
	Level += Amount;
	OnLevelChanged.Broadcast(Level, true);
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

void AAuraPlayerState::SetAttributePoints(int32 InAttributePoints)
{
	AttributePoints = InAttributePoints;
	OnAttributePointsChanged.Broadcast(AttributePoints);
}

void AAuraPlayerState::AddAttributePoints(int32 Amount)
{
	AttributePoints += Amount;
	OnAttributePointsChanged.Broadcast(AttributePoints);
}

void AAuraPlayerState::SetSpellPoints(int32 InSpellPoints)
{
	SpellPoints = InSpellPoints;
	OnSpellPointsChanged.Broadcast(SpellPoints);
}

void AAuraPlayerState::AddSpellPoints(int32 Amount)
{
	SpellPoints += Amount;
	OnSpellPointsChanged.Broadcast(SpellPoints);
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChanged.Broadcast(Level, true);
}

void AAuraPlayerState::OnRep_Experience(int32 OldExperience)
{
	OnExperienceChanged.Broadcast(Experience);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
	OnAttributePointsChanged.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldSpellPoints)
{
	OnSpellPointsChanged.Broadcast(SpellPoints);
}

