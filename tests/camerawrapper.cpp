#include "catch.hpp"
#include "camera/CameraWrapper.h"
#include <string>

using namespace std;
using namespace gphotow;

TEST_CASE("Camera Connection" ) {
    CameraWrapper camera;
    string serial;
    REQUIRE_NOTHROW(serial = camera.connect());
    REQUIRE(serial == camera.getSerialNumber());

    REQUIRE(camera.isConnected());
    REQUIRE(camera.isResponsive());

    camera.disconnect();

    REQUIRE_FALSE(camera.isConnected());
    REQUIRE_FALSE(camera.isResponsive());
    REQUIRE_THROWS(camera.getSerialNumber());
}

TEST_CASE("Camera Exposure setting" ) {
    INFO("Put the camera in manual mode!");
    CameraWrapper camera;

    SECTION("Camera is connected")
    {
        REQUIRE_NOTHROW(camera.connect());

        REQUIRE(camera.getExposureProgram() == "M");

        vector<int> exp_times;
        REQUIRE_NOTHROW(exp_times = camera.listExposureTimes());
        REQUIRE(exp_times.size() > 0);
        int exp_t;
        REQUIRE_NOTHROW(exp_t = camera.getExposureTime());
        REQUIRE_NOTHROW(camera.setExposureTime(exp_times[10]));
        REQUIRE(camera.getExposureTime() == exp_times[10]);


        REQUIRE_NOTHROW(camera.setExposureTime(exp_t));
        REQUIRE(camera.getExposureTime() == exp_t);
    }

    SECTION("Camera is NOT connected")
    {
        REQUIRE_THROWS(camera.listExposureTimes());
        REQUIRE_THROWS(camera.setExposureTime(0));
        REQUIRE_THROWS(camera.getExposureTime());
    }
}

TEST_CASE("Camera Aperture setting" ) {
    INFO("Put the camera in manual mode!");
    CameraWrapper camera;

    SECTION("Camera is connected")
    {
        REQUIRE_NOTHROW(camera.connect());

        REQUIRE(camera.getExposureProgram() == "M");
        
        vector<int> apertures;
        REQUIRE_NOTHROW(apertures = camera.listApertures());
        REQUIRE(apertures.size() > 0);
        int a;
        REQUIRE_NOTHROW(a = camera.getAperture());
        REQUIRE_NOTHROW(camera.setAperture(apertures[10]));
        REQUIRE(camera.getAperture() == apertures[10]);


        REQUIRE_NOTHROW(camera.setAperture(a));
        REQUIRE(camera.getAperture() == a);
    }

    SECTION("Camera is NOT connected")
    {
        REQUIRE_THROWS(camera.listApertures());
        REQUIRE_THROWS(camera.setAperture(0));
        REQUIRE_THROWS(camera.getAperture());
    }
}

TEST_CASE("Camera ISO setting" ) {
    INFO("Put the camera in manual mode!");
    CameraWrapper camera;

    SECTION("Camera is connected")
    {
        REQUIRE_NOTHROW(camera.connect());

        REQUIRE(camera.getExposureProgram() == "M");
        
        vector<int> isos;
        REQUIRE_NOTHROW(isos = camera.listISOs());
        REQUIRE(isos.size() > 0);
        int a;
        REQUIRE_NOTHROW(a = camera.getISO());
        REQUIRE_NOTHROW(camera.setISO(isos[2]));
        REQUIRE(camera.getISO() == isos[2]);
    

        REQUIRE_NOTHROW(camera.setISO(a));
        REQUIRE(camera.getISO() == a);
    }

    SECTION("Camera is NOT connected")
    {
        REQUIRE_THROWS(camera.listISOs());
        REQUIRE_THROWS(camera.setISO(0));
        REQUIRE_THROWS(camera.getISO());
    }
}

TEST_CASE("Camera TriggerMode setting" ) {
    INFO("Put the camera in manual mode!");
    CameraWrapper camera;

    SECTION("Camera is connected")
    {
        REQUIRE_NOTHROW(camera.connect());

        REQUIRE(camera.getExposureProgram() == "M");
        
        vector<string> trigger_modes;
        REQUIRE_NOTHROW(trigger_modes = camera.listTriggerModes());
        REQUIRE(trigger_modes.size() > 0);
        string a;
        REQUIRE_NOTHROW(a = camera.getTriggerMode());
        REQUIRE_NOTHROW(camera.setTriggerMode(trigger_modes[2]));
        REQUIRE(camera.getTriggerMode() == trigger_modes[2]);


        REQUIRE_NOTHROW(camera.setTriggerMode(a));
        REQUIRE(camera.getTriggerMode() == a);
    }

    SECTION("Camera is NOT connected")
    {
        REQUIRE_THROWS(camera.listTriggerModes());
        REQUIRE_THROWS(camera.setTriggerMode(""));
        REQUIRE_THROWS(camera.getTriggerMode());
    }
}