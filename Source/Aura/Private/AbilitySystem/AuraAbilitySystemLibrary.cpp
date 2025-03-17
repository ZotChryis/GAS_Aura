#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AuraAbilityTypes.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Engine/OverlapResult.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Engine/World.h"

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
	if (!CharacterClassInfo)
	{
		return;
	}
	
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

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(WorldContextObject);
	if (!CharacterClassInfo)
	{
		return;
	}

	for (TSubclassOf<UGameplayAbility> Ability : CharacterClassInfo->Abilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);
		ASC->GiveAbility(AbilitySpec);	
	}

	const FCharacterClassDefaultInfo& ClassInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (TSubclassOf<UGameplayAbility> Ability : ClassInfo.Abilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(ASC->GetAvatarActor()))
		{
			AbilitySpec.Level = CombatInterface->GetPlayerLevel();
		}
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

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle ContextHandle)
{
	const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return false;
	}
	
	return AuraContext->IsBlocked();
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle ContextHandle)
{
	const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return false;
	}
	
	return AuraContext->IsCriticalHit();
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& ContextHandle, bool Value)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return;
	}

	AuraContext->SetIsBlocked(Value);
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& ContextHandle, bool Value)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return;
	}

	AuraContext->SetIsCriticalHit(Value);
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutActors, const TArray<AActor*>& IgnoreActors, const FVector& Origin, float Radius)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return;
	}

	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(IgnoreActors);
	
	TArray<FOverlapResult> OverlapResults;
	World->OverlapMultiByObjectType(OverlapResults, Origin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);

	for (FOverlapResult& OverlapResult : OverlapResults)
	{
		AActor* OverlapActor = OverlapResult.GetActor();
		const bool ImplementsCombatInterface = OverlapActor->Implements<UCombatInterface>();
		if (!ImplementsCombatInterface)
		{
			continue;
		}

		if (ICombatInterface::Execute_IsDead(OverlapActor))
		{
			continue;
		}

		OutActors.AddUnique(OverlapActor);
	}
}
