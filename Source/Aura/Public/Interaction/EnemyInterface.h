#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

class AURA_API IEnemyInterface
{
	GENERATED_BODY()
public:
	virtual bool IsRanged() = 0;
	virtual void HighlightActor() = 0;
	virtual void UnHighlightActor() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetCombatTarget(AActor* Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetCombatTarget() const;
};
