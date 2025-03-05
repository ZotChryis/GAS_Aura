#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UAuraInputConfig;
class UInputAction;
class UInputMappingContext;
class IEnemyInterface;
class UAuraAbilitySystemComponent;
class USplineComponent;
struct FInputActionValue;

UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AAuraPlayerController();

	virtual void PlayerTick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

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

	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent();
	void CursorTrace();
	void Move(const FInputActionValue& InputActionValue);
	void AutoRun();
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	TScriptInterface<IEnemyInterface> LastActor;
	TScriptInterface<IEnemyInterface> ThisActor;
	FHitResult CursorHit;
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	bool bAutoRunning = false;
	bool bTargeting = false;
};
