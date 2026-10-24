#include "Actor/AuraFireball.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayCueManager.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void AAuraFireball::BeginPlay()
{
	Super::BeginPlay();
	StartOutgoingTimeline();
}

void AAuraFireball::OnHit()
{
	if (GetOwner())
	{
		FGameplayCueParameters Params;
		Params.Location = GetActorLocation();
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(), FAuraGameplayTags::Get().GameplayCue_Fireblast, Params);
	}
	
	bHit = true;
}

void AAuraFireball::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor))
	{
		return;	
	}
	
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		DamageEffectParams.TargetASC = ASC;
		DamageEffectParams.DeathImpulse = (GetActorForwardVector() + GetActorUpVector()) * DamageEffectParams.DeathImpulseMagnitude;
		DamageEffectParams.KnockbackForce = (GetActorForwardVector() + GetActorUpVector()) * DamageEffectParams.KnockbackForceMagnitude;
		UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
	}
}
