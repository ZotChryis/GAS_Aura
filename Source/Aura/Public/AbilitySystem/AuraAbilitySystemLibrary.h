#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AuraAbilitySystemLibrary.generated.h"

struct FDamageEffectParams;
class UCharacterClassInfo;
class UAbilitySystemComponent;
class UAttributeMenuWidgetController;
class USpellMenuWidgetController;
class UOverlayWidgetController;
struct FGameplayEffectContextHandle;
enum class ECharacterClass : uint8;

UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/*
	 *	Widget Controller
	 */
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutParams, AAuraHUD*& OutAuraHUD);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);

	/*
	 *	Ability System Class Defaults
	 */
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|AbilityInfo")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

	
	/*
	 *	Effect Context Getters
	 */
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle ContextHandle);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsSuccessfulDebuff(const FGameplayEffectContextHandle ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsSuccessfulKnockback(const FGameplayEffectContextHandle ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool GetCausesHitReact(const FGameplayEffectContextHandle ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool GetDebuffCausesHitReact(const FGameplayEffectContextHandle ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static float GetDebuffDamage(const FGameplayEffectContextHandle ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static float GetDebuffDuration(const FGameplayEffectContextHandle ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static float GetDebuffFrequency(const FGameplayEffectContextHandle ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static FGameplayTag GetDamageType(const FGameplayEffectContextHandle ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static FVector GetDeathImpulse(FGameplayEffectContextHandle ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static FVector GetKnockbackForce(FGameplayEffectContextHandle ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsRadialDamage(FGameplayEffectContextHandle ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static float GetRadialDamageInnerRadius(FGameplayEffectContextHandle ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static float GetRadialDamageOuterRadius(FGameplayEffectContextHandle ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static FVector GetRadialDamageOrigin(FGameplayEffectContextHandle ContextHandle);

	/*
	 *	Effect Context Setters
	 */
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, bool Value);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, bool Value);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsSuccessfulDebuff(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, bool Value);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsSuccessfulKnockback(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, bool Value);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetCausesHitReact(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, bool Value);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffCausesHitReact(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, bool Value);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffDamage(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, float Value);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffDuration(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, float Value);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffFrequency(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, float Value);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDamageType(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, const FGameplayTag& InDamageType);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDeathImpulse(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, const FVector& Impulse);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetKnockbackForce(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, const FVector& Impulse);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsRadialDamage(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, bool Value);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageInnerRadius(UPARAM(ref) FGameplayEffectContextHandle ContextHandle, float Value);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageOuterRadius(UPARAM(ref) FGameplayEffectContextHandle ContextHandle, float Value);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageOrigin(UPARAM(ref) FGameplayEffectContextHandle ContextHandle, const FVector& Origin);
	
	/*
	 *	Gameplay Mechanics
	 */
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|Gameplay Mechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutActors, const TArray<AActor*>& IgnoreActors, const FVector& Origin, float Radius);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|Gameplay Mechanics")
	static void GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Targets, TArray<AActor*>& OutClosestTargets, const FVector& Origin);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|Gameplay Mechanics")
	static bool AreAllied(AActor* Actor, AActor* OtherActor);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|Gameplay Mechanics")
	static bool AreEnemies(AActor* Actor, AActor* OtherActor);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|Experience")
	static int32 GetExperienceRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 Level);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|Damage")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& Params);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|Gameplay Mechanics")
	static TArray<FRotator> EvenlySpreadRotations(const FVector& Forward, const FVector& Axis, const float Spread, const int32 NumRotators);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|Gameplay Mechanics")
	static TArray<FVector> EvenlySpreadVectors(const FVector& Forward, const FVector& Axis, const float Spread, const int32 NumVectors);
};
