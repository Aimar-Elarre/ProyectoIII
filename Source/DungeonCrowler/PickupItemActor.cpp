#include "PickupItemActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "MyPlayerCharacter.h"
#include "InventoryComponent.h"
#include "ItemData.h"

APickupItemActor::APickupItemActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);

	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Trigger);
}

void APickupItemActor::SetItemData(const UItemData* NewItemData)
{
	ItemData = NewItemData;
}

const UItemData* APickupItemActor::GetItemData() const
{
	return ItemData;
}

void APickupItemActor::BeginPlay()
{
	Super::BeginPlay();

	// Guardar la posición base para la oscilación
	BaseLocation = GetActorLocation();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &APickupItemActor::OnOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &APickupItemActor::OnOverlapEnd);
}

void APickupItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Oscilación de arriba hacia abajo
	OscillationTime += DeltaTime * OscillationSpeed;
	float OffsetZ = FMath::Sin(OscillationTime) * OscillationHeight;

	FVector NewLocation = BaseLocation;
	NewLocation.Z += OffsetZ;

	SetActorLocation(NewLocation);
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
	AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor);
	if (!Player) return;

	if (!Player->IsInventoryUnlocked())
	{
		return;
	}

	Player->SetNearbyPickup(this);
}

void APickupItemActor::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor);
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PICKUP] Cast a AMyPlayerCharacter falló. OtherActor: %s"), *OtherActor->GetClass()->GetName());
		return;
	}

	Player->ClearNearbyPickup(this);
}

void APickupItemActor::TryPickup(AMyPlayerCharacter* Player)
{
	if (!Player) return;
	if (!ItemData) return;
	if (!Player->InventoryComponent) return;
	if (!Player->IsInventoryUnlocked()) return;

	const bool bAdded = Player->InventoryComponent->AddItem(ItemData, 1);
	if (!bAdded)
	{
		UE_LOG(LogTemp, Warning, TEXT("No se pudo añadir al inventario"));
		return;
	}

	Player->ClearNearbyPickup(this);

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}

	UE_LOG(LogTemp, Warning, TEXT("Objeto recogido: %s"), *ItemData->DisplayName.ToString());
	// Añadir dinero del item al jugador
	if (ItemData && ItemData->MoneyValue > 0.f)
	{
		Player->AddMoney(ItemData->MoneyValue);
	}

	Destroy();
}