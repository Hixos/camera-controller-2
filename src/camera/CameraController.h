#include "Events.h"
#include "events/HSM.h"
#include "collections/CircularBuffer.h"
#include "CameraData.h"
#include "camera/wrapper/CameraWrapper.h"

using gphotow::CameraWrapper;

class CameraController : public HSM<CameraController>
{
public:
    CameraController(CameraWrapper& camera);

private:
    State stateInit(const EventPtr& ev);

    State stateDisconnected(const EventPtr& ev);
    State stateConnecting(const EventPtr& ev);
    State stateError(const EventPtr& ev);

    State stateConnected(const EventPtr& ev);

    State stateIdle(const EventPtr& ev);
    State stateCaptureWired(const EventPtr& ev);
    State stateDownloading(const EventPtr& ev);
    

    void connect();
    void disconnect();
    void setConfig();
    
    void readStatus();
    void readOptions();

    void captureWired();

    bool criticalError(int gp_result);
    void postGPhotoError(const gphotow::GPhotoError& error);

    int reconnect_attempts = 0;

    CircularBuffer<EventPtr, 1000> deferred_events;

    CameraConfig camera_config;

    CameraWrapper& camera;

    static const int MAX_RECONNECT_ATTEMPS = 3;
};