#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	check(ProjectileClass);
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const bool bServer = HasAuthority(&ActivationInfo);
	if (!bServer)
	{
		return;	
	}

	AActor* Owner = GetOwningActorFromActorInfo();
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());

	// TODO: Set Rotation
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(CombatInterface ? CombatInterface->GetCombatSocketLocation() : FVector());
	Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, Owner,Cast<APawn>(Owner), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	// TODO: Give the projectile a GameplayEffectSpec for causing damage, etc.
	
	Projectile->FinishSpawning(SpawnTransform);
}
