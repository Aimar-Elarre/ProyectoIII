#include "PendulumTrapActor.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"

APendulumTrapActor::APendulumTrapActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Crear pivot como nueva raíz
	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	SetRootComponent(Pivot);

	// Recolocar el Trigger heredado debajo del Pivot
	if (Trigger)
	{
		Trigger->SetupAttachment(Pivot);
	}

	// El Mesh ya cuelga del Trigger en la clase padre
}

void APendulumTrapActor::BeginPlay()
{
	Super::BeginPlay();

	if (Pivot)
	{
		InitialPivotRotation = Pivot->GetRelativeRotation();
	}

	if (SwingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SwingSound, GetActorLocation());
	}
}

void APendulumTrapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Pivot) return;

	RunningTime += DeltaTime;

	const float PhaseRadians = FMath::DegreesToRadians(PhaseOffsetDegrees);
	const float Oscillation = FMath::Sin((RunningTime * SwingSpeed * 2.0f * PI) + PhaseRadians);
	const float CurrentAngle = Oscillation * SwingAngle;

	FRotator NewRotation = InitialPivotRotation;

	switch (SwingAxis)
	{
	case EPendulumAxis::Pitch:
		NewRotation.Pitch += CurrentAngle;
		break;

	case EPendulumAxis::Yaw:
		NewRotation.Yaw += CurrentAngle;
		break;

	case EPendulumAxis::Roll:
		NewRotation.Roll += CurrentAngle;
		break;

	default:
		break;
	}

	Pivot->SetRelativeRotation(NewRotation);
}