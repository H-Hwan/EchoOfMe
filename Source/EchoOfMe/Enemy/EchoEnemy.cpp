// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EchoEnemy.h"
#include "EchoEnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEchoEnemy::AEchoEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AEchoEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationYaw = false;

	// 캐릭터 이동 컴포넌트가 이동을 위해 회전 시 부드러운 회전을 사용함
	GetCharacterMovement()->bUseControllerDesiredRotation = true;



}

// Called when the game starts or when spawned
void AEchoEnemy::BeginPlay()
{
	Super::BeginPlay();


	
}

// Called every frame
void AEchoEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}


