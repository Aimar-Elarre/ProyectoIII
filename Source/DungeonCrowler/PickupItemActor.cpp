#include "PickupItemActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MyPlayerCharacter.h"

APickupItemActor::APickupItemActor()
{
	PrimaryActorTick.bCanEverTick = false;

	//CREAR TRIGGER (ROOT)
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);

	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->SetGenerateOverlapEvents(true);

	//CREAR MESH
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Trigger);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APickupItemActor::BeginPlay()
{
	Super::BeginPlay();

	//ACTIVAR EVENTO OVERLAP
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &APickupItemActor::OnOverlapBegin);
}

void APickupItemActor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!OtherActor) return;

	//CAST AL PLAYER
	AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor);

	if (Player)
	{
		Player->AddItemsCarried(1);

		UE_LOG(LogTemp, Warning, TEXT("Objeto recogido. Total: %d"), Player->GetItemsCarried());

		//DESTRUIR OBJETO
		Destroy();
	}
}