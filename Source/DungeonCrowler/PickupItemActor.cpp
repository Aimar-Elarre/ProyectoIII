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
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);

	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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

	AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor);
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PICKUP] Cast a AMyPlayerCharacter falló. OtherActor: %s"), *OtherActor->GetClass()->GetName());
		return;
	}

	// ===== BREAKPOINT: Aquí entra cuando el jugador toca un pickup =====
	if (ItemData && Player->InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PICKUP] Item tocado: %s"), *ItemData->GetName());
		
		bool bAdded = Player->InventoryComponent->AddItem(ItemData, 1);
		if (!bAdded)
		{
			UE_LOG(LogTemp, Warning, TEXT("[PICKUP] ¡No se pudo añadir al inventario! Peso máximo alcanzado"));
			return;
		}
		
		UE_LOG(LogTemp, Warning, TEXT("[PICKUP] Item añadido al inventario correctamente"));
	}

	Player->AddCarriedItem(1);

	// Añadir dinero del item al jugador
	if (ItemData && ItemData->MoneyValue > 0.f)
	{
		Player->AddMoney(ItemData->MoneyValue);
		
		// ===== BREAKPOINT: Confirmar adición de dinero =====
		float NewMoney = Player->GetCurrentMoney();
		UE_LOG(LogTemp, Warning, TEXT("[PICKUP] Dinero añadido: +%.2f | Dinero total ahora: %.2f"), 
			ItemData->MoneyValue, NewMoney);
	}
	else if (ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PICKUP] Item sin valor monetario: %s (MoneyValue = %.2f)"), 
			*ItemData->GetName(), ItemData ? ItemData->MoneyValue : 0.0f);
	}

	UE_LOG(LogTemp, Warning, TEXT("Objeto recogido. Total: %d"), Player->GetItemsCarried());

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}

	Destroy();
}