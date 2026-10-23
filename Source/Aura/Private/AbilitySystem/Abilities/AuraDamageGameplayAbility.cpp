// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "SAdvancedRotationInputBox.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, ScaledDamage);

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC)
	{
		return;
	}

	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
	OwnerASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor,
	const FVector Origin, const bool bOverrideKnockback, const FVector InKnockbackDirectionOverride,
	const bool bOverrideDeathImpulse, const FVector InDeathImpulseDirectionOverride, const bool bOverridePitch,
	const float InPitchOverride) const
{
	FDamageEffectParams DamageEffectParams;
	DamageEffectParams.WorldContextObject = GetAvatarActorFromActorInfo();
	DamageEffectParams.DamageEffectClass = DamageEffectClass.Get();
	DamageEffectParams.SourceASC = GetAbilitySystemComponentFromActorInfo();
	DamageEffectParams.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	DamageEffectParams.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	DamageEffectParams.AbilityLevel = GetAbilityLevel();
	DamageEffectParams.DamageType = DamageType;
	DamageEffectParams.CausesHitReact = CausesHitReact;
	DamageEffectParams.DebuffCausesHitReact = DebuffCausesHitReact;
	DamageEffectParams.DebuffChance = DebuffChance;
	DamageEffectParams.DebuffDamage = DebuffDamage;
	DamageEffectParams.DebuffDuration = DebuffDuration;
	DamageEffectParams.DebuffFrequency = DebuffFrequency;
	DamageEffectParams.DeathImpulseMagnitude = DeathImpulseMagnitude;
	DamageEffectParams.KnockbackChance = KnockbackChance;
	DamageEffectParams.KnockbackForceMagnitude = KnockbackForceMagnitude;
	
	DamageEffectParams.bIsRadialDamage = bIsRadialDamage;
	if (bIsRadialDamage)
	{
		DamageEffectParams.RadialDamageInnerRadius = RadialDamageInnerRadius;
		DamageEffectParams.RadialDamageOuterRadius = RadialDamageOuterRadius;
		DamageEffectParams.RadialDamageOrigin = Origin;
	}
	
	if (IsValid(TargetActor))
	{
		FRotator Rot = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).GetSafeNormal().Rotation();
		const FVector ToTarget = Rot.Vector();
		Rot.Pitch = bOverridePitch ? InPitchOverride : 45.f;
		
		if (!bOverrideKnockback)
		{
			DamageEffectParams.KnockbackForce = ToTarget * KnockbackForceMagnitude;
		}
		
		if (!bOverrideDeathImpulse)
		{
			DamageEffectParams.DeathImpulse = ToTarget * DeathImpulseMagnitude;
		}
	}
	
	if (bOverrideKnockback)
	{
		DamageEffectParams.KnockbackForce = InKnockbackDirectionOverride.GetSafeNormal() * KnockbackForceMagnitude;
		if (bOverridePitch)
		{
			FRotator KnockbackRotation = InKnockbackDirectionOverride.GetSafeNormal().Rotation();
			KnockbackRotation.Pitch = InPitchOverride;
			DamageEffectParams.KnockbackForce = KnockbackRotation.Vector() * KnockbackForceMagnitude;
		}
	}
	
	if (bOverrideDeathImpulse)
	{
		DamageEffectParams.DeathImpulse = InDeathImpulseDirectionOverride.GetSafeNormal() * DeathImpulseMagnitude;
		if (bOverridePitch)
		{
			FRotator DeathImpulseRotation = InDeathImpulseDirectionOverride.GetSafeNormal().Rotation();
			DeathImpulseRotation.Pitch = InPitchOverride;
			DamageEffectParams.DeathImpulse = DeathImpulseRotation.Vector() * DeathImpulseMagnitude;
		}
	}
	
	return DamageEffectParams;
}

float UAuraDamageGameplayAbility::GetDamageAtLevel(int32 Level) const
{
	return Damage.GetValueAtLevel(Level);
}
