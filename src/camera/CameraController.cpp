#include "CameraController.h"

#include "events/Event.h"
#include "events/EventBroker.h"
#include "utils/logger.h"

using namespace gphotow;
using std::static_pointer_cast;

CameraController::CameraController(CameraWrapper& camera)
    : HSM(&CameraController::stateInit), camera(camera)
{
    sEventBroker.subscribe(this, TOPIC_CAMERA_CMD);
    sEventBroker.subscribe(this, TOPIC_CAMERA_EVENT);
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
            Log.i("[CameraController] (Disconnected) EV_ENTRY");
            break;
        case EV_INIT:
            // retState = transition(&HSMUTTest::state_S1);
            break;
        case EV_EXIT:
            Log.i("[CameraController] (Disconnected) EV_EXIT");
            break;
        case EV_CAMERA_CONNECT:
        {
            Log.i("[CameraController] (Disconnected) EV_CAMERA_CONNECT");
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
            Log.i("[CameraController] (Connecting) EV_ENTRY");

            connect();
            break;
        case EV_INIT:
            break;
        case EV_EXIT:
            Log.i("[CameraController] (Connecting) EV_EXIT");
            break;
        case EV_CAMERA_CONNECTION_ERROR:
        {
            Log.i("[CameraController] (Connecting) EV_CAMERA_CONNECTION_ERROR");
            retState = transition(&CameraController::stateError);
            break;
        }
        case EV_CAMERA_GENERIC_ERROR:
        {
            auto err_ev = static_pointer_cast<const CameraErrorEvent>(ev);
            Log.e("[CameraController] (Connecting) %s", err_ev->what().c_str());

            break;
        }
        case EV_CAMERA_CONNECTED:
        {
            Log.i("[CameraController] (Connecting) EV_CAMERA_CONNECTED");

            setConfig();
            readStatus();
            readOptions();

            retState = transition(&CameraController::stateConnected);
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
            disconnect();
            Log.i("[CameraController] (Error) EV_ENTRY");
            break;
        case EV_INIT:
            // retState = transition(&HSMUTTest::state_S1);
            break;
        case EV_EXIT:
            Log.i("[CameraController] (Error) EV_EXIT");
            break;
        case EV_CAMERA_CONNECT:
        {
            Log.i("[CameraController] (Error) EV_CAMERA_CONNECT");
            retState = transition(&CameraController::stateConnecting);
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
            reconnect_attempts = MAX_RECONNECT_ATTEMPS;
            Log.i("[CameraController] (Connected) EV_ENTRY");
            break;
        case EV_INIT:
            retState = transition(&CameraController::stateIdle);
            break;
        case EV_EXIT:
            Log.i("[CameraController] (Connected) EV_EXIT");
            break;
        case EV_CAMERA_GENERIC_ERROR:
        {
            auto err_ev = static_pointer_cast<const CameraErrorEvent>(ev);

            if (criticalError(err_ev->errorcode))
            {
                Log.e(
                    "[CameraController] (Connected) EV_CAMERA_GENERIC_ERROR "
                    "%d: %s",
                    err_ev->errorcode, err_ev->what().c_str());
                retState = transition(&CameraController::stateError);
            }
            else
            {
                Log.d(
                    "[CameraController] (Connected) EV_CAMERA_GENERIC_ERROR "
                    "%d: %s",
                    err_ev->errorcode, err_ev->what().c_str());
            }
            break;
        }
        case EV_CAMERA_SET_CONFIG:
        case EV_CAMERA_GET_STATUS:
        case EV_CAMERA_GET_OPTIONS:
        {
            deferred_events.put(ev);
            break;
        }
        default:
            retState = tran_super(&CameraController::Hsm_top);
            break;
    }
    return retState;
}

State CameraController::stateIdle(const EventPtr& ev)
{
    State retState = HANDLED;
    switch (ev->sig)
    {
        case EV_ENTRY:
            Log.i("[CameraController] (Idle) EV_ENTRY");

            if (!camera.isResponsive())
            {
                Log.e("[CameraController] (Idle) Camera not responsive!");

                retState = transition(&CameraController::stateError);
                break;
            }

            readStatus();

            Log.d("[CameraController] (Connected) Deffered events count: %d",
                  deferred_events.count());
            while (!deferred_events.isEmpty())
            {
                postEvent(deferred_events.pop());
            }

            break;
        case EV_INIT:
            break;
        case EV_EXIT:
            Log.i("[CameraController] (Idle) EV_EXIT");
            break;
        case EV_CAMERA_SET_CONFIG:
        {
            auto cfgev    = static_pointer_cast<const CameraConfigEvent>(ev);
            camera_config = cfgev->config;
            setConfig();
            readStatus();
            break;
        }
        case EV_CAMERA_GET_STATUS:
        {
            readStatus();
            break;
        }
        case EV_CAMERA_GET_OPTIONS:
        {
            readOptions();
            break;
        }
        case EV_CAMERA_CAPTURE_WIRED:
        {
            Log.i("[CameraController] (Idle) EV_CAMERA_CAPTURE_WIRED");
            retState = transition(&CameraController::stateCaptureWired);
            break;
        }
        case EV_CAMERA_DISCONNECT:
        {
            Log.i("[CameraController] (Idle) EV_CAMERA_DISCONNECT");
            retState = transition(&CameraController::stateDisconnected);
            break;
        }
        default:
            retState = tran_super(&CameraController::stateConnected);
            break;
    }
    return retState;
}

State CameraController::stateCaptureWired(const EventPtr& ev)
{
    State retState = HANDLED;
    switch (ev->sig)
    {
        case EV_ENTRY:
            Log.i("[CameraController] (Capturing) EV_ENTRY");

            if (!camera.isResponsive())
            {
                Log.e(
                    "[CameraController] (CaptureWired) Camera not responsive!");

                retState = transition(&CameraController::stateError);

                break;
            }

            captureWired();
            break;
        case EV_INIT:
            break;
        case EV_EXIT:
            Log.i("[CameraController] (Capturing) EV_EXIT");
            break;
        case EV_CAMERA_CAPTURE_ERROR:
            Log.i("[CameraController] (Capturing) EV_CAMERA_CAPTURE_ERROR");
            retState = transition(&CameraController::stateError);
            break;
        case EV_CAMERA_CAPTURE_SUCCESS:
            Log.i("[CameraController] (Capturing) EV_CAMERA_CAPTURE_SUCCESS");
            retState = transition(&CameraController::stateIdle);
            break;
        default:
            retState = tran_super(&CameraController::stateConnected);
            break;
    }
    return retState;
}

State CameraController::stateDownloading(const EventPtr& ev)
{
    State retState = HANDLED;
    switch (ev->sig)
    {
        case EV_ENTRY:
            Log.i("[CameraController] (Capturing) EV_ENTRY");

            if (!camera.isResponsive())
            {
                Log.e(
                    "[CameraController] (Downloading) Camera not responsive!");

                retState = transition(&CameraController::stateError);
                break;
            }

            break;
        case EV_INIT:
            break;
        case EV_EXIT:
            Log.i("[CameraController] (Capturing) EV_EXIT");
            break;
        default:
            retState = tran_super(&CameraController::Hsm_top);
            break;
    }
    return retState;
}

void CameraController::connect()
{
    try
    {
        string serial = camera.connect();

        EventPtr ev = getEvPointer(CameraConnectedEvent(serial));
        sEventBroker.post(ev, TOPIC_CAMERA_EVENT);
    }
    catch (const gphotow::GPhotoError& gpe)
    {
        EventPtr ev = getEvPointer(
            CameraErrorEvent(EV_CAMERA_CONNECTION_ERROR, gpe.error));
        sEventBroker.post(ev, TOPIC_CAMERA_EVENT);
    }
}

void CameraController::disconnect()
{
    if (camera.disconnect())
    {
        EventPtr ev = getEvPointer(Event(EV_CAMERA_DISCONNECTED));
        sEventBroker.post(ev, TOPIC_CAMERA_EVENT);
    }
}

void CameraController::setConfig()
{
    try
    {
        if (camera_config.aperture > 0)
        {
            camera.setAperture(camera_config.aperture);
        }
    }
    catch (const gphotow::GPhotoError& gpe)
    {
        postGPhotoError(gpe);

        Log.e(
            "[CameraController] (setConfig) setAperture: GPhoto error %d "
            "(%s)",
            gpe.error, gpe.what());
    }

    try
    {
        if (camera_config.shutter_speed >= 0)
        {
            camera.setExposureTime(camera_config.shutter_speed);
        }
    }
    catch (const gphotow::GPhotoError& gpe)
    {
        postGPhotoError(gpe);

        Log.e(
            "[CameraController] (setConfig) setShutterSpeed: GPhoto error "
            "%d (%s)",
            gpe.error, gpe.what());
    }

    try
    {
        if (camera_config.iso > 0)
        {
            camera.setISO(camera_config.iso);
        }
    }
    catch (const gphotow::GPhotoError& gpe)
    {
        postGPhotoError(gpe);

        Log.e("[CameraController] (setConfig) setISO: GPhoto error %d (%s)",
              gpe.error, gpe.what());
    }

    try
    {
        if (camera_config.trigger_mode.compare("") != 0)
        {
            camera.setTriggerMode(camera_config.trigger_mode);
        }
    }
    catch (const gphotow::GPhotoError& gpe)
    {
        postGPhotoError(gpe);

        Log.e(
            "[CameraController] (setConfig) setTriggerMode: GPhoto error "
            "%d (%s)",
            gpe.error, gpe.what());
    }
}

void CameraController::readStatus()
{
    try
    {
        CameraConfig config;
        config.shutter_speed = camera.getExposureTime();
        config.aperture      = camera.getAperture();
        config.iso           = camera.getISO();
        config.trigger_mode  = camera.getTriggerMode();

        EventPtr ev1 =
            getEvPointer(CameraConfigEvent(EV_CAMERA_VAL_CONFIG, config));
        sEventBroker.post(ev1, TOPIC_CAMERA_STATUS);

        CameraStatus status;
        status.battery          = camera.getBatteryPercent();
        status.exposure_program = camera.getExposureProgram();
        status.focal_length     = camera.getFocalLength();

        EventPtr ev2 = getEvPointer(CameraStatusEvent(status));
        sEventBroker.post(ev2, TOPIC_CAMERA_STATUS);
    }
    catch (const gphotow::GPhotoError& gpe)
    {
        postGPhotoError(gpe);
        Log.e("[CameraController] (readStatus) %s", gpe.what());
    }
}

void CameraController::readOptions()
{
    try
    {
        EventPtr ev1 = getEvPointer(CameraOptionListEvent(
            EV_CAMERA_OPTIONS_SHUTTER_SPEED, camera.listExposureTimes()));

        sEventBroker.post(ev1, TOPIC_CAMERA_STATUS);
    }
    catch (const gphotow::GPhotoError& gpe)
    {
        postGPhotoError(gpe);
        Log.e(
            "[CameraController] (readStatus) Error calling listExposureTimes: "
            "%s",
            gpe.what());
    }

    try
    {
        EventPtr ev2 = getEvPointer(CameraOptionListEvent(
            EV_CAMERA_OPTIONS_APERTURE, camera.listApertures()));

        sEventBroker.post(ev2, TOPIC_CAMERA_STATUS);
    }
    catch (const gphotow::GPhotoError& gpe)
    {
        postGPhotoError(gpe);
        Log.e("[CameraController] (readStatus) Error calling listApertures: %s",
              gpe.what());
    }

    try
    {
        EventPtr ev3 = getEvPointer(
            CameraOptionListEvent(EV_CAMERA_OPTIONS_ISO, camera.listISOs()));

        sEventBroker.post(ev3, TOPIC_CAMERA_STATUS);
    }
    catch (const gphotow::GPhotoError& gpe)
    {
        postGPhotoError(gpe);
        Log.e("[CameraController] (readStatus) Error calling listISOs: %s",
              gpe.what());
    }

    try
    {

        EventPtr ev4 = getEvPointer(CameraOptionListEvent(
            EV_CAMERA_OPTIONS_TRIGGER_MODE, camera.listTriggerModes()));

        sEventBroker.post(ev4, TOPIC_CAMERA_STATUS);
    }
    catch (const gphotow::GPhotoError& gpe)
    {
        postGPhotoError(gpe);
        Log.e(
            "[CameraController] (readStatus) Error calling listTriggerModes: "
            "%s",
            gpe.what());
    }
}

bool CameraController::criticalError(int gp_result)
{
    return gp_result <= -4 && gp_result >= -70;
}

void CameraController::postGPhotoError(const GPhotoError& error)
{
    EventPtr ev =
        getEvPointer(CameraErrorEvent(EV_CAMERA_GENERIC_ERROR, error.error));
    sEventBroker.post(ev, TOPIC_CAMERA_EVENT);
}

void CameraController::captureWired()
{
    try
    {
        CameraPath p = camera.wiredCapture();
        EventPtr ev  = getEvPointer(CameraCapturedEvent(p));
        sEventBroker.post(ev, TOPIC_CAMERA_EVENT);
    }
    catch (const GPhotoError& gpe)
    {
        EventPtr ev =
            getEvPointer(CameraErrorEvent(EV_CAMERA_CAPTURE_ERROR, gpe.error));
        sEventBroker.post(ev, TOPIC_CAMERA_EVENT);

        Log.e("[CameraController] (captureWired) Error capturing: %d (%s)",
              gpe.error, gpe.what());
    }
}