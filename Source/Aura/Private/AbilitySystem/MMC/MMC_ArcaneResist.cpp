#include "AbilitySystem/MMC/MMC_ArcaneResist.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_ArcaneResist::UMMC_ArcaneResist()
{
	IntelligenceDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute();
	IntelligenceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	IntelligenceDef.bSnapshot = false;
	
	RelevantAttributesToCapture.Add(IntelligenceDef);
}

float UMMC_ArcaneResist::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Arcane Resist = % of Intelligence + Player Level Scalar
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = SourceTags;
	EvaluateParams.TargetTags = TargetTags;
	
	float Intelligence;
	GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluateParams, Intelligence);
	Intelligence = FMath::Max<float>(Intelligence, 0.0f);

	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}
	
	return Intelligence * 0.25f + 1.5 * PlayerLevel;
}
