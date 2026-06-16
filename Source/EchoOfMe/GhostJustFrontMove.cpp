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
}

void AGhostJustFrontMove::BeginPlay()
{
	Super::BeginPlay();

	// [수정됨] AHorrorPassbyEvent -> AGhostJustFrontMove 로 클래스 이름 변경
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AGhostJustFrontMove::OnOverlapBegin);

	// 시작할 때 귀신 액터를 목적지 방향으로 쳐다보게 회전시켜둠
	if (SpookyActor && DestinationPoint)
	{
		FVector Direction = DestinationPoint->GetActorLocation() - SpookyActor->GetActorLocation();
		SpookyActor->SetActorRotation(Direction.Rotation());
	}
}

void AGhostJustFrontMove::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 이벤트가 발동되었고, 관련 액터들이 유효하다면 이동 시작
	if (bIsTriggered && SpookyActor && DestinationPoint)
	{
		FVector CurrentLoc = SpookyActor->GetActorLocation();
		FVector TargetLoc = DestinationPoint->GetActorLocation();

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
	// 이미 발동되었거나 밟은 사람이 플레이어가 아니면 무시
	if (bIsTriggered || !OtherActor->IsA(ACharacter::StaticClass())) return;

	// 이벤트 발동
	bIsTriggered = true;

	// 콜리전 비활성화 (두 번 다시 밟히지 않도록)
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UE_LOG(LogTemp, Warning, TEXT("호러 이벤트 발동! 귀신이 지나갑니다."));
}
