#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "AbilitySystem/DebuffNiagaraComponent.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UPassiveNiagaraComponent;
class UAbilitySystemComponent;
class UAttributeSet;
class UAnimMontage;

UCLASS()
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const;

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);
	
	UFUNCTION()
	virtual void OnRep_Burned();
	
	UFUNCTION()
	virtual void OnRep_Stunned();
	
	/** Combat Interface **/
	virtual void Die(const FVector& DeathImpulse) override;
	virtual void Knockback(const FVector& KnockbackImpulse) override;
	virtual USkeletalMeshComponent* GetWeaponMesh_Implementation() const override;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual ECharacterClass GetCharacterClass_Implementation() const override;
	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() override;
	virtual FOnDeath& GetOnDeathDelegate() override;
	virtual bool GetInShockLoop_Implementation() override;
	virtual void SetInShockLoop_Implementation(const bool bInLoop) override;
	/** End Combat Interface **/
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;
	
	FOnASCRegistered OnASCRegistered;
	
	UPROPERTY()
	FOnDeath OnDeath;
	
	UPROPERTY(ReplicatedUsing=OnRep_Burned, BlueprintReadOnly, Category = "Combat")
	bool bIsBurned = false;
	
	UPROPERTY(ReplicatedUsing=OnRep_Stunned, BlueprintReadOnly, Category = "Combat")
	bool bIsStunned = false;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
	bool bIsShockLoop = false;

protected:
	UPROPERTY(EditAnywhere, Category = "Combat");
	float BaseWalkSpeed = 600.f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TailSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo();
	virtual void InitializeDefaultAttributes() const;
	
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float level) const;
	void AddCharacterAbilities() const;

	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	
	void Dissolve();
	virtual void StunTagChanged(const FGameplayTag Tag, int32 Count);

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UNiagaraSystem* BloodEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	USoundBase* DeathSound;

	UPROPERTY(VisibleAnywhere, Category = "Debuffs")
	TObjectPtr<UDebuffNiagaraComponent> BurnDebuffVfx;
	
	UPROPERTY(VisibleAnywhere, Category = "Debuffs")
	TObjectPtr<UDebuffNiagaraComponent> StunDebuffVfx;
	
	/** Minions **/
	int32 MinionCount = 0;
	virtual int32 GetMinionCount_Implementation() override;
	virtual void UpdateMinionCount_Implementation(int32 Amount) override;

private:
	UPROPERTY(EditAnywhere, Category="Combat")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, Category="Combat")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;

	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	TObjectPtr<UPassiveNiagaraComponent> HaloOfProtectionNiagaraComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Combat")
	TObjectPtr<UPassiveNiagaraComponent> LifeSiphonNiagaraComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Combat")
	TObjectPtr<UPassiveNiagaraComponent> ManaSiphonNiagaraComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Combat")
	TObjectPtr<USceneComponent> EffectAttachComponent;
	
	bool bDead = false;
};
