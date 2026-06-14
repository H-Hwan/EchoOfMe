// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/EndingDoorActor.h"

#include "EchoGameManager.h"


AEndingDoorActor::AEndingDoorActor() {
	/*	엔딩 문은 근접 자동개폐를 막고 상호작용으로만 처리
		(부모 OnProximityBegin은 bIsLocked면 자동 개폐를 건너뜀) */
	bIsLocked = true;
}


void AEndingDoorActor::Interact_Implementation(AActor* Interactor) {
	if (bEndingTriggered) return;

	// 대면 문인데 플래그 부족 — 문 안 열림, "아직은..." 연출은 BP에서
	if (EndingType == EEndingType::Facing && !CanEnterFacing()) {
		OnFacingLocked();
		UE_LOG(LogTemp, Log, TEXT("[EndingDoor] 대면 문 잠김 (플래그 부족)"));
		return;
	}

	TriggerEnding();
}


FText AEndingDoorActor::GetInteractionPrompt_Implementation() const {
	return bEndingTriggered ? FText::GetEmpty() : OpenPrompt;
}


bool AEndingDoorActor::CanEnterFacing() const {
	const UEchoGameManager* GM = UEchoGameManager::Get(this);
	if (!GM) return false;
	return GM->GetFacingFlagCount() >= RequiredFlagCount;
}


void AEndingDoorActor::TriggerEnding() {
	bEndingTriggered = true;

	// 문 물리적으로 열기 (부모 헬퍼)
	ForceOpen();

	// BP 연출 훅
	OnEndingChosen(EndingType);

	// 엔딩 페이즈 진입 — 추격 정지/연출은 OnLevelPhaseChanged 구독자가 처리
	if (UEchoGameManager* GM = UEchoGameManager::Get(this)) {
		GM->SetLevelPhase(ELevelPhase::Ending);
	}

	OnEndingDoorOpened.Broadcast(EndingType, EndingStory);

	UE_LOG(LogTemp, Log, TEXT("[EndingDoor] 엔딩 확정: %s"),
		EndingType == EEndingType::Escape ? TEXT("도망") : TEXT("대면"));
}
