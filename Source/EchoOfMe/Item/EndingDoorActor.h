// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/DoorActor.h"
#include "EndingDoorActor.generated.h"


// 엔딩 종류
UENUM(BlueprintType)
enum class EEndingType : uint8 {
	Escape   UMETA(DisplayName = "도망 엔딩"),   // 출구 문 E
	Facing   UMETA(DisplayName = "대면 엔딩")    // 안쪽 방 문 D
};

// 엔딩 문이 열렸을 때 방송
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEndingDoorOpened, EEndingType, Ending, UStorySequence*, EndingStory);


UCLASS()
class ECHOOFME_API AEndingDoorActor : public ADoorActor
{
	GENERATED_BODY()

public:
	AEndingDoorActor();

	// [IInteractable] — 상호작용 시 엔딩 게이트 판정
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

	// 엔딩 확정(문 열림) 시 방송
	UPROPERTY(BlueprintAssignable, Category = "Ending")
	FOnEndingDoorOpened OnEndingDoorOpened;

protected:
	// 이 문이 향하는 엔딩
	UPROPERTY(EditAnywhere, Category = "Ending")
	EEndingType EndingType = EEndingType::Escape;

	// 대면 엔딩 진입에 필요한 플래그 수 (도망 문은 무시)
	UPROPERTY(EditAnywhere, Category = "Ending", meta = (ClampMin = "0", ClampMax = "4"))
	int32 RequiredFlagCount = 3;

	// 이 문에 연결된 엔딩 스토리 컷
	UPROPERTY(EditAnywhere, Category = "Ending")
	TObjectPtr<UStorySequence> EndingStory;

	// 상호작용 프롬프트 (비워두면 표시 안 함)
	UPROPERTY(EditAnywhere, Category = "Ending")
	FText OpenPrompt;

	/*	대면 문 플래그 부족 시 BP 연출 훅
		"문은 열리지 않는다. 아직은, 아무 소리도 들리지 않는다." 자막 + 출구 유도 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ending")
	void OnFacingLocked();

	// 엔딩 확정 시 BP 연출 훅 (필요 연출은 오케스트레이터/BP에서)
	UFUNCTION(BlueprintImplementableEvent, Category = "Ending")
	void OnEndingChosen(EEndingType Ending);

private:
	// 중복 발동 방지
	bool bEndingTriggered = false;

	// 대면 문 진입 가능 여부
	bool CanEnterFacing() const;

	// 엔딩 확정 처리 (문 열기 + 방송)
	void TriggerEnding();
};
