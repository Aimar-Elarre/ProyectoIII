#include "MovingTrapActor.h"

AMovingTrapActor::AMovingTrapActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMovingTrapActor::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
}

void AMovingTrapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = CurrentLocation;

	if (bMoveOnX)
	{
		NewLocation.X += Direction * MoveSpeed * DeltaTime;

		if (FMath::Abs(NewLocation.X - StartLocation.X) >= MoveDistance)
		{
			Direction *= -1;
		}
	}
	else
	{
		NewLocation.Y += Direction * MoveSpeed * DeltaTime;

		if (FMath::Abs(NewLocation.Y - StartLocation.Y) >= MoveDistance)
		{
			Direction *= -1;
		}
	}

	SetActorLocation(NewLocation);
}