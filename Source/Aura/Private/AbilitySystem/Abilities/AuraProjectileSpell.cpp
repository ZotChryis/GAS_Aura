#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	AActor* Actor = GetAvatarActorFromActorInfo();
	if (!Actor->HasAuthority())
	{
		return;
	}
	
	check(ProjectileClass);

	AActor* Owner = GetOwningActorFromActorInfo();
	FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(Actor, FAuraGameplayTags::Get().Event_Montage_Weapon);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	Rotation.Pitch = 0;
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	// Diff from Class: Actor instead of Owner
	Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, Actor,Cast<APawn>(Owner), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(Projectile);
	
	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);

	for (auto& Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
	}
	
	Projectile->DamageEffectSpecHandle = SpecHandle;
	Projectile->FinishSpawning(SpawnTransform);
}
