#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "DebuffNiagaraComponent.generated.h"

UCLASS()
class AURA_API UDebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
	
public:
	
	UDebuffNiagaraComponent(const FObjectInitializer& Initializer);
	
	UPROPERTY(VisibleAnywhere)
	FGameplayTag DebuffTag;
	
protected:
	virtual void BeginPlay() override;
	void DebuffTagChanged(const FGameplayTag InDebuffTag, int32 Amount);
	
	UFUNCTION()
	void OnOwnerDeath(AActor* DeadActor);
};
