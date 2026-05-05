#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "FireProjectile.generated.h"

UCLASS()
class DUNGEONCROWLER_API AFireProjectile : public AActor
{
    GENERATED_BODY()

public:
    AFireProjectile();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    USphereComponent* CollisionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    UStaticMeshComponent* ProjectileMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    UProjectileMovementComponent* ProjectileMovement;

    // Da�o que hace � se puede sobreescribir desde FireTrapActor
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    float Damage = 25.f;

    // Tiempo de vida antes de destruirse
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    float Lifetime = 3.f;

    // Velocidad del proyectil
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    float Speed = 800.f;

private:
UFUNCTION()
void OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult);
};