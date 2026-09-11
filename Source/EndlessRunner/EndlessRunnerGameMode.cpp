#include "EndlessRunnerGameMode.h"
#include "Engine/World.h"

#include "EndlessRunner/Actor/FloorTile.h"
#include "EndlessRunner/Actor/FloorTileCorner.h"
#include "EndlessRunner/UI/RunHud.h"


AEndlessRunnerGameMode::AEndlessRunnerGameMode()
{
	
}

void AEndlessRunnerGameMode::BeginPlay()
{
	Super::BeginPlay();

	ResetFloorTileCount();

	for (int32 Num = 0; Num < 10; ++Num)
	{
		AddFloorTile();
	}

	if (URunHud* RunHud = CreateWidget<URunHud>(GetWorld(), RunHudClass))
	{
		RunHud->AddToViewport();
	}
}

void AEndlessRunnerGameMode::AddFloorTile()
{
	if (CurrentStraightTileCount >= TargetStraightTileCount)
	{
		AddFloorCorner();
		ResetFloorTileCount();
	}
	else
	{
		AddFloorStraight();
		++CurrentStraightTileCount;
	}
}

void AEndlessRunnerGameMode::AddFloorStraight()
{
	if (StraightFloorTileClassArray.IsEmpty())
	{
		return;
	}

	const int32 RandTileIndex = FMath::RandRange(0, StraightFloorTileClassArray.Num() - 1);

	AFloorTile* SpawnendFloorTile = GetWorld()->SpawnActor<AFloorTile>(StraightFloorTileClassArray[RandTileIndex], NextSpawnPoint);
	if (SpawnendFloorTile)
	{
		NextSpawnPoint = SpawnendFloorTile->GetAttachTrasform();
	}
}

void AEndlessRunnerGameMode::AddFloorCorner()
{
	if (CornerTileClassArray.IsEmpty())
	{
		return;
	}

	const int32 RandTileIndex = FMath::RandRange(0, CornerTileClassArray.Num() - 1);

	AFloorTileCorner* SpawnendCorner = GetWorld()->SpawnActor<AFloorTileCorner>(CornerTileClassArray[RandTileIndex], NextSpawnPoint);
	if (SpawnendCorner)
	{
		NextSpawnPoint = SpawnendCorner->GetAttachTrasform();
	}
}

void AEndlessRunnerGameMode::ResetFloorTileCount()
{
	TargetStraightTileCount = FMath::RandRange(MinStraightTileCount, MaxStraightTileCount);
	CurrentStraightTileCount = 0;
}