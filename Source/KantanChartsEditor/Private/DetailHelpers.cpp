// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#include "KantanChartsEditor.h"
#include "DetailHelpers.h"
#include "PropertyHandle.h"
#include "ContentBrowserModule.h"


namespace DetailHelpers {

	TSharedRef< SWidget > MakeInterfaceAssetPicker(
		TSharedRef< IPropertyHandle > Property,
		TSubclassOf< UInterface > InterfaceClass
		)
	{
		/* @TODO:
		This will need some further compatibility checks, to ensure that the property is not only an object
		property (checked below, though perhaps not in the best way?), but that InterfaceClass is compatible
		with the property type.
		*/

		if (Property->IsValidHandle() == false ||
			InterfaceClass == nullptr ||
			Property->GetPropertyClass() != UObjectProperty::StaticClass()
			)
		{
			return SNullWidget::NullWidget;
		}

		auto OnAssetSelected = [Property](const class FAssetData& AssetData)
		{
			if (Property->IsValidHandle())
			{
				const UObject* Asset = AssetData.GetAsset();
				Property->SetValue(Asset);
			}
		};

		auto IsValidAsset = [InterfaceClass](const class FAssetData& AssetData) -> bool
		{
			// @TODO: Calling GetAsset causes all incoming assets to be force-loaded if not already,
			// which is clearly not going to work. 
			// Can't get class if the asset is not loaded though, so can't determine if it implements an interface.
			// Currently only considering already loaded assets, alternative is to make a UObject base class for
			// static datasource assets.
			auto Cls = AssetData.GetClass();
			return Cls == nullptr || Cls->ImplementsInterface(InterfaceClass) == false;
		};

		auto OnGetMenuContent = [Property, OnAssetSelected, IsValidAsset]() -> TSharedRef< SWidget >
		{
			auto& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked< FContentBrowserModule >(TEXT("ContentBrowser"));

			UObject* InitialObject = nullptr;
			if (Property->IsValidHandle())
			{
				Property->GetValue(InitialObject);
			}

			FAssetPickerConfig AssetPickerConfig;
			// We want any valid object implementing the interface, so any child of UObject as the filter
			AssetPickerConfig.Filter.ClassNames.Add(UObject::StaticClass()->GetFName());
			// Allow child classes
			AssetPickerConfig.Filter.bRecursiveClasses = true;
			// Set a delegate for setting the asset from the picker
			AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateLambda(OnAssetSelected);
			// Use the list view by default
			AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
			// The initial selection should be the current value
			AssetPickerConfig.InitialAssetSelection = FAssetData(InitialObject);
			// We'll do clearing ourselves
			AssetPickerConfig.bAllowNullSelection = true;
			// Focus search box
			AssetPickerConfig.bFocusSearchBoxWhenOpened = true;
			// Apply custom filter
			AssetPickerConfig.OnShouldFilterAsset = FOnShouldFilterAsset::CreateLambda(IsValidAsset);
			// Don't allow dragging
			AssetPickerConfig.bAllowDragging = false;
			// Save the settings into a special section for asset pickers for properties
			//AssetPickerConfig.SaveSettingsName = TEXT("AssetPropertyPicker");

			return SNew(SBox)
				.WidthOverride(300.0f)
				.HeightOverride(300.0f)
				[
					ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
				];
		};

		auto OnGetCurrentName = [Property]() -> FText
		{
			FText Text = FText::GetEmpty();
			Property->GetValueAsDisplayText(Text);
			return Text;
		};

		auto ComboButton = SNew(SComboButton)
			//		.ToolTipText(this, &SPropertyEditorAsset::OnGetToolTip)
			//		.ButtonStyle(FEditorStyle::Get(), "PropertyEditor.AssetComboStyle")
			//		.ForegroundColor(FEditorStyle::GetColor("PropertyEditor.AssetName.ColorAndOpacity"))
			.OnGetMenuContent_Lambda(OnGetMenuContent)
			//.OnMenuOpenChanged(this, &SPropertyEditorAsset::OnMenuOpenChanged)
			//.IsEnabled(IsEnabledAttribute)
			.ContentPadding(2.0f)
			.ButtonContent()
			[
				// Show the name of the asset or actor
				SNew(STextBlock)
				.Font(FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
				.Text_Lambda(OnGetCurrentName)
			];

		return ComboButton;
	}

}

