#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FireTrapActor.generated.h"

class AFireProjectile;

UCLASS()
class DUNGEONCROWLER_API AFireTrapActor : public AActor
{
    GENERATED_BODY()

public:
    AFireTrapActor();

protected:
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override;

public:
    virtual void Tick(float DeltaTime) override;

    // MESH de la trampa
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
    UStaticMeshComponent* TrapMesh;

    // Zona de activación — cuando el jugador entra, empieza a disparar
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
    UBoxComponent* ActivationTrigger;

    // Punto desde donde sale el fuego
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
    UStaticMeshComponent* FireSpawnPoint;

    // Clase del proyectil de fuego — asigna en el Blueprint
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap|Fire")
    TSubclassOf<AActor> FireProjectileClass;

    // Intervalo entre disparos (segundos)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap|Fire")
    float FireInterval = 1.5f;

    // Velocidad del proyectil
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap|Fire")
    float ProjectileSpeed = 800.f;

    // Daño que hace al jugador
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap|Fire")
    float FireDamage = 25.f;

    // Dirección de disparo (se puede rotar el actor en el editor)
    // Si bUseCustomDirection es false, usa el forward vector del actor
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap|Fire")
    bool bUseCustomDirection = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap|Fire", meta = (EditCondition = "bUseCustomDirection"))
    FRotator CustomFireDirection = FRotator::ZeroRotator;

    // Tamaño de la zona de activación
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap|Trigger")
    FVector ActivationTriggerSize = FVector(400.f, 400.f, 150.f);

    // Offset del punto de spawn del fuego
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap|Fire")
    FVector FireSpawnOffset = FVector(100.f, 0.f, 0.f);

private:
    bool bPlayerInRange = false;
    FTimerHandle FireTimerHandle;

    void StartFiring();
    void StopFiring();
    void SpawnFireProjectile();
    void UpdateTriggerSize();

    UFUNCTION()
    void OnActivationBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnActivationEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};