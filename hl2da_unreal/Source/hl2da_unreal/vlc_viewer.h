// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <memory>
#include "vlc_viewer.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HL2DA_UNREAL_API Uvlc_viewer : public UActorComponent
{
	GENERATED_BODY()

	UTexture2D* tex;
	int last_fs;
	std::shared_ptr<FUpdateTextureRegion2D> region;

public:	
	// Sets default values for this component's properties
	Uvlc_viewer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int VLC_Index;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
