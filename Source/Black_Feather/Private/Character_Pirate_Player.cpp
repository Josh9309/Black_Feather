// Copyright 2020 Josh MalmquistAll Rights Reserved


#include "Character_Pirate_Player.h"
#include "DrawDebugHelpers.h" //Debug Draws

// Sets default values
ACharacter_Pirate_Player::ACharacter_Pirate_Player(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	m_camera = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, FName("PirateCamera"));// CreateDefaultSubobject<UCameraComponent>(FName("PirateCamera"));
	USkeletalMeshComponent* playerMesh = FindComponentByClass<USkeletalMeshComponent>();
	if (m_camera && playerMesh)
	{
		m_camera->SetupAttachment(playerMesh, "Pirate_CameraFocus");
	}
}

#pragma region Base Character Core Functions
// Called when the game starts or when spawned
void ACharacter_Pirate_Player::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacter_Pirate_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector vLocation = GetActorLocation();
	const FVector vForward = GetActorRotation().Vector().GetSafeNormal();
	const FVector vRight = FVector::CrossProduct(FVector::UpVector, vForward);

	//DrawDebugLine(GetWorld(), vLocation, vLocation + vForward * 500.0f, FColor::Blue);
	//DrawDebugLine(GetWorld(), vLocation, vLocation + vRight * 500.0f, FColor::Green);
	if (const USkeletalMeshComponent* pirateMesh = GetMesh())
	{
		FVector vSocketLocation;
		FRotator rSocketRotation;
		pirateMesh->GetSocketWorldLocationAndRotation("Pirate_CameraFocus", vSocketLocation, rSocketRotation);
		const FVector vCamLoc = m_camera->GetComponentLocation();

		if (UWorld* world = GetWorld())
		{
			UE_LOG(LogTemp, Display, TEXT("Socket Location X: %f, Y: %f Z: %f"), vSocketLocation.X, vSocketLocation.Y, vSocketLocation.Z);
			UE_LOG(LogTemp, Display, TEXT("Cam Location X: %f, Y: %f Z: %f"), vCamLoc.X, vCamLoc.Y, vCamLoc.Z);
			//DrawDebugLine(world, vSocketLocation, vCamLoc, FColor::Purple);
			//
			const FVector vToCamera = vCamLoc - vSocketLocation;
			//DrawDebugLine(world, vSocketLocation, vSocketLocation + vToCamera, FColor::Green);

			const FRotator rCamYawRotation = FRotator(0.0f, 30.0f, 0.0f);
			const FVector vToNewCamLocation = rCamYawRotation.RotateVector(vToCamera);
			//DrawDebugLine(world, vSocketLocation, vSocketLocation + vToNewCamLocation, FColor::Green);
		}
	}

}
#pragma endregion

// Called to bind functionality to input
void ACharacter_Pirate_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent)

	PlayerInputComponent->BindAxis(FName("Forward"), this, &ACharacter_Pirate_Player::MoveForward);
	PlayerInputComponent->BindAxis(FName("Right"), this, &ACharacter_Pirate_Player::MoveRight);
	PlayerInputComponent->BindAxis(FName("Jump"), this, &ACharacter_Pirate_Player::MoveJump);
	PlayerInputComponent->BindAxis(FName("Camera Rotate Pitch"), this, &ACharacter_Pirate_Player::RotateCamPitch);
	PlayerInputComponent->BindAxis(FName("Camera Rotate Yaw"), this, &ACharacter_Pirate_Player::RotateCamYaw);
}

#pragma region Movement Logic
void ACharacter_Pirate_Player::MoveForward(const float fInputValue)
{
	const FVector vForward = m_camera && m_bUseCameraForward ? m_camera->GetForwardVector().GetSafeNormal() : GetActorRotation().Vector().GetSafeNormal();
	AddMovementInput(vForward, fInputValue);
}

void ACharacter_Pirate_Player::MoveRight(const float fInputValue)
{
	const FVector vForward = m_camera && m_bUseCameraForward ? m_camera->GetForwardVector().GetSafeNormal() : GetActorRotation().Vector().GetSafeNormal();
	const FVector vRight = FVector::CrossProduct(FVector::UpVector, vForward);
	AddMovementInput(vRight, fInputValue);
}

void ACharacter_Pirate_Player::MoveJump(const float fInputValue)
{
	if (fInputValue > 0.0f)
	{
		Jump();
	}
	else
	{
		StopJumping();
	}
}

FVector ACharacter_Pirate_Player::RotateAroundPoint(const float fRotationAmount, const FVector vRotationPoint, const FVector vPointToRotate) const
{
	return FVector::ZeroVector;
	//const FVector vToPoint = (vPointToRotate - vRotationPoint).GetSafeNormal();
	//const float fDistanceFromRotationPoint = FVector::Distance(vRotationPoint, vPointToRotate);

	//FRotator rRotation = (vToPoint * fDistanceFromRotationPoint).Rotation();
	//rRotation.Yaw += fRotationAmount;

	//const FVector vRotatedPoint = vPointToRotate + rRotation.Vector() * fDistanceFromRotationPoint;

	////DrawDebugLine(GetWorld(), vRotationPoint, vRotationPoint + vPointToRotate * fDistanceFromRotationPoint, FColor::Blue);
	//DrawDebugLine(GetWorld(), vRotationPoint, vRotationPoint + rRotation.Vector() * fDistanceFromRotationPoint, FColor::Green);

	//return vRotatedPoint;
}

void ACharacter_Pirate_Player::RotateCamYaw(const float fInputValue)
{
	const USkeletalMeshComponent* pirateMesh = GetMesh();
	if (!pirateMesh || !m_camera || fInputValue == 0.0f) { return; }

	FVector vSocketLocation;
	FRotator rSocketRotation;
	pirateMesh->GetSocketWorldLocationAndRotation("Pirate_CameraFocus", vSocketLocation, rSocketRotation);
	DrawDebugSphere(GetWorld(), vSocketLocation, 100, 20, FColor::Blue);

	//Rotate Camera Location
	const FRotator rCamYawRotation = FRotator(0.0f, fInputValue * m_fCameraYawRotationAmount, 0.0f);
	const FVector vSocketToCurrentCamLocation = m_camera->GetComponentLocation() - vSocketLocation;
	const FVector vToNewCamLocation = rCamYawRotation.RotateVector(vSocketToCurrentCamLocation);
	m_camera->SetRelativeLocation(vToNewCamLocation);
	
	
	//Set Camera to point at Camera Focus point
	const FVector vCameraToSocket = vSocketLocation - m_camera->GetComponentLocation();
	const FRotator rCameraToSocketRelativeRotation = vCameraToSocket.Rotation() - GetActorRotation();
	const FRotator rNewCameraFocusRotation = FRotator(m_camera->GetRelativeRotation().Pitch, rCameraToSocketRelativeRotation.Yaw, 0.0f);
	m_camera->SetRelativeRotation(rNewCameraFocusRotation);
}

void ACharacter_Pirate_Player::RotateCamPitch(const float fInputValue)
{
	if (!m_camera || fInputValue == 0.0f) { return; }

	const FRotator rCameraPitchRotation = FRotator(fInputValue * m_fCameraPitchRotationAmount, 0.0f, 0.0f);
	m_camera->AddRelativeRotation(rCameraPitchRotation);
}
#pragma endregion