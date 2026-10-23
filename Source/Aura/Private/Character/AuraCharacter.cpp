#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
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
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetSpellPoints();
}

void AAuraCharacter::ShowMagicCircle_Implementation()
{
	AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController());
	check(AuraPlayerController);
	
	AuraPlayerController->ShowMagicCircle();
	AuraPlayerController->SetShowMouseCursor(false);
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController());
	check(AuraPlayerController);
	
	AuraPlayerController->HideMagicCircle();
	AuraPlayerController->SetShowMouseCursor(true);
}

void AAuraCharacter::SetMagicCircleMaterial_Implementation(UMaterialInterface* InMaterial)
{
	AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController());
	check(AuraPlayerController);
	
	AuraPlayerController->SetMagicCircleMaterial(InMaterial);
}

void AAuraCharacter::OnRep_Burned()
{
	Super::OnRep_Burned();
}

void AAuraCharacter::OnRep_Stunned()
{
	Super::OnRep_Stunned();
	
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(FAuraGameplayTags::Get().Player_Block_InputHeld);
	TagContainer.AddTag(FAuraGameplayTags::Get().Player_Block_InputPressed);
	TagContainer.AddTag(FAuraGameplayTags::Get().Player_Block_InputReleased);
	TagContainer.AddTag(FAuraGameplayTags::Get().Player_Block_CursorTrace);
	
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		if (bIsStunned)
		{
			AuraASC->AddLooseGameplayTags(TagContainer);
		}
		else
		{
			AuraASC->RemoveLooseGameplayTags(TagContainer);
		}
	}
}

/** End Player Interface **/

void AAuraCharacter::InitAbilityActorInfo()
{
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
	
	OnASCRegistered.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraCharacter::StunTagChanged);
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
