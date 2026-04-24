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

	Trigger->SetBoxExtent(FVector(40.f, 40.f, 40.f));
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Trigger);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

	BaseLocation = GetActorLocation();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &APickupItemActor::OnOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &APickupItemActor::OnOverlapEnd);
}

void APickupItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsInPhysicsMode)
	{
		return;
	}

	OscillationTime += DeltaTime * OscillationSpeed;
	float OffsetZ = FMath::Sin(OscillationTime) * OscillationHeight;

	FVector NewLocation = BaseLocation;
	NewLocation.Z += OffsetZ;

	SetActorLocation(NewLocation);
}

void APickupItemActor::SpawnAsDropped(FVector LaunchVelocity)
{
	bIsInPhysicsMode = true;

	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Trigger->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Trigger->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	Trigger->SetSimulatePhysics(true);
	Trigger->SetPhysicsLinearVelocity(LaunchVelocity);

	// Tras 2.5 segundos el objeto habrá caído al suelo
	GetWorldTimerManager().SetTimer(
		PhysicsSettleTimer,
		this,
		&APickupItemActor::OnPhysicsSettle,
		2.5f,
		false
	);
}

void APickupItemActor::OnPhysicsSettle()
{
	bIsInPhysicsMode = false;

	Trigger->SetSimulatePhysics(false);
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);

	BaseLocation = GetActorLocation();
	OscillationTime = 0.f;
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
	if (bIsInPhysicsMode) return;

	AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor);
	if (!Player) return;

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
	if (bIsInPhysicsMode) return;

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

	if (ItemData->MoneyValue > 0.f)
	{
		Player->AddMoney(ItemData->MoneyValue);
	}

	Destroy();
}
