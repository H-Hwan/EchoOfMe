// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EchoEnemy.h"
#include "EchoOfMe/Enemy/EchoEnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EchoOfMe/Enemy/EchoEnemyBehaviorComponent.h"



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

	GetCharacterMovement()->MaxWalkSpeed = 222.0f;

	GetCharacterMovement()->MaxAcceleration = 1500.0f;

	GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터 이동 방향으로 자동 회전

	GetCharacterMovement()->bUseRVOAvoidance = true; // AI 캐릭터들끼리 겹쳐지지 않도록 설정

	EnemyBrain = CreateDefaultSubobject<UEchoEnemyBehaviorComponent>(TEXT("Brain"));

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
// 플레이어 추격시작 시 변경 스텟
void AEchoEnemy::IsLockOnToTarget(bool bLockOn)
{
	if(bLockOn)
	{
		GetCharacterMovement()->MaxAcceleration = 500.0f;
		GetCharacterMovement()->MaxWalkSpeed = 340.0f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 222.0f;
		GetCharacterMovement()->MaxAcceleration = 1500.0f;
	}

}
// 빛감지
void AEchoEnemy::LightDetect(float DeltaTime)
{

	if (!IsDetect) return;

	DetectCount += DeltaTime;







}


