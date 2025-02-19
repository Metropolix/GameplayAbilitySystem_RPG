 // Copyright BlueBerry Inc.


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

 AAuraPlayerController::AAuraPlayerController()
{
    bReplicates = true;

}

 void AAuraPlayerController::PlayerTick(float DeltaTime)
 {
	Super::PlayerTick(DeltaTime);
 	
 	CursorTrace();
 }

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit)
		return;

 	LastActor = ThisActor;
 	ThisActor = CursorHit.GetActor();

 	/**
 	 * Line Trace from cursor. There are serveral scenarios
 	 * A. LastActor is null && ThisActor is null
 	 *		- Do nothing.
 	 * B. Last actor is null && ThisActor is valid
 	 *		- Highlight ThisActor
 	 * C. Lastactor is valid && This Actor is null
 	 *		- UnHighlight LastActor
 	 * D. Both actors are valid, but LastActor != ThisActor
 	 *		- UnHighlight LastActor
 	 *		- Hightlight ThisActor
 	 *	E. Both actor are valid, but LastActor == ThisActor
 	 *		- Do nothing.
 	 */

 	if (LastActor == nullptr)
 	{
 		if (ThisActor != nullptr)
 		{
 			// Case B
 			ThisActor->HighlightActor();
 		}
	    else
	    {
		    // Case A - both are null, do nothing.
	    }
 	}
    else // Lastactor is valid
    {
	    if (ThisActor == nullptr)
	    {
		    LastActor->UnHighlightActor();
	    }
    	else // Both actor are valid
    	{
    		if (LastActor != ThisActor) // Case D
    		{
    			LastActor->UnHighlightActor();
    			ThisActor->HighlightActor();
    		}
		    else
		    {
			    // Case E, both are valid and the same, do nothing.
		    }
    	}
    }
}

 void AAuraPlayerController::BeginPlay()
{
    Super::BeginPlay();
    check(AuraContext);

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    check(Subsystem);

    Subsystem->AddMappingContext(AuraContext, 0);

    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;

    FInputModeGameAndUI InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputModeData.SetHideCursorDuringCapture(false);
    SetInputMode(InputModeData);
}

 void AAuraPlayerController::SetupInputComponent()
 {
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
 }

 void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
 {
	const FVector2d InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);

	}
 }
