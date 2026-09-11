#include "Actor/FloorTileCorner.h"
#include "EndlessRunner/EndlessRunnerCharacter.h"
#include "Components/BoxComponent.h"


AFloorTileCorner::AFloorTileCorner()
{
	CornerBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CornerBox"));
	CornerBoxComponent->SetupAttachment(RootSceneComponent);
}

void AFloorTileCorner::BeginPlay()
{
	Super::BeginPlay();

	if (AttachBoxComponent)
	{
		CornerBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AFloorTileCorner::OnCornerBoxBeginOverlap);
		CornerBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AFloorTileCorner::OnCornerBoxEndOverlap);
	}
}

void AFloorTileCorner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AttachBoxComponent)
	{
		CornerBoxComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AFloorTileCorner::OnCornerBoxBeginOverlap);
		CornerBoxComponent->OnComponentEndOverlap.RemoveDynamic(this, &AFloorTileCorner::OnCornerBoxEndOverlap);
	}

	Super::EndPlay(EndPlayReason);
}

void AFloorTileCorner::OnCornerBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AEndlessRunnerCharacter* EndlessRunnerCharacter = Cast<AEndlessRunnerCharacter>(OtherActor))
	{
		EndlessRunnerCharacter->SetCanTurn();
	}
}

void AFloorTileCorner::OnCornerBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AEndlessRunnerCharacter* EndlessRunnerCharacter = Cast<AEndlessRunnerCharacter>(OtherActor))
	{
		EndlessRunnerCharacter->ClearCanTurn();
	}
}