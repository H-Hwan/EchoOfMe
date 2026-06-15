// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/EndingDoorActor.h"

#include "EchoGameManager.h"
#include "Component/StoryPlayerComponent.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"


AEndingDoorActor::AEndingDoorActor() {
	// Escape stays interactive. Facing selects its mode in BeginPlay.
	bIsLocked = true;

	EndingTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("EndingTrigger"));
	EndingTrigger->SetupAttachment(RootComponent);
	EndingTrigger->SetCollisionProfileName(TEXT("Trigger"));
	EndingTrigger->SetGenerateOverlapEvents(true);
	EndingTrigger->SetBoxExtent(FVector(80.f, 80.f, 100.f));
}


void AEndingDoorActor::BeginPlay() {
	Super::BeginPlay();

	if (EndingTrigger) {
		EndingTrigger->OnComponentBeginOverlap.AddDynamic(
			this,
			&AEndingDoorActor::OnEndingTriggerOverlap);
	}

	if (EndingType == EEndingType::Facing) {
		if (UEchoGameManager* GM = UEchoGameManager::Get(this)) {
			GM->OnMemoryFlagAdded.AddDynamic(this, &AEndingDoorActor::HandleMemoryFlagAdded);
		}

		// Also handles a level that starts with previously collected fragments.
		UpdateDoorMode();
	}
}


void AEndingDoorActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (EndingTrigger) {
		EndingTrigger->OnComponentBeginOverlap.RemoveDynamic(
			this,
			&AEndingDoorActor::OnEndingTriggerOverlap);
	}

	if (UEchoGameManager* GM = UEchoGameManager::Get(this)) {
		GM->OnMemoryFlagAdded.RemoveDynamic(this, &AEndingDoorActor::HandleMemoryFlagAdded);
	}

	Super::EndPlay(EndPlayReason);
}


void AEndingDoorActor::HandleMemoryFlagAdded(FName /*FlagKey*/) {
	UpdateDoorMode();
}


void AEndingDoorActor::UpdateDoorMode() {
	if (bEndingTriggered || EndingType != EEndingType::Facing) return;

	const UEchoGameManager* GM = UEchoGameManager::Get(this);
	const int32 Count = GM ? GM->GetMemoryFlagCount() : 0;
	const bool bReady = (Count >= RequiredFlagCount);

	// Below the requirement: automatic. At the requirement: locked and interactive.
	bIsLocked = bReady;

	UE_LOG(LogTemp, Log, TEXT("[EndingDoor] Facing mode: memory fragments %d/%d, %s"),
		Count,
		RequiredFlagCount,
		bReady ? TEXT("interactive") : TEXT("automatic"));
}


void AEndingDoorActor::Interact_Implementation(AActor* Interactor) {
	if (bEndingTriggered) return;

	// Facing becomes interactive only after collecting enough fragments.
	if (EndingType == EEndingType::Facing && !CanEnterFacing()) return;

	// Interaction only opens the physical door. The passage trigger starts the ending.
	const bool bWasLocked = bIsLocked;
	Super::Interact_Implementation(Interactor);
	if (bWasLocked && !bIsLocked) {
		bDoorUnlockedForEnding = true;
	}
}


FText AEndingDoorActor::GetInteractionPrompt_Implementation() const {
	if (bEndingTriggered) return FText::GetEmpty();
	if (EndingType == EEndingType::Facing && !CanEnterFacing()) return FText::GetEmpty();

	return Super::GetInteractionPrompt_Implementation();
}


bool AEndingDoorActor::CanEnterFacing() const {
	const UEchoGameManager* GM = UEchoGameManager::Get(this);
	return GM && GM->GetMemoryFlagCount() >= RequiredFlagCount;
}


void AEndingDoorActor::OnEndingTriggerOverlap(
	UPrimitiveComponent* /*OverlappedComp*/,
	AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/,
	int32 /*OtherBodyIndex*/,
	bool /*bFromSweep*/,
	const FHitResult& /*SweepResult*/) {

	if (bEndingTriggered) return;
	if (!bDoorUnlockedForEnding) return;

	const APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn || !Pawn->IsPlayerControlled()) return;

	// Facing may be crossed as an automatic door before all fragments are collected.
	if (EndingType == EEndingType::Facing && !CanEnterFacing()) return;

	TriggerEnding();
}


void AEndingDoorActor::TriggerEnding() {
	if (EndingType == EEndingType::Facing && !CanEnterFacing()) {
		UE_LOG(LogTemp, Warning, TEXT("[EndingDoor] Facing ending blocked: insufficient memory fragments"));
		return;
	}

	bEndingTriggered = true;

	OnEndingChosen(EndingType);

	if (UEchoGameManager* GM = UEchoGameManager::Get(this)) {
		GM->SetLevelPhase(ELevelPhase::Ending);
	}

	// 스토리 종료 후 레벨 전환을 위해 미리 구독
	if (UStoryPlayerComponent* StoryPlayer = GetStoryPlayer()) {
		StoryPlayer->OnStoryFinished.AddDynamic(this, &AEndingDoorActor::HandleEndingStoryFinished);
	}

	// 스토리 먼저 재생 (StoryPlayer가 OnEndingDoorOpened 구독 → PlayStory)
	OnEndingDoorOpened.Broadcast(EndingType, EndingStory);

	// 스토리가 없으면 바로 레벨 전환
	if (!EndingStory) {
		HandleEndingStoryFinished();
	}

	UE_LOG(LogTemp, Log, TEXT("[EndingDoor] 엔딩 확정: %s"),
		EndingType == EEndingType::Escape ? TEXT("도망") : TEXT("대면"));
}


void AEndingDoorActor::HandleEndingStoryFinished() {
	if (UStoryPlayerComponent* StoryPlayer = GetStoryPlayer()) {
		StoryPlayer->OnStoryFinished.RemoveDynamic(this, &AEndingDoorActor::HandleEndingStoryFinished);
	}

	OpenEndingLevel();
}


void AEndingDoorActor::OpenEndingLevel() {
	if (EndingLevelName.IsNone()) {
		UE_LOG(LogTemp, Warning, TEXT("[EndingDoor] EndingLevelName 미설정 - 레벨 전환 생략"));
		return;
	}

	UGameplayStatics::OpenLevel(this, EndingLevelName);
}


UStoryPlayerComponent* AEndingDoorActor::GetStoryPlayer() const {
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	return PC ? PC->FindComponentByClass<UStoryPlayerComponent>() : nullptr;
}
