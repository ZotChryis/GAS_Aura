#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_PhysicalResist.generated.h"

UCLASS()
class AURA_API UMMC_PhysicalResist : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public :
	UMMC_PhysicalResist();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition StrengthDef;
	FGameplayEffectAttributeCaptureDefinition VigorDef;
};
