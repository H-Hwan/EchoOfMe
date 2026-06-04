// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/EquipmentComponent.h"

#include "Component/FlashlightComponent.h"

#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"
#include "Engine/World.h"


UEquipmentComponent::UEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}


void UEquipmentComponent::SetEquipmentMeshes(USceneComponent* InFlashlightMesh, USceneComponent* InRecorderMesh, UFlashlightComponent* InFlashlight)
{
    FlashlightMesh = InFlashlightMesh;
    RecorderMesh = InRecorderMesh;
    Flashlight = InFlashlight;
}


void UEquipmentComponent::RequestFlashlight()
{
    if (bIsSwitching) return;

    const EEquipmentSlot Target = (CurrentEquipment == EEquipmentSlot::Flashlight)
        ? EEquipmentSlot::None : EEquipmentSlot::Flashlight;

    StartSwitch(Target);
}


void UEquipmentComponent::RequestRecorder()
{
    if (bIsSwitching) return;

    const EEquipmentSlot Target = (CurrentEquipment == EEquipmentSlot::Recorder)
        ? EEquipmentSlot::None : EEquipmentSlot::Recorder;

    StartSwitch(Target);
}


void UEquipmentComponent::StartSwitch(EEquipmentSlot NewTarget)
{
    if (CurrentEquipment == NewTarget) return;

    bIsSwitching = true;
    TargetEquipment = NewTarget;

    // 현재 들고 있는 게 있으면 집어넣기부터
    if (CurrentEquipment != EEquipmentSlot::None)
    {
        // 손전등 집어넣을 때 자동 OFF
        if (CurrentEquipment == EEquipmentSlot::Flashlight && Flashlight)
        {
            Flashlight->SetFlashLightOn(false);
        }

        UAnimMontage* HolsterMontage = (CurrentEquipment == EEquipmentSlot::Flashlight)
            ? FlashlightHolsterMontage
            : RecorderHolsterMontage;

        PlayMontage(HolsterMontage, &UEquipmentComponent::HandleHolsterFinished);
    }
    else
    {
        // 빈손이었으면 바로 꺼내기
        HandleHolsterFinished();
    }
}


void UEquipmentComponent::HandleHolsterFinished()
{
    // 집어넣기 끝남 — 메시 숨기고 빈손 상태
    if (CurrentEquipment != EEquipmentSlot::None)
    {
        ShowMesh(CurrentEquipment, false);
        CurrentEquipment = EEquipmentSlot::None;
    }

    // 빈손이 최종 목표면 여기서 끝
    if (TargetEquipment == EEquipmentSlot::None)
    {
        bIsSwitching = false;
        OnEquipmentChanged.Broadcast(EEquipmentSlot::None);
        UE_LOG(LogTemp, Log, TEXT("[Equipment] 빈손 상태"));
        return;
    }

    // 새 장비 꺼내기
    UAnimMontage* DrawMontage = (TargetEquipment == EEquipmentSlot::Flashlight)
        ? FlashlightDrawMontage
        : RecorderDrawMontage;

    ShowMesh(TargetEquipment, true);   // 꺼내는 동안 손에 보여야 함
    PlayMontage(DrawMontage, &UEquipmentComponent::HandleDrawFinished);
}


void UEquipmentComponent::HandleDrawFinished()
{
    CurrentEquipment = TargetEquipment;
    bIsSwitching = false;

    OnEquipmentChanged.Broadcast(CurrentEquipment);

    UE_LOG(LogTemp, Log, TEXT("[Equipment] 장비 전환 완료: %d"), (int32)CurrentEquipment);
}


void UEquipmentComponent::ShowMesh(EEquipmentSlot Slot, bool bVisible)
{
    USceneComponent* Target = nullptr;

    switch (Slot)
    {
        case EEquipmentSlot::Flashlight: Target = FlashlightMesh; break;
        case EEquipmentSlot::Recorder:   Target = RecorderMesh; break;
        default: return;
    }

    if (Target)
    {
        Target->SetVisibility(bVisible, true);   // 자식까지 propagate
    }
}


void UEquipmentComponent::PlayMontage(UAnimMontage* Montage, void (UEquipmentComponent::*Callback)())
{
    // 몽타주 없으면 즉시 콜백 — 1차 빌드 중 몽타주 미할당 상태에서도 흐름은 굴러가게
    if (!Montage)
    {
        (this->*Callback)();
        return;
    }

    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (!OwnerChar)
    {
        (this->*Callback)();
        return;
    }

    USkeletalMeshComponent* SkelMesh = OwnerChar->GetMesh();
    if (!SkelMesh)
    {
        (this->*Callback)();
        return;
    }

    UAnimInstance* AnimInst = SkelMesh->GetAnimInstance();
    if (!AnimInst)
    {
        (this->*Callback)();
        return;
    }

    const float Duration = AnimInst->Montage_Play(Montage, 1.f);

    if (Duration <= 0.f)
    {
        // 재생 실패 시 즉시 콜백
        (this->*Callback)();
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        (this->*Callback)();
        return;
    }

    World->GetTimerManager().SetTimer(
        MontageTimerHandle,
        FTimerDelegate::CreateUObject(this, Callback),
        Duration,
        false
    );
}
