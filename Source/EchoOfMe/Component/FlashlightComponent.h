// Fill out your copyright notice in the Description page of Project Settings.

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

private:
	float CalculateTargetRadius() const;

public:
	UFUNCTION(BlueprintCallable, Category = "Flash Light")
	void ToggleFlashLight();

	UFUNCTION(BlueprintPure, Category = "Flash Light")
	bool IsFlashLightOn() const { return bIsOn; }

private:
	bool bIsOn = false;
};
