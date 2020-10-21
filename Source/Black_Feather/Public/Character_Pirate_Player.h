// Copyright 2020 Josh MalmquistAll Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character_Pirate_Player.generated.h" //Must Be Last Header

class UCameraComponent;

UCLASS()
class BLACK_FEATHER_API ACharacter_Pirate_Player : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacter_Pirate_Player(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* m_camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool m_bInvertCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float m_fCameraYawRotationAmount = 1.0f; //Stored in Degrees

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float m_fCameraPitchRotationAmount = 0.5f; //Stored in Degrees

	const bool m_bUseCameraForward = false;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//Movement Functions
	void MoveForward(const float fInputValue);
	void MoveRight(const float fInputValue);
	void MoveJump(const float fInputValue);

	//Camera Functions
	FVector RotateAroundPoint(const float fRotationAmount, const FVector vRotationPoint, const FVector vPointToRotate) const;
	void RotateCamYaw(const float fInputValue);
	void RotateCamPitch(const float fInputValue);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
