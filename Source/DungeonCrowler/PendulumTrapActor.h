#pragma once

#include "CoreMinimal.h"
#include "TrapDamageActor.h"
#include "PendulumTrapActor.generated.h"

UENUM(BlueprintType)
enum class EPendulumAxis : uint8
{
	Pitch UMETA(DisplayName = "Pitch"),
	Yaw UMETA(DisplayName = "Yaw"),
	Roll UMETA(DisplayName = "Roll")
};

UCLASS()
class DUNGEONCROWLER_API APendulumTrapActor : public ATrapDamageActor
{
	GENERATED_BODY()

public:
	APendulumTrapActor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Pivot component used as the rotation point for the swinging motion
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
	TObjectPtr<USceneComponent> Pivot;

	// Cached rotation when play begins
	FRotator InitialPivotRotation;

	// Time accumulator for the oscillation
	float RunningTime = 0.f;

public:
	// Maximum swing angle from rest (degrees)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pendulum")
	float SwingAngle = 45.f;

	// Swing speed in cycles per second (Hz)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pendulum", meta = (ClampMin = "0.01"))
	float SwingSpeed = 1.f;

	// Axis around which the pendulum swings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pendulum")
	EPendulumAxis SwingAxis = EPendulumAxis::Pitch;

	// Phase offset for the oscillation (degrees)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pendulum")
	float PhaseOffsetDegrees = 0.f;
};
