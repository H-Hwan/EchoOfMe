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
class UAmbushStateComponent;
class AEchoEnemy;
class UFlashlightComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLightNoise,bool,bIsShock);


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

	UPROPERTY(VisibleAnywhere, Category = "State")
	TObjectPtr<UAmbushStateComponent> AmbushStateComp;

	UPROPERTY()
	TObjectPtr<UFSMStateBase> CurrentStateComp;

	UPROPERTY(VisibleAnywhere, Category = "State")
	EFSMState CurrentState;

	TObjectPtr<AEchoEnemy> Echo;

	UPROPERTY()
	TSet<AActor*> VisitedPeekPoints;

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
	bool IsLightDetected();

	FVector GetPlayerLocation();

	UFUNCTION(BlueprintCallable, Category = "Search | Location")
	bool IsPlayerInDetectedSight();

	UFUNCTION(BlueprintCallable, Category = "Search | Location")
	bool IsTargetInSight(const FVector& TargetLocation);

	UPROPERTY(EditAnywhere, Category = "Search | Location")
	float AcceptanceRadius = 0.0f;
	// 타겟 태그 지정
	UPROPERTY(EditAnywhere, Category = "Teleportation | Flag")
	FName SelectTag = TEXT("Flag_EchosTeleportPoint");

	UFUNCTION(BlueprintCallable, Category = "Teleportation | Flag")
	FVector PickTeleportToNewPoint();

	// 일정거리유지, 유저감시 위치 지정
	UFUNCTION(BlueprintCallable, Category = "Teleportation | Flag")
	FVector FindPeekPoint();
	
	// 텔레포트 지점 태그지정
	UPROPERTY(EditAnywhere, Category = "Teleportation | Flag")
	FName PeekPointName = TEXT("Flag_PeekPoint");

	UPROPERTY()
	TObjectPtr<APawn> CachedPlayer;

	UPROPERTY()
	TObjectPtr<UFlashlightComponent> FlashlightComponent;

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
	UFUNCTION(BlueprintPure, Category = "Sensor")
	int32 GetSensorValue()const { return CurrentSensorValue;  }

	// 소리 감지게이지 델리게이트 메소드
	UFUNCTION(BlueprintCallable, Category = "Sensor")
	void SetSoundSensorValue(bool bSound, int32 Value);
	// 빛 감지게이지 델리게이트 메소드
	UFUNCTION(BlueprintCallable, Category = "Sensor")
	void SetLightSensorValue(bool bLight, int32 Value);
	// 소리 감지게이지 델리게이트 메소드
	UFUNCTION(BlueprintCallable, Category = "Sensor")
	void SetResonanceSensorValue(bool bResonance, int32 Value);

	//블루프린트용
	//UFUNCTION(BlueprintImplementableEvent, Category = "Dead")
	//void IsPlayerDEAD(bool bIsAlive);

	// 끼면 알아서판정함
	void CheckIfStuck(float DeltaTime);


	// 막힘 감지용
	UPROPERTY()
	FVector LastPosition = FVector::ZeroVector;

	UPROPERTY()
	float StuckTimer = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Move")
	float StuckCheckInterval = 1.5f;   // 몇 초마다 체크

	UPROPERTY(EditAnywhere, Category = "Move")
	float StuckThreshold = 50.0f;      // 이 거리 이하면 막힌 것으로 판단

	UPROPERTY()
	FVector LastDestination = FVector::ZeroVector;  // 마지막 목적지 저장

	bool RequestMoveToInternal(const FVector& Destination); // LastDestination 갱신 안 함

};
