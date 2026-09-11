// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorTile.generated.h"

class USceneComponent;
class UArrowComponent;
class UBoxComponent;

UENUM(BlueprintType)
enum class EFloorTileContentsType : uint8
{	
	None,
	Random,
	Blocker,
	Coin
};

UCLASS()
class ENDLESSRUNNER_API AFloorTile : public AActor
{
	GENERATED_BODY()
	
public:	
	AFloorTile();

	const FTransform& GetAttachTrasform();

protected:		
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void GenerateSpawnPointArray();
	
	UFUNCTION()
	void OnAttachBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnWallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit); 

	void SpawnByContentType();
	void SpawnBlocker();
	void SpawnCoin();
	void DestroyAllSpawnedActor();
	
	FORCEINLINE void SetFloorTileContentsType(EFloorTileContentsType InType) { FloorTileContentsType = InType; }
	FORCEINLINE EFloorTileContentsType GetFloorTileContentsType() const { return FloorTileContentsType; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "EndlessRunner")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(EditDefaultsOnly, Category = "EndlessRunner")
	TObjectPtr<UArrowComponent> AttachArrowComponent;

	UPROPERTY(EditDefaultsOnly, Category = "EndlessRunner")
	TObjectPtr<UBoxComponent> AttachBoxComponent;

	UPROPERTY(EditDefaultsOnly, Category = "EndlessRunner")
	TObjectPtr<UBoxComponent> CoinAreaComponent;

	UPROPERTY(EditDefaultsOnly, Category = "EndlessRunner")
	TSubclassOf<AActor> BlockerActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "EndlessRunner")
	TSubclassOf<AActor> CoinActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EndlessRunner")
	EFloorTileContentsType FloorTileContentsType;

	UPROPERTY(Transient)
	TObjectPtr<AActor> SpawnedBlocker;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> SpawnedCoinArray;
	
	TArray<FTransform> SpawnTransformArray;
};
