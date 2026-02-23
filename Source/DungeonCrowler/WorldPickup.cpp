#include "WorldPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "InventoryComponent.h"

AWorldPickup::AWorldPickup()
{
    PrimaryActorTick.bCanEverTick = false;

    Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
    RootComponent = Sphere;
    Sphere->InitSphereRadius(90.f);
    Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AWorldPickup::OnSphereBeginOverlap);
}

void AWorldPickup::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep)
{
    if (!ItemData || Quantity <= 0) return;

    ACharacter* Char = Cast<ACharacter>(OtherActor);
    if (!Char) return;

    UInventoryComponent* Inv = Char->FindComponentByClass<UInventoryComponent>();
    if (!Inv) return;

    if (Inv->AddItem(ItemData, Quantity))
    {
        Destroy();
    }
    // Si no cabe por peso, no se destruye.
}