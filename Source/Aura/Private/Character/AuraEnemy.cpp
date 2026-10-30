#include "Character/AuraEnemy.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AI/AuraAIController.h"
#include "Aura/Aura.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECR_Block);
	
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	if (Weapon)
	{
		Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	}
	
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	BaseWalkSpeed = 250.f;
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority())
	{
		return;
	}

	AuraAIController = Cast<AAuraAIController>(NewController);
	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AuraAIController->RunBehaviorTree(BehaviorTree);

	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsRanged"), IsRanged());
}

/** Highlight Interface **/
void AAuraEnemy::HighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
}

void AAuraEnemy::UnHighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	// No-op
}

/** End Highlight Interface **/

/** Combat Interface **/
int32 AAuraEnemy::GetPlayerLevel_Implementation()
{
	return Level;
}

bool AAuraEnemy::IsRanged()
{
	return CharacterClass == ECharacterClass::Elementalist || CharacterClass == ECharacterClass::Ranger;
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InTarget)
{
	CombatTarget = InTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

/** End Combat Interface **/

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	
	if (HasAuthority())
	{
		UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
	}
	
	if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		AuraUserWidget->SetWidgetController(this);
	}
	
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(GetAttributeSet());
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
		[&](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
		[&](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());

	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Event_Montage_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraEnemy::HitReactTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraEnemy::StunTagChanged);
}

void AAuraEnemy::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();
	
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}
	
	OnASCRegistered.Broadcast(AbilitySystemComponent);
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AAuraEnemy::StunTagChanged(const FGameplayTag Tag, int32 Count)
{
	Super::StunTagChanged(Tag, Count);
	
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsStunned"), bIsStunned);
	}
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag Tag, int32 Count)
{
	bHitReacting = Count > 0;

	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0 : BaseWalkSpeed;

	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsHitReacting"), bHitReacting);
	}
}

void AAuraEnemy::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);

	if (AuraAIController)
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsDead"), true);
	}
	
	SetLifeSpan(Lifespan);
}
