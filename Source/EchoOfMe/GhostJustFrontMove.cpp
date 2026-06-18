// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostJustFrontMove.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h" // [추가됨] 박스 콜리전 헤더
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AGhostJustFrontMove::AGhostJustFrontMove()
{
	PrimaryActorTick.bCanEverTick = true;

	// 트리거 박스 생성 및 초기화
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	// 플레이어만 감지하도록 콜리전 셋팅
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));

	bIsTriggered = false;
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);
}

void AGhostJustFrontMove::BeginPlay()
{
	Super::BeginPlay();

	// [수정됨] AHorrorPassbyEvent -> AGhostJustFrontMove 로 클래스 이름 변경
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AGhostJustFrontMove::OnOverlapBegin);

	SpookyActor->SetActorHiddenInGame(true);
	
}

void AGhostJustFrontMove::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 이벤트가 발동되었고, 관련 액터들이 유효하다면 이동 시작
	if (bIsTriggered && SpookyActor && DestinationPoint)
	{
		if (bIsTriggered)
		{
			UE_LOG(LogTemp, Warning, TEXT("Triggered"));
		}

		if (!SpookyActor)
		{
			UE_LOG(LogTemp, Error, TEXT("SpookyActor NULL"));
		}

		if (!DestinationPoint)
		{
			UE_LOG(LogTemp, Error, TEXT("DestinationPoint NULL"));
		}
		FVector CurrentLoc = SpookyActor->GetActorLocation();
		FVector TargetLoc = DestinationPoint->GetActorLocation();
		TargetLoc.Z = 0.0f;
		// VInterpConstantTo: 처음부터 끝까지 일정한 속도로 이동
		FVector NewLoc = FMath::VInterpConstantTo(CurrentLoc, TargetLoc, DeltaTime, MoveSpeed);
		SpookyActor->SetActorLocation(NewLoc);

		// 목적지에 거의 다 도달했는지 확인 (오차 범위 20.0f)
		if (FVector::Dist(NewLoc, TargetLoc) < 20.0f)
		{
			// 목적지 도달 시 귀신 액터 즉시 삭제
			SpookyActor->Destroy();

			// 역할을 다한 트리거 액터도 삭제
			this->Destroy();
		}
	}
}

void AGhostJustFrontMove::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsTriggered || !OtherActor->IsA(APawn::StaticClass())) return;
	NowSound();
	bIsTriggered = true;
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpookyActor->SetActorHiddenInGame(false);
	// 1. 일단 콜리전이 제대로 밟혔는지 확인하는 로그 (노란색)
	UE_LOG(LogTemp, Warning, TEXT("✅ [호러 이벤트] 플레이어가 트리거를 밟았습니다!"));

	// 2. 에디터에서 액터 할당을 깜빡했는지 검사하는 로그 (빨간색 에러)
	if (!SpookyActor)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ [에러] SpookyActor(지나갈 귀신)가 에디터에서 지정되지 않았습니다!"));
	}
	if (!DestinationPoint)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ [에러] DestinationPoint(도착 지점)가 에디터에서 지정되지 않았습니다!"));
	}
}
