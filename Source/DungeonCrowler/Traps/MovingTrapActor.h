#pragma once

#include "CoreMinimal.h"
#include "TrapDamageActor.h"
#include "MovingTrapActor.generated.h"

UCLASS()
class DUNGEONCROWLER_API AMovingTrapActor : public ATrapDamageActor
{
	GENERATED_BODY()

public:
	AMovingTrapActor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Punto inicial
	FVector StartLocation;

	// Dirección actual
	int32 Direction = 1;

public:
	// Distancia máxima a cada lado
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MoveDistance = 300.f;

	// Velocidad de movimiento
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MoveSpeed = 150.f;

	// Eje de movimiento
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bMoveOnX = true;
};