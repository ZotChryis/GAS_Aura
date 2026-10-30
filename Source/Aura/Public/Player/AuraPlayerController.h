#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class AMagicCircle;
class UNiagaraSystem;
class UDamageTextComponent;
class UAuraInputConfig;
class UInputAction;
class UInputMappingContext;
class IHighlightInterface;
class UAuraAbilitySystemComponent;
class USplineComponent;
struct FInputActionValue;

enum ETargettingStatus
{
	TargettingNone,
	TargettingEnemy,
	TargettingNonEnemy
};

UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AAuraPlayerController();

	virtual void PlayerTick(float DeltaTime) override;
	FHitResult GetCursorHit() const;

	UFUNCTION(Client, Reliable)
	void ShowDamageText(float Damage, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);
	
	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle();
	
	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();
	
	UFUNCTION(BlueprintCallable)
	void SetMagicCircleMaterial(UMaterialInterface* InMaterial);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ShiftAction;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraASC;
	
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float ShortPressThreshold = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere, Category="Movement")
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMagicCircle> MagicCircleActorClass;
	
	UPROPERTY()
	TObjectPtr<AMagicCircle> MagicCircleActor;
	
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent();
	void HighlightActor(AActor* InActor);
	void UnHighlightActor(AActor* InActor);
	void CursorTrace();
	void Move(const FInputActionValue& InputActionValue);
	void AutoRun();
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	void ShiftPressed() { bShiftKeyDown = true;}
	void ShiftReleased() { bShiftKeyDown = false;}
	
	void UpdateMagicCircle() const;
	
	TObjectPtr<AActor> LastActor;
	TObjectPtr<AActor> ThisActor;
	FHitResult CursorHit;
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	bool bAutoRunning = false;
	ETargettingStatus TargetStatus = TargettingNone;
	bool bShiftKeyDown = false;
};
