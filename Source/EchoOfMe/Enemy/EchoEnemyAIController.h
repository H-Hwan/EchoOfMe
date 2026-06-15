// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "Enemy/ResonanceSensorComponent.h"

#include "EchoEnemyAIController.generated.h"

class AEchoEnemy;


UCLASS()
class ECHOOFME_API AEchoEnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	AEchoEnemy* OwnerPawn;


public:
	// 빙의/해제 시점에 센서 구독·해제
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	//UPROPERTY()
	//TObjectPtr<UResonanceSensorComponent> CachedSensor;
	UFUNCTION()
	void HandleSenseLevelChanged(ESenseChannel Channel, ESenseLevel Level);
};
