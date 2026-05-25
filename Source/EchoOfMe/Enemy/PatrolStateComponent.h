// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSMStateBase.h"
#include "PatrolStateComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (FSM), meta = (BlueprintSpawnableComponent))
class ECHOOFME_API UPatrolStateComponent : public UFSMStateBase
{
	GENERATED_BODY()

public:

	bool PickRandomNavMovePoint(FVector& OutLocation) const;

	UPROPERTY(EditAnywhere, Category = "Move", meta = (ClampMin = 0.0, Units = "cm"))
	float PatrolRadius = 500.0f;

	virtual void OnStateEnter() override;
	virtual void OnStateUpdate(float Delta) override;
	virtual void OnStateExit() override;

};
