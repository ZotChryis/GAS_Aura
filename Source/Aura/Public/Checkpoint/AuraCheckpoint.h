#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/HighlightInterface.h"
#include "Interaction/SaveInterface.h"
#include "AuraCheckpoint.generated.h"

class USphereComponent;

UCLASS()
class AURA_API AAuraCheckpoint : public APlayerStart, public ISaveInterface, public IHighlightInterface
{
	GENERATED_BODY()
	
public:
	AAuraCheckpoint(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointReached(UMaterialInstanceDynamic* DynamicMaterialInstance);
	
	UFUNCTION(BlueprintCallable)
	void HandleGlowEffects();
	
	/** IHighlightInterface */
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;
	/** End IHighlightInterface */
	
	/** ISaveInterface */
	virtual bool ShouldLoadTransform_Implementation() override { return false; }
	virtual void LoadActor_Implementation() override;
	/** End ISaveInterface */
	
	UPROPERTY(BlueprintReadWrite, SaveGame)
	bool bReached = false;
	
	UPROPERTY(EditAnywhere)
	bool bBindOverlapCallback = true;
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MoveToComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
};
