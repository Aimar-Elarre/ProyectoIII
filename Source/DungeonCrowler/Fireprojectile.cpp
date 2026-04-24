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
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionSphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    CollisionSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    CollisionSphere->SetMobility(EComponentMobility::Movable);

    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMesh->SetupAttachment(CollisionSphere);
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 0.f;
    ProjectileMovement->MaxSpeed = 2000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AFireProjectile::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("FireProjectile spawned, speed: %f"), Speed);

    ProjectileMovement->SetUpdatedComponent(CollisionSphere);
    ProjectileMovement->Velocity = -GetActorForwardVector() * Speed;
    ProjectileMovement->InitialSpeed = Speed;
    ProjectileMovement->MaxSpeed = Speed;
    ProjectileMovement->Activate();

    UE_LOG(LogTemp, Warning, TEXT("Proyectil forward vector: X=%f Y=%f Z=%f"), 
    GetActorForwardVector().X, 
    GetActorForwardVector().Y, 
    GetActorForwardVector().Z);

    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AFireProjectile::OnProjectileOverlap);

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

    SetLifeSpan(Lifetime);
}

void AFireProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("Proyectil overlap con: %s"), OtherActor ? *OtherActor->GetName() : TEXT("null"));

    if (!OtherActor || OtherActor == this || OtherActor == GetOwner()) return;

    if (AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor))
    {
        Player->TakeDamageCustom(Damage);
        Destroy();
    }
}