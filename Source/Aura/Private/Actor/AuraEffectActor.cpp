#include "Actor/AuraEffectActor.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

bool AAuraEffectActor::TryApplyEffectsToTargetByPolicy(AActor* TargetActor, EEffectApplicationPolicy Policy)
{
	bool bApplied = false;
	if (InstantEffectApplicationPolicy == Policy)
	{
		for (TSubclassOf<UGameplayEffect> GameplayEffectClass : InstantGameplayEffectClasses)
		{
			bApplied = ApplyEffectToTarget(TargetActor, GameplayEffectClass);
		}
	}

	if (DurationEffectApplicationPolicy == Policy)
	{
		for (TSubclassOf<UGameplayEffect> GameplayEffectClass : DurationGameplayEffectClasses)
		{
			bApplied = ApplyEffectToTarget(TargetActor, GameplayEffectClass);
		}
	}

	if (InfiniteEffectApplicationPolicy == Policy)
	{
		for (TSubclassOf<UGameplayEffect> GameplayEffectClass : InfiniteGameplayEffectClasses)
		{
			bApplied = ApplyEffectToTarget(TargetActor, GameplayEffectClass);
		}
	}

	return bApplied;
}

bool AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	check(GameplayEffectClass);
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	if (!TargetASC)
	{
		return false;
	}

	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpec = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContext);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
	
	const bool bInfinite = EffectSpec.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bInfinite && InfiniteEffectRemovePolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		if (!ActiveEffectHandles.Contains(TargetASC))
		{
			ActiveEffectHandles.Add(TargetASC, TArray<FActiveGameplayEffectHandle>());
		}
		ActiveEffectHandles[TargetASC].Add(ActiveEffectHandle);
	}

	return true;
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	bool bApplied = TryApplyEffectsToTargetByPolicy(TargetActor, EEffectApplicationPolicy::ApplyOnOverlap);
	if (bApplied && bDestroyOnEffectApplication)
	{
		Destroy();
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	bool bApplied = TryApplyEffectsToTargetByPolicy(TargetActor, EEffectApplicationPolicy::ApplyOnEndOverlap);

	bool bRemoved = false;
	if (InfiniteEffectRemovePolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
		if (!IsValid(TargetASC))
		{
			return;
		}
		TArray<FActiveGameplayEffectHandle> EffectHandles;
		if (ActiveEffectHandles.RemoveAndCopyValue(TargetASC, EffectHandles))
		{
			for (FActiveGameplayEffectHandle EffectHandle : EffectHandles)
			{
				TargetASC->RemoveActiveGameplayEffect(EffectHandle, 1);
				bRemoved = true;
			}
		}
	}

	// Only try to destroy after we've determined to apply and remove.
	if (bApplied && bDestroyOnEffectApplication)
	{
		Destroy();
		return;
	}
	
	if (bRemoved && bDestroyOnEffectRemoval)
	{
		Destroy();
		return;
	}
}
