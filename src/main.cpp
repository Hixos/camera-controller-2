#include <gphoto2/gphoto2.h>

#include <cxxopts.hpp>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>

#include "EventLogger.h"
#include "async/ActiveObject.h"
#include "cameracontroller/cameramanager/CameraManager.h"
#include "events/EventBroker.h"
#include "events/EventHandler.h"
#include "utils/EventInjector.h"
#include "utils/EventSniffer.h"
#include "utils/logger.h"

using namespace std;

EventInjector* injector;
EventSniffer* sniffer;

CameraWrapper* camera;
CameraController* cc;

string download_dir;


void readOptions(int argc, char** argv)
{
    cxxopts::Options options("test", "A brief description");

    options.add_options()("d,download_dir", "Photo download directory",
                          cxxopts::value<std::string>()->default_value("."));

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        exit(0);
    }
    download_dir = result["download_dir"].as<string>();
}

void init()
{
    sEventBroker.start();

    injector = new EventInjector();
    injector->start();

    sniffer = new EventSniffer(sEventBroker, &onEventReceived);

    camera = new CameraWrapper();

    cc = new CameraController(*camera, download_dir);
    cc->start();
}

void cleanUp()
{
    sEventBroker.stop();

    injector->stop();
    delete injector;

    delete sniffer;

    cc->stop();
    delete cc;

    camera->disconnect();
    delete camera;
}

int main(int argc, char** argv)
{
    readOptions(argc, argv);

    Log.d("CameraController");

    init();

    for (;;)
    {
        if (!injector->isStopping())
            this_thread::sleep_for(chrono::seconds(1));
    }

    injector->stop();

    Log.d("Cleanup...");
    cleanUp();
    Log.d("Done.");

    return 0;
}