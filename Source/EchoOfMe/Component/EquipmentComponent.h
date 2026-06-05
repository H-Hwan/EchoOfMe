// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"


class UAnimMontage;
class UFlashlightComponent;

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	None        UMETA(DisplayName="빈손"),
	Flashlight  UMETA(DisplayName="손전등"),
	Recorder    UMETA(DisplayName="녹음기")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentChanged, EEquipmentSlot, NewSlot);


UCLASS( ClassGroup=(EchoOfMe), meta=(BlueprintSpawnableComponent) )
class ECHOOFME_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEquipmentComponent();


	//---
	// 입력 요청
public:
	// 손전등 토글
	UFUNCTION(BlueprintCallable, Category="Equipment")
	void RequestFlashlight();

	// 녹음기 토글
	UFUNCTION(BlueprintCallable, Category="Equipment")
	void RequestRecorder();


	//---
	// 상태 조회
public:
	UFUNCTION(BlueprintPure, Category="Equipment")
	EEquipmentSlot GetCurrentEquipment() const { return CurrentEquipment; }

	UFUNCTION(BlueprintPure, Category="Equipment")
	EEquipmentSlot GetAnimEquipment() const { return CurrentEquipment; }

	UFUNCTION(BlueprintPure, Category="Equipment")
	bool IsSwitching() const { return bIsSwitching; }


	//---
	// 메시 / 라이트 등록 (캐릭터 BeginPlay에서 호출)
public:
	void SetEquipmentMeshes(USceneComponent* InFlashlightMesh, USceneComponent* InRecorderMesh, UFlashlightComponent* InFlashlight);


	//---
	// 방송
public:
	// 장비 전환 완료 시 방송
	UPROPERTY(BlueprintAssignable, Category="Equipment")
	FOnEquipmentChanged OnEquipmentChanged;

	//---
	// 몽타주
protected:
	UPROPERTY(EditAnywhere, Category="Equipment|Anim")
	TObjectPtr<UAnimMontage> FlashlightDrawMontage;
	UPROPERTY(EditAnywhere, Category="Equipment|Anim")
	TObjectPtr<UAnimMontage> FlashlightHolsterMontage;
	UPROPERTY(EditAnywhere, Category="Equipment|Anim")
	TObjectPtr<UAnimMontage> RecorderDrawMontage;
	UPROPERTY(EditAnywhere, Category="Equipment|Anim")
	TObjectPtr<UAnimMontage> RecorderHolsterMontage;


	//---
	// 내부 흐름
private:
	void StartSwitch(EEquipmentSlot TargetSlot);
	void HandleHolsterFinished();
	void HandleDrawFinished();

	void ShowMesh(EEquipmentSlot Slot, bool bVisible);
	void PlayMontage(UAnimMontage* Montage, void (UEquipmentComponent::*Callback)());


private:
	EEquipmentSlot CurrentEquipment = EEquipmentSlot::None;
	EEquipmentSlot TargetEquipment = EEquipmentSlot::None;
	EEquipmentSlot HolsteringEquipment = EEquipmentSlot::None;
	bool bIsSwitching = false;

	// 녹음기/손전등 메시 참조
	UPROPERTY()
	TObjectPtr<USceneComponent> FlashlightMesh;
	UPROPERTY()
	TObjectPtr<USceneComponent> RecorderMesh;
	UPROPERTY()
	TObjectPtr<UFlashlightComponent> Flashlight;

	FTimerHandle MontageTimerHandle;
};
