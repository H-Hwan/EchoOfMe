// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/NoiseMakerComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
UNoiseMakerComponent::UNoiseMakerComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


void UNoiseMakerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return;

	const FVector CurrentLoc = OwnerChar->GetActorLocation();

	// 첫 기준점 잡고 누적 시작
	if (!bHasInitialLocation) {
		LastFootstepLocation = CurrentLoc;
		bHasInitialLocation = true;
		return;
	}

	// 공중에서 예외처리
	const UCharacterMovementComponent* Move = OwnerChar->GetCharacterMovement();
	if (!Move || Move->IsFalling()) {
		// 착지 후의 갑자기 큰 누적 막기
		LastFootstepLocation = CurrentLoc;
		return;
	}

	// 거의 정지라면 예외처리
	if (OwnerChar->GetVelocity().SizeSquared() < 10.f * 10.f) {
		LastFootstepLocation = CurrentLoc;
		return;
	}

	AccumulatedDistance += FVector::Dist(CurrentLoc, LastFootstepLocation);
	LastFootstepLocation = CurrentLoc;

	// 달리는 중 체크
	const bool bSprinting = OwnerChar->GetVelocity().Size() > 400.f;

	const float StepDistance = bSprinting ? SprintStepDistance : WalkStepDistance;
	const float Loudness = bSprinting ? SprintLoudness : WalkLoudness;

	if (AccumulatedDistance >= StepDistance) {
		ReportNoise(Loudness);
		AccumulatedDistance -= StepDistance;
	}
}


// 1회성 소리 ex) 문 열기, 충돌 등
void UNoiseMakerComponent::ReportNoise(float Loudness) {
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) {
		UE_LOG(LogTemp, Warning, TEXT("[Noise] 참조 실패!!"));
		return;
	}

	OwnerPawn->MakeNoise(Loudness, nullptr, OwnerPawn->GetActorLocation());

	UE_LOG(LogTemp, Log, TEXT("[Noise] %s 소리 발생 (Loudness=%.2f)"), *OwnerPawn->GetName(), Loudness);
}
