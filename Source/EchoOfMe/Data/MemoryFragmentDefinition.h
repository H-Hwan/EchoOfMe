#pragma once

#include "CoreMinimal.h"
#include "Data/InventoryItemDefinition.h"
#include "MemoryFragmentDefinition.generated.h"


class USoundBase;
class UStorySequence;


UCLASS(BlueprintType)
class ECHOOFME_API UMemoryFragmentDefinition : public UInventoryItemDefinition {
	GENERATED_BODY()

public:
	// 회수 시 재생할 음성
	UPROPERTY(EditDefaultsOnly, Category = "Memory")
	TObjectPtr<USoundBase> FlashbackSound;

	// 자막 텍스트
	UPROPERTY(EditDefaultsOnly, Category = "Memory", meta = (MultiLine = true))
	FText FlashbackText;

	// 해당 조각으로 켜지는 플래그 키
	UPROPERTY(EditDefaultsOnly, Category = "Memory")
	FName FlagKey;

	// 회수음 종료 후 재생할 스토리 컷
	UPROPERTY(EditDefaultsOnly, Category = "Memory")
	TObjectPtr<UStorySequence> Story;
};
