// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EchoPlayerCharacter.h"
#include "Components/CapsuleComponent.h"             //캡슐콜리전
#include "Engine/World.h"                            //UWorld.(타이머, 트레이스, 시간) 객체 모듈포함
#include "GameFramework/CharacterMovementComponent.h"// CMC (캐릭터 이동 컴포넌트) 객체 모듈 포함
#include "Components/SkeletalMeshComponent.h"		 // 스켈레탈 메시 컴포넌트 모듈 포함

#include "EnhancedInputSubsystems.h"				 // Enhanced Input 서브시스템 모듈 포함
#include "EnhancedInputComponent.h"					 // Enhanced Input 바인딩 컴포넌트 모듈 포함
#include "TimerManager.h"							 // FTimerManager (벽 점프 쿨다운) 타이머 모듈 포함
#include "Engine/LocalPlayer.h"						 // 로컬 플레이어 모듈 포함
#include "DrawDebugHelpers.h"						 // DrawDebugSphere, DrawDebugLine 디버깅 관련 모듈 포함



// Sets default values
AEchoPlayerCharacter::AEchoPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 캡슐 콜리전 크기 설정
	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	// * 이동 키 입력 방향으로 캐릭터를 회전
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// * 중력 크기 설정
	GetCharacterMovement()->GravityScale = 2.5f;
	// * 이동 시작/변경 시 가속 및 제동력
	GetCharacterMovement()->MaxAcceleration = 1500.0f;


}
// Called to bind functionality to input
void AEchoPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,
			this, &AEchoPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered,
			this, &AEchoPlayerCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started,
			this, &AEchoPlayerCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed,
			this, &AEchoPlayerCharacter::DoJumpEnd);
	}
}

void AEchoPlayerCharacter::Move(const FInputActionValue& Value)
{
	// 입력 액션값을 기준으로 캐릭터 이동 처리를 수행함
	FVector2D MovementVector = Value.Get<FVector2D>();

}

void AEchoPlayerCharacter::Look(const FInputActionValue& Value)
{
}

void AEchoPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
}

// Called when the game starts or when spawned
void AEchoPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEchoPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AEchoPlayerCharacter::DoJumpStart()
{
}

void AEchoPlayerCharacter::ResetWallJump()
{
}

void AEchoPlayerCharacter::DoJumpEnd()
{
}

void AEchoPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	// 점프 트레일 이펙트 비활성화
	SetJumpTrailState(false);
	
}

