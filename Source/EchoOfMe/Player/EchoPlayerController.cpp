// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EchoPlayerController.h"

#include "Interface/Interactable.h"
#include "Component/InventoryComponent.h"
#include "Component/RecorderComponent.h"
#include "UI/InventoryWidget.h"

#include "EnhancedInputSubsystems.h"	// Enhanced Input 서브시스템
#include "InputMappingContext.h"		// IMC 클래스
#include "Kismet/GameplayStatics.h"		// UGameplayStatics (범용 게임플레이 유틸리티)
#include "GameFramework/PlayerStart.h"	// 플레이어 스타트 액터
#include "EchoPlayerCharacter.h"		// 전투 캐릭터 클래스
#include "Engine/LocalPlayer.h"			// 로컬 플레이어
#include "Engine/World.h"				// UWorld (월드 접근)
#include "TimerManager.h"

#include "EnhancedInputComponent.h"
#include "Camera/PlayerCameraManager.h"



AEchoPlayerController::AEchoPlayerController() {
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Recorder = CreateDefaultSubobject<URecorderComponent>(TEXT("Recorder"));
}


void AEchoPlayerController::BeginPlay() {
	Super::BeginPlay();

	// 체크포인트를 아직 밟지 않았더라도, 최소한 시작 위치로는 되돌아갈 수 있게 설정
	if (APawn* CurrentPawn = GetPawn()) {
		CachePawnClassIfNeeded(CurrentPawn);
		CacheInitialRespawnTransformIfNeeded(CurrentPawn);
	}
	else {
		RespawnTransform = GetFallbackRespawnTransform();
		bHasRespawnTransform = true;
	}

	if (IsLocalPlayerController() && InventoryWidgetClass) {
		InventoryWidget = CreateWidget<UInventoryWidget>(this, InventoryWidgetClass);
		if (InventoryWidget) {
			InventoryWidget->BindInventory(Inventory);
			InventoryWidget->AddToViewport();
			InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}


void AEchoPlayerController::SetupInputComponent() {
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
		if (ToggleInventoryAction) {
			EIC->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &AEchoPlayerController::ToggleInventory);
		}
	}
}


void AEchoPlayerController::ToggleInventory() {

}


void AEchoPlayerController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	if (!InPawn) return;

	// 같은 Pawn에 중복 바인딩되는 상황 방지
	InPawn->OnDestroyed.RemoveDynamic(this, &AEchoPlayerController::OnPawnDestroyed);
	InPawn->OnDestroyed.AddDynamic(this, &AEchoPlayerController::OnPawnDestroyed);

	CachePawnClassIfNeeded(InPawn);
	CacheInitialRespawnTransformIfNeeded(InPawn);
}


void AEchoPlayerController::CachePawnClassIfNeeded(APawn* InPawn) {
	if (CharacterClass || !InPawn) return;

	if (AEchoPlayerCharacter* EchoCharacter = Cast<AEchoPlayerCharacter>(InPawn)) {
		CharacterClass = EchoCharacter->GetClass();
	}
}


void AEchoPlayerController::CacheInitialRespawnTransformIfNeeded(APawn* InPawn) {
	if (bHasRespawnTransform || !InPawn) return;

	SetRespawnTransform(InPawn->GetActorTransform());
}


void AEchoPlayerController::SetRespawnTransform(const FTransform& NewRespawn) {
	RespawnTransform = NewRespawn;
	bHasRespawnTransform = true;
}


void AEchoPlayerController::KillPlayer() {
	if (bIsRespawning) return;

	bIsRespawning = true;

	/*	TODO
		실패 연출을 연결	*/

	APawn* CurrentPawn = GetPawn();
	if (CurrentPawn) {
		// Destroy 이벤트로 중복 예약되지 않게 제거
		CurrentPawn->OnDestroyed.RemoveDynamic(this, &AEchoPlayerController::OnPawnDestroyed);
		CurrentPawn->DisableInput(this);
		CurrentPawn->SetActorEnableCollision(false);

		UnPossess();
		CurrentPawn->Destroy();
	}

	ScheduleRespawn();
}


void AEchoPlayerController::OnPawnDestroyed(AActor* DestroyActor) {
	if (bIsRespawning) return;

	bIsRespawning = true;
	UnPossess();
	ScheduleRespawn();
}


void AEchoPlayerController::ScheduleRespawn() {
	UWorld* World = GetWorld();
	if (!World) {
		bIsRespawning = false;
		return;
	}

	if (!bHasRespawnTransform) {
		RespawnTransform = GetFallbackRespawnTransform();
		bHasRespawnTransform = true;
	}

	if (RespawnDelay <= 0.f) {
		RespawnPlayer();
		return;
	}

	World->GetTimerManager().ClearTimer(RespawnTimerHandle);
	World->GetTimerManager().SetTimer(RespawnTimerHandle, this, &AEchoPlayerController::RespawnPlayer, RespawnDelay, false);
}


void AEchoPlayerController::RespawnPlayer() {
	UWorld* World = GetWorld();
	if (!World) {
		bIsRespawning = false;
		return;
	}

	if (!CharacterClass) {
		UE_LOG(LogTemp, Error, TEXT("[Respawn] CharacterClass가 지정되지 않음!! CharacterClass를 설정 요망!!"));
		bIsRespawning = false;
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEchoPlayerCharacter* RespawnedCharacter = World->SpawnActor<AEchoPlayerCharacter>(CharacterClass, RespawnTransform, SpawnParams);

	if (!RespawnedCharacter) {
		UE_LOG(LogTemp, Error, TEXT("[Respawn] 플레이어 캐릭터 스폰 실패!!"));
		bIsRespawning = false;
		return;
	}

	Possess(RespawnedCharacter);
	SetViewTarget(RespawnedCharacter);

	bIsRespawning = false;

	UE_LOG(LogTemp, Log, TEXT("[Respawn] 플레이어 리스폰 완료: %s"), *RespawnTransform.GetLocation().ToString());
}


FTransform AEchoPlayerController::GetFallbackRespawnTransform() const {
	if (const APawn* CurrentPawn = GetPawn()) {
		return CurrentPawn->GetActorTransform();
	}

	if (const AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass())) {
		return PlayerStart->GetActorTransform();
	}

	return FTransform::Identity;
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
