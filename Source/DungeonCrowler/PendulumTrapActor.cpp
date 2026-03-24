#include "PendulumTrapActor.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"

APendulumTrapActor::APendulumTrapActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	SetRootComponent(Pivot);

	if (IsValid(Trigger))
	{
		Trigger.Get()->SetupAttachment(Pivot);
	}

	if (IsValid(Trigger))
	{
		Mesh.Get()->SetupAttachment(Trigger.Get());
	}
}

void APendulumTrapActor::BeginPlay()
{
	Super::BeginPlay();

	InitialPivotRotation = Pivot->GetRelativeRotation();
	RunningTime = 0.f;
}

void APendulumTrapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	// Swing is a sine wave: -1..1, scaled by SwingAngle
	float Angle = FMath::Sin((RunningTime * SwingSpeed) * 2.f * PI + FMath::DegreesToRadians(PhaseOffsetDegrees)) * SwingAngle;

	FRotator NewRot = InitialPivotRotation;
	switch (SwingAxis)
	{
	case EPendulumAxis::Pitch:
		NewRot.Pitch += Angle;
		break;
	case EPendulumAxis::Yaw:
		NewRot.Yaw += Angle;
		break;
	case EPendulumAxis::Roll:
		NewRot.Roll += Angle;
		break;
	}

	Pivot->SetRelativeRotation(NewRot);
}
