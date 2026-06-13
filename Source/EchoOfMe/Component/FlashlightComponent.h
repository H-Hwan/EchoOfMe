#pragma once

#include "CoreMinimal.h"
#include "Components/SpotLightComponent.h"
#include "FlashlightComponent.generated.h"


//	[플레이어 손전등 컴포넌트]
UCLASS(ClassGroup = (EchoOfMe), meta = (BlueprintSpawnableComponent))
class ECHOOFME_API UFlashlightComponent : public USpotLightComponent {
	GENERATED_BODY()

public:
	UFlashlightComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 손전등 On/Off
	UFUNCTION(BlueprintCallable, Category = "Flash Light")
	void SetFlashLightOn(bool bOn);

protected:
	virtual void BeginPlay() override;

	// 벽이 없을 때 최대 도달 거리
	UPROPERTY(EditAnywhere, Category = "Flash Light", meta = (ClmapMin = "0", ClampMax = "3000"))
	float MaxAttenuationRadius = 2000.f;

	// 벽에 붙었을 때 줄어들 수 있는 하한
	UPROPERTY(EditAnywhere, Category = "Flash Light", meta = (ClmapMin = "0", ClampMax = "3000"))
	float MinAttenuationRadius = 100.f;

	UPROPERTY(EditAnywhere, Category = "Flash Light")
	float RadiusInterpSpeed = 50.f;

	// 의심 게이지 누적 속도 (/sec)
	UPROPERTY(EditAnywhere, Category="Flash Light|Suspicion")
	float SuspicionGainPerSec = 10.f;

	// ON 상태가 아닐 때 의심 게이지 감쇠 속도 (/sec)
	UPROPERTY(EditAnywhere, Category="Flash Light|Suspicion")
	float SuspicionDecayPerSec = 6.f;

	// 빛의 실패 발동 임계치
	UPROPERTY(EditAnywhere, Category="Flash Light|Suspicion")
	float SuspicionThreshold = 100.f;

	// 강제 OFF 지속 시간
	UPROPERTY(EditAnywhere, Category="Flash Light|Suspicion")
	float ForcedOffDuration = 1.0f;

private:
	float CalculateTargetRadius() const;

public:
	UFUNCTION(BlueprintCallable, Category = "Flash Light")
	void ToggleFlashLight();

	UFUNCTION(BlueprintPure, Category = "Flash Light")
	bool IsFlashLightOn() const { return bIsOn; }

	UPROPERTY()
	FVector CachedLightHitPoint;


	// 빛의 실패 1초간 강제 OFF
	void TriggerLightFailure();

	// 라이트 엔드포인트 위치 계산
	UFUNCTION()
	void LightTrace();

	// 라이트 엔드포인트 위치 계산
	UFUNCTION(BlueprintCallable, Category = "Flash Light")
	FVector LightEndPoint();


	UPROPERTY()
	FVector CurrentLight;

private:
	bool bIsOn = false;
	float Suspicion = 0.f;
	bool bIsLocked = false; // 빛의 실패 중 ON 입력 차단
	FTimerHandle ForcedOffTimerHandle;

	void EndLightFailure();
};
