#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	bool IsBlocked() const { return bBlocked; }
	bool IsCriticalHit() const { return bCriticalHit; }
	bool IsSuccessfulDebuff() const { return bSuccessfulDebuff; }
	float GetDebuffDamage() const { return DebuffDamage; }
	float GetDebuffDuration() const { return DebuffDuration; }
	float GetDebuffFrequency() const { return DebuffFrequency; }
	TSharedPtr<FGameplayTag> GetDamageType () const { return DamageType; }

	void SetIsBlocked(bool Blocked) { bBlocked = Blocked; }
	void SetIsCriticalHit(bool CriticalHit) { bCriticalHit = CriticalHit; }
	void SetIsSuccessfulDebuff(bool bSuccess) { bSuccessfulDebuff = bSuccess; }
	void SetDebuffDamage(float Damage) { DebuffDamage = Damage; }
	void SetDebuffDuration(float Duration) { DebuffDuration = Duration; }
	void SetDebuffFrequency(float Frequency) { DebuffFrequency = Frequency; }
	void SetDamageType (const TSharedPtr<FGameplayTag>& InDamageType) { DamageType = InDamageType; }
	
	virtual UScriptStruct* GetScriptStruct() const override;
	virtual FAuraGameplayEffectContext* Duplicate() const override;
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

protected:
	UPROPERTY()
	bool bBlocked = false;
	
	UPROPERTY()
	bool bCriticalHit = false;
	
	UPROPERTY()
	bool bSuccessfulDebuff = false;
	
	UPROPERTY()
	float DebuffDamage = 0.f;
	
	UPROPERTY()
	float DebuffDuration = 0.f;
	
	UPROPERTY()
	float DebuffFrequency = 0.f;
	
	TSharedPtr<FGameplayTag> DamageType;
};

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()
	
	FDamageEffectParams(){}
	
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;
	
	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageEffectClass = nullptr;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC = nullptr;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetASC = nullptr;
	
	UPROPERTY()
	float BaseDamage = 0.f;
	
	UPROPERTY()
	float AbilityLevel = 1.f;
	
	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag();
	
	UPROPERTY()
	float DebuffChance = 0.f;
	
	UPROPERTY()
	float DebuffDamage = 0.f;
	
	UPROPERTY()
	float DebuffDuration = 0.f;
	
	UPROPERTY()
	float DebuffFrequency = 0.f;
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WIthCopy = true
	};	
};