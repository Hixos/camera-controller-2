
#include "CameraWrapper2.h"
#include <gphoto2.h>
#include <cmath>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <thread>

#include "utils/logger.h"
// #include "utils/RemoteTrigger.h"

using namespace std::this_thread;

using std::max;
using std::min;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

typedef system_clock Clock;

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
            Log.e("Error instantiating camera: %d", result);
            return NOT_A_GOOD_SERIAL;
        }

        result = gp_camera_init(camera, context);

        if (result == GP_OK)
        {
            serial = getSerialNumber();

            // Sleep for 2 seconds to avoid errors if capturing too early
            std::this_thread::sleep_for(seconds(2));

            Log.i("Connected to camera: %s", serial.c_str());
            connected = true;
            return serial;
        }

        Log.e("Error initiating camera: %d", result);
        gp_camera_free(camera);
        return NOT_A_GOOD_SERIAL;
    }

    return serial;
}

void CameraWrapper::disconnect()
{
    if (connected)
    {
        freeCamera();
        connected = false;
    }
}

bool CameraWrapper::isConnected() { return connected; }

bool CameraWrapper::isResponsive()
{
    return connected && getSerialNumber() == serial;
}

string CameraWrapper::getSerialNumber()
{
    try
    {
        CameraTextWidgetWrapper serial{*this, CONFIG_SERIAL_NUMBER};
        return serial.getValue();
    }
    catch (GPhotoException gpe)
    {
        Log.e("Could not read serial: %s", gpe.what());
    }

    return NOT_A_GOOD_SERIAL;
}


bool CameraWrapper::downloadFile(CameraFilePath path, string dest_file_path)
{
    CameraFile* file;
    int result, fd;
    bool success = false, gp_file_created = false;
    Log.d("Download file: %s, fld: %s", path.name, path.folder);

    Log.d("Download dest: %s", dest_file_path.c_str());
    FILE* f = fopen(dest_file_path.c_str(), "w");

    if (f == NULL)
    {
        Log.e("Error opening file (%s): %s", dest_file_path.c_str(),
              std::strerror(errno));
        goto out;
    }

    fd = fileno(f);

    if (fd < 0)
    {
        Log.e("Error getting file descriptor (%s): %d", dest_file_path.c_str(),
              fd);
        goto out;
    }

    result = gp_file_new_from_fd(&file, fd);
    if (result != GP_OK)
    {
        Log.e("Error creating CameraFile (%s): %d", dest_file_path.c_str(),
              result);
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
        Log.e("Error getting file from camera (%s): %d", dest_file_path.c_str(),
              result);
        goto out;
    }
    success = true;
out:
    if (f != NULL)
    {
        fclose(f);
    }
    if (gp_file_created)
    {
        gp_file_free(file);
    }
    return success;
}



CameraWrapper::CameraWidgetWrapper::CameraWidgetWrapper(CameraWrapper& camera,
                                                        string config_name)
    : camera(camera)
{
    int result = gp_camera_get_single_config(camera.camera, config_name.c_str(),
                                             &widget, camera.context);

    if (result != GP_OK)
    {
        throw GPhotoException(result);
    }
}

CameraWrapper::CameraWidgetWrapper::CameraWidgetWrapper(CameraWrapper& camera,
                                                        CameraWidget* widget)
    : camera(camera), widget(widget)
{
}

CameraWrapper::CameraWidgetWrapper::~CameraWidgetWrapper()
{
    if (widget != nullptr)
        gp_widget_free(widget);
}

CameraWidgetType CameraWrapper::CameraWidgetWrapper::getType()
{
    return getType(widget);
}

CameraWidgetType CameraWrapper::CameraWidgetWrapper::getType(
    CameraWidget* widget)
{
    CameraWidgetType type;
    int result = gp_widget_get_type(widget, &type);
    if (result != GP_OK)
    {
        throw GPhotoException(result);
    }
    return type;
}

string CameraWrapper::CameraWidgetWrapper::getName()
{
    char* value;
    int result = gp_widget_get_name(widget, &value);

    if (result != GP_OK)
    {
        return "";
    }
    else
    {
        return string(value);
    }
}

bool CameraWrapper::CameraWidgetWrapper::isReadOnly()
{
    int* value;
    int result = gp_widget_get_readonly(widget, value);

    if (result != GP_OK)
    {
        return true;
    }
    else
    {
        return (bool)(*value);
    }
}

void CameraWrapper::CameraWidgetWrapper::apply()
{
    int result = gp_camera_set_single_config(camera.camera, getName().c_str(),
                                             widget, camera.context);

    if (result != GP_OK)
    {
        throw GPhotoException(result);
    }
}

CameraWrapper::CameraTextWidgetWrapper::CameraTextWidgetWrapper(
    CameraWrapper& camera, string config_name)
    : CameraWidgetWrapper(camera, config_name)
{
    CameraWidgetType type = CameraWidgetWrapper::getType();
    if (type != GP_WIDGET_MENU && type != GP_WIDGET_RADIO &&
        type != GP_WIDGET_TEXT)
    {
        throw GPhotoException(GP_ERROR_BAD_PARAMETERS);
    }
}

CameraWrapper::CameraTextWidgetWrapper::CameraTextWidgetWrapper(
    CameraWrapper& camera, CameraWidget* widget)
    : CameraWidgetWrapper(camera, widget)
{
    CameraWidgetType type = CameraWidgetWrapper::getType();
    if (type != GP_WIDGET_MENU && type != GP_WIDGET_RADIO &&
        type != GP_WIDGET_TEXT)
    {
        throw GPhotoException(GP_ERROR_BAD_PARAMETERS);
    }
}

string CameraWrapper::CameraTextWidgetWrapper::getValue()
{
    char* value;
    int result = gp_widget_get_value(widget, &value);

    if (result != GP_OK)
    {
        return "";
    }
    else
    {
        return string(value);
    }
}

void CameraWrapper::CameraTextWidgetWrapper::setValue(string value)
{
    gp_widget_set_value(widget, value.c_str());
}
