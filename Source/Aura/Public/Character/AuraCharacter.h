#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class UAuraLoadScreenSaveGame;
class UCameraComponent;
class USpringArmComponent;
class UNiagaraComponent;

UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()
	
public:
	AAuraCharacter();
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/** Combat Interface **/
	virtual int32 GetPlayerLevel_Implementation() override;
	/** End Combat Interface **/

	/** Player Interface **/
	virtual int32 FindLevelForExperience_Implementation(int32 InExperience) const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 InLevel) const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 InLevel) const override;
	virtual int32 GetExperience_Implementation() const override;
	virtual void AddExperience_Implementation(int32 InExperience) override;
	virtual void AddAttributePoints_Implementation(int32 InPoints) override;
	virtual void AddSpellPoints_Implementation(int32 InPoints) override;
	virtual void AddPlayerLevel_Implementation(int32 InLevels) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;
	virtual void ShowMagicCircle_Implementation() override;
	virtual void HideMagicCircle_Implementation() override;
	virtual void SetMagicCircleMaterial_Implementation(UMaterialInterface* InMaterial) override;
	virtual void SaveProgress_Implementation(const FName CheckpointTag) override;
	/** End Player Interface **/

	virtual void OnRep_Burned() override;
	virtual void OnRep_Stunned() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

protected:
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeAttributesFromDisk(UAuraLoadScreenSaveGame* SaveData) const;
	void LoadProgress() const;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> FromDiskPrimaryAttributes;

private:

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpVfx() const;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
};
