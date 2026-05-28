// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EchoPlayerCharacter.h"

#include "Components/CapsuleComponent.h"				//캡슐콜리전
#include "Components/SkeletalMeshComponent.h"			// 스켈레탈 메시 컴포넌트 모듈 포함
#include "Engine/LocalPlayer.h"							// 로컬 플레이어 모듈 포함
#include "Engine/World.h"								//UWorld.(타이머, 트레이스, 시간) 객체 모듈포함
#include "GameFramework/CharacterMovementComponent.h"	// CMC (캐릭터 이동 컴포넌트) 객체 모듈 포함
#include "GameFramework/SpringArmComponent.h"			// 필수 포함!
#include "Camera/CameraComponent.h"						//
#include "EnhancedInputSubsystems.h"					// Enhanced Input 서브시스템 모듈 포함
#include "EnhancedInputComponent.h"						// Enhanced Input 바인딩 컴포넌트 모듈 포함
#include "TimerManager.h"								// FTimerManager (벽 점프 쿨다운) 타이머 모듈 포함
#include "DrawDebugHelpers.h"							// DrawDebugSphere, DrawDebugLine 디버깅 관련 모듈 포함

#include "Component/ListeningComponent.h"
#include "Component/FlashlightComponent.h"
#include "Component/NoiseMakerComponent.h"


// Sets default values
AEchoPlayerCharacter::AEchoPlayerCharacter() {
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 캡슐 콜리전 크기 설정
	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);

	// 이동 키 입력 방향으로 캐릭터를 회전
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 컨트롤러의 Yaw 회전값을 캐릭터에 적용
	bUseControllerRotationYaw = true;
	// 중력 크기 설정
	GetCharacterMovement()->GravityScale = 1.0f;
	// 이동 시작/변경 시 가속 및 제동력
	GetCharacterMovement()->MaxAcceleration = 1500.0f;
	GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
	GetCharacterMovement()->JumpZVelocity = 380.f;
	// 카메라 설정
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 5.0f; //
	// 마우스 입력으로 카메라 회전
	CameraBoom->bUsePawnControlRotation = true;
	// 카메라가 캐릭터를 부드럽게 추적
	CameraBoom->bEnableCameraLag = false;
	// 카메라가 캐릭터 회전을 부드럽게 추적
	CameraBoom->bEnableCameraRotationLag = true;

	// 카메라 컴포넌트 추가
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// 카메라는 캐릭터 추적 회전을 사용하지 않음
	FollowCamera->bUsePawnControlRotation = false;

	// 듣기 능력 추가
	Listening = CreateDefaultSubobject<UListeningComponent>(TEXT("Listening"));
	// 손전등 추가
	FlashLight = CreateDefaultSubobject<UFlashlightComponent>(TEXT("FlashLight"));
	FlashLight->SetupAttachment(FollowCamera);
	FlashLight->SetRelativeLocation(FVector(10.f, 0.f, 0.f));
	FlashLight->Intensity = 5000.f;
	FlashLight->OuterConeAngle = 35.f;
	FlashLight->InnerConeAngle = 19.f;
	FlashLight->LightColor = FColor(1.f, 0.7f, 0.37f);

	// 소리 발생 컴포넌트
	NoiseMaker = CreateDefaultSubobject<UNoiseMakerComponent>(TEXT("NoiseMaker"));
}

// Called when the game starts or when spawned
void AEchoPlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	if (Listening) {
		Listening->OnListeningChanged.AddDynamic(this, &AEchoPlayerCharacter::HandleListeningChanged);
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController())) {
		PC->PlayerCameraManager->ViewPitchMin = ViewPitchMin;
		PC->PlayerCameraManager->ViewPitchMax = ViewPitchMax;
	}
	
}

// Called every frame
void AEchoPlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}


// Called to bind functionality to input
void AEchoPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
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
		if (FlashlightAction) {
			EnhancedInputComponent->BindAction(FlashlightAction, ETriggerEvent::Started,
				this, &AEchoPlayerCharacter::OnFlashLightInput);
		}

		if (ListenAction) {
			EnhancedInputComponent->BindAction(ListenAction, ETriggerEvent::Started,
				this, &AEchoPlayerCharacter::OnListenStarted);
			EnhancedInputComponent->BindAction(ListenAction, ETriggerEvent::Completed,
				this, &AEchoPlayerCharacter::OnListenCompleted);
		}

	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("[EnhancedInputComponent] 참조 실패"));
	}
}

// 입력 액션값 기준 캐릭터 이동 처리 수행
void AEchoPlayerCharacter::Move(const FInputActionValue& Value) {
	FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}
// 이동 수행 메소드
void AEchoPlayerCharacter::DoMove(float Right, float Forward) {
	if (GetController() != nullptr) {
		// 컨트롤러의 Yaw(좌우 회전)만 추출하여 수평 방향 벡터를 계산
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 이동 방향 벡터 구함
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 실제 이동 입력 적용
		if (Forward != 0.0f) {
			AddMovementInput(ForwardDirection, Forward);
		}
		if (Right != 0.0f) {
			AddMovementInput(RightDirection, Right);
		}
	}
}

// 마우스 입력 [방향]벡터로 시점 변경 수행 메소드 호출
void AEchoPlayerCharacter::Look(const FInputActionValue& Value) {
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

// 시점 변경 수행 메소드
void AEchoPlayerCharacter::DoLook(float Yaw, float Pitch) {

	if (GetController() != nullptr) 	{
		// 각 회전 축별 카메라 및 캐릭터 회전 수행
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AEchoPlayerCharacter::DoRunning() {
	GetCharacterMovement()->MaxAcceleration = 700.0f;
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
}

void AEchoPlayerCharacter::StopRunning() {
	GetCharacterMovement()->MaxAcceleration = 1500.0f;
	GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
}

void AEchoPlayerCharacter::DoJumpStart() {
	Jump(); 
}

void AEchoPlayerCharacter::DoJumpEnd() {
	StopJumping();
}

void AEchoPlayerCharacter::Landed(const FHitResult& Hit) {
	Super::Landed(Hit);
}

void AEchoPlayerCharacter::HandleListeningChanged(bool bIsListening) {
	if (UCharacterMovementComponent* Move = GetCharacterMovement()) {
		Move->MaxWalkSpeed = bIsListening ? CurrentSpeed * 0.5 : CurrentSpeed;
	}
}

void AEchoPlayerCharacter::OnListenStarted() {
	if (Listening) {
		Listening->StartListening();
	}
}

void AEchoPlayerCharacter::OnListenCompleted() {
	if (Listening) {
		Listening->StopListening();
	}
}

void AEchoPlayerCharacter::OnFlashLightInput() {
	if (FlashLight) {
		FlashLight->ToggleFlashLight();
	}
}

