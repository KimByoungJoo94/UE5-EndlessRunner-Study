// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RunHud.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSRUNNER_API URunHud : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void OnAddedCoin(int32 InNewCount, int32 InOldCount);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TotalCoinText;

	FDelegateHandle AddedCoinDelegateHandle;
};
