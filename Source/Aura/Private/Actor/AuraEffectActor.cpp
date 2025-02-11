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

void AAuraEffectActor::ApplyEffectToTarget(AActor* Target, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	check(GameplayEffectClass);
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
	if (!TargetASC)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	FGameplayEffectSpecHandle EffectSpec = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, EffectContext);
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get()); 
}
