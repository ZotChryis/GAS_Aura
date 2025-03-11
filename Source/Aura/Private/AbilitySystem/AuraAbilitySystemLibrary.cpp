#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Game/AuraGameModeBase.h"
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

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(WorldContextObject);
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(ASC->GetAvatarActor());
	
	const FGameplayEffectSpecHandle PrimaryAttributesEffect = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributesGameplayEffect, Level, EffectContext);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesEffect.Data.Get());

	const FGameplayEffectSpecHandle SecondaryAttributesEffect = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributesGameplayEffect, Level, EffectContext);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesEffect.Data.Get());

	const FGameplayEffectSpecHandle VitalAttributesEffect = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributesGameplayEffect, Level, EffectContext);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesEffect.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveCommonAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(WorldContextObject);

	for (TSubclassOf<UGameplayAbility> Ability : CharacterClassInfo->Abilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);
		ASC->GiveAbility(AbilitySpec);	
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!AuraGameMode)
	{
		return nullptr;
	}

	return AuraGameMode->CharacterClassInfo;
}
