// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryWidget.h"

#include "Data/InventoryItemDefinition.h"
#include "Component/InventoryComponent.h"
#include "UI/InventoryEntryWidget.h"

#include "Components/PanelWidget.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/TextBlock.h"


void UInventoryWidget::BindInventory(UInventoryComponent* InInventory) {
	if (!InInventory) return;

	// 이미 다른 인벤토리를 구독 중이면 정리 후 교체 (중복 구독 방지)
	if (InventoryComp && InventoryComp != InInventory) {
		InventoryComp->OnInventoryChanged.RemoveDynamic(this, &UInventoryWidget::RefreshItems);
	}

	InventoryComp = InInventory;
	if (InventoryComp) {
		InventoryComp->OnInventoryChanged.AddDynamic(this, &UInventoryWidget::RefreshItems);
		RefreshItems();
	}
}


void UInventoryWidget::NativeDestruct() {
	if (InventoryComp) {
		InventoryComp->OnInventoryChanged.RemoveDynamic(this, &UInventoryWidget::RefreshItems);
	}

	Super::NativeDestruct();
}


void UInventoryWidget::RefreshItems() {
	if (!ItemList || !InventoryComp || !EntryWidgetClass) return;

	ItemList->ClearChildren();

	for (UInventoryItemDefinition* Item : InventoryComp->GetItem()) {
		if (!Item) continue;

		UInventoryEntryWidget* Entry = CreateWidget<UInventoryEntryWidget>(this, EntryWidgetClass);
		if (Entry) {
			Entry->SetVisibility(ESlateVisibility::Visible);
			Entry->SetItem(Item);
			Entry->OnEntryClicked.AddDynamic(this, &UInventoryWidget::HandleEntryClicked);

			UPanelSlot* EntrySlot = ItemList->AddChild(Entry);
			if (UScrollBoxSlot* SBSlot = Cast<UScrollBoxSlot>(EntrySlot))
			{
				SBSlot->SetHorizontalAlignment(HAlign_Fill);
			}
		}
	}
}


void UInventoryWidget::HandleEntryClicked(UInventoryItemDefinition* Item) {
	if (InventoryComp) {
		InventoryComp->UseItem(Item);
	}
}
