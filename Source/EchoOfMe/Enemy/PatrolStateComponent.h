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

	UPROPERTY()
	float MaxStuckTime = 4.0f;

	uint8 bIsNowMove : 1;

	float Suspectmin = 0.0f;

	UPROPERTY()
	float Suspectmax = 6.0f;

	float TargetRetryTimer = 0.0f;

	UPROPERTY()
	bool bLooking;

	float LookingTime;
	FRotator RotationToTarget;
	float LookingForwardTime;
	UPROPERTY(EditAnywhere, Category = "Patrol | RandRangeMax", meta = (ClampMin = "0", ClampMax = "10"))
	int32 PatrolToAmbush = 5;
	UPROPERTY(EditAnywhere, Category = "Suspect | Angle")
	float LookingAngle = 65.0f;
	int32 CurrentRandomChangePatrolToAmbush = 0.0f;

	FVector RandomPickTarget = FVector::ZeroVector;

	bool bHasTarget = false;



	virtual void OnStateEnter() override;

	virtual void OnStateUpdate(float Delta) override;

	virtual void OnStateExit() override;

};
