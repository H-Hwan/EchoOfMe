// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EchoEnemyBehaviorComponent.generated.h"

class UFSMStateBase;
class UPatrolStateComponent;
class USuspectStateComponent;
class UChaseStateComponent;
class USearchStateComponent;
class ULostStateComponent;
class AEchoEnemy;

UENUM(BlueprintType)
enum class EFSMState : uint8
{
	Patrol	UMETA(DisplayName = "Patrol"),
	Suspect	UMETA(DisplayName = "Suspect"),
	Chase	UMETA(DisplayName = "Chase"),
	Search	UMETA(DisplayName = "Search"),
	Lost	UMETA(DisplayName = "Lost"),
	Ambush	UMETA(DisplayName = "Ambush")
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOOFME_API UEchoEnemyBehaviorComponent : public UActorComponent
{
	GENERATED_BODY()


public:	
	// Sets default values for this component's properties
	UEchoEnemyBehaviorComponent();

	UPROPERTY(VisibleAnywhere, Category = "State")
	TObjectPtr<UPatrolStateComponent> PatrolStateComp;

	UPROPERTY(VisibleAnywhere, Category = "State")
	TObjectPtr<USuspectStateComponent> SuspectStateComp;

	UPROPERTY(VisibleAnywhere, Category = "State")
	TObjectPtr<UChaseStateComponent> ChaseStateComp;

	UPROPERTY(VisibleAnywhere, Category = "State")
	TObjectPtr<USearchStateComponent> SearchStateComp;

	UPROPERTY(VisibleAnywhere, Category = "State")
	TObjectPtr<ULostStateComponent> LostStateComp;

	UPROPERTY()
	TObjectPtr<UFSMStateBase> CurrentStateComp;

	UPROPERTY(VisibleAnywhere, Category = "State")
	EFSMState CurrentState;

	AEchoEnemy* Echo;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	UFSMStateBase* GetStateComponent(EFSMState NewState);

	UFUNCTION(BlueprintCallable, Category = "State")
	EFSMState GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintCallable, Category = "State")
	void ChangeState(EFSMState NewState);

	UFUNCTION(BlueprintCallable, Category = "State|Movement")
	bool RequestMoveTo(const FVector& Destination, float InAcceptanceRadius = -1.f);

	UFUNCTION(BlueprintCallable, Category = "Search")
	APawn* GetPlayerInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Search | Location")
	float GetDistanceToPlayer()const;

	UFUNCTION(BlueprintCallable, Category = "Search | Location")
	FVector GetPlayerLocation();

	UFUNCTION(BlueprintCallable, Category = "Search | Location")
	bool IsPlayerInDetectedSight();



	UPROPERTY(EditAnywhere, Category = "Search | Location")
	float AcceptanceRadius = 0.0f;
	// 타겟 태그 지정
	UPROPERTY(EditAnywhere, Category = "Search")
	FName SelectTag = TEXT("");

	UFUNCTION(BlueprintCallable, Category = "Teleportation | Flag")
	FVector PickTeleportToNewPoint();

	UPROPERTY()
	TArray<TObjectPtr<AActor>> CachedFlags;

	// 플레이어와 Echo의 최소 텔레포트 거리
	UPROPERTY(EditAnywhere, Category = "State", meta = (Units = "cm"))
	float SpawnMinimumDistance = 50.0f;

	UPROPERTY(EditAnywhere, Category = "State", meta = ( Units = "cm" ))
	float DetectedMinimumDistanceRadius = 3000.0f;

	//시야 거리
	UPROPERTY(EditAnywhere, Category = "State", meta = ( Units = "cm" ))
	float MaxDistance = 5000.0f;

	// 시야범위
	UPROPERTY(EditAnywhere, Category = "State", meta = (Units = "deg"))
	float MaxDegreeLimit = 90.0f;

	UPROPERTY()
	float CosAngle = 0.0f;


	UFUNCTION()
	bool IsNavMoving()const;

	UFUNCTION()
	bool PickCustomRadiusNavLocation(FVector& OutLocation,float Radius);

	bool PickRandomNavMovePoint(FVector& OutLocation) const;

	bool IsPlayerLoseInSight();

	UPROPERTY(EditAnywhere, Category = "Move", meta = (ClampMin = 0.0, Units = "m"))
	float LoseDistance = 500.0f;


	UPROPERTY(EditAnywhere, Category = "Move", meta = (ClampMin = 0.0, Units = "m"))
	float PatrolRadius = 500.0f;
	//감지 게이지
	UPROPERTY(EditAnywhere, Category = "Echo")
	int32 MaxSensorValue = 100;
	// 현 감지 게이지
	UPROPERTY()
	int32 CurrentSensorValue = 0;
	// 센서 값 반환
	/*UFUNCTION(BlueprintPure, Category = "Sensor")
	int32 GetSensorValue()const;*/
	//// 소리 감지게이지 델리게이트 메소드
	//UFUNCTION(BlueprintCallable, Category = "Sensor")
	//void SetSoundSensorValue();
	//// 빛 감지게이지 델리게이트 메소드
	//UFUNCTION(BlueprintCallable, Category = "Sensor")
	//void SetLightSensorValue();
	//// 소리 감지게이지 델리게이트 메소드
	//UFUNCTION(BlueprintCallable, Category = "Sensor")
	//void SetResonanceSensorValue();

	//블루프린트용
	//UFUNCTION(BlueprintImplementableEvent, Category = "Dead")
	//void IsPlayerDEAD(bool bIsAlive);




};
