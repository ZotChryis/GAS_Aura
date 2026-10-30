#pragma once

#include "CoreMinimal.h"
#include "Checkpoint/AuraCheckpoint.h"
#include "AuraMapEntrance.generated.h"


UCLASS()
class AURA_API AAuraMapEntrance : public AAuraCheckpoint
{
	GENERATED_BODY()

public:
	
	AAuraMapEntrance(const FObjectInitializer& ObjectInitializer);
	
	/* SaveActorInterface */
	virtual void LoadActor_Implementation() override;
	/* End SaveActorInterface */
	
	/* HighlightInterface */
	virtual void HighlightActor_Implementation() override;
	/* End IHighlightInterface */
	
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;
	
	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag;
};
