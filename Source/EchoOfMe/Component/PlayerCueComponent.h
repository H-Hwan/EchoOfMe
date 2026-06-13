// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCueComponent.generated.h"


class UAudioComponent;
class USoundBase;
class UCurveFloat;


// 플레이어에 부착 — 공명 수치에 따라 커지는 2D 피드백음 (거리/CurrentCue 없음)
UCLASS(ClassGroup=(EchoOfMe), meta=(BlueprintSpawnableComponent))
class ECHOOFME_API UPlayerCueComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerCueComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// 공명 루프음 (루프 가능한 사운드)
	UPROPERTY(EditAnywhere, Category = "Cue|Resonance")
	TObjectPtr<USoundBase> ResonanceLoopSound;

	UPROPERTY(EditAnywhere, Category = "Cue|Resonance")
	float MaxVolume = 1.f;

	UPROPERTY(EditAnywhere, Category = "Cue|Resonance")
	float ResonanceMax = 100.f;

	// 선택: 0~1 입력(공명 정규화) → 0~1 볼륨. 비우면 선형
	UPROPERTY(EditAnywhere, Category = "Cue|Resonance")
	TObjectPtr<UCurveFloat> ResponseCurve;

private:
	UPROPERTY() TObjectPtr<UAudioComponent> ResonanceAudio;
};
