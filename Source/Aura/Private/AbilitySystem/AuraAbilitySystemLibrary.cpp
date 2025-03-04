#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (!PlayerController)
	{
		return nullptr;
	}

	AAuraHUD* HUD = Cast<AAuraHUD>(PlayerController->GetHUD());
	if (!HUD)
	{
		return nullptr;
	}

	AAuraPlayerState* PlayerState = PlayerController->GetPlayerState<AAuraPlayerState>();
	if (!PlayerState)
	{
		return nullptr;
	}

	UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
	UAttributeSet* AttributeSet  = PlayerState->GetAttributeSet();
	const FWidgetControllerParams WidgetControllerParams(PlayerController, PlayerState, ASC, AttributeSet);
	
	return HUD->GetOverlayWidgetController(WidgetControllerParams);
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (!PlayerController)
	{
		return nullptr;
	}

	AAuraHUD* HUD = Cast<AAuraHUD>(PlayerController->GetHUD());
	if (!HUD)
	{
		return nullptr;
	}

	AAuraPlayerState* PlayerState = PlayerController->GetPlayerState<AAuraPlayerState>();
	if (!PlayerState)
	{
		return nullptr;
	}

	UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
	UAttributeSet* AttributeSet  = PlayerState->GetAttributeSet();
	const FWidgetControllerParams WidgetControllerParams(PlayerController, PlayerState, ASC, AttributeSet);
	
	return HUD->GetAttributeMenuWidgetController(WidgetControllerParams);
}
