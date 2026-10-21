#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (!HitResult.bBlockingHit)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	MouseHitLocation = HitResult.ImpactPoint;
	MouseHitActor = HitResult.GetActor();
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	if (!OwnerCharacter)
	{
		return;
	}
	
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeaponMesh(OwnerCharacter))
		{
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(OwnerCharacter);
			const FVector& SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));
			FHitResult HitResult;
			UKismetSystemLibrary::SphereTraceSingle(
				OwnerCharacter, 
				SocketLocation, 
				BeamTargetLocation, 
				10.f, 
				TraceTypeQuery1, 
				false, 
				ActorsToIgnore, 
				EDrawDebugTrace::None,
				HitResult,
				true
			);
			
			if (HitResult.bBlockingHit)
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
	}
	
	if (ICombatInterface* HitActorCombatInterface = Cast<ICombatInterface>(MouseHitActor))
	{
		if (!HitActorCombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::PrimaryTargetDied))
		{
			HitActorCombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
		}
	}
}

void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);
	TArray<AActor*> AllOverlappingActors;
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(), 
		AllOverlappingActors, 
		ActorsToIgnore,
		MouseHitActor->GetActorLocation(),
		850.f
	);
	
	int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);
	UAuraAbilitySystemLibrary::GetClosestTargets(
		NumAdditionalTargets, 
		AllOverlappingActors, 
		OutAdditionalTargets, 
		MouseHitActor->GetActorLocation()
	);
	
	for (AActor* Actor : OutAdditionalTargets)
	{
		if (ICombatInterface* AdditionalTargetCombatInterface = Cast<ICombatInterface>(Actor))
		{
			if (!AdditionalTargetCombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::AdditionalTargetDied))
			{
				AdditionalTargets.Add(TWeakObjectPtr(Actor));
				AdditionalTargetCombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
			}
		}
	}
}

void UAuraBeamSpell::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if (MouseHitActor)
	{
		if (ICombatInterface* MainTargetCombatInterface = Cast<ICombatInterface>(MouseHitActor))
		{
			MainTargetCombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
		}
	}
	
	for (TWeakObjectPtr ActorPtr : AdditionalTargets)
	{
		if (AActor* Actor = ActorPtr.Get())
		{
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Actor))
			{
				CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
			}
		}
	}
	AdditionalTargets.Empty();
}
