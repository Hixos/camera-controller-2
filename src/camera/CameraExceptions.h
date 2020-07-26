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

#include <exception>
#include <sstream>
#include <string>
#include <gphoto2/gphoto2.h>

using std::exception;
using std::stringstream;
using std::string;

namespace gphotow
{
struct CameraException : public exception
{
    CameraException(string wh) : wh(wh) {}

    const char* what() const noexcept override { return wh.c_str(); }

private:
    string wh;
};

struct CameraConnectionError : public CameraException
{
    CameraConnectionError(string reason = "")
        : CameraException("Error connecting to camera: " + reason)
    {
    }
};
struct WidgetReadOnlyError : public CameraException
{
    WidgetReadOnlyError(string widget)
        : CameraException("Widget " + widget + " is read only.")
    {
    }
};


struct GPhotoError : public CameraException
{
    int error;

    GPhotoError(int gphoto_error)
        : CameraException("GPhoto returned error code" + gphoto_error)
    {
        error = gphoto_error;
    }

    virtual const char* what() const noexcept override
    {
        return gp_result_as_string(error);
    }

};

}  // namespace gphotow