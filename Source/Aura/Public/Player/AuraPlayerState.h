#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class ULevelUpInfo;
class UAbilitySystemComponent;
class UAttributeSet;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32);

UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const;
	
	FOnPlayerStatChanged OnLevelChanged;
	FOnPlayerStatChanged OnExperienceChanged;
	FOnPlayerStatChanged OnAttributePointsChanged;
	FOnPlayerStatChanged OnSpellPointsChanged;
	
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	void SetPlayerLevel(int32 InLevel);
	void AddPlayerLevel(int32 Amount);
	
	FORCEINLINE int32 GetExperience() const { return Experience; }
	void SetExperience(int32 InExperience);
	void AddExperience(int32 Amount);
	
	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; }
	void SetAttributePoints(int32 InAttributePoints);
	void AddAttributePoints(int32 Amount);
	
	FORCEINLINE int32 GetSpellPoints() const { return SpellPoints; }
	void SetSpellPoints(int32 InSpellPoints);
	void AddSpellPoints(int32 Amount);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpData;
	
protected:
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Experience)
	int32 Experience = 0;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttributePoints)
	int32 AttributePoints = 0;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SpellPoints)
	int32 SpellPoints = 0;
	
	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_Experience(int32 OldExperience);
	
	UFUNCTION()
	void OnRep_AttributePoints(int32 OldLevel);
	
	UFUNCTION()
	void OnRep_SpellPoints(int32 OldLevel);
};