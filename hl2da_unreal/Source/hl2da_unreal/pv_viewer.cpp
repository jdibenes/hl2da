// Fill out your copyright notice in the Description page of Project Settings.


#include "pv_viewer.h"
#include "hl2da_api.h"
#include "hl2da_framebuffer.h"
#include "hl2da_imt.h"

// Sets default values for this component's properties
Upv_viewer::Upv_viewer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void Upv_viewer::BeginPlay()
{
	Super::BeginPlay();

	// ...
	AActor* top = GetOwner();
	UStaticMeshComponent* mesh = top->FindComponentByClass<UStaticMeshComponent>();
	UMaterialInstanceDynamic* mat = mesh->CreateAndSetMaterialInstanceDynamic(0);

	tex = UTexture2D::CreateTransient(640, 360, PF_B8G8R8A8);
	tex->UpdateResource();
	mat->SetTextureParameterValue(FName("MainTexture"), tex);

	last_fs = -1;
	region = std::make_shared<FUpdateTextureRegion2D>(0, 0, 0, 0, 640, 360);
}


// Called every frame
void Upv_viewer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	std::shared_ptr<hl2da_framebuffer> fb = hl2da_framebuffer::GetFrame(hl2da_api::SENSOR_ID::PV, -1);
	if (fb->Status() != hl2da_api::STATUS::OK) { return; }
	if (fb->Framestamp() <= last_fs) { return; }
	last_fs = fb->Framestamp();

	std::shared_ptr<hl2da_imt> fc = hl2da_imt::Convert((uint8_t*)fb->Buffer(0), hl2da_imt::GetStride_PV(640), 360, hl2da_api::IMT_Format::Nv12, hl2da_api::IMT_Format::Bgra8);
	
	tex->UpdateTextureRegions(0, 1, region.get(), 640 * 4, 4, (uint8_t*)fc->Buffer(), [fc](uint8*, FUpdateTextureRegion2D const*) {});
}

