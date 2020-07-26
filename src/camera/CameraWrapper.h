/**
 * Copyright (c) 2020 Luca Erbetta
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <gphoto2/gphoto2.h>
#include <stdlib.h>

#include <functional>
#include <map>
#include <string>
#include <vector>
#include <cstring>

#include "CameraExceptions.h"
#include "CameraWidget.h"
#include "camera_mappings/mappings.h"

using std::map;
using std::string;
using std::vector;

namespace gphotow
{
static const string NOT_A_GOOD_SERIAL = "NOT_A_GOOD_SERIAL";

struct CameraPath
{
    string folder;
    string name;

    CameraPath(CameraFilePath path)
    {
        folder = path.folder;
        name = path.name;
    }

    string getPath() const
    {
        return folder + "/" +  name;
    }

    CameraFilePath toCameraFilePath() const
    {
        CameraFilePath path;
        strcpy(path.name, name.c_str());
        strcpy(path.folder, folder.c_str());

    return path;
    }
};

class CameraWrapper
{
    friend class CameraWidgetBase;

public:
    CameraWrapper();
    ~CameraWrapper();

    CameraWrapper(CameraWrapper const&) = delete;
    void operator=(CameraWrapper const&) = delete;

    /**
     * @brief Connects to a camera and returns its serial.
     *
     * @throw GPhotoError
     */
    string connect();

    /**
     * @brief Disconnects from a connected camera.
     *
     */
    bool disconnect();

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
     * @throw GPhotoError
     * @return string Serial number
     */
    string getSerialNumber();

    /**
     * @brief Returns the current exposure time in useconds.
     * @throw GPhotoError
     * @return exposure time in usec or 0 if error, -1 if BULB
     */
    int getExposureTime();

    /**
     * @brief Sets the exposure time
     * @throw GPhotoError
     * @param    exposuretime Exposure time in us. Must be one of the values
     * returned in listExposureTimes()
     */
    void setExposureTime(int exposuretime);

    /**
     * @brief Lists all the available exposure times in the current exposure
     * mode.
     * @throw GPhotoError
     * @return list of exposure times, or empty list if error.
     */
    vector<int> listExposureTimes();

    /**
     * @brief Returns the current aperture.
     * @throw GPhotoError
     * @return F Number * 100, 0 if error
     */
    int getAperture();

    /**
     * @brief Sets the aperture
     * @throw GPhotoError
     * @param    aperture F-Number * 100. Must be one of the values
     * returned in listApertures()
     */
    void setAperture(int aperture);

    /**
     * @brief Lists all the available apertures.
     * @throw GPhotoError
     * @return list of apertures (F-Number * 100), or empty list if error.
     */
    vector<int> listApertures();

    /**
     * @brief Returns the current ISO.
     * @throw GPhotoError
     * @return ISO, 0 if error
     */
    int getISO();

    /**
     * @brief Sets the aperture
     * @throw GPhotoError
     * @param    iso Must be one of the values returned in listISOs()
     */
    void setISO(int iso);

    /**
     * @brief Lists all the available ISOs.
     * @throw GPhotoError
     * @return list of ISOs, or empty list if error.
     */
    vector<int> listISOs();

    /**
     * @brief Returns the current battery percent of the camera
     * @throw GPhotoError
     * @return Battery percent, or -1 if error
     */
    int getBatteryPercent();

    /**
     * @brief Get the current Exposure Program of the camera
     * @throw GPhotoError
     */
    string getExposureProgram();

    /**
     * @brief Returns the current trigger mode.
     * @throw GPhotoError
     */
    string getTriggerMode();

    /**
     * @brief Returns the current focal length.
     * @throw GPhotoError
     */
    int getFocalLength();

    /**
     * @brief Sets the trigger mode
     * @throw GPhotoError
     */
    void setTriggerMode(string mode);

    /**
     * @brief Lists all the available trigger modes
     * @throw GPhotoError
     */
    vector<string> listTriggerModes();

    /**
     * @brief Returns the number of captures in a single burst
     * @throw GPhotoError
     * @return Number of captures in a burst
     */
    int getBurstCount();

    /**
     * @brief Set the number of captures in a burst
     * @throw GPhotoError
     */
    void setBurstCount(int count);

    /**
     * @brief Returns the maximum number of captures in a single burst
     * @throw GPhotoError
     */
    int getMaxBurstCount();

    // CameraFilePath capture();
    CameraPath wiredCapture();

    void downloadFile(CameraFilePath path, string destination);

private:
    void freeCamera();
    

    bool connected = false;

    string serial = NOT_A_GOOD_SERIAL;

    Camera* camera = nullptr;
    GPContext* context;
};

class CameraStringConversion
{
public:
    static int shutterSpeedToInt(string shutter_speed);
    static string shutterSpeedToString(int shutter_speed);

    static int apertureToInt(string aperture);
    static string apertureToString(int aperture);

    // static int isoToInt(string iso);
    // static string isoToString(int iso);

    static int batteryLevelToInt(string battery_level);

private:
    static int findNearest(const vector<int>& choices, int value);
    static string toString(float value, int precision);
    CameraStringConversion() {}
};

}  // namespace gphotow