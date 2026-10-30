#include "Checkpoint/AuraMapEntrance.h"

#include "Components/SphereComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

AAuraMapEntrance::AAuraMapEntrance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Sphere->SetupAttachment(MoveToComponent);
}

void AAuraMapEntrance::LoadActor_Implementation()
{
	// No-op, don't need to do anything for map entrances
}

void AAuraMapEntrance::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}

void AAuraMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGameMode->SaveWorldState(GetWorld(), DestinationMap.ToSoftObjectPath().GetAssetName(), DestinationPlayerStartTag);
		}
		
		FString Options = TEXT("?PlayerStartTag=" +  DestinationPlayerStartTag.ToString());
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap, true, Options);
	}
}
