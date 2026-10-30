#include "Checkpoint/AuraCheckpoint.h"

#include "Aura/Aura.h"
#include "Components/SphereComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

AAuraCheckpoint::AAuraCheckpoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	
	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckpointMesh");
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);
	CheckpointMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
	
	MoveToComponent = CreateDefaultSubobject<USceneComponent>("MoveToComponent");
	MoveToComponent->SetupAttachment(CheckpointMesh);
	
	Sphere = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	Sphere->SetupAttachment(CheckpointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AAuraCheckpoint::HandleGlowEffects()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(CheckpointMesh->GetMaterial(0), this);
	CheckpointMesh->SetMaterial(0, DynamicMaterialInstance);
	
	CheckpointReached(DynamicMaterialInstance);
}

void AAuraCheckpoint::HighlightActor_Implementation()
{
	if (!bReached)
	{
		CheckpointMesh->SetRenderCustomDepth(true);
	}
}

void AAuraCheckpoint::UnHighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(false);
}

void AAuraCheckpoint::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToComponent->GetComponentLocation();
}

void AAuraCheckpoint::LoadActor_Implementation()
{
	if (bReached)
	{
		HandleGlowEffects();
	}
}

void AAuraCheckpoint::BeginPlay()
{
	Super::BeginPlay();
	
	if (bBindOverlapCallback)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraCheckpoint::OnSphereOverlap);
	}
}

void AAuraCheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;
		
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGameMode->SaveWorldState(GetWorld());
		}
		
		HandleGlowEffects();
	}
}
