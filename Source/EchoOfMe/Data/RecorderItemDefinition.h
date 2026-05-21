// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/InventoryItemDefinition.h"
#include "RecorderItemDefinition.generated.h"


UCLASS(BlueprintType)
class ECHOOFME_API URecorderItemDefinition : public UInventoryItemDefinition {
	GENERATED_BODY()

public:
	// 4단계 재생음 등록을 위한 배열
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recorder")
	TArray<TObjectPtr<USoundBase>> StageSound;	
};
