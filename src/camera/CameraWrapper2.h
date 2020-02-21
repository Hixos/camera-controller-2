/*
 *  Created on: Jul 12, 2018
 *      Author: Luca Erbetta
 */

#ifndef SRC_CAMERA_CAMERA_H
#define SRC_CAMERA_CAMERA_H

#include <gphoto2/gphoto2.h>
#include <stdlib.h>
#include <exception>
#include <sstream>
#include <string>
#include <vector>

using std::exception;
using std::string;
using std::vector;

static const string NOT_A_GOOD_SERIAL = "NOT_A_GOOD_SERIAL";

static const string CONFIG_SERIAL_NUMBER = "serialnumber";
static const string CONFIG_EXPOSURE_TIME = "500d";

struct GPhotoException : public exception
{
    GPhotoException(int gphoto_error) : error_code(gphoto_error) {}

    const char* what() const noexcept override
    {
        std::stringstream ss;
        ss << "GPhoto returned error code " << error_code;
        return ss.str().c_str();
    }

private:
    int error_code;
};

class CameraWrapper
{
public:
    CameraWrapper();
    ~CameraWrapper();

    CameraWrapper(CameraWrapper const&) = delete;
    void operator=(CameraWrapper const&) = delete;

    /**
     * @brief Connects to a camera and returns its serial.
     *
     * @return string Serial number of connected camera if successfull
     * connection, NOT_A_GOOD_SERIAL otherwise.
     */
    string connect();

    /**
     * @brief Disconnects from a connected camera.
     *
     */
    void disconnect();

    /**
     * @brief True if connected to a camera.
     *
     * @return Wether a camera is connected or not.
     */
    bool isConnected();

    /**
     * @brief Pools the camera to see if it's responsive.
     *
     * @return Wether a camera is responsive or not.
     */
    bool isResponsive();

    /**
     * @brief Returns the serial number of the connected camera.
     *
     * @return string Serial number
     */
    string getSerialNumber();

    bool downloadFile(CameraFilePath path, string destination);
private:
    class CameraWidgetWrapper
    {
    public:
        CameraWidgetWrapper(CameraWrapper& camera, string config_name);
        CameraWidgetWrapper(CameraWrapper& camera, CameraWidget* widget);

        ~CameraWidgetWrapper();
        
        CameraWidgetWrapper(const CameraWidgetWrapper&) = delete;
        CameraWidgetWrapper& operator=(const CameraWidgetWrapper&) = delete;
        CameraWidgetWrapper(CameraWidgetWrapper&& other) noexcept = delete;
        CameraWidgetWrapper& operator=(CameraWidgetWrapper&& other) noexcept =
            delete;
        

        CameraWidgetType getType();

        static CameraWidgetType getType(CameraWidget* widget);

        string getName();

        bool isReadOnly();

        void apply();

    protected:
        CameraWrapper& camera;
        CameraWidget* widget = nullptr;
    };

    class CameraTextWidgetWrapper : public CameraWidgetWrapper
    {
    public:
        CameraTextWidgetWrapper(CameraWrapper& camera, string config_name);
        CameraTextWidgetWrapper(CameraWrapper& camera, CameraWidget* widget);

        string getValue();

        void setValue(string value);
    };

    void freeCamera();

    bool connected = false;

    static int exposureTimeFromString(string exposure_time);

    string serial = NOT_A_GOOD_SERIAL;

    Camera* camera = nullptr;
    GPContext* context;
};

#endif /* SRC_CAMERA_CAMERA_H_ */
