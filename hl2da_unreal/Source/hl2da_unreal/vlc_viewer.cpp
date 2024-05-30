// Fill out your copyright notice in the Description page of Project Settings.


#include "vlc_viewer.h"
#include "hl2da_api.h"
#include "hl2da_framebuffer.h"

// Sets default values for this component's properties
Uvlc_viewer::Uvlc_viewer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void Uvlc_viewer::BeginPlay()
{
	Super::BeginPlay();

	// ...
	hl2da_api::InitializeLibrary();
	hl2da_api::InitializeComponents();
	hl2da_api::Initialize(0, 30);
	hl2da_api::SetEnable(0, true);

	AActor* top = GetOwner();
	UStaticMeshComponent* mesh = top->FindComponentByClass<UStaticMeshComponent>();
	UMaterialInstanceDynamic* mat = mesh->CreateAndSetMaterialInstanceDynamic(0);

	tex = UTexture2D::CreateTransient(640, 480, PF_R8);
	tex->UpdateResource();
	mat->SetTextureParameterValue(FName("MainTexture"), tex);

	last_fs = -1;
	region = std::make_shared<FUpdateTextureRegion2D>(0, 0, 0, 0, 640, 480);
}


// Called every frame
void Uvlc_viewer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	std::shared_ptr<hl2da_framebuffer> fb = hl2da_framebuffer::GetFrame(0, -1);
	if (fb->Status() != 0) { return; }
	if (fb->Framestamp() <= last_fs) { return; }
	last_fs = fb->Framestamp();

	tex->UpdateTextureRegions(0, 1, region.get(), 640 * 1, 1, (uint8_t*)fb->Buffer(0), [fb](uint8*, FUpdateTextureRegion2D const*) { fb->Destroy(); });
}
