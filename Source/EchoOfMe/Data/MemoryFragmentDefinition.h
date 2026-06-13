#pragma once

#include "CoreMinimal.h"
#include "Data/InventoryItemDefinition.h"
#include "MemoryFragmentDefinition.generated.h"


class USoundBase;
class USoundAttenuation;
class UStorySequence;


UCLASS(BlueprintType)
class ECHOOFME_API UMemoryFragmentDefinition : public UInventoryItemDefinition {
	GENERATED_BODY()

public:
	// 평상시 재생되는 기억 조각의 공간음
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Memory|Cue")
	TObjectPtr<USoundBase> CueAmbientSound;

	// 듣기 모드에서 평상시 공간음을 대체하는 소리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Memory|Cue")
	TObjectPtr<USoundBase> CueListeningSound;

	// 비어 있으면 사운드 자산 자체의 감쇠 설정을 사용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Memory|Cue")
	TObjectPtr<USoundAttenuation> CueAttenuation;

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
