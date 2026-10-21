#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "PassiveNiagaraComponent.generated.h"

UCLASS()
class AURA_API UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
	
public:
	
	UPassiveNiagaraComponent(const FObjectInitializer& Initializer);
	
	UPROPERTY(VisibleAnywhere)
	FGameplayTag PassiveAbilityTag;
	
	virtual void BeginPlay() override;
	
protected:
	virtual void OnPassiveAbilityActivated(const FGameplayTag& AbilityTag);
	virtual void OnPassiveAbilityDeactivated(const FGameplayTag& AbilityTag);
};
