// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GhostJustFrontMove.generated.h"

class UBoxComponent;

UCLASS()
class ECHOOFME_API AGhostJustFrontMove : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGhostJustFrontMove();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 트리거 박스 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Trigger Event")
	UBoxComponent* TriggerBox;

	// 지나갈 귀신 액터
	UPROPERTY(EditInstanceOnly, Category = "Trigger Event")
	AActor* SpookyActor;

	// 도착 지점 액터
	UPROPERTY(EditInstanceOnly, Category = "Trigger Event")
	AActor* DestinationPoint;

	// 이동 속도
	UPROPERTY(EditAnywhere, Category = "Trigger Event")
	float MoveSpeed = 2000.0f;
	UFUNCTION(BlueprintImplementableEvent, Category = "Sound")

	void NowSound();
private:
	// 단발성 이벤트 체크용
	bool bIsTriggered;



	// 오버랩 이벤트 함수
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
