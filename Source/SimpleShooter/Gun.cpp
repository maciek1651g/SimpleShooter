// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"

#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGun::PullTrigger()
{
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, "MuzzleFlashSocket");

	APawn *OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
		return;
	AController *OwnerController = OwnerPawn->GetController();
	if (!OwnerController)
		return;

	FVector CameraLocation;
	FRotator CameraRotation;

	OwnerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector End = CameraLocation + CameraRotation.Vector() * MaxRange;
	FHitResult HitResult;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, End, ECollisionChannel::ECC_GameTraceChannel1))
	{
		FVector ShotDirection = -CameraRotation.Vector();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, HitResult.Location, ShotDirection.Rotation());

		AActor *HitActor = HitResult.GetActor();
		if (HitActor)
		{
			// strange notation, this is the same - FPointDamageEvent DamageEvent = FPointDamageEvent(Damage, HitResult, ShotDirection, nullptr);
			FPointDamageEvent DamageEvent(Damage, HitResult, ShotDirection, nullptr);
			HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
		}
	}
}
