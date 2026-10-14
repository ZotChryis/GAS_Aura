#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	GetAuraASC()->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	const bool bLogNotFound = true;
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag, bLogNotFound);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeMenuWidgetController::BroadcastAttributePoints() const
{
	AAuraPlayerState* PS = Cast<AAuraPlayerState>(PlayerState);
	AttributePointsChangedDelegate.Broadcast(PS->GetAttributePoints());
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
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();

	check(AttributeInfo);
	
	for (auto& Pair : GetAuraAttributeSet()->TagToAttributeMap)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	} 
	
	BroadcastAttributePoints();
}
