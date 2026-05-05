#pragma once

#include "CoreMinimal.h"
#include "TrapDamageActor.h"
#include "Sound/SoundBase.h"
#include "PendulumTrapActor.generated.h"

class USceneComponent;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
	TObjectPtr<USceneComponent> Pivot;

	FRotator InitialPivotRotation;
	float RunningTime = 0.f;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pendulum")
	float SwingAngle = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pendulum", meta = (ClampMin = "0.01"))
	float SwingSpeed = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pendulum")
	EPendulumAxis SwingAxis = EPendulumAxis::Pitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pendulum")
	float PhaseOffsetDegrees = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* SwingSound = nullptr;
};