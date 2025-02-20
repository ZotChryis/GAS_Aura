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

bool AAuraEffectActor::TryApplyEffectsToTargetByPolicy(AActor* TargetActor)
{
	bool bApplied = false;
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		for (TSubclassOf<UGameplayEffect> InstantGameplayEffectClass : InstantGameplayEffectClasses)
		{
			bApplied = ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
		}
	}

	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		for (TSubclassOf<UGameplayEffect> InstantGameplayEffectClass : DurationGameplayEffectClasses)
		{
			bApplied = ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
		}
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		for (TSubclassOf<UGameplayEffect> InstantGameplayEffectClass : InfiniteGameplayEffectClasses)
		{
			bApplied = ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
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
	const FGameplayEffectSpecHandle EffectSpec = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, EffectContext);
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
	bool bApplied = TryApplyEffectsToTargetByPolicy(TargetActor);
	if (bApplied && bDestroyOnEffectApplication)
	{
		Destroy();
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	bool bApplied = TryApplyEffectsToTargetByPolicy(TargetActor);

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
