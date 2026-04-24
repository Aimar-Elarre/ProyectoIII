#include "FireProjectile.h"
#include "MyPlayerCharacter.h"
#include "TimerManager.h"

AFireProjectile::AFireProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    SetRootComponent(CollisionSphere);
    CollisionSphere->SetSphereRadius(20.f);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Block);
    CollisionSphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMesh->SetupAttachment(CollisionSphere);
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 800.f;
    ProjectileMovement->MaxSpeed = 800.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AFireProjectile::BeginPlay()
{
    Super::BeginPlay();

    ProjectileMovement->InitialSpeed = Speed;
    ProjectileMovement->MaxSpeed = Speed;

    CollisionSphere->OnComponentHit.AddDynamic(this, &AFireProjectile::OnHit);

    // Leer daño desde el tag si lo tiene
    for (const FName& Tag : Tags)
    {
        FString TagStr = Tag.ToString();
        if (TagStr.StartsWith(TEXT("FireDamage_")))
        {
            FString DamageStr = TagStr.RightChop(11);
            Damage = FCString::Atof(*DamageStr);
            break;
        }
    }

    // Autodestruirse después de Lifetime segundos
    SetLifeSpan(Lifetime);
}

void AFireProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse,
    const FHitResult& Hit)
{
    if (!OtherActor) return;

    if (AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor))
    {
        Player->TakeDamageCustom(Damage);
    }

    Destroy();
}