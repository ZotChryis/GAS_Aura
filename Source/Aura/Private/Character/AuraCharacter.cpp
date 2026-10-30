#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Camera/CameraComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Game/AuraLoadScreenSaveGame.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
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
	LoadProgress();
	
	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		AuraGameMode->LoadWorldState(GetWorld());
	}
}

void AAuraCharacter::LoadProgress() const
{
	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		if (UAuraLoadScreenSaveGame* SaveData = AuraGameMode->RetrieveInGameSaveData())
		{
			if (SaveData->FirstTimeLoadIn)
			{
				InitializeDefaultAttributes();
				AddCharacterAbilities();
			}
			else
			{
				if (AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>())
				{
					AuraPlayerState->SetPlayerLevel(SaveData->PlayerLevel);
					AuraPlayerState->SetExperience(SaveData->XP);
					AuraPlayerState->SetAttributePoints(SaveData->AttributePoints);
					AuraPlayerState->SetSpellPoints(SaveData->SpellPoints);
				}
				
				InitializeAttributesFromDisk(SaveData);
				if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
				{
					AuraASC->AddCharacterAbilitiesFromDisk(SaveData);
				}
			}
		}
	}
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

void AAuraCharacter::SaveProgress_Implementation(const FName CheckpointTag)
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		if (UAuraLoadScreenSaveGame* SaveData = AuraGameMode->RetrieveInGameSaveData())
		{
			SaveData->FirstTimeLoadIn = false;
			SaveData->PlayerStartTag = CheckpointTag;
			
			if (AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>())
			{
				SaveData->PlayerLevel = AuraPlayerState->GetPlayerLevel();
				SaveData->XP = AuraPlayerState->GetExperience();
				SaveData->AttributePoints = AuraPlayerState->GetAttributePoints();
				SaveData->SpellPoints = AuraPlayerState->GetSpellPoints();
			}
			
			SaveData->Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
			SaveData->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
			SaveData->Resilience = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
			SaveData->Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());
			
			SaveData->Abilities.Empty();
			UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
			UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(this);
			FForEachAbility SaveAbilityDelegate;
			SaveAbilityDelegate.BindLambda([&AbilityInfo, &AuraASC, &SaveData](const FGameplayAbilitySpec& Spec)
			{
				const FGameplayTag& AbilityTag = AuraASC->GetAbilityTagFromSpec(Spec);
				const FAuraAbilityInfo& Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
				
				FSavedAbility SavedAbility;
				SavedAbility.Ability = Info.Ability;
				SavedAbility.AbilityType = Info.AbilityType;
				SavedAbility.AbilityTag = AbilityTag;
				SavedAbility.InputTag = AuraASC->GetAbilityInputTagFromSpec(Spec);
				SavedAbility.AbilityStatus = AuraASC->GetAbilityStatusFromSpec(Spec);
				SavedAbility.AbilityLevel = Spec.Level;
				
				SaveData->Abilities.AddUnique(SavedAbility);
			});
			AuraASC->ForEachAbility(SaveAbilityDelegate);
			
			AuraGameMode->SaveInGameProgress(SaveData);
		}
	}
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
	
	OnASCRegistered.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraCharacter::StunTagChanged);
}

void AAuraCharacter::InitializeAttributesFromDisk(UAuraLoadScreenSaveGame* SaveData) const
{
	FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponent()->MakeEffectContext();
	EffectContext.AddSourceObject(GetAbilitySystemComponent()->GetAvatarActor());
	const FGameplayEffectSpecHandle PrimaryAttributesEffect = GetAbilitySystemComponent()->MakeOutgoingSpec(FromDiskPrimaryAttributes, 1, EffectContext);
	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimaryAttributesEffect, FAuraGameplayTags::Get().Attributes_Primary_Strength, SaveData->Strength);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimaryAttributesEffect, FAuraGameplayTags::Get().Attributes_Primary_Intelligence, SaveData->Intelligence);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimaryAttributesEffect, FAuraGameplayTags::Get().Attributes_Primary_Resilience, SaveData->Resilience);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(PrimaryAttributesEffect, FAuraGameplayTags::Get().Attributes_Primary_Vigor, SaveData->Vigor);
	
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*PrimaryAttributesEffect.Data.Get(), GetAbilitySystemComponent()); 
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
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
