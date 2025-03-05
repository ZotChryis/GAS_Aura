#include "Player/AuraPlayerController.h"

#include "AbilitySystemGlobals.h"
#include "Interaction/EnemyInterface.h"
#include "Input/AuraEnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraEnhancedInputComponent* AuraEnhancedInputComponent = CastChecked<UAuraEnhancedInputComponent>(InputComponent);
	AuraEnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraEnhancedInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator YawRotation (0.f, GetControlRotation().Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>()) 
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit)
	{
		return;
	}

	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

	if (LastActor == nullptr && ThisActor == nullptr)
	{
		return;
	}
	else if (LastActor == nullptr && ThisActor != nullptr) 
	{
		ThisActor->HighlightActor();
	}
	else if (LastActor != nullptr && ThisActor == nullptr) {
		LastActor->UnHighlightActor();
	}
	else if (LastActor != ThisActor)
	{
		LastActor->UnHighlightActor();
		ThisActor->HighlightActor();
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetAuraAbilitySystemComponent() == nullptr)
	{
		return;
	}
	
	GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetAuraAbilitySystemComponent() == nullptr)
	{
		return;
	}
	
	GetAuraAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraAbilitySystemComponent()
{
	if (!AuraASC)
	{
		AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn()));
	}

	return AuraASC;
}
