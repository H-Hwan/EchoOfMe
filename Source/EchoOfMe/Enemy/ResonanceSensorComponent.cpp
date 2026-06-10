// Fill out your copyright notice in the Description page of Project Settings.


#include "ResonanceSensorComponent.h"
#include "Enemy/EchoEnemy.h"
#include "Enemy/EchoEnemyBehaviorComponent.h"


// Sets default values for this component's properties
UResonanceSensorComponent::UResonanceSensorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UResonanceSensorComponent::BeginPlay()
{
	Super::BeginPlay();

	EnemyBrain = GetOwner()->FindComponentByClass<UEchoEnemyBehaviorComponent>();
	EchoEnemy = Cast<AEchoEnemy>(GetOwner());

	//플레시 쪽에서 넘어온 벽에닿은 빛 위치 델리게이트
 
	//플레이어 쪽에서 넘어온 달리기 여부 델리게이트

	//스킬 쪽에서 넘어온 소리 위치 델리게이트
	
}


// Called every frame
void UResonanceSensorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UResonanceSensorComponent::LightSensorActivate()
{



}

void UResonanceSensorComponent::SoundSensorActivate()
{



}

void UResonanceSensorComponent::NaturalPhenomenonSensor(FVector FlashLightHitLocation)
{
	if (!EchoEnemy || !EnemyBrain->GetPlayerInfo()) return;

	FVector EnemyLocation = EchoEnemy->GetActorLocation();

	FVector PlayerLocation = EnemyBrain->GetPlayerLocation();

	float Distance = FVector::Distance(EnemyLocation, PlayerLocation);

	if (Distance > MaxDistanceToReflectedLight) return;

	FVector ForwardV = EchoEnemy->GetActorForwardVector();

	FVector TDV = (PlayerLocation - EnemyLocation).GetSafeNormal();

	float DotResult = FVector::DotProduct(ForwardV, TDV);

	CosAngle = FMath::Cos(FMath::DegreesToRadians(MaxLightDetectSight));

	if (DotResult <= CosAngle) return;

	FHitResult HitResult;

	FCollisionQueryParams Param;

	Param.AddIgnoredActor(EchoEnemy);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, EnemyLocation + FVector(0.0f, 0.0f, 60.0f), PlayerLocation + FVector(0.0f, 0.0f, 60.0f), ECC_Pawn, Param);

	if (bHit && HitResult.GetActor() == EnemyBrain->GetPlayerInfo())
	{
		DrawDebugLine(GetWorld(), EnemyLocation + FVector(0.0f, 0.0f, 60.0f), PlayerLocation + FVector(0.0f, 0.0f, 60.0f), FColor::Purple, false, -1.0f, 0, 2.0f);
		return;
	}

	return;
	
}

