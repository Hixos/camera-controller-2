/**
 * Copyright (c) 2020
 * Authors: Luca Erbetta (luca.erbetta105@gmail.com)
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

#include <cstdint>
#include <string>
#include <vector>

#include "camera/CameraData.h"
#include "camera/wrapper/CameraWrapper.h"
#include "events/Event.h"

using std::string;
using std::vector;


enum CCEvents : uint16_t
{
    EV_CAMERA_CONNECT = EV_FIRST_SIGNAL,
    EV_CAMERA_CONNECTED,
    EV_CAMERA_CONNECTION_ERROR,
    EV_CAMERA_DISCONNECT,
    EV_CAMERA_DISCONNECTED,
    EV_CAMERA_GENERIC_ERROR,

    EV_CAMERA_SET_CONFIG,
    EV_CAMERA_GET_STATUS,
    EV_CAMERA_VAL_CONFIG,
    EV_CAMERA_VAL_STATUS,

    EV_CAMERA_GET_OPTIONS,
    EV_CAMERA_OPTIONS_SHUTTER_SPEED,
    EV_CAMERA_OPTIONS_APERTURE,
    EV_CAMERA_OPTIONS_ISO,
    EV_CAMERA_OPTIONS_TRIGGER_MODE,

    EV_CAMERA_CAPTURE_WIRED,
    EV_CAMERA_CAPTURE_SUCCESS,
    EV_CAMERA_CAPTURE_ERROR,

    EV_CAMERA_DOWNLOAD
};

enum CCTopic : uint8_t
{
    TOPIC_CAMERA_CMD,
    TOPIC_CAMERA_EVENT,
    TOPIC_CAMERA_STATUS
};

struct CameraConnectedEvent : public Event
{
    string serial;

    CameraConnectedEvent(string serial)
        : Event(EV_CAMERA_CONNECTED), serial(serial)
    {
    }
};

struct CameraErrorEvent : public Event
{
    int errorcode;
    CameraErrorEvent(uint16_t sig, int errorcode)
        : Event(sig), errorcode(errorcode)
    {
    }

    string what() const { return string(gp_result_as_string(errorcode)); }
};

struct CameraStatusEvent : public Event
{
    CameraStatus status;

    CameraStatusEvent(CameraStatus status = {})
        : Event(EV_CAMERA_VAL_STATUS), status(status)
    {
    }
};
struct CameraConfigEvent : public Event
{
    CameraConfig config;

    CameraConfigEvent(uint16_t sig, CameraConfig config = {})
        : Event(sig), config(config)
    {
    }
};

template<typename OptionT>
struct CameraOptionListEvent : public Event
{
    vector<OptionT> options;

    CameraOptionListEvent(uint16_t sig, const vector<OptionT>& options)
        : Event(sig), options(options)
    {
    }
};

struct CameraCapturedEvent : public Event
{
    gphotow::CameraPath path;

    CameraCapturedEvent(gphotow::CameraPath path)
        : Event(EV_CAMERA_CAPTURE_SUCCESS), path(path)
    {
    }
};

struct CameraDownloadEvent : public Event
{
    gphotow::CameraPath path;
    string dest;

    CameraDownloadEvent(gphotow::CameraPath path, string dest)
        : Event(EV_CAMERA_DOWNLOAD), path(path), dest(dest)
    {
    }
};