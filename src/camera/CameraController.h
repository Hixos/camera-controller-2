#include "Events.h"
#include "events/HSM.h"

class CameraController : public  HSM<CameraController>
{
public:
    CameraController();
private:
    State stateInit(const EventPtr &ev);
    State stateDisconnected(const EventPtr &ev);
    State stateConnecting(const EventPtr &ev);
    State stateConnected(const EventPtr &ev);
    State stateError(const EventPtr &ev);



};