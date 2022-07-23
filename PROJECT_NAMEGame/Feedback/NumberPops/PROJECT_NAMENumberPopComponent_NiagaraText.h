// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "<PROJECT_NAME>NumberPopComponent.h"
#include "<PROJECT_NAME>NumberPopComponent_NiagaraText.generated.h"

class U<PROJECT_NAME>DamagePopStyleNiagara;

UCLASS(Blueprintable)
class U<PROJECT_NAME>NumberPopComponent_NiagaraText : public U<PROJECT_NAME>NumberPopComponent
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>NumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~U<PROJECT_NAME>NumberPopComponent interface
	virtual void AddNumberPop(const F<PROJECT_NAME>NumberPopRequest& NewRequest) override;
	//~End of U<PROJECT_NAME>NumberPopComponent interface

protected:
	
	TArray<int32> DamageNumberArray;

	/** Style patterns to attempt to apply to the incoming number pops */
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	U<PROJECT_NAME>DamagePopStyleNiagara* Style;

	//Niagara Component used to display the damage
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	UNiagaraComponent* NiagaraComp;
};
