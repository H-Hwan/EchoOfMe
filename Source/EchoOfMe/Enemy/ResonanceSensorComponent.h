// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResonanceSensorComponent.generated.h"

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
	void SuperNaturalPhenomenonSensor(FVector FlashLightHitLocation);


	UPROPERTY()
	TObjectPtr<UEchoEnemyBehaviorComponent> EnemyBrain;

	UPROPERTY()
	TObjectPtr<AEchoEnemy> EchoEnemy;
};
