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
	Lost	UMETA(DisplayName = "Lost")
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
	FVector GetDistanceToPlayer() ;

	UFUNCTION(BlueprintCallable, Category = "Search | Location")
	FVector GetPlayerLocation() ;


	UPROPERTY(EditAnywhere, Category = "Search | Location")
	float AcceptanceRadius = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Search")
	FName SelectTag;

	UFUNCTION(BlueprintCallable, Category = "Teleportation | Flag")
	void PickTeleportToNewPoint();

	UPROPERTY()
	TArray<TObjectPtr<AActor>> CachedFlags;


	UPROPERTY(EditAnywhere, Category = "State")
	float OutRange = 1500.0f;

};
