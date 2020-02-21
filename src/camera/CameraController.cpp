#include "CameraController.h"
#include "events/EventBroker.h"
#include "utils/logger.h"

CameraController::CameraController() : HSM(&CameraController::stateInit)
{
    sEventBroker->subscribe(this, 1);
}

State CameraController::stateInit(const EventPtr& ev)
{
    return transition(&CameraController::stateDisconnected);
}

State CameraController::stateDisconnected(const EventPtr& ev)
{
    State retState = HANDLED;
    switch (ev->sig)
    {
        case EV_ENTRY:
            Log.d("[CameraController] (Disconnected) EV_ENTRY");
            break;
        case EV_INIT:
            // retState = transition(&HSMUTTest::state_S1);
            break;
        case EV_EXIT:
            Log.d("[CameraController] (Disconnected) EV_EXIT");
            break;
        case EV_CAMERA_CONNECT:
        {
            Log.d("[CameraController] (Disconnected) EV_CAMERA_CONNECT");
            retState = transition(&CameraController::stateConnecting);
            break;
        }
        default:
            retState = tran_super(&CameraController::Hsm_top);
            break;
    }
    return retState;
}

State CameraController::stateConnecting(const EventPtr& ev)
{
    State retState = HANDLED;
    switch (ev->sig)
    {
        case EV_ENTRY:
            Log.d("[CameraController] (Connecting) EV_ENTRY");
            break;
        case EV_INIT:
            // retState = transition(&HSMUTTest::state_S1);
            break;
        case EV_EXIT:
            Log.d("[CameraController] (Connecting) EV_EXIT");
            break;
        case EV_CAMERA_ERROR:
        {
            Log.d("[CameraController] (Connecting) EV_CAMERA_ERROR");
            retState = transition(&CameraController::stateError);
            break;
        }
        case EV_CAMERA_CONNECTED:
        {
            Log.d("[CameraController] (Connecting) EV_CAMERA_CONNECTED");
            retState = transition(&CameraController::stateConnected);
            break;
        }
        default:
            retState = tran_super(&CameraController::Hsm_top);
            break;
    }
    return retState;
}

State CameraController::stateConnected(const EventPtr& ev)
{
    State retState = HANDLED;
    switch (ev->sig)
    {
        case EV_ENTRY:
            Log.d("[CameraController] (Connected) EV_ENTRY");
            break;
        case EV_INIT:
            // retState = transition(&HSMUTTest::state_S1);
            break;
        case EV_EXIT:
            Log.d("[CameraController] (Connected) EV_EXIT");
            break;
        case EV_CAMERA_ERROR:
        {
            Log.d("[CameraController] (Connected) EV_CAMERA_ERROR");
            retState = transition(&CameraController::stateError);
            break;
        }
        case EV_CAMERA_DISCONNECT:
        {
            Log.d("[CameraController] (Connected) EV_CAMERA_DISCONNECT");
            retState = transition(&CameraController::stateDisconnected);
            break;
        }
        default:
            retState = tran_super(&CameraController::Hsm_top);
            break;
    }
    return retState;
}

State CameraController::stateError(const EventPtr& ev)
{
    State retState = HANDLED;
    switch (ev->sig)
    {
        case EV_ENTRY:
            Log.d("[CameraController] (Error) EV_ENTRY");
            break;
        case EV_INIT:
            // retState = transition(&HSMUTTest::state_S1);
            break;
        case EV_EXIT:
            Log.d("[CameraController] (Error) EV_EXIT");
            break;
        default:
            retState = tran_super(&CameraController::Hsm_top);
            break;
    }
    return retState;
}