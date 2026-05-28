// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NoiseMakerComponent.generated.h"


UCLASS(ClassGroup=(EchoOfMe), meta=(BlueprintSpawnableComponent))
class ECHOOFME_API UNoiseMakerComponent : public UActorComponent {
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNoiseMakerComponent();

	// 1회성 소리 ex) 문 열기, 충돌 등
	UFUNCTION(BlueprintCallable, Category = "Noise")
	void ReportNoise(float Loudness);

	// 이동 상태에 따른 발소리 >> 캐릭터에서 호출
	UFUNCTION(BlueprintCallable, Category = "Noise")
	void ReportFootstep(bool bSprinting);


	//---
	// 소리 크기 설정
protected:
	UPROPERTY(EditAnywhere, Category = "Noise")
	float SprintLoudness = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Noise")
	float WalkLoudness = 0.4f;
};
