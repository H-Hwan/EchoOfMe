// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResonanceSensorComponent.generated.h"

class AEchoEnemy;
class UEchoEnemyBehaviorComponent;

// 감지 관련 컴포넌트
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOOFME_API UResonanceSensorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UResonanceSensorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "Resonance")
	float MaxResonanceCount;

	UPROPERTY()
	float CurrentResonanceCount;


	/// 불빛 감지
	UFUNCTION(BlueprintCallable, Category = "Resonance")
	void LightSensorActivate();
	/// 소리 감지
	UFUNCTION(BlueprintCallable, Category = "Resonance")
	void SoundSensorActivate();
	/// 공명 감지
	UFUNCTION(BlueprintCallable, Category = "Resonance")
	void NaturalPhenomenonSensor(FVector FlashLightHitLocation);


	UPROPERTY()
	TObjectPtr<UEchoEnemyBehaviorComponent> EnemyBrain;

	float CosAngle = 0.0f;

	// Echo가 플레이어가 쏜빛의 도달지점을 확인하는 시야각
	UPROPERTY(EditAnywhere, Category = "Resonance", meta = (Units = "deg"))
	float MaxLightDetectSight = 45.0f;

	// Echo가 플레이어가 쏜빛의 도달지점을 확인하는 시야거리
	UPROPERTY(EditAnywhere, Category = "Resonance", meta = (Units = "cm"))
	float MaxDistanceToReflectedLight = 3000.0f;

	UPROPERTY()
	TObjectPtr<AEchoEnemy> EchoEnemy;
};
