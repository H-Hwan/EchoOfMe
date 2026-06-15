// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/DoorActor.h"
#include "EndingDoorActor.generated.h"


class UBoxComponent;
class UStoryPlayerComponent;


// 엔딩 종류
UENUM(BlueprintType)
enum class EEndingType : uint8 {
	Escape   UMETA(DisplayName = "도망 엔딩"),
	Facing   UMETA(DisplayName = "대면 엔딩")
};

// 엔딩 문이 열렸을 때 방송
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEndingDoorOpened, EEndingType, Ending, UStorySequence*, EndingStory);


UCLASS()
class ECHOOFME_API AEndingDoorActor : public ADoorActor
{
	GENERATED_BODY()

public:
	AEndingDoorActor();

	// [IInteractable] - 부모 Door의 잠금 해제/열림 처리 후 엔딩 발동
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

	UPROPERTY(BlueprintAssignable, Category = "Ending")
	FOnEndingDoorOpened OnEndingDoorOpened;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Door passage trigger. ProximityBox only controls the physical door.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ending")
	TObjectPtr<UBoxComponent> EndingTrigger;

	UPROPERTY(EditAnywhere, Category = "Ending")
	EEndingType EndingType = EEndingType::Facing;

	// Memory fragments required to change Facing from automatic to interactive.
	UPROPERTY(EditAnywhere, Category = "Ending", meta = (ClampMin = "0", ClampMax = "3"))
	int32 RequiredFlagCount = 3;

	UPROPERTY(EditAnywhere, Category = "Ending")
	TObjectPtr<UStorySequence> EndingStory;

	UPROPERTY(EditAnywhere, Category = "Ending")
	FName EndingLevelName;

	// Kept for compatibility with existing Blueprint assets.
	UPROPERTY(EditAnywhere, Category = "Ending")
	FText OpenPrompt;

	// Kept for compatibility with existing Blueprint assets.
	UFUNCTION(BlueprintImplementableEvent, Category = "Ending")
	void OnFacingLocked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ending")
	void OnEndingChosen(EEndingType Ending);

private:
	// Starts the ending after the player has passed through the opened door.
	UFUNCTION()
	void OnEndingTriggerOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	// Refresh the Facing door mode when a memory fragment is added.
	UFUNCTION()
	void HandleMemoryFlagAdded(FName FlagKey);

	// 엔딩 스토리 종료 수신 → 레벨 전환
	UFUNCTION()
	void HandleEndingStoryFinished();

	// Switch Facing between automatic and interactive door modes.
	void UpdateDoorMode();

	bool CanEnterFacing() const;

	// 엔딩 확정 처리
	void TriggerEnding();

	void OpenEndingLevel();
	UStoryPlayerComponent* GetStoryPlayer() const;

	// True only after the parent Door interaction actually unlocks this door.
	bool bDoorUnlockedForEnding = false;

	// 중복 발동 방지
	bool bEndingTriggered = false;
};
