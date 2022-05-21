// Fill out your copyright notice in the Description page of Project Settings.


#include "SSaveGameSettings.h"

USSaveGameSettings::USSaveGameSettings()
{
	/* In case that the slot name is not overwritten in the project settings, provide a "default for the default" at least*/
	SaveSlotName = TEXT("UnchangedSaveSlotName");
}
