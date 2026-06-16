// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EchoCatchComponent.h"

#include "Enemy/ResonanceSensorComponent.h"   // ESenseLevel / GetThreatLevel
#include "Player/EchoPlayerController.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "EchoEnemy.h"

UEchoCatchComponent::UEchoCatchComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}


void UEchoCatchComponent::BeginPlay() {
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	USceneComponent* Root = Owner ? Owner->GetRootComponent() : nullptr;
	if (!Root) {
		UE_LOG(LogTemp, Error, TEXT("[Catch] 루트 컴포넌트 없음 - 잡기 구체 생성 실패"));
		return;
	}

	// 잡기 구체 런타임 생성 (체크포인트/큐액터와 동일한 오버랩 구성)
	CatchSphere = NewObject<USphereComponent>(Owner, TEXT("CatchSphere"));
	CatchSphere->SetupAttachment(Root);
	CatchSphere->SetRelativeLocation(CatchOffset);
	CatchSphere->InitSphereRadius(CatchRadius);

	CatchSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CatchSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CatchSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CatchSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CatchSphere->SetGenerateOverlapEvents(true);

	CatchSphere->OnComponentBeginOverlap.AddDynamic(this, &UEchoCatchComponent::OnCatchBeginOverlap);
	CatchSphere->OnComponentEndOverlap.AddDynamic(this, &UEchoCatchComponent::OnCatchEndOverlap);

	CatchSphere->RegisterComponent();
}


void UEchoCatchComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (CatchSphere) {
		CatchSphere->OnComponentBeginOverlap.RemoveAll(this);
		CatchSphere->OnComponentEndOverlap.RemoveAll(this);
	}
	Super::EndPlay(EndPlayReason);
}


void UEchoCatchComponent::OnCatchBeginOverlap(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
	int32 BodyIndex, bool bFromSweep, const FHitResult& Sweep) {
	if (bCaughtPending) return;

	const APawn* Pawn = Cast<APawn>(Other);
	if (!Pawn)
	{
		UE_LOG(LogTemp, Error, TEXT("--> 실패: 닿은 액터가 Pawn(플레이어)이 아닙니다!"));
		return;
	}

	// 플레이어 컨트롤러일 때만 — 자기/다른 잔향(AIController)은 무시
	AEchoPlayerController* PC = Cast<AEchoPlayerController>(Pawn->GetController());
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("--> 실패: 조종 중인 컨트롤러가 EchoPlayerController가 아닙니다!"));
		return;
	}

	if (!CanCatchNow())
	{
		UE_LOG(LogTemp, Error, TEXT("--> 실패: CanCatchNow 조건 불만족 (아마 Alert 상태가 아님)"));
		return;
	}

	bCaughtPending = true;

	UE_LOG(LogTemp, Warning, TEXT("[Catch] 플레이어 잡힘 → KillPlayer"));

	OnPlayerCaughtBP();        // 잔향 쪽 연출 훅
	OnPlayerCaught.Broadcast();

	PC->KillPlayer();          // 마지막 안전 지점 복귀 (player-side)
}


void UEchoCatchComponent::OnCatchEndOverlap(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex) {
	const APawn* Pawn = Cast<APawn>(Other);
	if (!Pawn) return;
	if (!Cast<AEchoPlayerController>(Pawn->GetController())) return;

	// 플레이어가 리스폰되어 떠나면 다시 잡을 수 있게
	bCaughtPending = false;
}


bool UEchoCatchComponent::CanCatchNow() {
	if (!bRequireAlertToCatch) return true;

	// 센서는 폰 또는 컨트롤러에 붙을 수 있음 — 지연 캐싱
	if (!CachedSensor) {
		AActor* Owner = GetOwner();
		CachedSensor = Owner ? Owner->FindComponentByClass<UResonanceSensorComponent>() : nullptr;

		if (!CachedSensor) {
			if (const APawn* OwnerPawn = Cast<APawn>(Owner)) {
				if (const AController* Ctrl = OwnerPawn->GetController()) {
					CachedSensor = Ctrl->FindComponentByClass<UResonanceSensorComponent>();
				}
			}
		}
	}

	// 센서를 못 찾으면 제한 없이 잡음 (안전 폴백)
	if (!CachedSensor) return true;

	return CachedSensor->GetThreatLevel() == ESenseLevel::Alert;
}
