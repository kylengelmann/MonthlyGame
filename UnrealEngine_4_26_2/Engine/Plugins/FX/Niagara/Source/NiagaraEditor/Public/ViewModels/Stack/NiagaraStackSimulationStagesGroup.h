// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ViewModels/Stack/NiagaraStackItemGroup.h"
#include "NiagaraStackSimulationStagesGroup.generated.h"

class UNiagaraSimulationStageBase;

UCLASS()
/** Container for one or more NiagaraStackEventScriptItemGroups, allowing multiple event handlers per script.*/
class NIAGARAEDITOR_API UNiagaraStackSimulationStagesGroup : public UNiagaraStackItemGroup
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE(FOnItemAdded);

public:
	void Initialize(FRequiredEntryData InRequiredEntryData);

	void SetOnItemAdded(FOnItemAdded InOnItemAdded);

private:
	void ItemAddedFromUtilties(UNiagaraSimulationStageBase* AddedShaderStage);

private:
	TSharedPtr<INiagaraStackItemGroupAddUtilities> AddUtilities;
	FOnItemAdded ItemAddedDelegate;
};
