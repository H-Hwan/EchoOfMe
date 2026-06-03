// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerManager.h"
#include "RecorderComponent.generated.h"


class UInventoryItemDefinition;
class URecorderItemDefinition;
class UAudioComponent;
class USoundBase;

/*	[회수 7초 시퀀스의 단계]
	C++: 시퀀스의 시간표와 상태만 관리
	블루프린트: 실제 조명 변화 / 카메라 흔들림 / 왼손 장착 / LCD 점등 같은 연출	*/
UENUM(BlueprintType)
enum class ERecorderRecoverySequenceStep : uint8 {
	Started				UMETA(DisplayName = "시퀀스 시작"),
	LowFrequency		UMETA(DisplayName = "저주파음"),
	FirstVoice			UMETA(DisplayName = "첫 음성"),
	ColdLighting		UMETA(DisplayName = "조명 전환"),
	FlashlightOff		UMETA(DisplayName = "손전등 OFF"),
	FlashlightOn		UMETA(DisplayName = "손전등 ON"),
	HandAttachStart		UMETA(DisplayName = "왼손 장착 시작"),
	RecorderAttached	UMETA(DisplayName = "녹음기 장착 완료"),
	LCDOn				UMETA(DisplayName = "LCD 점등"),
	Footstep			UMETA(DisplayName = "방 밖 발소리"),
	FootstepStopped		UMETA(DisplayName = "발소리 멈춤"),
	Finished			UMETA(DisplayName = "시퀀스 완료")
};

// 재생 종료
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRecorderFinished);
// 단계 재생 시작
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecorderStagePlayed, int32, Stage);
// 녹음기 회수 완료
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecorderCollected, URecorderItemDefinition*, Definition);
// 회수 7초 시퀀스 단계 방송
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecorderRecoverySequenceStep, ERecorderRecoverySequenceStep, Step);


UCLASS(ClassGroup = (EchoOfMe), meta = (BlueprintSpawnableComponent))
class ECHOOFME_API URecorderComponent : public UActorComponent {
	GENERATED_BODY()

public:
	URecorderComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	//---
	// 녹음기 회수 / 상태
public:
	/*	[녹음기 픽업 액터에서 호출]
		회수 상태를 저장, CP3 갱신 후, 7초 시퀀스 시작	*/
	UFUNCTION(BlueprintCallable, Category = "Recorder")
	void HandleRecorderCollected(URecorderItemDefinition* Definition);

	UFUNCTION(BlueprintPure, Category = "Recorder")
	bool HasRecorder() const { return bHasRecorder; }

	UFUNCTION(BlueprintPure, Category = "Recorder")
	bool IsRecorderCollected() const { return bRecorderCollected; }

	UFUNCTION(BlueprintPure, Category = "Recorder")
	bool IsRecoverySequencePlaying() const { return bRecoverySequencePlaying; }


	//---
	// 녹음기 재생
public:
	// 현재 단계의 음성 재생
	UFUNCTION(BlueprintCallable, Category = "Recorder")
	void PlayCurrentStage();

	// 재생 중인 음성 중단
	UFUNCTION(BlueprintCallable, Category = "Recorder")
	void StopPlayBack();

	// 재생 중 여부 반환
	UFUNCTION(BlueprintPure, Category = "Recorder")
	bool IsPlaying() const;


	//---
	// 방송
public:
	UPROPERTY(BlueprintAssignable, Category = "Recorder")
	FOnRecorderFinished OnPlaybackFinished;

	UPROPERTY(BlueprintAssignable, Category = "Recorder")
	FOnRecorderStagePlayed OnStagePlayed;

	UPROPERTY(BlueprintAssignable, Category = "Recorder")
	FOnRecorderCollected OnRecorderCollected;

	UPROPERTY(BlueprintAssignable, Category = "Recorder")
	FOnRecorderRecoverySequenceStep OnRecoverySequenceStep;


	//---
	// 인벤토리 사용 이벤트
protected:
	// 인벤토리에서 아이템 사용되면 호출 >> 녹음기일 때만
	UFUNCTION()
	void HandleItemUsed(UInventoryItemDefinition* Item);

	// 음성 재생이 끝났을 때 콜백
	UFUNCTION()
	void HandleAudioFinished();


	//---
	// 데이터 / 사운드
protected:
	// 녹음기 정의
	UPROPERTY(EditDefaultsOnly, Category = "Recorder")
	TObjectPtr<URecorderItemDefinition> RecorderDefinition;

	// 잠긴 트랙을 재생하려 할 때 나오는 노이즈
	UPROPERTY(EditDefaultsOnly, Category = "Recorder|Sound")
	TObjectPtr<USoundBase> LockedNoiseSound;

	// T+1.0 저주파음
	UPROPERTY(EditDefaultsOnly, Category = "Recorder|Recovery Sequence")
	TObjectPtr<USoundBase> LowFrequencySound;

	// T+2.0 첫 음성: "...아직 남아 있었네."
	UPROPERTY(EditDefaultsOnly, Category = "Recorder|Recovery Sequence")
	TObjectPtr<USoundBase> AwakeningVoiceSound;

	// T+6.0 방 밖 구두 소리 1회
	UPROPERTY(EditDefaultsOnly, Category = "Recorder|Recovery Sequence")
	TObjectPtr<USoundBase> DistantFootstepSound;

	// 입력 제한 시간. 기획상 5.5초.
	UPROPERTY(EditDefaultsOnly, Category = "Recorder|Recovery Sequence", meta = (ClampMin = "0.0"))
	float MovementLockDuration = 1.f;

	// 전체 시퀀스 시간. 기획상 7초.
	UPROPERTY(EditDefaultsOnly, Category = "Recorder|Recovery Sequence", meta = (ClampMin = "0.0"))
	float SequenceDuration = 7.0f;


	//---
	// 시퀀스 내부 처리
private:
	void StartRecoverySequence();
	void FinishRecoverySequence();

	void LockPlayerInput(bool bLocked);
	void UnlockPlayerInput();

	// 시퀀스 각 단계 처리
	void PlayLowFrequencyCue();
	void PlayAwakeningVoiceCue();
	void ApplyColdLightingCue();
	void ForceFlashlightOff();
	void ForceFlashlightOn();
	void BroadcastHandAttachStart();
	void BroadcastRecorderAttached();
	void BroadcastLCDOn();
	void PlayDistantFootstepCue();
	void BroadcastFootstepStopped();

	void ForceFlashlight(bool bOn) const;
	void PlayLockedNoise();
	void ClearRecoverySequenceTimers();


private:
	// 현재 재생 중인 음성 >> 중복 재생 방지 및 중단에 사용
	UPROPERTY()
	TObjectPtr<UAudioComponent> ActiveAudio;

	// 녹음기 액터를 이미 회수했는지.
	// 중복 회수 / 중복 시퀀스 방지용.
	bool bRecorderCollected = false;

	// 실제 플레이 기능이 해금되었는지.
	// T+7.0 이후 true.
	bool bHasRecorder = false;

	bool bRecoverySequencePlaying = false;

	// 시퀀스 타이머 핸들 묶음 >> 단계 추가/제거 시 배열만 갱신
	TArray<FTimerHandle> SequenceTimerHandles;
};
