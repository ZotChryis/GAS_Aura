#include "AbilitySystem/Abilities/AuraFireblast.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireball.h"

FString UAuraFireblast::GetDescription(int32 Level)
{
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	const float ScaledDamage = Damage.GetValueAtLevel(Level);
	return FString::Printf(TEXT(
		"<Title>FIRE BLAST</>\n\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><Mana>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		"<Default>Launches %d a balls of fire in all directions, each coming back and "
		"exploding upon return, causing </>"
		"<Damage>%.1f</><Default> radial fire damage with a chance to burn.</>"), 
		Level,
		ManaCost,
		Cooldown,
		12,
		ScaledDamage
	);
}

FString UAuraFireblast::GetNextLevelDescription(int32 Level)
{
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	const float ScaledDamage = Damage.GetValueAtLevel(Level);
	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL</>\n\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><Mana>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		"<Default>Launches %d a balls of fire in all directions, each coming back and "
		"exploding upon return, causing </>"
		"<Damage>%.1f</><Default> radial fire damage with a chance to burn.</>"), 
		Level,
		ManaCost,
		Cooldown,
		12,
		ScaledDamage
	);
}

TArray<AAuraFireball*> UAuraFireblast::SpawnFireballs()
{
	TArray<AAuraFireball*> Fireballs;
	
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpreadRotations(Forward, FVector::UpVector, 360.f, NumFireballs);
	for (FRotator Rot : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rot.Quaternion());
		AAuraFireball* Fireball = GetWorld()->SpawnActorDeferred<AAuraFireball>(
			FireballClass, 
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);	
		
		Fireball->SetOwner(GetAvatarActorFromActorInfo());
		Fireball->ExplosionDamageParams = MakeDamageEffectParamsFromClassDefaults(); 
		Fireball->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		Fireball->ReturnToActor = GetAvatarActorFromActorInfo();
		Fireball->FinishSpawning(SpawnTransform);
		Fireballs.Add(Fireball);
	}
	
	return Fireballs;
}
