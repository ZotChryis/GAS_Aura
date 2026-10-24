#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraFireblast.generated.h"

class AAuraFireball;

UCLASS()
class AURA_API UAuraFireblast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
	
	UFUNCTION(BlueprintCallable)
	TArray<AAuraFireball*> SpawnFireballs();
	
protected:
	UPROPERTY(EditDefaultsOnly)
	int NumFireballs = 12;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAuraFireball> FireballClass;
};
