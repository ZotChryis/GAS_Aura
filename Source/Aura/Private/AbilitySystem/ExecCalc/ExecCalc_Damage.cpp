#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

void AuraDamageStatics::InitializeTagsToCaptureDefsMap()
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_Armor, ArmorDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_BlockChance, BlockChanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Fire, FireResistanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Lightning, LightningResistanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Physical, PhysicalResistanceDef);
	
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);
}

void InitializeStaticTagsToCaptureDefsMap()
{
	DamageStatics().InitializeTagsToCaptureDefsMap();
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);

	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	check(SourceASC);
	check(TargetASC);
	
	AActor* SourceAvatar = SourceASC->GetAvatarActor();
	AActor* TargetAvatar = TargetASC->GetAvatarActor();

	check(SourceAvatar);
	check(TargetAvatar);

	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	check(SourceCombatInterface);
	check(TargetCombatInterface);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
	
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = SourceTags;
	EvalParams.TargetTags = TargetTags;

	// Get the damage set by caller
	float Damage = 0.f;
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : FAuraGameplayTags::Get().DamageTypesToResistances)
	{
		const FGameplayTag& DamageTypeTag = Pair.Key;
		const FGameplayTag& ResistanceTag = Pair.Value;
		checkf(DamageStatics().TagsToCaptureDefs.Contains(ResistanceTag), TEXT("TagsToCaptureDefs does not contain %s in ExecCalc_Damage"), *ResistanceTag.ToString());
		
		float Resistance;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().TagsToCaptureDefs[ResistanceTag], EvalParams, Resistance);
		Resistance = FMath::Clamp<float>(Resistance, 0.f, 100.f);;
		
		Damage += ((100.f - Resistance) / 100.f) * Spec.GetSetByCallerMagnitude(DamageTypeTag, false);
	}
	
	// Capture block chance on target, and determine if there was a successful block
	float BlockChance;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvalParams, BlockChance);
	BlockChance = FMath::Max<float>(0, BlockChance);

	// If blocked, halve the damage
	const bool bBlockedHit = FMath::RandRange(1, 100) <= BlockChance;
	Damage = bBlockedHit ? Damage / 2.f : Damage;
	UAuraAbilitySystemLibrary::SetIsBlockedHit(ContextHandle, bBlockedHit);

	// Armor penetration ignores a percentage of the target's armor
	float TargetArmor;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvalParams, TargetArmor);
	TargetArmor = FMath::Max<float>(0, TargetArmor);

	float SourceArmorPenetration;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvalParams, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0, SourceArmorPenetration);
	
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	const TObjectPtr<UCurveTable> Coefficients = CharacterClassInfo->DamageCalculationCoefficients;
	const FRealCurve* ArmorCurve = Coefficients->FindCurve(FName("Armor"), FString());
	const FRealCurve* ArmorPenetrationCurve = Coefficients->FindCurve(FName("ArmorPenetration"), FString());

	check(ArmorPenetrationCurve);
	check(ArmorCurve);
	
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());
	const float ArmorCoefficient = ArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	
	// Armor penetration ignores a percentage of the Target's Armor
	const float EffectiveArmor = FMath::Max<float>(TargetArmor * (100.f - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f, 0.f);

	// Armor ignores a percentage of incoming damage (after block reductions)
	Damage *= (100.f - EffectiveArmor * ArmorCoefficient) / 100.f;

	// Determine if this hit is a critical hit from source's critical hit chance
	// Target can have resistance to critical hit that will skew towards their favor. This is scaled by level
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvalParams, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(0, SourceCriticalHitChance);

	float TargetCriticalHitResistance;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvalParams, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(0, TargetCriticalHitResistance);
	
	const FRealCurve* CritialHitResistanceCurve = Coefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float EffectiveCriticalHitResistance = FMath::Max<float>(0.f, TargetCriticalHitResistance * CritialHitResistanceCurve->Eval(TargetCombatInterface->GetPlayerLevel()));

	const float EffectCriticalHitChance = FMath::Max<float>(0, SourceCriticalHitChance - EffectiveCriticalHitResistance);
	const bool bCriticalHit = FMath::RandRange(1, 100) <= EffectCriticalHitChance;
	UAuraAbilitySystemLibrary::SetIsCriticalHit(ContextHandle, bCriticalHit);
	
	if (bCriticalHit)
	{
		float SourceCriticalHitDamage = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvalParams, SourceCriticalHitDamage);
		SourceCriticalHitDamage = FMath::Max<float>(0, SourceCriticalHitDamage);
		
		// Critical hits are x2 + CritHitDamage
		Damage = Damage * 2.f + SourceCriticalHitDamage;
	}
	
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
