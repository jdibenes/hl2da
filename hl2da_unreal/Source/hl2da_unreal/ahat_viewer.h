// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <memory>
#include "ahat_viewer.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HL2DA_UNREAL_API Uahat_viewer : public UActorComponent
{
	GENERATED_BODY()

	UTexture2D* tex[2];
	int last_fs;
	std::shared_ptr<FUpdateTextureRegion2D> region;

public:	
	// Sets default values for this component's properties
	Uahat_viewer();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
