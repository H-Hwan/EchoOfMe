// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryEntryWidget.h"

#include "Data/InventoryItemDefinition.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"


void UInventoryEntryWidget::SetItem(UInventoryItemDefinition* InItem) {
	Item = InItem;

	if (!Item) return;

	if (Text_Name) {
		Text_Name->SetText(Item->DisplayName);
	}

	if (Text_Sub) {
		Text_Sub->SetText(Item->Description);
	}

	if (Img_Icon) {
		if (Item->Icon) {
			Img_Icon->SetBrushFromTexture(Item->Icon);
			Img_Icon->SetVisibility(ESlateVisibility::Visible);
		}
		else {
			Img_Icon->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UInventoryEntryWidget::NativeConstruct() {
	Super::NativeConstruct();

	UWidget* Root = GetRootWidget();

	if (USizeBox* RootBox = Cast<USizeBox>(GetRootWidget())) {
		RootBox->SetWidthOverride(900.f);
		RootBox->SetHeightOverride(150.f);
	}
}

FReply UInventoryEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && Item) {
		OnEntryClicked.Broadcast(Item);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}
