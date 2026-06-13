// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EchoEnemy.generated.h"

class UEchoEnemyBehaviorComponent;
class UResonanceSensorComponent;

UCLASS()
class ECHOOFME_API AEchoEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEchoEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "EnemyBrain")
	TObjectPtr<UEchoEnemyBehaviorComponent> EnemyBrain;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// 액터가 월드에서 파괴될 때 엔진이 무조건 호출하는 함수입니다.
	virtual void Destroyed() override;
	// RInterp RotationSpeed 
	UPROPERTY(EditAnywhere, Category = "Rotation")
	float RotationSpeed = 1.0f;

	UFUNCTION()
	void IsLockOnToTarget(bool bLockOn);

	// 감지 게이지
	UPROPERTY()
	float DetectCurrentCount = 0.0f;
	//감지 카운트
	UPROPERTY(EditAnywhere, Category = "State | Search", meta = (ClampMin = 0.0f, ClampMax = 200.0f))
	float DetectMaxCount = 100.0f;

	// 감지 여부 게이지
	UPROPERTY()
	bool IsDetect = false;

	// 빛감지 처리 메소드
	UFUNCTION()
	void LightDetect(float Delta);

	// 플레이어를처다봐라 처리 메소드
	UFUNCTION()
	void LookPlayer();

	UPROPERTY()
	FVector SmoothedVelocity = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Category = "Sensor")
	TObjectPtr<UResonanceSensorComponent> ResonanceComp;



};
