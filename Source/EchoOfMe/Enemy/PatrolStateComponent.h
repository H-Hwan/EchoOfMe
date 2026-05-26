// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSMStateBase.h"
#include "PatrolStateComponent.generated.h"



UCLASS(ClassGroup = (FSM), meta = (BlueprintSpawnableComponent))
class ECHOOFME_API UPatrolStateComponent : public UFSMStateBase
{
	GENERATED_BODY()

public:

	float StuckTime = 0.0f;

	float MaxStuckTime = 8.0f;


	float Suspectmin = 0.0f;

	float Suspectmax = 6.0f;

	float TargetRetryTimer = 0.0f;

	bool PickRandomNavMovePoint(FVector& OutLocation) const;

	FVector RandomPickTarget = FVector::ZeroVector;

	bool bHasTarget = false;

	UPROPERTY(EditAnywhere, Category = "Move", meta = (ClampMin = 0.0, Units = "m"))
	float PatrolRadius = 500.0f;

	virtual void OnStateEnter() override;

	virtual void OnStateUpdate(float Delta) override;

	virtual void OnStateExit() override;

};
