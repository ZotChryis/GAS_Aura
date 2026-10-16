#include "AbilitySystem/Abilities/AuraFirebolt.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/CombatInterface.h"

FString UAuraFirebolt::GetDescription(int32 Level)
{
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	const float ScaledDamage = Damage.GetValueAtLevel(Level);
	return FString::Printf(TEXT(
		"<Title>FIRE BOLT</>\n\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><Mana>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		"<Default>Launches %d a bolt of fire, exploding on impact and dealing </>"
		"<Damage>%.1f</><Default> fire damage with a chance to burn.</>"), 
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, MaxNumProjectile), 
		ScaledDamage
	);
}

FString UAuraFirebolt::GetNextLevelDescription(int32 Level)
{
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	const float ScaledDamage = Damage.GetValueAtLevel(Level);
	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL</>\n\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><Mana>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		"<Default>Launches %d a bolt of fire, exploding on impact and dealing </>"
		"<Damage>%.1f</><Default> fire damage with a chance to burn.</>"), 
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, MaxNumProjectile), 
		ScaledDamage
	);
}

void UAuraFirebolt::SpawnProjectiles(AActor* TargetActor, const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, const bool bOverridePitch, float PitchOverride)
{
	AActor* Actor = GetAvatarActorFromActorInfo();
	if (!Actor->HasAuthority())
	{
		return;
	}
	
	check(ProjectileClass);
	
	FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(Actor, SocketTag);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}
	
	const int32 NumProjectiles = FMath::Min(MaxNumProjectile, GetAbilityLevel());
	const FVector Forward = Rotation.Vector();
	TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpreadRotations(Forward, FVector::UpVector, ProjectileSpread, NumProjectiles);
	for (const FRotator& Rot : Rotations)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());

		AAuraProjectile* ProjectileInstance = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, Actor,Cast<APawn>(Actor), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		ProjectileInstance->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		
		if (TargetActor && TargetActor->Implements<UCombatInterface>())
		{
			ProjectileInstance->ProjectileMovement->HomingTargetComponent = TargetActor->GetRootComponent();
		}
		else
		{
			ProjectileInstance->HomingTarget = NewObject<USceneComponent>(USceneComponent::StaticClass());
			ProjectileInstance->HomingTarget->SetWorldLocation(ProjectileTargetLocation);
			ProjectileInstance->ProjectileMovement->HomingTargetComponent = ProjectileInstance->HomingTarget;
		}
		
		ProjectileInstance->ProjectileMovement->bIsHomingProjectile = bLauchHomingProjectiles;
		ProjectileInstance->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
		
		ProjectileInstance->FinishSpawning(SpawnTransform);
	}
}
