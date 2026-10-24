#pragma once

#include "CoreMinimal.h"
#include "AuraProjectile.h"
#include "GameFramework/Actor.h"
#include "AuraFireball.generated.h"

UCLASS()
class AURA_API AAuraFireball : public AAuraProjectile
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StartOutgoingTimeline();
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> ReturnToActor;

	UPROPERTY(BlueprintReadWrite)
	FDamageEffectParams ExplosionDamageParams;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnHit() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
