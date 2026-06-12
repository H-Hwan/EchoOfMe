#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MemoryComponent.generated.h"


class UMemoryFragmentDefinition;
class UAudioComponent;

// [회수 완료] >> 자막·연출
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryCollected, const UMemoryFragmentDefinition*, Definition);
// [회수음 종료] >> 스토리 컷 시작 신호
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryFlashbackFinished, const UMemoryFragmentDefinition*, Definition);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOOFME_API UMemoryComponent : public UActorComponent {
	GENERATED_BODY()

public:
		UMemoryComponent();


public:
	// 회수 직후 호출
	UFUNCTION(BlueprintCallable, Category = "Memory")
	void HandleMemoryCollected(const UMemoryFragmentDefinition* Definition);

	// 자막 표시 시간
	UPROPERTY(EditAnywhere, Category = "Memory")
	float DefaultSubtitleDuration = 3.f;

	// 회수 완료 방송
	UPROPERTY(BlueprintAssignable, Category = "Memory")
	FOnMemoryCollected OnMemoryCollected;

	// 기억조각 재생 완료 방송
	UPROPERTY(BlueprintAssignable, Category = "Memory")
	FOnMemoryFlashbackFinished OnFlashbackFinished;


private:
	// 현재 재생중인 사운드 >> 중복 재생 방지
	UPROPERTY()
	TObjectPtr<UAudioComponent> ActiveFlashback;

	// 사운드 종료 이벤트 바인딩
	UFUNCTION()
	void HandleFlashbackFinished();

	// 회수음 종료 시점에 어떤 조각이었는지 저장
	UPROPERTY()
	TObjectPtr<UMemoryFragmentDefinition> PendingStoryDefinition;
};
