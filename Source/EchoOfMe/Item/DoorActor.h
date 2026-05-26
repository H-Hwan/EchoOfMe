// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Interface/Interactable.h"

#include "DoorActor.generated.h"


class UBoxComponent;


UCLASS()
class ECHOOFME_API ADoorActor : public AActor, public IInteractable {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// [IInteractable] — 잠긴 문을 상호작용으로 열 때 */
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

protected:
	// 근접했을 때 발동될 이벤트
	UFUNCTION()
	void OnProximityBegin(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& Sweep);
	// 떠날 때 발동될 이벤트
	UFUNCTION()
	void OnProximityEnd(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex);

	// 회전 축
	UPROPERTY(VisibleAnywhere, Category = "Door")
	TObjectPtr<USceneComponent> DoorRoot;

	// 문 메시
	UPROPERTY(VisibleAnywhere, Category = "Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	// 다가감 감지 콜리전
	UPROPERTY(VisibleAnywhere, Category = "Door")
	TObjectPtr<UBoxComponent> ProximityBox;

	// 잠긴 문 여부
	UPROPERTY(EditAnywhere, Category = "Door")
	bool bIsLocked = false;

	// 열리는 각도
	UPROPERTY(EditAnywhere, Category = "Door", meta = (ClampMin = "0"))
	float OpenAngle = 90.f;

	// 열릴 때 속도
	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenSpeed = 220.f;

	// 닫힐 때 속도
	UPROPERTY(EditAnywhere, Category = "Door")
	float CloseSpeed = 90.f;

private:
	// 현재 각도
	float CurrentAngle = 0.f;
	// 도달할 각도
	float TargetAngle = 0.f;
	// 트리거 안에 존재하는 액터 수
	int32 OverlapCount = 0;
};
