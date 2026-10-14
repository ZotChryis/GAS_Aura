#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
	
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(400.f, 400.f, 400.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CharacterClass = ECharacterClass::Elementalist;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init for the server
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init for the client
	InitAbilityActorInfo();
}

/** Combat Interface **/
int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	return AuraPlayerState->GetPlayerLevel();
}

/** End Combat Interface **/

/** Player Interface **/
int32 AAuraCharacter::FindLevelForExperience_Implementation(int32 InExperience) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	return AuraPlayerState->LevelUpData->FindLevelForExperience(InExperience);
}

int32 AAuraCharacter::GetExperience_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	return AuraPlayerState->GetExperience();
}

int32 AAuraCharacter::GetAttributePointsReward_Implementation(int32 InLevel) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	return AuraPlayerState->LevelUpData->LevelUpInformation[InLevel].AttributePointReward;
}

int32 AAuraCharacter::GetSpellPointsReward_Implementation(int32 InLevel) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	return AuraPlayerState->LevelUpData->LevelUpInformation[InLevel].SpellPointReward;
}

void AAuraCharacter::AddExperience_Implementation(int32 InExperience)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	AuraPlayerState->AddExperience(InExperience);
}

void AAuraCharacter::AddAttributePoints_Implementation(int32 InPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddAttributePoints(InPoints);
}

void AAuraCharacter::AddSpellPoints_Implementation(int32 InPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddSpellPoints(InPoints);
}

void AAuraCharacter::AddPlayerLevel_Implementation(int32 InLevels)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	AuraPlayerState->AddPlayerLevel(InLevels);
	
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AuraASC->UpdateAbilityStatuses(GetPlayerLevel_Implementation());
	}
}

void AAuraCharacter::LevelUp_Implementation()
{
	MulticastLevelUpVfx();
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetSpellPoints();
}

/** End Player Interface **/

void AAuraCharacter::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();
	
	AAuraPlayerState* AuraPlayerState = AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();
	
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	
	if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPC->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPC, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}

	InitializeDefaultAttributes();
}

void AAuraCharacter::MulticastLevelUpVfx_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = CameraComponent->GetComponentLocation();
		const FVector NiagaraLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - NiagaraLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		LevelUpNiagaraComponent->Activate(true);
	}
}
