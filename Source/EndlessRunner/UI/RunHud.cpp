#include "UI/RunHud.h"
#include "EndlessRunner/EndlessRunnerCharacter.h"
#include "Components/TextBlock.h"


void URunHud::NativeConstruct()
{
	Super::NativeConstruct();

	if (AEndlessRunnerCharacter* Character = Cast<AEndlessRunnerCharacter>(GetOwningPlayerPawn()))
	{
		AddedCoinDelegateHandle = Character->GetOnAddedCoin().AddUObject(this, &ThisClass::OnAddedCoin);
	}
}

void URunHud::NativeDestruct()
{
	if (AEndlessRunnerCharacter* Character = Cast<AEndlessRunnerCharacter>(GetOwningPlayerPawn()))
	{
		Character->GetOnAddedCoin().Remove(AddedCoinDelegateHandle);
	}

	Super::NativeDestruct();
}

void URunHud::OnAddedCoin(int32 InNewCount, int32 InOldCount)
{
	TotalCoinText->SetText(FText::AsNumber(InNewCount));
}