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

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// 1회성 소리 ex) 문 열기, 충돌 등
	UFUNCTION(BlueprintCallable, Category = "Noise")
	void ReportNoise(float Loudness);


	//---
	/*	[캐릭터 이동으로 인한 소리]
		한 걸음 사이의 거리가 작을수록 소리가 잦은 구조	*/ 
protected:
	UPROPERTY(EditAnywhere, Category = "Noise")
	float WalkStepDistance = 180.f;

	UPROPERTY(EditAnywhere, Category = "Noise")
	float SprintStepDistance = 220.f;

	float AccumulatedDistance = 0.f;

	FVector LastFootstepLocation = FVector::ZeroVector;

	bool bHasInitialLocation = false;


	//---
	// 소리 크기 설정
protected:
	UPROPERTY(EditAnywhere, Category = "Noise")
	float WalkLoudness = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Noise")
	float SprintLoudness = 1.5f;
};
