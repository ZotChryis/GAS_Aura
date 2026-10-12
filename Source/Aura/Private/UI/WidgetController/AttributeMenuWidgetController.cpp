#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UAuraAbilitySystemComponent* ASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	ASC->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	const bool bLogNotFound = true;
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag, bLogNotFound);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeMenuWidgetController::BroadcastAttributePoints(const int32& AttributePoints) const
{
	AAuraPlayerState* PS = Cast<AAuraPlayerState>(PlayerState);
	AttributePointsChangedDelegate.Broadcast(PS->GetAttributePoints());
}

void UAttributeMenuWidgetController::BroadcastSpellPoints(const int32& SpellPoints) const
{
	AAuraPlayerState* PS = Cast<AAuraPlayerState>(PlayerState);
	SpellPointsChangedDelegate.Broadcast(PS->GetSpellPoints());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	for (auto& Pair : AS->TagToAttributeMap)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}
	
	AAuraPlayerState* PS = Cast<AAuraPlayerState>(PlayerState);
	PS->OnAttributePointsChanged.AddLambda(
		[this](int32 Points)
		{
			AttributePointsChangedDelegate.Broadcast(Points);
		}
	);
	PS->OnSpellPointsChanged.AddLambda(
		[this](int32 Points)
		{
			SpellPointsChangedDelegate.Broadcast(Points);
		}
	);
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();

	check(AttributeInfo);
	
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	for (auto& Pair : AS->TagToAttributeMap)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	} 
	
	AAuraPlayerState* PS = Cast<AAuraPlayerState>(PlayerState);
	AttributePointsChangedDelegate.Broadcast(PS->GetAttributePoints());
	SpellPointsChangedDelegate.Broadcast(PS->GetSpellPoints());
}
