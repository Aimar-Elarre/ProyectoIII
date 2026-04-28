#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MovingPlatformActor.generated.h"

UCLASS()
class DUNGEONCROWLER_API AMovingPlatformActor : public AActor
{
    GENERATED_BODY()

public:
    AMovingPlatformActor();

protected:
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override;

public:
    virtual void Tick(float DeltaTime) override;

    // MESH
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
    UStaticMeshComponent* PlatformMesh;

    // TRIGGER — activa cuando el jugador está encima
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
    UBoxComponent* StandTrigger;

    // TRIGGER — activa cuando el jugador está cerca
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
    UBoxComponent* ProximityTrigger;

    // DIRECCION — activa/desactiva cada eje
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform|Movement")
    bool bMoveX = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform|Movement")
    bool bMoveY = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform|Movement")
    bool bMoveZ = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform|Movement")
    float DirectionX = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform|Movement")
    float DirectionY = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform|Movement")
    float DirectionZ = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform|Movement")
    float MoveSpeed = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform|Movement")
    float MoveDistance = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform|Movement")
    float WaitTimeAtDestination = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform|Movement")
    bool bReturnToOrigin = true;

    // Tamaño del trigger de proximidad — editable y visible en editor
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform|Trigger")
    FVector ProximityTriggerSize = FVector(300.f, 300.f, 100.f);

    // Tamaño del trigger de encima — editable y visible en editor
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform|Trigger")
    FVector StandTriggerSize = FVector(150.f, 150.f, 50.f);

    // Offset del StandTrigger sobre la plataforma
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform|Trigger")
    FVector StandTriggerOffset = FVector(0.f, 0.f, 50.f);

private:
    FVector OriginLocation;
    FVector TargetLocation;

    bool bIsMoving = false;
    bool bMovingForward = true;
    bool bWaiting = false;
    bool bPlayerNearby = false;

    FTimerHandle WaitTimerHandle;

    void StartMoving();
    void StopWaiting();
    void UpdateTriggerSizes();

    UFUNCTION()
    void OnStandBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnStandEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION()
    void OnProximityBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnProximityEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};