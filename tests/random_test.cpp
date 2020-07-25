#include <gphoto2/gphoto2.h>

#include <functional>
#include <iostream>
#include <map>
#include <sstream>

#include "async/ActiveObject.h"
#include "camera/CameraController.h"
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

string getEventName(uint16_t ev)
{
    map<int, string> events = {
        {EV_CAMERA_CONNECT, "EV_CAMERA_CONNECT"},
        {EV_CAMERA_CONNECTED, "EV_CAMERA_CONNECTED"},
        {EV_CAMERA_CONNECTION_ERROR, "EV_CAMERA_CONNECTION_ERROR"},
        {EV_CAMERA_DISCONNECT, "EV_CAMERA_DISCONNECT"},
        {EV_CAMERA_DISCONNECTED, "EV_CAMERA_DISCONNECTED"},
        {EV_CAMERA_GENERIC_ERROR, "EV_CAMERA_GENERIC_ERROR"},

        {EV_CAMERA_SET_CONFIG, "EV_CAMERA_SET_CONFIG"},
        {EV_CAMERA_GET_STATUS, "EV_CAMERA_GET_STATUS"},
        {EV_CAMERA_VAL_CONFIG, "EV_CAMERA_VAL_CONFIG"},
        {EV_CAMERA_VAL_STATUS, "EV_CAMERA_VAL_STATUS"},

        {EV_CAMERA_GET_OPTIONS, "EV_CAMERA_GET_OPTIONS"},
        {EV_CAMERA_OPTIONS_SHUTTER_SPEED, "EV_CAMERA_OPTIONS_SHUTTER_SPEED"},
        {EV_CAMERA_OPTIONS_APERTURE, "EV_CAMERA_OPTIONS_APERTURE"},
        {EV_CAMERA_OPTIONS_ISO, "EV_CAMERA_OPTIONS_ISO"},
        {EV_CAMERA_OPTIONS_TRIGGER_MODE, "EV_CAMERA_OPTIONS_TRIGGER_MODE"},

        {EV_CAMERA_CAPTURE_WIRED, "EV_CAMERA_CAPTURE_WIRED"},
        {EV_CAMERA_CAPTURE_SUCCESS, "EV_CAMERA_CAPTURE_SUCCESS"},
        {EV_CAMERA_CAPTURE_ERROR, "EV_CAMERA_CAPTURE_ERROR"},

        {EV_CAMERA_DOWNLOAD, "EV_CAMERA_DOWNLOAD"}};

    if (events.count(ev) > 0)
        return events.at(ev);
    else
    {
        return std::to_string(ev);
    }
}

string getTopicName(uint16_t ev)
{
    map<int, string> topics = {{TOPIC_CAMERA_STATUS, "TOPIC_CAMERA_STATUS"},
                               {TOPIC_CAMERA_EVENT, "TOPIC_CAMERA_EVENT"},
                               {TOPIC_CAMERA_CMD, "TOPIC_CAMERA_CMD"}};

    if (topics.count(ev) > 0)
        return topics.at(ev);
    else
    {
        return std::to_string(ev);
    }
}

void onEventReceived(const EventPtr& ev, uint8_t topic)
{
    switch (ev->sig)
    {
        case EV_CAMERA_CONNECTED:
        {
            auto cev = std::static_pointer_cast<const CameraConnectedEvent>(ev);

            Log.d("(%s) %s (%s)", getTopicName(topic).c_str(),
                  getEventName(ev->sig).c_str(), cev->serial.c_str());
            break;
        }
        case EV_CAMERA_CONNECTION_ERROR:
        {
            auto cev = std::static_pointer_cast<const CameraConnectedEvent>(ev);

            Log.d("(%s) %s (%s)", getTopicName(topic).c_str(),
                  getEventName(ev->sig).c_str(), cev->serial.c_str());
            break;
        }
        case EV_CAMERA_VAL_STATUS:
        {
            auto sev = std::static_pointer_cast<const CameraStatusEvent>(ev);

            Log.d("(%s) %s (battery=%d, program='%s', focal_length=%d)",
                  getTopicName(topic).c_str(), getEventName(ev->sig).c_str(),
                  sev->status.battery, sev->status.exposure_program.c_str(),
                  sev->status.focal_length);
            break;
        }
        case EV_CAMERA_VAL_CONFIG:
        {
            using namespace gphotow;
            using CSC = CameraStringConversion;

            auto cev = std::static_pointer_cast<const CameraConfigEvent>(ev);

            Log.d("(%s) %s (shutter=%s, aperture='%s', iso=%d, trigger='%s')",
                  getTopicName(topic).c_str(), getEventName(ev->sig).c_str(),
                  CSC::shutterSpeedToString(cev->config.shutter_speed).c_str(),
                  CSC::apertureToString(cev->config.aperture).c_str(),
                  cev->config.iso, cev->config.trigger_mode.c_str());
            break;
        }
        case EV_CAMERA_OPTIONS_SHUTTER_SPEED:
        case EV_CAMERA_OPTIONS_APERTURE:
        case EV_CAMERA_OPTIONS_ISO:
        {
            using namespace gphotow;
            using CSC = CameraStringConversion;
            stringstream ss;

            function<string(int)> to_s;

            switch (ev->sig)
            {
                case EV_CAMERA_OPTIONS_SHUTTER_SPEED:
                    to_s = &CSC::shutterSpeedToString;
                    break;
                case EV_CAMERA_OPTIONS_APERTURE:
                    to_s = &CSC::apertureToString;
                    break;
                default:
                    to_s = static_cast<string (*)(int)>(&to_string);
                    break;
            }

            auto oev =
                std::static_pointer_cast<const CameraOptionListEvent<int>>(ev);

            for (auto it = oev->options.begin(); it != oev->options.end(); it++)
            {
                ss << "'" << to_s(*it) << "'";
                if (it != oev->options.end() - 1)
                {
                    ss << ", ";
                }
            }
            Log.d("(%s) %s {%s}", getTopicName(topic).c_str(),
                  getEventName(ev->sig).c_str(), ss.str().c_str());
            break;
        }
        case EV_CAMERA_OPTIONS_TRIGGER_MODE:
        {
            stringstream ss;

            auto oev =
                std::static_pointer_cast<const CameraOptionListEvent<string>>(
                    ev);

            for (auto it = oev->options.begin(); it != oev->options.end(); it++)
            {
                ss << "'" << *it << "'";
                if (it != oev->options.end() - 1)
                {
                    ss << ", ";
                }
            }
            Log.d("(%s) %s {%s}", getTopicName(topic).c_str(),
                  getEventName(ev->sig).c_str(), ss.str().c_str());
            break;
        }
        case EV_CAMERA_CAPTURE_SUCCESS:
        {
            auto e = static_pointer_cast<const CameraCapturedEvent>(ev);
            Log.d("(%s) %s (%s)", getTopicName(topic).c_str(),
                  getEventName(ev->sig).c_str(), e->path.getPath().c_str());
            break;
        }
        case EV_CAMERA_DOWNLOAD:
        {
            auto e = static_pointer_cast<const CameraDownloadEvent>(ev);
            Log.d("(%s) %s (%s -> %s)", getTopicName(topic).c_str(),
                  getEventName(ev->sig).c_str(), e->path.getPath().c_str(),
                  e->dest.c_str());
            break;
        }
        default:
            Log.d("(%s) %s", getTopicName(topic).c_str(),
                  getEventName(ev->sig).c_str());
            break;
    }
}

void init()
{
    sEventBroker.start();

    injector = new EventInjector();
    injector->start();

    sniffer = new EventSniffer(sEventBroker, &onEventReceived);

    camera = new CameraWrapper();

    cc = new CameraController(*camera);
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

int main()
{
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