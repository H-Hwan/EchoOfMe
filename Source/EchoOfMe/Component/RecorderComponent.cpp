// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/RecorderComponent.h"

#include "Component/InventoryComponent.h"
#include "Component/EquipmentComponent.h"
#include "Component/FlashlightComponent.h"
#include "Data/RecorderItemDefinition.h"
#include "EchoGameManager.h"
#include "Player/EchoPlayerController.h"

#include "Components/AudioComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"


URecorderComponent::URecorderComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}


void URecorderComponent::BeginPlay() {
	Super::BeginPlay();

	// 인벤토리 아이템 사용 메소드 바인딩
	if (UInventoryComponent* Inventory = GetOwner()->FindComponentByClass<UInventoryComponent>()) {
		Inventory->OnItemUsed.AddDynamic(this, &URecorderComponent::HandleItemUsed);
	}
}


void URecorderComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	ClearRecoverySequenceTimers();

	if (ActiveAudio) {
		ActiveAudio->OnAudioFinished.RemoveAll(this);
		ActiveAudio->Stop();
		ActiveAudio = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}


// 녹음기 픽업 시 호출
void URecorderComponent::HandleRecorderCollected(URecorderItemDefinition* Definition) {
	if (!Definition) return;

	// 회수 7초 시퀀스는 한 번만 작동해야 한다.
	if (bRecorderCollected) return;

	RecorderDefinition = Definition;
	bRecorderCollected = true;
	bHasRecorder = false;

	UE_LOG(LogTemp, Log, TEXT("[Recorder] 녹음기 회수: %s"), *Definition->ItemID.ToString());

	OnRecorderCollected.Broadcast(Definition);

	/*	[CP3 갱신]
		회수 후 잡혔을 때 회수 시퀀스가 반복되지 않도록,
		녹음기 회수 직후의 플레이어 위치를 리스폰 지점으로 저장	*/
	if (AEchoPlayerController* PC = Cast<AEchoPlayerController>(GetOwner())) {
		if (APawn* Pawn = PC->GetPawn()) {
			PC->SetRespawnTransform(Pawn->GetActorTransform());

			UE_LOG(LogTemp, Log, TEXT("[Recorder] CP3 저장 완료: %s"), *Pawn->GetActorLocation().ToString());
		}
	}

	StartRecoverySequence();
}


/*	[회수 7초 시퀀스 시작]
	시간표를 멤버 함수 포인터와 함께 배열로 묶어 한 번에 등록
	단계를 추가/조정할 때 이 배열만 수정하면 되고, 핸들 관리도 TArray 하나로 끝남	*/
void URecorderComponent::StartRecoverySequence() {
	if (bRecoverySequencePlaying) return;

	UWorld* World = GetWorld();
	if (!World) return;

	bRecoverySequencePlaying = true;

	// T+0.0 — 시퀀스 시작과 입력 잠금
	LockPlayerInput(true);
	OnRecoverySequenceStep.Broadcast(ERecorderRecoverySequenceStep::Started);

	// 시간표
	struct FRecoveryStep {
		float Time;
		void (URecorderComponent::*Action)();
	};

	const TArray<FRecoveryStep> Steps = {
		{ 1.0f, &URecorderComponent::PlayLowFrequencyCue },
		{ 2.0f, &URecorderComponent::PlayAwakeningVoiceCue },
		{ 4.0f, &URecorderComponent::ApplyColdLightingCue },
		{ 4.5f, &URecorderComponent::ForceFlashlightOff },
		{ 5.5f, &URecorderComponent::ForceFlashlightOn },
		{ 5.5f, &URecorderComponent::BroadcastHandAttachStart },
		{ MovementLockDuration, &URecorderComponent::UnlockPlayerInput },
		{ 6.0f, &URecorderComponent::BroadcastRecorderAttached },
		{ 6.0f, &URecorderComponent::PlayDistantFootstepCue },
		{ 6.5f, &URecorderComponent::BroadcastLCDOn },
		{ 6.5f, &URecorderComponent::BroadcastFootstepStopped },
		{ SequenceDuration, &URecorderComponent::FinishRecoverySequence },
	};

	// 핸들 배열 초기화 후 일괄 등록
	SequenceTimerHandles.Reset();
	SequenceTimerHandles.Reserve(Steps.Num());

	FTimerManager& TimerManager = World->GetTimerManager();
	for (const FRecoveryStep& Step : Steps) {
		FTimerHandle Handle;
		TimerManager.SetTimer(Handle, this, Step.Action, Step.Time, false);
		SequenceTimerHandles.Add(Handle);
	}
}


void URecorderComponent::FinishRecoverySequence() {
	bRecoverySequencePlaying = false;

	// 꼬임 방지 안전장치
	LockPlayerInput(false);

	// 이 시점부터 녹음기 기능과 듣기 액션을 사용 가능
	bHasRecorder = true;

	OnRecoverySequenceStep.Broadcast(ERecorderRecoverySequenceStep::Finished);

	/*	회수 시퀀스의 일부로 1회차 트랙 자동 재생
		이 자동 재생은 F4 플래그 카운트에 포함	*/
	PlayCurrentStage();

	UE_LOG(LogTemp, Log, TEXT("[Recorder] 회수 7초 시퀀스 완료 / 녹음기 기능 해금"));
}


void URecorderComponent::UnlockPlayerInput() {
	LockPlayerInput(false);
}


// 플레이어 컨트롤러에 이동/시점 입력 잠금 적용
void URecorderComponent::LockPlayerInput(bool bLocked) {
	if (APlayerController* PC = Cast<APlayerController>(GetOwner())) {
		PC->SetIgnoreMoveInput(bLocked);
		PC->SetIgnoreLookInput(bLocked);
	}
}


void URecorderComponent::PlayLowFrequencyCue() {
	if (LowFrequencySound) {
		UGameplayStatics::PlaySound2D(this, LowFrequencySound);
	}

	OnRecoverySequenceStep.Broadcast(ERecorderRecoverySequenceStep::LowFrequency);
}


void URecorderComponent::PlayAwakeningVoiceCue() {
	if (AwakeningVoiceSound) {
		UGameplayStatics::PlaySound2D(this, AwakeningVoiceSound);
	}

	OnRecoverySequenceStep.Broadcast(ERecorderRecoverySequenceStep::FirstVoice);
}


void URecorderComponent::ApplyColdLightingCue() {
	// 실제 조명 색온도 변경은 BP / Level Blueprint / Level Sequence에서 처리
	OnRecoverySequenceStep.Broadcast(ERecorderRecoverySequenceStep::ColdLighting);
}


void URecorderComponent::ForceFlashlightOff() {
	ForceFlashlight(false);
	OnRecoverySequenceStep.Broadcast(ERecorderRecoverySequenceStep::FlashlightOff);
}


void URecorderComponent::ForceFlashlightOn() {
	ForceFlashlight(true);
	OnRecoverySequenceStep.Broadcast(ERecorderRecoverySequenceStep::FlashlightOn);
}


void URecorderComponent::BroadcastHandAttachStart() {
	// BP에서 1인칭 왼손 장착 애니메이션 시작 처리
	OnRecoverySequenceStep.Broadcast(ERecorderRecoverySequenceStep::HandAttachStart);
}


void URecorderComponent::BroadcastRecorderAttached() {
	// BP에서 실제 왼손 녹음기 메시 표시 / 장착 완료 처리
	OnRecoverySequenceStep.Broadcast(ERecorderRecoverySequenceStep::RecorderAttached);
}


void URecorderComponent::BroadcastLCDOn() {
	// BP에서 LCD 점등, TRACK 1/4 표시 처리
	OnRecoverySequenceStep.Broadcast(ERecorderRecoverySequenceStep::LCDOn);
}


void URecorderComponent::PlayDistantFootstepCue() {
	if (DistantFootstepSound) {
		UGameplayStatics::PlaySound2D(this, DistantFootstepSound);
	}

	OnRecoverySequenceStep.Broadcast(ERecorderRecoverySequenceStep::Footstep);
}


void URecorderComponent::BroadcastFootstepStopped() {
	OnRecoverySequenceStep.Broadcast(ERecorderRecoverySequenceStep::FootstepStopped);
}


void URecorderComponent::ForceFlashlight(bool bOn) const {
	const APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn) return;

	if (bOn)
	{
		const UEquipmentComponent* Equipment = Pawn->FindComponentByClass<UEquipmentComponent>();
		if (!Equipment || Equipment->GetCurrentEquipment() != EEquipmentSlot::Flashlight)
		{
			return;
		}
	}

	if (UFlashlightComponent* Flashlight = Pawn->FindComponentByClass<UFlashlightComponent>()) {
		Flashlight->SetFlashLightOn(bOn);
	}
}


void URecorderComponent::ClearRecoverySequenceTimers() {
	UWorld* World = GetWorld();
	if (!World) return;

	FTimerManager& TimerManager = World->GetTimerManager();
	for (FTimerHandle& Handle : SequenceTimerHandles) {
		TimerManager.ClearTimer(Handle);
	}
	SequenceTimerHandles.Reset();
}


// 인벤토리에서 아이템 사용되면 호출
void URecorderComponent::HandleItemUsed(UInventoryItemDefinition* Item) {
	if (!Item) return;
	if (!RecorderDefinition) return;

	// 사용된 게 녹음기일 때만 재생
	if (Item != RecorderDefinition) return;

	// 회수 시퀀스 도중에는 수동 재생 불가
	if (bRecoverySequencePlaying) return;

	PlayCurrentStage();
}


// 현재 단계의 음성 재생
void URecorderComponent::PlayCurrentStage() {
	if (!bHasRecorder) return;
	if (!RecorderDefinition || RecorderDefinition->StageSounds.Num() == 0) return;

	// 이미 재생 중이라면 무시
	if (IsPlaying()) return;

	UEchoGameManager* GM = UEchoGameManager::Get(this);

	int32 Index = GM ? GM->GetRecorderPlaybackCount() : 0;

	/*	[잠금 해제 구조]
		- 1회차: 회수 시 자동 해제
		- 2회차 이후: 기억 플래그 수에 따라 해금	*/
	const int32 MemoryFlagCount = GM ? GM->GetMemoryFlagCount() : 0;
	const int32 UnlockedStageCount = 1 + MemoryFlagCount;

	if (Index >= UnlockedStageCount || Index >= RecorderDefinition->StageSounds.Num()) {
		// 잠긴 트랙은 카운트에서 제외 >> F4 플래그(3회차 이상 재생) 조건에 영향 안 줌
		PlayLockedNoise();
		return;
	}

	USoundBase* Sound = RecorderDefinition->StageSounds[Index];
	if (!Sound) return;

	ActiveAudio = UGameplayStatics::SpawnSound2D(this, Sound);
	if (ActiveAudio) {
		ActiveAudio->OnAudioFinished.AddDynamic(this, &URecorderComponent::HandleAudioFinished);
	}

	if (GM) {
		GM->IncrementRecorderPlayBack();
	}
	OnStagePlayed.Broadcast(Index);

	UE_LOG(LogTemp, Log, TEXT("[Recorder] TRACK %d/%d 재생"), Index + 1, RecorderDefinition->StageSounds.Num());
}


void URecorderComponent::PlayLockedNoise() {
	if (LockedNoiseSound) {
		UGameplayStatics::PlaySound2D(this, LockedNoiseSound);
	}

	UE_LOG(LogTemp, Log, TEXT("[Recorder] 잠긴 트랙 / 노이즈 재생"));
}


// 재생 중인 음성 중단
void URecorderComponent::StopPlayBack() {
	if (ActiveAudio) {
		ActiveAudio->Stop();
	}
}


// 재생 중 여부 반환
bool URecorderComponent::IsPlaying() const {
	return ActiveAudio && ActiveAudio->IsPlaying();
}


// 음성 재생이 끝났을 때 콜백
void URecorderComponent::HandleAudioFinished() {
	if (ActiveAudio) {
		ActiveAudio->OnAudioFinished.RemoveAll(this);
		ActiveAudio = nullptr;
	}

	OnPlaybackFinished.Broadcast();
}
