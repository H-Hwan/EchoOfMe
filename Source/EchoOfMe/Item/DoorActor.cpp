// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DoorActor.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"


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
	const bool bOpening = TargetAngle > CurrentAngle;
	const float Speed = bOpening ? OpenSpeed : CloseSpeed;

	CurrentAngle = FMath::FInterpConstantTo(CurrentAngle, TargetAngle, DeltaTime, Speed);

	FRotator Rot = DoorMesh->GetRelativeRotation();
	Rot.Yaw = CurrentAngle;
	DoorMesh->SetRelativeRotation(Rot);
}


void ADoorActor::OnProximityBegin(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& Sweep) {
	// 플레이어/잔향 등 폰만 체크
	if (!Cast<APawn>(Other)) return;

	OverlapCount++;
	// 잠기지 않았으면
	if (!bIsLocked)	{
		// 다가오는 것만으로 열림
		TargetAngle = OpenAngle;

		/*	TODO(사운드)
			문 열림 시작음 ex) 삐걱이는 경첩	*/ 
	}
}


void ADoorActor::OnProximityEnd(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex) {
	// 플레이어/잔향 등 폰만 체크
	if (!Cast<APawn>(Other)) return;

	OverlapCount = FMath::Max(0, OverlapCount - 1);
	// 트리거 안에서 모두 나갔다면
	if (OverlapCount == 0) 	{
		// 아무도 안 밀면 서서히 닫힘
		TargetAngle = 0.f;

		/*	 TODO(사운드)
			문 닫힘음 ex) 천천히 닫히는 소리	*/
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
