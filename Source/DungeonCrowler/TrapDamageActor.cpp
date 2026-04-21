#include "TrapDamageActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayerCharacter.h"

ATrapDamageActor::ATrapDamageActor()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);

	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Trigger);
}

void ATrapDamageActor::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ATrapDamageActor::OnOverlapBegin);
}

void ATrapDamageActor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor);

	if (Player)
	{
		Player->KillPlayer();

		if (TrapSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, TrapSound, GetActorLocation());
		}
	}
}