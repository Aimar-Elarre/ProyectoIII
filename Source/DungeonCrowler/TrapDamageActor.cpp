#include "TrapDamageActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ATrapDamageActor::ATrapDamageActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Trigger);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ATrapDamageActor::OnTriggerBegin);
}

void ATrapDamageActor::OnTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
 	if (!OtherActor) return;

	// Aplica daño estandar al actor que entró en el trigger
	UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, nullptr);

	if (bDestroyOnHit)
	{
		Destroy();
	}
}