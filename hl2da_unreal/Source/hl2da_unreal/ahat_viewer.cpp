// Fill out your copyright notice in the Description page of Project Settings.


#include "ahat_viewer.h"
#include "hl2da_api.h"
#include "hl2da_framebuffer.h"

// Sets default values for this component's properties
Uahat_viewer::Uahat_viewer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void Uahat_viewer::BeginPlay()
{
	Super::BeginPlay();

	// ...
	AActor* top = GetOwner();
	TArray<AActor*> children;
	top->GetAllChildActors(children, true);

	for (int i = 0; i < 2; ++i)
	{
		AActor* actor = children[i];
		UStaticMeshComponent* mesh = actor->FindComponentByClass<UStaticMeshComponent>();
		UMaterialInstanceDynamic* mat = mesh->CreateAndSetMaterialInstanceDynamic(0);

		tex[i] = UTexture2D::CreateTransient(512, 512, PF_G16);
		tex[i]->UpdateResource();
		mat->SetTextureParameterValue(FName("MainTexture"), tex[i]);
		if (i != 0) { continue; }
		mat->SetScalarParameterValue(FName("Left"), 0.0f);
		mat->SetScalarParameterValue(FName("Right"), 1055.0f / 65535.0f);
	}

	last_fs = -1;
	region = std::make_shared<FUpdateTextureRegion2D>(0, 0, 0, 0, 512, 512);
}


// Called every frame
void Uahat_viewer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	std::shared_ptr<hl2da_framebuffer> fb = hl2da_framebuffer::GetFrame(hl2da_api::SENSOR_ID::RM_DEPTH_AHAT, -1);
	if (fb->Status() != hl2da_api::STATUS::OK) { return; }
	if (fb->Framestamp() <= last_fs) { return; }
	last_fs = fb->Framestamp();

	hl2da_api::IMT_ZHTInvalidate((uint16_t*)fb->Buffer(0), (uint16_t*)fb->Buffer(0));

	tex[0]->UpdateTextureRegions(0, 1, region.get(), 512 * 2, 2, (uint8_t*)fb->Buffer(0), [fb](uint8*, FUpdateTextureRegion2D const*) { });
	tex[1]->UpdateTextureRegions(0, 1, region.get(), 512 * 2, 2, (uint8_t*)fb->Buffer(1), [fb](uint8*, FUpdateTextureRegion2D const*) { });
}

