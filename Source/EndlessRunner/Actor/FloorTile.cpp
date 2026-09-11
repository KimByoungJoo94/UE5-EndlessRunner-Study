#include "Actor/FloorTile.h"

#include "EndlessRunner/EndlessRunnerGameMode.h"
#include "EndlessRunner/EndlessRunnerCharacter.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"


AFloorTile::AFloorTile()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootSceneComponent);

	AttachArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("AttachArrow"));
	AttachArrowComponent->SetupAttachment(RootSceneComponent);

	AttachBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("AttachBox"));
	AttachBoxComponent->SetupAttachment(RootSceneComponent);

	CoinAreaComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CoinAreaBox"));
	CoinAreaComponent->SetupAttachment(RootSceneComponent);

	PrimaryActorTick.bCanEverTick = false;
}

void AFloorTile::BeginPlay()
{
	Super::BeginPlay();

	if (AttachBoxComponent)
	{
		AttachBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AFloorTile::OnAttachBoxBeginOverlap);
	}

	const TArray<UActorComponent*> ComponentArray = GetComponentsByTag(UStaticMeshComponent::StaticClass(), TEXT("HitWall"));
	for (UActorComponent* Component : ComponentArray)
	{
		if (UStaticMeshComponent* WallComponent = Cast<UStaticMeshComponent>(Component))
		{
			WallComponent->OnComponentHit.AddDynamic(this, &AFloorTile::OnWallHit);
		}
	}

	GenerateSpawnPointArray();

	DestroyAllSpawnedActor();
	SpawnByContentType();
}

void AFloorTile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AttachBoxComponent)
	{
		AttachBoxComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AFloorTile::OnAttachBoxBeginOverlap);
	}

	const TArray<UActorComponent*> ComponentArray = GetComponentsByTag(UStaticMeshComponent::StaticClass(), TEXT("HitWall"));
	for (UActorComponent* Component : ComponentArray)
	{
		if (UStaticMeshComponent* WallComponent = Cast<UStaticMeshComponent>(Component))
		{
			WallComponent->OnComponentHit.RemoveDynamic(this, &AFloorTile::OnWallHit);
		}
	}

	DestroyAllSpawnedActor();

	Super::EndPlay(EndPlayReason);
}

void AFloorTile::GenerateSpawnPointArray()
{
	if (SpawnTransformArray.IsEmpty() == false)
	{
		return;
	}

	const TArray<UActorComponent*> ComponentArray = GetComponentsByTag(UArrowComponent::StaticClass(), TEXT("SpawnPoint"));
	for (UActorComponent* Component : ComponentArray)
	{
		if (UArrowComponent* ArrowComponent = Cast<UArrowComponent>(Component))
		{
			SpawnTransformArray.Add(ArrowComponent->GetComponentTransform());
		}
	}
}

void AFloorTile::OnAttachBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AEndlessRunnerCharacter>(OtherActor))
	{
		if (UWorld* World = GetWorld())
		{
			if (AEndlessRunnerGameMode* AuthGameMode = Cast<AEndlessRunnerGameMode>(World->GetAuthGameMode()))
			{
				AuthGameMode->AddFloorTile();
				SetLifeSpan(2.0f);
			}
		}
	}
}

void AFloorTile::OnWallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (AEndlessRunnerCharacter* EndlessRunnerCharacter = Cast<AEndlessRunnerCharacter>(OtherActor))
	{
		EndlessRunnerCharacter->DoDeath();
	}
}

const FTransform& AFloorTile::GetAttachTrasform()
{
	if (AttachArrowComponent)
	{
		return AttachArrowComponent->GetComponentTransform();
	}

	return FTransform::Identity;
}

void AFloorTile::SpawnByContentType()
{
	switch (FloorTileContentsType)
	{	
	case EFloorTileContentsType::Random:
		FMath::RandRange(0, 1) > 0 ? SpawnBlocker() : SpawnCoin();
		break;
	case EFloorTileContentsType::Blocker:
		SpawnBlocker();
		break;
	case EFloorTileContentsType::Coin:
		SpawnCoin();
		break;	
	}
}

void AFloorTile::SpawnBlocker()
{
	if (SpawnTransformArray.IsEmpty() || BlockerActorClass == nullptr)
	{
		return;
	}

	if (SpawnedBlocker)
	{
		SpawnedBlocker->Destroy();
		SpawnedBlocker = nullptr;
	}

	const int32 RandIndex = FMath::RandRange(0, SpawnTransformArray.Num() - 1);
	if (SpawnTransformArray.IsValidIndex(RandIndex))
	{
		SpawnedBlocker = GetWorld()->SpawnActor<AActor>(BlockerActorClass, SpawnTransformArray[RandIndex]);
	}
}

void AFloorTile::SpawnCoin()
{
	if (CoinActorClass == nullptr || CoinAreaComponent == nullptr)
	{
		return;
	}

	if (SpawnedCoinArray.IsEmpty() == false)
	{
		for (AActor* SpawnedCoin : SpawnedCoinArray)
		{
			SpawnedCoin->Destroy();
		}

		SpawnedCoinArray.Empty();
	}

	const int32 CoinNum = FMath::RandRange(3, 8);
	for (int32 Num = 1; Num <= CoinNum; ++Num)
	{
		const FVector& Location = CoinAreaComponent->GetComponentLocation();
		const FVector& BoxSize = CoinAreaComponent->GetScaledBoxExtent();
		const FVector RandPoint = FMath::RandPointInBox(FBox(Location - BoxSize, Location + BoxSize));

		if (AActor* SpawnedCoin = GetWorld()->SpawnActor<AActor>(CoinActorClass, RandPoint, FRotator::ZeroRotator))
		{
			SpawnedCoinArray.Add(SpawnedCoin);
		}
	}
}

void AFloorTile::DestroyAllSpawnedActor()
{
	auto InnerDestroy = [](TObjectPtr<AActor>& InActor)
		{
			if (IsValid(InActor))
			{
				InActor->Destroy();
				InActor = nullptr;
			}
		};

	InnerDestroy(SpawnedBlocker);

	for (TObjectPtr<AActor>& SpawnedCoin : SpawnedCoinArray)
	{
		InnerDestroy(SpawnedCoin);
	}

	SpawnedCoinArray.Empty();
}