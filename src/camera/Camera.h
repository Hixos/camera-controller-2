#include "CameraWrapper.h"
#include "events/EventHandler.h"
#include "Events.h"

class Camera : public EventHandler
{
public:
    CameraWrapper& getCamera();
    {
        return camera;
    }

    Camera(Camera const&) = delete;
    void operator=(Camera const&) = delete;

protected:
    void handleEvent(const EventPtr& ev) override;

private:
    void capture();
    
    CameraWrapper camera;
};