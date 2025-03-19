#include "AbilitySystem/Abilities/AuraSummonGameplayAbility.h"

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonGameplayAbility::GetSpawnLocations()
{
	TArray<FVector> SpawnLocations;

	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();

	const FVector MinSpread = Forward.RotateAngleAxis(-SpawnSpread/2, FVector::UpVector);
	const FVector MaxSpread = Forward.RotateAngleAxis(SpawnSpread/2, FVector::UpVector);
	
	const float DeltaSpread = SpawnSpread / NumMinions;
	for (int i = 0; i < NumMinions; i++)
	{
		const FVector Direction = MinSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		const float Distance = FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);
		FVector GeneratedLocation = Location + Direction * Distance;

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, GeneratedLocation + FVector(0.f, 0.f, 400.f), GeneratedLocation - FVector(0.f, 0.f, 400.f), ECC_Visibility);

		if (Hit.bBlockingHit)
		{
			GeneratedLocation = Hit.ImpactPoint;
		}
		
		//UKismetSystemLibrary::DrawDebugArrow(this, Location + Direction * MinSpawnDistance, Location + Direction * MaxSpawnDistance, 1.f, FColor::Green, 1.f);
		//DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 15.f, 12, FColor::Red, false, 1.f);
		//DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 15.f, 12, FColor::Red, false, 1.f);
		//DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 15.f, 12, FColor::Red, false, 1.f);
		//DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 15.f, 12, FColor::Red, false, 1.f);

		SpawnLocations.Add(GeneratedLocation);
	}
	
	return SpawnLocations;
}
