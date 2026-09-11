// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/FloorTile.h"
#include "FloorTileCorner.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API AFloorTileCorner : public AFloorTile
{
	GENERATED_BODY()
	
public:
	AFloorTileCorner();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnCornerBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCornerBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "EndlessRunner")
	TObjectPtr<UBoxComponent> CornerBoxComponent;
};
