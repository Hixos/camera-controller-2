#include "async/ActiveObject.h"
#include "events/EventBroker.h"
#include "events/EventHandler.h"

#include "utils/EventInjector.h"
#include "utils/logger.h"

#include "camera/CameraController.h"
#include <iostream>


#include "camera/CameraWrapper2.h"

using namespace std;

#define USE_EVENT_INJECTOR

EventInjector* injector;

CameraController* cc;

void init()
{
    sEventBroker->start();
#ifdef USE_EVENT_INJECTOR
    injector = new EventInjector();
    injector->start();
#endif
    cc = new CameraController();
    cc->start();
}

void cleanUp()
{
    sEventBroker->stop();
#ifdef USE_EVENT_INJECTOR
    injector->stop();
    delete injector;
#endif
    cc->stop();
    delete cc;
}

int main()
{
    Log.d("CameraController");
    // init();
    CameraWrapper wrapper;
    wrapper.connect();

    for (;;)
    {
        this_thread::sleep_for(chrono::seconds(1));
    }

    Log.d("Cleanup...");
    cleanUp();
    Log.d("Done.");
    
    return 0;
}