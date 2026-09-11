#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EndlessRunnerGameMode.generated.h"

class URunHud;
class AFloorTile;
class AFloorTileCorner;

UCLASS(abstract)
class AEndlessRunnerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:	
	AEndlessRunnerGameMode();
	virtual void BeginPlay() override;

	void AddFloorTile();

protected:
	void AddFloorStraight();
	void AddFloorCorner();
	void ResetFloorTileCount();

private:
	UPROPERTY(EditDefaultsOnly, Category = "EndlessRunner")
	TSubclassOf<URunHud> RunHudClass;

	UPROPERTY(EditDefaultsOnly, Category = "EndlessRunner")
	TArray<TSubclassOf<AFloorTile>> StraightFloorTileClassArray;

	UPROPERTY(EditDefaultsOnly, Category = "EndlessRunner")
	TArray<TSubclassOf<AFloorTileCorner>> CornerTileClassArray;

	UPROPERTY(EditAnywhere, Category = "Floor Generation")
	int32 MinStraightTileCount = 5;

	UPROPERTY(EditAnywhere, Category = "Floor Generation")
	int32 MaxStraightTileCount = 10;

	int32 TargetStraightTileCount = 0;
	int32 CurrentStraightTileCount = 0;
	
	FTransform NextSpawnPoint;
};



