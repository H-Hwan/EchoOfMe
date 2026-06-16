// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EchoCatchComponent.generated.h"


class USphereComponent;
class UResonanceSensorComponent;

// 플레이어 잡힘 방송 (필요 시 외부 구독)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCaught);


/*	[잡기 판정]
	잔향에 부착 — 잡기 구체가 플레이어와 오버랩하면 KillPlayer 호출	*/
UCLASS(ClassGroup = (EchoOfMe), meta = (BlueprintSpawnableComponent))
class ECHOOFME_API UEchoCatchComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UEchoCatchComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 잡기 판정 반경
	UPROPERTY(EditAnywhere, Category = "Catch", meta = (ClampMin = "10"))
	float CatchRadius = 100.0f;

	// 루트 기준 오프셋 (앞쪽으로 살짝 빼고 싶을 때)
	UPROPERTY(EditAnywhere, Category = "Catch")
	FVector CatchOffset;

	// 추적(Alert) 상태에서만 잡도록 제한
	UPROPERTY(EditAnywhere, Category = "Catch")
	bool bRequireAlertToCatch = true;

	// 잡힘 순간 BP 연출 훅 (그랩 모션/SFX 등 — 잔향 쪽 반응)
	UFUNCTION(BlueprintImplementableEvent, Category = "Catch")
	void OnPlayerCaughtBP();



private:

	UFUNCTION()
	void OnCatchBeginOverlap(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
		int32 BodyIndex, bool bFromSweep, const FHitResult& Sweep);
	UFUNCTION()
	void OnCatchEndOverlap(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex);

	// 지금 잡아도 되는 상태인지 (Alert 제한 등)
	bool CanCatchNow();

	UPROPERTY()
	TObjectPtr<USphereComponent> CatchSphere;

	UPROPERTY()
	TObjectPtr<UResonanceSensorComponent> CachedSensor;

	// 중복 KillPlayer 방지 (오버랩 떠나면 재무장)
	bool bCaughtPending = false;

public:
	// 잡힘 방송
	UPROPERTY(BlueprintAssignable, Category = "Catch")
	FOnPlayerCaught OnPlayerCaught;


};
