// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPointActor.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "LudumDare54/Enemy/BaseEnemy.h"

// Sets default values
ASpawnPointActor::ASpawnPointActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	CollisionComponent->InitSphereRadius(100.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	SetRootComponent(CollisionComponent);

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASpawnPointActor::OnOverlap);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ASpawnPointActor::OnEndOverlap);
}

ABaseEnemy* ASpawnPointActor::Spawn(UClass* Monster)
{
	if (bIsBlocked || State != EWaveState::Ready || GetWorld() == nullptr) return nullptr;

	State = EWaveState::InSpawn;

	const auto Location = GetActorLocation();
	const auto Rotation = GetActorRotation();
	const auto Actor = Cast<ABaseEnemy>(GetWorld()->SpawnActor(Monster, &Location, &Rotation));
	if (Actor == nullptr) return nullptr;

	// FHitResult HitResult;
	// HitResult.ImpactPoint = Actor->GetActorLocation();
	// ProjectileFX->PlayFXAtPoint(HitResult);

	Cast<ABaseEnemy>(Actor)->GetMesh()->SetVisibility(true);
	State = EWaveState::Frozen;

	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASpawnPointActor::RemoveFreeze, FreezingTime);

	return Actor;
}

void ASpawnPointActor::OnOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	bIsBlocked = true;
}

void ASpawnPointActor::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int OtherBodyIndex)
{
	TArray<AActor*> OutOverlappingActors;
	GetOverlappingActors(OutOverlappingActors, {ACharacter::StaticClass()});
	if (OutOverlappingActors.Num() == 0 ) bIsBlocked = false;
}