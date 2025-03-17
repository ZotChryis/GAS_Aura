#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
class UBlackboardComponent;
class UBehaviorTree;
class AAuraAIController;

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
	
public:
	AAuraEnemy();
	
	virtual void PossessedBy(AController* NewController) override;

	/** Enemy Interface **/
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/** End Enemy Interface **/

	/** Combat Interface **/
	virtual bool IsRanged() override;
	virtual int32 GetPlayerLevel() override;
	virtual void SetCombatTarget_Implementation(AActor* InTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	/** End Combat Interface **/

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	void HitReactTagChanged(const FGameplayTag Tag, int32 Count);

	virtual void Die() override;

	UPROPERTY(BlueprintReadOnly, Category = "Combat");
	bool bHitReacting = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat");
	float BaseWalkSpeed = 250.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat");
	float Lifespan = 5.f;

	UPROPERTY(BlueprintReadWrite, Category = "Combat");
	TObjectPtr<AActor> CombatTarget;
 
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;

	UPROPERTY(EditAnywhere, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
	
	UPROPERTY(EditAnywhere, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI");
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere, Category = "Aura AI");
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY();
	TObjectPtr<AAuraAIController> AuraAIController;
};
