// Copyright (C) 2015 Cameron Angus. All Rights Reserved.

#pragma once

#include "SWidget.h"
#include "Templates/SharedPointer.h"


class IPropertyHandle;
class UInterface;

namespace DetailHelpers {

	TSharedRef< SWidget > MakeInterfaceAssetPicker(
		TSharedRef< IPropertyHandle > Property,
		TSubclassOf< UInterface > InterfaceClass
		);

}

