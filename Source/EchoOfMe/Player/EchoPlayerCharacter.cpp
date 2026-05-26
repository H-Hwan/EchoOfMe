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
	PrimaryActorTick.bCanEverTick = true;

	// 캡슐 콜리전 크기 설정
	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	// * 이동 키 입력 방향으로 캐릭터를 회전
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// * 중력 크기 설정
	GetCharacterMovement()->GravityScale = 1.0f;
	// * 이동 시작/변경 시 가속 및 제동력
	GetCharacterMovement()->MaxAcceleration = 1500.0f;
	GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
	GetCharacterMovement()->JumpZVelocity = 380.f;

}
// Called to bind functionality to input
void AEchoPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("SetupPlayerInputComponent 호출됨!")); // 추가
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		UE_LOG(LogTemp, Log, TEXT("[EnhancedInputComponent] 참조 성공"));

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,
			this, &AEchoPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered,
			this, &AEchoPlayerCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started,
			this, &AEchoPlayerCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed,
			this, &AEchoPlayerCharacter::DoJumpEnd);
		EnhancedInputComponent->BindAction(RunningAction, ETriggerEvent::Triggered,
			this, &AEchoPlayerCharacter::DoRunning);
		EnhancedInputComponent->BindAction(RunningAction, ETriggerEvent::Completed,
			this, &AEchoPlayerCharacter::StopRunning);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("[EnhancedInputComponent] 참조 실패"));
	}
}

void AEchoPlayerCharacter::Move(const FInputActionValue& Value)
{
	// 입력 액션값을 기준으로 캐릭터 이동 처리를 수행함
	FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}
// 이동 수행 메소드
void AEchoPlayerCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// 컨트롤러의 Yaw(좌우 회전)만 추출하여 수평 방향 벡터를 계산
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 이동 방향 벡터 구함
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// [추가된 부분] 실제 이동 입력 적용
		if (Forward != 0.0f)
		{
			AddMovementInput(ForwardDirection, Forward);
		}
		if (Right != 0.0f)
		{
			AddMovementInput(RightDirection, Right);
		}
	}
}

void AEchoPlayerCharacter::Look(const FInputActionValue& Value)
{
	// 마우스 입력 방향 벡터를 구성하여 시점 변경 수행 메소드를 호출함
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

// 시점 변경 수행 메소드
void AEchoPlayerCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// 각 회전 축별 카메라 및 캐릭터 회전을 수행함
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}
void AEchoPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
}

void AEchoPlayerCharacter::DoRunning()
{
	GetCharacterMovement()->MaxAcceleration = 700.0f;
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
}

void AEchoPlayerCharacter::StopRunning()
{
	GetCharacterMovement()->MaxAcceleration = 1500.0f;
	GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
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
	Jump(); 
}


void AEchoPlayerCharacter::DoJumpEnd()
{
	StopJumping(); // 점프 중지
}

void AEchoPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	
}

