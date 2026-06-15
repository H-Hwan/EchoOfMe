#pragma once

#include "CoreMinimal.h"
#include "Data/InventoryItemDefinition.h"
#include "RecorderItemDefinition.generated.h"


class UStorySequence;


UCLASS(BlueprintType)
class ECHOOFME_API URecorderItemDefinition : public UInventoryItemDefinition {
	GENERATED_BODY()

public:
	// 4단계 재생음 등록을 위한 배열
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recorder")
	TArray<TObjectPtr<USoundBase>> StageSounds;

	// 녹음기 회수 시 재생할 스토리 컷
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recorder")
	TObjectPtr<UStorySequence> CollectStory;
};
