// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResonanceSensorComponent.generated.h"

class AEchoEnemy;
class UEchoEnemyBehaviorComponent;

class APawn;

// 감지 채널
UENUM(BlueprintType)
enum class ESenseChannel : uint8 {
	Sound		UMETA(DisplayName = "소리"),
	Resonance	UMETA(DisplayName = "공명")
};

// 위협 단계
UENUM(BlueprintType)
enum class ESenseLevel : uint8 {
	Calm		UMETA(DisplayName = "평온"),   // < 의심 임계치
	Suspicious	UMETA(DisplayName = "의심"),   // >= 30
	Alert		UMETA(DisplayName = "추적")    // >= 70
};

// 소리 1회성 이벤트 종류
UENUM(BlueprintType)
enum class ESoundCue : uint8 {
	DoorRough		UMETA(DisplayName = "문 거칠게 열기"),   // +20
	ItemDrop		UMETA(DisplayName = "물건 떨어뜨림"),     // +25
	HideFail		UMETA(DisplayName = "숨기 실패"),         // +15
	RecorderManual	UMETA(DisplayName = "녹음기 수동 재생")   // +30
};

// 공명 1회성 이벤트 종류
UENUM(BlueprintType)
enum class EResonanceCue : uint8 {
	MemoryFragment	UMETA(DisplayName = "기억 단서 획득"),       // +25
	RecordingFull	UMETA(DisplayName = "기억 녹음 끝까지 들음")  // +40
};
//
//
//// 감지 단계 변화 방송 — 잔향 상태머신이 구독
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSenseLevelChanged, ESenseChannel, Channel, ESenseLevel, Level);
//
//// 감지 관련 컴포넌트
//UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
//class ECHOOFME_API UResonanceSensorComponent : public UActorComponent
//{
//	GENERATED_BODY()
//
//public:	
//	// Sets default values for this component's properties
//	UResonanceSensorComponent();
//
//protected:
//	// Called when the game starts
//	virtual void BeginPlay() override;
//
//public:	
//	// Called every frame
//	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
//
//	UPROPERTY(EditAnywhere, Category = "Resonance")
//	float MaxResonanceCount;
//
//	UPROPERTY()
//	float CurrentResonanceCount;
//
//	/// 불빛 감지
//	UFUNCTION(BlueprintCallable, Category = "Resonance")
//	void LightSensorActivate();
//	/// 소리 감지 — 1회성 소음 이벤트 (위치 포함)
//	UFUNCTION(BlueprintCallable, Category = "Sense|Sound")
//	void SoundSensorActivate(ESoundCue Cue, FVector NoiseLocation);
//	/// 공명 감지
//	/// 공명 감지 — 1회성 공명 이벤트 (위치 없음)
//	UFUNCTION(BlueprintCallable, Category = "Sense|Resonance")
//	void SuperNaturalPhenomenonSensor(EResonanceCue Cue);
//
//
//	UPROPERTY()
//	TObjectPtr<UEchoEnemyBehaviorComponent> EnemyBrain;
//
//	float CosAngle = 0.0f;
//
//	// Echo가 플레이어가 쏜빛의 도달지점을 확인하는 시야각
//	UPROPERTY(EditAnywhere, Category = "Resonance", meta = (Units = "deg"))
//	float MaxLightDetectSight = 45.0f;
//
//	// Echo가 플레이어가 쏜빛의 도달지점을 확인하는 시야거리
//	UPROPERTY(EditAnywhere, Category = "Resonance", meta = (Units = "cm"))
//	float MaxDistanceToReflectedLight = 3000.0f;
//
//	UPROPERTY()
//	TObjectPtr<AEchoEnemy> EchoEnemy;
//
//	// Fill out your copyright notice in the Description page of Project Settings.
//		// 감지 단계가 바뀔 때 방송
//	UPROPERTY(BlueprintAssignable, Category = "Sense")
//	FOnSenseLevelChanged OnSenseLevelChanged;
//
//	UFUNCTION(BlueprintPure, Category = "Sense")
//	float GetSoundGauge() const { return SoundGauge; }
//
//	// 현재 공명 값 (GameManager에서 읽음)
//	UFUNCTION(BlueprintPure, Category = "Sense")
//	float GetResonance() const;
//
//	UFUNCTION(BlueprintPure, Category = "Sense")
//	FVector GetLastNoiseLocation() const { return LastNoiseLocation; }
//
//	// 소리/공명 중 더 높은 단계 (상태 전환 판단용)
//	UFUNCTION(BlueprintPure, Category = "Sense")
//	ESenseLevel GetThreatLevel() const;
//
//	// 소리 게이지 초기화 (잡힘/체크포인트 시)
//	UFUNCTION(BlueprintCallable, Category = "Sense")
//	void ResetSoundSense();
//
//protected:
//	UPROPERTY(EditAnywhere, Category = "Sense|Threshold")
//	float SuspectThreshold = 30.f;   // 의심 진입
//
//	UPROPERTY(EditAnywhere, Category = "Sense|Threshold")
//	float ChaseThreshold = 70.f;     // 추적 진입
//
//	// 소리 연속
//	UPROPERTY(EditAnywhere, Category = "Sense|Sound")
//	float SoundGaugeMax = 100.f;
//
//	UPROPERTY(EditAnywhere, Category = "Sense|Sound")
//	float RunGainPerSec = 8.f;       // 달리기 +8/s
//
//	UPROPERTY(EditAnywhere, Category = "Sense|Sound")
//	float IdleDecayPerSec = 5.f;     // 정지 -5/s
//
//	// 달리기/정지 판정 속도 (실제 플레이어 이동속도에 맞춰 튜닝)
//	UPROPERTY(EditAnywhere, Category = "Sense|Sound")
//	float RunSpeedThreshold = 400.f;
//
//	UPROPERTY(EditAnywhere, Category = "Sense|Sound")
//	float IdleSpeedThreshold = 10.f;
//
//	// 소리 1회성 가중치
//	UPROPERTY(EditAnywhere, Category = "Sense|Sound|Cue")
//	float DoorRoughAmount = 20.f;
//	UPROPERTY(EditAnywhere, Category = "Sense|Sound|Cue")
//	float ItemDropAmount = 25.f;
//	UPROPERTY(EditAnywhere, Category = "Sense|Sound|Cue")
//	float HideFailAmount = 15.f;
//	UPROPERTY(EditAnywhere, Category = "Sense|Sound|Cue")
//	float RecorderManualAmount = 30.f;
//
//	// 이 거리 밖의 소리는 무시 (<=0 이면 무제한). 폐상담센터 기준 1500~2000 권장
//	UPROPERTY(EditAnywhere, Category = "Sense|Sound")
//	float HearingRange = 2000.f;
//
//	// 공명 1회성 가중치
//	UPROPERTY(EditAnywhere, Category = "Sense|Resonance")
//	float MemoryFragmentAmount = 25.f;
//	UPROPERTY(EditAnywhere, Category = "Sense|Resonance")
//	float RecordingFullAmount = 40.f;
//
//	// 화면에 게이지/단계 실시간 표시 (검증용)
//	UPROPERTY(EditAnywhere, Category = "Sense|Debug")
//	bool bDrawDebug = false;
//
//
//	//---
//	// 상태
//protected:
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sense|Sound")
//	float SoundGauge = 0.f;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sense|Sound")
//	FVector LastNoiseLocation = FVector::ZeroVector;
//
//
//	//---
//private:
//	// 자동 연결 핸들러 (GM·녹음기 방송 구독)
//	UFUNCTION() void HandleMemoryFlagAdded(FName FlagKey);              // 공명 +25
//	UFUNCTION() void HandleRecorderPlaybackChanged(int32 NewCount);    // 소리 +30 (수동 재생)
//	UFUNCTION() void HandleRecordingFinished();                        // 공명 +40 (완청)
//	UFUNCTION() void HandleNoiseEmitted(float Amount, FVector Location); // 문/물건/숨기
//
//	void TryBindRecorder();
//
//	ESenseLevel SoundLevel = ESenseLevel::Calm;
//	ESenseLevel ResonanceLevel = ESenseLevel::Calm;
//	bool bRecorderBound = false;
//
//	UPROPERTY()
//	TObjectPtr<APawn> CachedPlayer;
//
//	// 헬퍼
//	ESenseLevel EvaluateLevel(float Value) const;
//	void UpdateChannelLevel(ESenseChannel Channel, float Value, ESenseLevel& InOutLevel);
//	void AddSoundNoise(float Amount, const FVector& Location);
//	FVector GetSensorLocation() const;
//	bool IsWithinHearingRange(const FVector& Location) const;
//
//
//};

