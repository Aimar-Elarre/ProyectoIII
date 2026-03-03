// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapDamageActor.h"

// Sets default values
ATrapDamageActor::ATrapDamageActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATrapDamageActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATrapDamageActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

