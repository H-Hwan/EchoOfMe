// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DoorActor.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

#include "Component/NoiseMakerComponent.h"


// Sets default values
ADoorActor::ADoorActor() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorRoot"));
	SetRootComponent(DoorRoot);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(DoorRoot);

	ProximityBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProximityBox"));
	ProximityBox->SetupAttachment(DoorRoot);
	ProximityBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	NoiseComp = CreateDefaultSubobject<UNoiseMakerComponent>(TEXT("NoiseComp"));
}


// Called when the game starts or when spawned
void ADoorActor::BeginPlay() {
	Super::BeginPlay();

	ProximityBox->OnComponentBeginOverlap.AddDynamic(this, &ADoorActor::OnProximityBegin);
	ProximityBox->OnComponentEndOverlap.AddDynamic(this, &ADoorActor::OnProximityEnd);
}


// Called every frame
void ADoorActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (FMath::IsNearlyEqual(CurrentAngle, TargetAngle)) return;

	// 열림/닫힘 방향에 따라 다른 속도 — 닫힐 때 더 느리게
	const bool bOpening = FMath::Abs(TargetAngle) > FMath::Abs(CurrentAngle);
	const float Speed = bOpening ? OpenSpeed : CloseSpeed;

	CurrentAngle = FMath::FInterpConstantTo(CurrentAngle, TargetAngle, DeltaTime, Speed);

	FRotator Rot = DoorMesh->GetRelativeRotation();
	Rot.Yaw = CurrentAngle;
	DoorMesh->SetRelativeRotation(Rot);
}


void ADoorActor::OnProximityBegin(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& Sweep) {
	// 플레이어/잔향 등 폰만 체크
	if (!Cast<APawn>(Other)) return;

	NoiseComp->ReportNoise(1.f);

	OverlapCount++;
	if (bIsLocked) return;

	// 문 정면 기준, 플레이어가 앞쪽인지 뒤쪽인지
	const FVector ToOther = Other->GetActorLocation() - GetActorLocation();
	const float Side = FVector::DotProduct(GetActorForwardVector(), ToOther);

	// 플레이어가 앞(Side>0)이면 뒤로 밀리고, 뒤면 앞으로 밀림
	const float Direction = (Side >= 0.f) ? 1.f : -1.f;

	// 거의 닫힌 상태에서만 방향 결정
	if (FMath::Abs(CurrentAngle) < 5.f) {
		TargetAngle = OpenAngle * Direction;
	}
	// 열리는 중이면 그 방향 유지
	else {
		TargetAngle = OpenAngle * FMath::Sign(CurrentAngle);
	}
}


void ADoorActor::OnProximityEnd(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex) {
	// 플레이어/잔향 등 폰만 체크
	if (!Cast<APawn>(Other)) return;

	GetWorld()->GetTimerManager().SetTimer(DoorCloseTimerHandle, this, &ADoorActor::DoorClose, CloseDelay, false);
}


void ADoorActor::DoorClose() {
	OverlapCount = FMath::Max(0, OverlapCount - 1);
	// 트리거 안에서 모두 나갔다면
	if (OverlapCount == 0) {
		// 아무도 안 밀면 서서히 닫힘
		TargetAngle = 0.f;

		NoiseComp->ReportNoise(1.f);
	}
}


void ADoorActor::Interact_Implementation(AActor* Interactor) {
	// 잠긴 문만 체크
	if (!bIsLocked) return;            

	// 잠금 해제
	bIsLocked = false;	

	/*	TODO(사운드)
		잠금 풀리는 소리 ex) 자물쇠/걸쇠	*/ 

	// 해제 직후 플레이어가 트리거 안에 있으면 바로 열림
	if (OverlapCount > 0) TargetAngle = OpenAngle;
}


FText ADoorActor::GetInteractionPrompt_Implementation() const {
	return bIsLocked ? NSLOCTEXT("Door", "Unlock", "문 열기") : FText::GetEmpty();
}
