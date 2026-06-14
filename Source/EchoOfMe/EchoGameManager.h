#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EchoGameManager.generated.h"


// 레벨 진행 단계
UENUM(BlueprintType)
enum class ELevelPhase : uint8 {
	Exploration  UMETA(DisplayName = "탐색"),   // 회수 전
	Return       UMETA(DisplayName = "복귀"),   // 녹음기 회수 후 추격 구간
	Ending       UMETA(DisplayName = "엔딩")    // C2 분기 진입 후
};

// [메모리 플래그] >> 어떤 키가 새로 켜졌는지 알림
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryFlagAdded, FName, FlagKey);
// 녹음기 재생 횟수 변화
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecorderPlaybackCountChanged, int32, NewCount);
// 공명 수치 변화 >> AI가 임계치 판단에 사용
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResonanceChanged, float, NewResonance);
// 환경 소음 발생 >> 소리 센서가 구독
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNoiseEmitted, float, Amount, FVector, Location);
// 레벨 진행 단계 변화 >> 방 상태/추격/연출이 구독
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelPhaseChanged, ELevelPhase, NewPhase);


UCLASS()
class ECHOOFME_API UEchoGameManager : public UWorldSubsystem {
	GENERATED_BODY()

	//---
	// 메모리 플래그
public:
	UFUNCTION(BlueprintCallable, Category = "GameManager|Memory")
	void AddMemoryFlag(FName FlagKey);

	UFUNCTION(BlueprintPure, Category = "GameManager|Memory")
	bool HasMemoryFlag(FName FlagKey) const { return CollectedMemoryFlags.Contains(FlagKey); }

	UFUNCTION(BlueprintPure, Category = "GameManager|Memory")
	int32 GetMemoryFlagCount() const { return CollectedMemoryFlags.Num(); }

	//---
	// 녹음기 재생 횟수
public:
	UFUNCTION(BlueprintCallable, Category = "GameManager|Recorder")
	void IncrementRecorderPlayBack();

	UFUNCTION(BlueprintPure, Category = "GameManager|Recorder")
	int32 GetRecorderPlaybackCount() const { return RecorderPlaybackCount; }

	//---
	// 공명
public:
	UFUNCTION(BlueprintCallable, Category = "GameManager|Resonance")
	void SetResonance(float NewResonance);

	UFUNCTION(BlueprintPure, Category = "GameManager|Resonance")
	float GetResonance() { return Resonance; }


	//---
	// 소음
public:
	UFUNCTION(BlueprintCallable, Category = "GameManager|Noise")
	void EmitNoise(float Amount, FVector Location) { OnNoiseEmitted.Broadcast(Amount, Location); }


	//---
	// 방송
public:
	UPROPERTY(BlueprintAssignable, Category = "GameManager|Memory")
	FOnMemoryFlagAdded OnMemoryFlagAdded;

	UPROPERTY(BlueprintAssignable, Category = "GameManager|Recorder")
	FOnRecorderPlaybackCountChanged OnRecorderPlaybackCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "GameManager|Resonance")
	FOnResonanceChanged OnResonanceChanged;

	UPROPERTY(BlueprintAssignable, Category = "GameManager|Noise")
    FOnNoiseEmitted OnNoiseEmitted;

	UPROPERTY(BlueprintAssignable, Category = "GameManager|Phase")
	FOnLevelPhaseChanged OnLevelPhaseChanged;

private:
	UPROPERTY()
	TSet<FName> CollectedMemoryFlags;

	UPROPERTY()
	int32 RecorderPlaybackCount = 0;

	UPROPERTY()
	float Resonance = 0.f;

	UPROPERTY()
	ELevelPhase LevelPhase = ELevelPhase::Exploration;


	//---
	// static 헬퍼
public:
	// 어디서든 GameManager 인스턴스 가져옴
	UFUNCTION(BlueprintPure, Category = "GameManager", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Echo Game Manager"))
	static UEchoGameManager* Get(const UObject* WorldContextObject);


	//---
	// 엔딩 / 대면 플래그
public:
	/*	대면 엔딩 플래그 집계
		기억 조각 플래그 수 + (녹음기 N회 이상 재생 시 +1)
		[주의] CollectedMemoryFlags에는 기억 조각 플래그만 들어간다는 전제 */
	UFUNCTION(BlueprintPure, Category = "GameManager|Ending")
	int32 GetFacingFlagCount() const;

	// 녹음기 N회 이상 재생이면 대면 플래그(F4)로 인정
	static constexpr int32 RecorderFacingThreshold = 3;


	//---
	// 레벨 진행 단계
public:
	UFUNCTION(BlueprintCallable, Category = "GameManager|Phase")
	void SetLevelPhase(ELevelPhase NewPhase);

	UFUNCTION(BlueprintPure, Category = "GameManager|Phase")
	ELevelPhase GetLevelPhase() const { return LevelPhase; }

	// 현재 페이즈가 기준 단계 이상인지 (전진 판정용)
	UFUNCTION(BlueprintPure, Category = "GameManager|Phase")
	bool IsAtLeastPhase(ELevelPhase Phase) const { return (uint8)LevelPhase >= (uint8)Phase; }
};
