
#include "PickupItemActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "InventoryComponent.h"

APickupItemActor::APickupItemActor()
{
    PrimaryActorTick.bCanEverTick = false;

    Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
    SetRootComponent(Trigger);
    Trigger->InitSphereRadius(80.f);
    Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Trigger->OnComponentBeginOverlap.AddDynamic(this, &APickupItemActor::OnOverlapBegin);
}

void APickupItemActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!ItemData) return;

    ACharacter* Char = Cast<ACharacter>(OtherActor);
    if (!Char) return;

    UInventoryComponent* Inv = Char->FindComponentByClass<UInventoryComponent>();
    if (!Inv) return;

    if (Inv->AddItem(ItemData, Quantity))
    {
        Destroy();
    }
}