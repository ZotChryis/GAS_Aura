#include "Actor/PointCollection.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

APointCollection::APointCollection()
{
	PrimaryActorTick.bCanEverTick = true;

	Pt_0 = CreateDefaultSubobject<USceneComponent>("Pt_0");
	ImmatablePts.Add(Pt_0);
	SetRootComponent(Pt_0);
	
	Pt_1 = CreateDefaultSubobject<USceneComponent>("Pt_1");
	Pt_1->SetupAttachment(GetRootComponent());
	ImmatablePts.Add(Pt_1);
	
	Pt_2 = CreateDefaultSubobject<USceneComponent>("Pt_2");
	Pt_2->SetupAttachment(GetRootComponent());
	ImmatablePts.Add(Pt_2);
	
	Pt_3 = CreateDefaultSubobject<USceneComponent>("Pt_3");
	Pt_3->SetupAttachment(GetRootComponent());
	ImmatablePts.Add(Pt_3);
	
	Pt_4 = CreateDefaultSubobject<USceneComponent>("Pt_4");
	Pt_4->SetupAttachment(GetRootComponent());
	ImmatablePts.Add(Pt_4);
	
	Pt_5 = CreateDefaultSubobject<USceneComponent>("Pt_5");
	Pt_5->SetupAttachment(GetRootComponent());
	ImmatablePts.Add(Pt_5);
	
	Pt_6 = CreateDefaultSubobject<USceneComponent>("Pt_6");
	Pt_6->SetupAttachment(GetRootComponent());
	ImmatablePts.Add(Pt_6);
	
	Pt_7 = CreateDefaultSubobject<USceneComponent>("Pt_7");
	Pt_7->SetupAttachment(GetRootComponent());
	ImmatablePts.Add(Pt_7);
	
	Pt_8 = CreateDefaultSubobject<USceneComponent>("Pt_8");
	Pt_8->SetupAttachment(GetRootComponent());
	ImmatablePts.Add(Pt_8);
	
	Pt_9 = CreateDefaultSubobject<USceneComponent>("Pt_9");
	Pt_9->SetupAttachment(GetRootComponent());
	ImmatablePts.Add(Pt_9);
	
	Pt_10 = CreateDefaultSubobject<USceneComponent>("Pt_10");
	Pt_10->SetupAttachment(GetRootComponent());
	ImmatablePts.Add(Pt_10);
}

TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, int32 Amount, float YawOverride)
{
	checkf(ImmatablePts.Num() >= Amount, TEXT("Attempted to access ImmutablePts out of bounds."));
	checkf(Amount > 0, TEXT("Attempted to access ImmutablePts out of bounds."));

	TArray<USceneComponent*> Pts;
	for (USceneComponent* Pt : ImmatablePts)
	{
		if (Pts.Num() >= Amount)
		{
			break;
		}
		
		if (Pt != Pt_0)
		{
			FVector ToPoint = Pt->GetComponentLocation() - Pt_0->GetComponentLocation();
			ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);
			Pt->SetWorldLocation(Pt_0->GetComponentLocation() + ToPoint);
		}
		
		const FVector RaisedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z + 500.f);
		const FVector LoweredLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z - 500.f);
		
		TArray<AActor*> IgnoreActors;
		UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(this, IgnoreActors, TArray<AActor*>(), GetActorLocation(), 1500.f);
		
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(IgnoreActors);
		GetWorld()->LineTraceSingleByProfile(HitResult, RaisedLocation, LoweredLocation, FName("BlockAll"), QueryParams);
		
		const FVector AdjustedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, HitResult.ImpactPoint.Z);
		Pt->SetWorldLocation(AdjustedLocation);
		Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));
		Pts.Add(Pt);
	}
	
	return Pts;
}

void APointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}
