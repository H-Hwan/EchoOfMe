// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EchoPlayerController.h"

#include "Interface/Interactable.h"
#include "Component/InventoryComponent.h"
#include "Component/RecorderComponent.h"
#include "UI/InventoryWidget.h"

#include "EnhancedInputSubsystems.h"     // Enhanced Input 서브시스템
#include "InputMappingContext.h"         // IMC 클래스
#include "Kismet/GameplayStatics.h"      // UGameplayStatics (범용 게임플레이 유틸리티)
#include "GameFramework/PlayerStart.h"   // 플레이어 스타트 액터
#include "EchoPlayerCharacter.h"             // 전투 캐릭터 클래스
#include "Engine/LocalPlayer.h"          // 로컬 플레이어
#include "Engine/World.h"                // UWorld (월드 접근)

#include "EnhancedInputComponent.h"
#include "Camera/PlayerCameraManager.h"



AEchoPlayerController::AEchoPlayerController() {
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Recorder = CreateDefaultSubobject<URecorderComponent>(TEXT("Recorder"));
}

void AEchoPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AEchoPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 현재 컨트롤러가 로컬 플레이어의 컨트롤러라면
	if (IsLocalPlayerController())
	{
		// 현재 로컬 플레이어의 향상된 입력 로컬 플레이어 서브 시스템을 구함
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			// 등록된 입력 매핑 컨텍스트들을 추가함
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent)) {
		if (InteractAction) {
			EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &AEchoPlayerController::HandleInteract);
		}
	}
}

void AEchoPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn) {
		InPawn->OnDestroyed.AddDynamic(this, &AEchoPlayerController::OnPawnDestroyed);
	}
}

void AEchoPlayerController::SetRespawnTransform(const FTransform& NewRespawn)
{
	RespawnTransform = NewRespawn;
}

void AEchoPlayerController::OnPawnDestroyed(AActor* DestroyActor)
{
	// 1. 기존 빙의 해제 (안전장치)
	UnPossess();

	// 2. 캐릭터 스폰
	if (AEchoPlayerCharacter* RespawnedCharacter = GetWorld()->SpawnActor<AEchoPlayerCharacter>(CharacterClass, RespawnTransform))
	{
		// 3. 빙의 수행
		Possess(RespawnedCharacter);

		// 4. [중요] 빙의 직후 입력 시스템 강제 업데이트 (필요한 경우)
		// 캐릭터가 컨트롤러를 즉시 인식하도록 합니다.
		if (PlayerCameraManager)
		{
			PlayerCameraManager->SetViewTarget(RespawnedCharacter);
		}
	}
}


void AEchoPlayerController::HandleInteract() {
	if (!PlayerCameraManager) return;

	const FVector Start = PlayerCameraManager->GetCameraLocation();
	const FVector End = Start + PlayerCameraManager->GetCameraRotation().Vector() * InteractDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetPawn());

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params)) {
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->Implements<UInteractable>()) {
			IInteractable::Execute_Interact(HitActor, GetPawn());
		}
	}
}
