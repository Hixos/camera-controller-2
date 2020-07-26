
#include "CameraWrapper.h"

#include <gphoto2/gphoto2.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <sstream>
#include <system_error>
#include <thread>

#include "utils/logger.h"
// #include "utils/RemoteTrigger.h"

using namespace std::this_thread;
using std::bind;

using std::max;
using std::min;
using std::stringstream;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

typedef system_clock Clock;

namespace gphotow
{

CameraWrapper::CameraWrapper() : context(gp_context_new()) {}

CameraWrapper::~CameraWrapper() { freeCamera(); }

void CameraWrapper::freeCamera()
{
    if (camera != nullptr)
    {
        gp_camera_exit(camera, context);
        gp_camera_free(camera);
        camera = nullptr;
    }
}

string CameraWrapper::connect()
{
    if (!connected)
    {
        int result = gp_camera_new(&camera);

        if (result != GP_OK)
        {
            throw GPhotoError(result);
        }

        result = gp_camera_init(camera, context);

        if (result == GP_OK)
        {
            serial = getSerialNumber();

            // Sleep for 2 seconds to avoid errors if capturing too early
            // std::this_thread::sleep_for(seconds(2));

            connected = true;
            return serial;
        }
        else
        {
            gp_camera_free(camera);
            throw GPhotoError(result);
        }
    }

    return serial;
}

bool CameraWrapper::disconnect()
{
    if (connected)
    {
        freeCamera();
        connected = false;

        return true;
    }
    return false;
}

bool CameraWrapper::isConnected() { return connected; }

bool CameraWrapper::isResponsive()
{
    try
    {
        return connected && getSerialNumber() == serial;
    }
    catch (CameraException& e)
    {
        return false;
    }
}

string CameraWrapper::getSerialNumber()
{
    CameraWidgetString serial{*this, CONFIG_SERIAL_NUMBER};
    return serial.getValue();
}

int CameraWrapper::getExposureTime()
{
    CameraWidgetString widget{*this, CONFIG_EXPOSURE_TIME};
    return CameraStringConversion::shutterSpeedToInt(widget.getValue());
}

void CameraWrapper::setExposureTime(int exposure_time)
{
    CameraWidgetString widget{*this, CONFIG_EXPOSURE_TIME};
    widget.setValue(
        CameraStringConversion::shutterSpeedToString(exposure_time));
    widget.apply();
}

vector<int> CameraWrapper::listExposureTimes()
{
    CameraWidgetString widget{*this, CONFIG_EXPOSURE_TIME};
    vector<string> choices = widget.getAvailableValues();
    vector<int> out;
    for (string c : choices)
    {
        out.push_back(CameraStringConversion::shutterSpeedToInt(c));
    }

    return out;
}

int CameraWrapper::getAperture()
{
    CameraWidgetString widget{*this, CONFIG_APERTURE};
    return CameraStringConversion::apertureToInt(widget.getValue());
}

void CameraWrapper::setAperture(int aperture)
{
    CameraWidgetString widget{*this, CONFIG_APERTURE};
    widget.setValue(CameraStringConversion::apertureToString(aperture));
    widget.apply();
}

vector<int> CameraWrapper::listApertures()
{
    CameraWidgetString widget{*this, CONFIG_APERTURE};
    vector<string> choices = widget.getAvailableValues();
    vector<int> out;
    for (string c : choices)
    {
        out.push_back(CameraStringConversion::apertureToInt(c));
    }

    return out;
}

int CameraWrapper::getISO()
{
    CameraWidgetString widget{*this, CONFIG_ISO};
    return std::stoi(widget.getValue());
}

void CameraWrapper::setISO(int iso)
{
    CameraWidgetString widget{*this, CONFIG_ISO};
    widget.setValue(std::to_string(iso));
    widget.apply();
}

vector<int> CameraWrapper::listISOs()
{
    CameraWidgetString widget{*this, CONFIG_ISO};
    vector<string> choices = widget.getAvailableValues();
    vector<int> out;
    for (string c : choices)
    {
        out.push_back(std::stoi(c));
    }

    return out;
}

int CameraWrapper::getBatteryPercent()
{
    CameraWidgetString widget{*this, CONFIG_BATTERY_LEVEL};
    return CameraStringConversion::batteryLevelToInt(widget.getValue());
}

string CameraWrapper::getExposureProgram()
{

    CameraWidgetString widget{*this, CONFIG_EXPOSURE_PROGRAM};
    return widget.getValue();
}

string CameraWrapper::getTriggerMode()
{

    CameraWidgetString widget{*this, CONFIG_TRIGGER_MODE};
    return widget.getValue();
}

int CameraWrapper::getFocalLength()
{

    CameraWidgetRange widget{*this, CONFIG_FOCAL_LENGHT};
    return int(widget.getValue());
}

void CameraWrapper::setTriggerMode(string mode)
{

    CameraWidgetString widget{*this, CONFIG_TRIGGER_MODE};
    widget.setValue(mode);
    widget.apply();
}

vector<string> CameraWrapper::listTriggerModes()
{

    CameraWidgetString widget{*this, CONFIG_TRIGGER_MODE};
    vector<string> choices = widget.getAvailableValues();
    return choices;
}

int CameraWrapper::getBurstCount()
{
    CameraWidgetString widget{*this, CONFIG_BURST_COUNT};
    return std::stoi(widget.getValue());
}

void CameraWrapper::setBurstCount(int burst)
{
    CameraWidgetString widget{*this, CONFIG_BURST_COUNT};
    widget.setValue(std::to_string(burst));
    widget.apply();
}

int CameraWrapper::getMaxBurstCount()
{
    CameraWidgetString widget{*this, CONFIG_BURST_COUNT};
    vector<string> choices = widget.getAvailableValues();
    return std::stoi(choices[choices.size() - 1]);
}


CameraPath CameraWrapper::wiredCapture()
{
    CameraFilePath path;
    int result = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &path, context);

    if (result == GP_OK)
    {
        return CameraPath(path);
    }
    else
    {
        throw GPhotoError(result);
    }
}

void CameraWrapper::downloadFile(CameraFilePath path, string dest_file_path)
{
    CameraFile* file;
    int result, fd;
    bool gp_file_created = false;

    FILE* f = fopen(dest_file_path.c_str(), "w");

    if (f == NULL)
    {
        goto out;
    }

    fd = fileno(f);

    if (fd < 0)
    {
        goto out;
    }

    result = gp_file_new_from_fd(&file, fd);
    if (result != GP_OK)
    {
        goto out;
    }
    else
    {
        gp_file_created = true;
    }

    result = gp_camera_file_get(camera, path.folder, path.name,
                                GP_FILE_TYPE_RAW, file, context);
    if (result != GP_OK)
    {
        goto out;
    }

out:
    if (f != NULL)
    {
        fclose(f);
    }
    if (f == NULL || fd < 0)
    {
        throw std::filesystem::filesystem_error(
            "Cannot open file", dest_file_path,
            std::error_code(errno, std::system_category()));
    }

    if (gp_file_created)
    {
        gp_file_free(file);
    }

    if (result != GP_OK)
    {
        throw GPhotoError(result);
    }
}

int CameraStringConversion::shutterSpeedToInt(string shutter_speed)
{
    if (shutter_speed.compare("Bulb") == 0)
    {
        return 0;
    }
    try
    {
        float ss = std::stof(shutter_speed.substr(0, shutter_speed.size() - 1));
        return int(ss * 1000 * 1000);
    }
    catch (std::invalid_argument& ia)
    {
        Log.e(ia.what());
    }
    catch (std::out_of_range& oor)
    {
        Log.e(oor.what());
    }
    return -1;
}

string CameraStringConversion::shutterSpeedToString(int shutter_speed)
{
    if (shutter_speed == 0)
        return "Bulb";
    else
        return toString(shutter_speed / 1000000.0f, 4) + "s";
}

int CameraStringConversion::apertureToInt(string aperture)
{
    try
    {
        float ap = std::stof(aperture.substr(2, aperture.size() - 2));
        return int(ap * 100);
    }
    catch (std::invalid_argument& ia)
    {
        Log.e(ia.what());
    }
    catch (std::out_of_range& oor)
    {
        Log.e(oor.what());
    }
    return -1;
}

string CameraStringConversion::apertureToString(int aperture)
{
    return "f/" + toString(aperture / 100.0f, -1);
}

int CameraStringConversion::batteryLevelToInt(string battery)
{
    try
    {
        return std::stoi(battery.substr(0, battery.size() - 1));
    }
    catch (std::invalid_argument& ia)
    {
        Log.e(ia.what());
    }
    catch (std::out_of_range& oor)
    {
        Log.e(oor.what());
    }
    return -1;
}

string CameraStringConversion::toString(float value, int precision)
{
    stringstream ss;

    if (precision >= 0)
    {
        ss.precision(precision);

        ss << std::fixed << value;
    }
    else
    {
        ss << value;
    }
    return ss.str();
}

int CameraStringConversion::findNearest(const vector<int>& choices, int value)
{
    if (choices.size() == 1)
    {
        return choices[0];
    }
    else if (choices.size() == 0)
    {
        return -1;
    }

    if (value <= choices[0])
    {
        return choices[0];
    }
    else if (value >= choices[choices.size() - 1])
    {
        return choices[choices.size() - 1];
    }
    else
    {
        int a = 0;
        int b = choices.size() - 1;

        while (b - a > 1)
        {
            int c = (b + a) / 2;

            if (value == choices[c])
            {
                return choices[c];
            }
            else if (value > choices[c])
            {
                a = c;
            }
            else
            {
                b = c;
            }
        }

        if (choices[b] - value < value - choices[a])
        {
            return choices[b];
        }
        else
        {
            return choices[a];
        }
    }
}

}  // namespace gphotow