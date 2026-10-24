#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Engine/OverlapResult.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "Engine/World.h"

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutParams, AAuraHUD*& OutAuraHUD)
{
	OutParams = FWidgetControllerParams();
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (!PlayerController)
	{
		return false;
	}

	OutAuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD());
	if (!OutAuraHUD)
	{
		return false;
	}

	AAuraPlayerState* PlayerState = PlayerController->GetPlayerState<AAuraPlayerState>();
	if (!PlayerState)
	{
		return false;
	}

	UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
	UAttributeSet* AttributeSet  = PlayerState->GetAttributeSet();
	OutParams.AbilitySystemComponent = ASC;
	OutParams.PlayerState = PlayerState;
	OutParams.PlayerController = PlayerController;
	OutParams.AttributeSet = AttributeSet;
	return true;
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WidgetParams, AuraHUD))
	{
		return AuraHUD->GetOverlayWidgetController(WidgetParams);	
	}
	
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WidgetParams, AuraHUD))
	{
		return AuraHUD->GetAttributeMenuWidgetController(WidgetParams);	
	}
	
	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WidgetParams, AuraHUD))
	{
		return AuraHUD->GetSpellMenuWidgetController(WidgetParams);	
	}
	
	return nullptr;
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
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			AbilitySpec.Level = ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor());
		}
		ASC->GiveAbility(AbilitySpec);
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!AuraGameMode)
	{
		return nullptr;
	}

	return AuraGameMode->CharacterClassInfo;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!AuraGameMode)
	{
		return nullptr;
	}
	
	return AuraGameMode->AbilityInfo;
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

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle ContextHandle)
{
	const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return false;
	}
	
	return AuraContext->IsSuccessfulDebuff();
}

bool UAuraAbilitySystemLibrary::IsSuccessfulKnockback(const FGameplayEffectContextHandle ContextHandle)
{
	const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return false;
	}
	
	return AuraContext->IsSuccessfulKnockback();
}

bool UAuraAbilitySystemLibrary::GetCausesHitReact(const FGameplayEffectContextHandle ContextHandle)
{
	const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return false;
	}
	
	return AuraContext->GetCausesHitReact(); 
}

bool UAuraAbilitySystemLibrary::GetDebuffCausesHitReact(const FGameplayEffectContextHandle ContextHandle)
{
	const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return false;
	}
	
	return AuraContext->GetDebuffCausesHitReact(); 
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle ContextHandle)
{
	const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return false;
	}
	
	return AuraContext->GetDebuffDamage();
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle ContextHandle)
{
	const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return false;
	}
	
	return AuraContext->GetDebuffDuration();
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle ContextHandle)
{
	const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return false;
	}
	
	return AuraContext->GetDebuffFrequency();
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle ContextHandle)
{
	const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return FGameplayTag();;
	}
	
	if (!AuraContext->GetDamageType().IsValid())
	{
		return FGameplayTag();;
	}
	
	return *AuraContext->GetDamageType();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(FGameplayEffectContextHandle ContextHandle)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return FVector::ZeroVector;
	}

	return AuraContext->GetDeathImpulse();
}

FVector UAuraAbilitySystemLibrary::GetKnockbackForce(FGameplayEffectContextHandle ContextHandle)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return FVector::ZeroVector;
	}

	return AuraContext->GetKnockbackForce();
}

bool UAuraAbilitySystemLibrary::IsRadialDamage(FGameplayEffectContextHandle ContextHandle)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return false;
	}

	return AuraContext->IsRadialDamage();
}

float UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(FGameplayEffectContextHandle ContextHandle)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return 0.f;
	}

	return AuraContext->GetRadialDamageInnerRadius();
}

float UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(FGameplayEffectContextHandle ContextHandle)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return 0.f;
	}

	return AuraContext->GetRadialDamageOuterRadius();
}

FVector UAuraAbilitySystemLibrary::GetRadialDamageOrigin(FGameplayEffectContextHandle ContextHandle)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return FVector::ZeroVector;
	}

	return AuraContext->GetRadialDamageOrigin();
}

void UAuraAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& ContextHandle, bool Value)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return;
	}
	
	AuraContext->SetIsRadialDamage(Value);
}

void UAuraAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle ContextHandle, float Value)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return;
	}
	
	AuraContext->SetRadialDamageInnerRadius(Value);
}

void UAuraAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle ContextHandle, float Value)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return;
	}
	
	AuraContext->SetRadialDamageOuterRaidus(Value);
}

void UAuraAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle ContextHandle, const FVector& Origin)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return;
	}
	
	AuraContext->SetRadialDamageOrigin(Origin);
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

void UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& ContextHandle, bool Value)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return;
	}

	AuraContext->SetIsSuccessfulDebuff(Value);
}

void UAuraAbilitySystemLibrary::SetIsSuccessfulKnockback(FGameplayEffectContextHandle& ContextHandle, bool Value)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return;
	}

	AuraContext->SetIsSuccessfulKnockback(Value);
}

void UAuraAbilitySystemLibrary::SetCausesHitReact(FGameplayEffectContextHandle& ContextHandle, bool Value)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return;
	}
    
	AuraContext->SetCausesHitReact(Value);
}

void UAuraAbilitySystemLibrary::SetDebuffCausesHitReact(FGameplayEffectContextHandle& ContextHandle, bool Value)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return;
	}
    
	AuraContext->SetDebuffCausesHitReact(Value);
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& ContextHandle, float Value)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return;
	}

	AuraContext->SetDebuffDamage(Value);
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& ContextHandle, float Value)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return;
	}

	AuraContext->SetDebuffDuration(Value);
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& ContextHandle, float Value)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return;
	}

	AuraContext->SetDebuffFrequency(Value);
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& ContextHandle, const FGameplayTag& InDamageType)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return;
	}

	const TSharedPtr<FGameplayTag>& DamageType = MakeShared<FGameplayTag>(InDamageType);
	AuraContext->SetDamageType(DamageType);
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& ContextHandle, const FVector& Impulse)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return;
	}
	
	AuraContext->SetDeathImpulse(Impulse);
}

void UAuraAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& ContextHandle, const FVector& Impulse)
{
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());
	if (!AuraContext)
	{
		return;
	}
	
	AuraContext->SetKnockbackForce(Impulse);
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

void UAuraAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Targets, TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	if (Targets.Num() <= MaxTargets)
	{
		OutClosestTargets = Targets;
		return;
	}
	
	TArray<AActor*> ActorsToCheck = Targets;
	ActorsToCheck.Sort([Origin](const AActor& A, const AActor& B)  {
		return FVector::DistSquared(Origin, A.GetActorLocation()) < FVector::DistSquared(Origin, B.GetActorLocation());
	});
	
	OutClosestTargets.Empty();
	OutClosestTargets.Reserve(MaxTargets);
	for (int32 i = 0; i < MaxTargets; i++)
	{
		OutClosestTargets.Add(ActorsToCheck[i]);
	}
}

bool UAuraAbilitySystemLibrary::AreAllied(AActor* Actor, AActor* OtherActor)
{
	const bool bAIsPlayer = Actor->ActorHasTag(FName("Player"));
	const bool bBIsPlayer = OtherActor->ActorHasTag(FName("Player"));
	
	const bool bAIsEnemy = Actor->ActorHasTag(FName("Enemy"));
	const bool bBIsEnemy = OtherActor->ActorHasTag(FName("Enemy"));

	if (bAIsPlayer && bBIsPlayer)
	{
		return true;
	}
	if (bAIsEnemy && bBIsEnemy)
	{
		return true;
	}
	
	return false;
}

bool UAuraAbilitySystemLibrary::AreEnemies(AActor* Actor, AActor* OtherActor)
{
	return !AreAllied(Actor, OtherActor);
}

int32 UAuraAbilitySystemLibrary::GetExperienceRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 Level)
{
	UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (!ClassInfo)
	{
		return 0;
	}

	FCharacterClassDefaultInfo DefaultClassInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);
	return static_cast<int32>(DefaultClassInfo.ExperienceReward.GetValueAtLevel(Level));
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& Params)
{
	if (!Params.TargetASC || !Params.SourceASC)
	{
		return FGameplayEffectContextHandle();
	}
	
	FGameplayEffectContextHandle ContextHandle = Params.SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(Params.SourceASC->GetAvatarActor());
	ContextHandle.AddOrigin(Params.SourceASC->GetAvatarActor()->GetActorLocation());
	SetDeathImpulse(ContextHandle, Params.DeathImpulse);
	SetKnockbackForce(ContextHandle, Params.KnockbackForce);
	SetCausesHitReact(ContextHandle, Params.CausesHitReact);
	SetDebuffCausesHitReact(ContextHandle, Params.DebuffCausesHitReact);
	SetIsSuccessfulKnockback(ContextHandle, FMath::RandRange(1, 100) < Params.KnockbackChance);
	
	SetIsRadialDamage(ContextHandle, Params.bIsRadialDamage);
	SetRadialDamageInnerRadius(ContextHandle, Params.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(ContextHandle, Params.RadialDamageOuterRadius);
	SetRadialDamageOrigin(ContextHandle, Params.RadialDamageOrigin);
	
	FGameplayEffectSpecHandle SpecHandle = Params.SourceASC->MakeOutgoingSpec(Params.DamageEffectClass, Params.AbilityLevel, ContextHandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Params.DamageType, Params.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FAuraGameplayTags::Get().Debuff_Chance, Params.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FAuraGameplayTags::Get().Debuff_Damage, Params.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FAuraGameplayTags::Get().Debuff_Duration, Params.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FAuraGameplayTags::Get().Debuff_Frequency, Params.DebuffFrequency);
	
	Params.SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, Params.TargetASC);
	return ContextHandle;
}

TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpreadRotations(const FVector& Forward, const FVector& Axis, const float Spread, const int32 NumRotators)
{
	TArray<FRotator> Result;
	if (NumRotators > 1)
	{
		const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread/2, Axis);
		const float DeltaSpread = Spread / (NumRotators - 1);
		for (int32 i = 0; i < NumRotators; ++i)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, Axis);
			Result.Add(Direction.Rotation());
		}
	}
	else
	{
		Result.Add(Forward.Rotation());
	}
	
	return Result;
}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlySpreadVectors(const FVector& Forward, const FVector& Axis, const float Spread, const int32 NumVectors)
{
	TArray<FVector> Result;
	if (NumVectors > 1)
	{
		const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread/2, Axis);
		const float DeltaSpread = Spread / (NumVectors - 1);
		for (int32 i = 0; i < NumVectors; ++i)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, Axis);
			Result.Add(Direction);
		}
	}
	else
	{
		Result.Add(Forward);
	}
	
	return Result;
}

void UAuraAbilitySystemLibrary::SetTargetDamageEffectParams(FDamageEffectParams& Params, AActor* Target)
{
	if (Target)
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
		{
			Params.TargetASC = ASC;
		}
	}
}

void UAuraAbilitySystemLibrary::SetIsRadialDamageEffectParams(FDamageEffectParams& Params, bool Value, float InnerRadius, float OuterRadius, FVector Origin)
{
	Params.bIsRadialDamage = Value;
	Params.RadialDamageInnerRadius = InnerRadius;
	Params.RadialDamageOuterRadius = OuterRadius;
	Params.RadialDamageOrigin = Origin;
}

void UAuraAbilitySystemLibrary::SetKnockbackDirectionEffectParams(FDamageEffectParams& Params, FVector Direction, float Magnitude)
{
	const float MagitudeToUse = FMath::IsNearlyZero(Magnitude) ? Params.KnockbackForceMagnitude : Magnitude;
	Params.KnockbackForce = Direction.GetSafeNormal() * MagitudeToUse;
}

void UAuraAbilitySystemLibrary::SetDeathImpulseDirectionEffectParams(FDamageEffectParams& Params, FVector Direction, float Magnitude)
{
	const float MagitudeToUse = FMath::IsNearlyZero(Magnitude) ? Params.DeathImpulseMagnitude : Magnitude;
	Params.DeathImpulse = Direction.GetSafeNormal() * MagitudeToUse;
}
