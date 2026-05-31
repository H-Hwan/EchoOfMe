// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/FSMStateBase.h"
#include "AmbushStateComponent.generated.h"

/**
 * 
 */
UCLASS()
class ECHOOFME_API UAmbushStateComponent : public UFSMStateBase
{
	GENERATED_BODY()

public:

	virtual void OnStateEnter()override;

	virtual void OnStateUpdate(float Delta)override;

	virtual void OnStateExit()override;

	UPROPERTY(EditAnywhere, Category = "Ambush")
	float MaxStayTime = 5.0f;

	float CurrentTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Ambush", meta = (ClampMin = 0, Units = "cm"))
	float MinimumDistance = 500.0f;



};
