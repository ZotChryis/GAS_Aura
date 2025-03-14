#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_ArcaneResist.generated.h"

UCLASS()
class AURA_API UMMC_ArcaneResist : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public :
	UMMC_ArcaneResist();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
};
