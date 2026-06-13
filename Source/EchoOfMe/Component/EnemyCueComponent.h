// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/CueComponentBase.h"
#include "Enemy/ResonanceSensorComponent.h"   // ESenseChannel / ESenseLevel
#include "EnemyCueComponent.generated.h"


class USoundBase;


// 잔향에 부착 — 듣기 큐(Echo) + 상태 전환 스팅어
UCLASS(ClassGroup=(EchoOfMe), meta=(BlueprintSpawnableComponent))
class ECHOOFME_API UEnemyCueComponent : public UCueComponentBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void ResolveCueData() override;

	// 상태 전환 스팅어 (SFX 슬롯 — 비워두면 해당 단계 무음)
	UPROPERTY(EditAnywhere, Category = "Cue|State")
	TObjectPtr<USoundBase> StingerCalm;

	UPROPERTY(EditAnywhere, Category = "Cue|State")
	TObjectPtr<USoundBase> StingerSuspicious;

	UPROPERTY(EditAnywhere, Category = "Cue|State")
	TObjectPtr<USoundBase> StingerAlert;

private:
	UPROPERTY() TObjectPtr<UResonanceSensorComponent> Sensor;
	ESenseLevel LastThreat = ESenseLevel::Calm;

	UFUNCTION() void HandleSenseLevelChanged(ESenseChannel Channel, ESenseLevel Level);
	void PlayStinger(USoundBase* Sound) const;
};
