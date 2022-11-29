// Copyright Gargowle. All Rights Reserved.


#include "SSaveGameSettings.h"

USSaveGameSettings::USSaveGameSettings()
{
	/* In case that the slot name is not overwritten in the project settings, provide a "default for the default" at least*/
	SaveSlotName = TEXT("UnchangedSaveSlotName");
}
