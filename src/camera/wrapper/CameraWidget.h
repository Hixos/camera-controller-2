#include <gphoto2.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

#pragma once

namespace gphotow
{
class CameraWrapper;

class CameraWidgetBase
{
public:
    /**
     * @brief Constructs a camera widget from the config name
     *
     * @throw GPhotoError
     * @param    camera
     * @param    config_name
     * @return
     */
    CameraWidgetBase(CameraWrapper& camera, string config_name);

    /**
     * @brief Constructs a camera widget from an already existing one
     *
     * @param    camera
     * @param    widget
     * @return
     */
    CameraWidgetBase(CameraWrapper& camera, CameraWidget* widget);

    ~CameraWidgetBase();

    CameraWidgetBase(const CameraWidgetBase&) = delete;
    CameraWidgetBase& operator=(const CameraWidgetBase&) = delete;
    CameraWidgetBase(CameraWidgetBase&& other) noexcept  = delete;
    CameraWidgetBase& operator=(CameraWidgetBase&& other) noexcept = delete;

    /**
     * @brief Get the type of the widget
     * @throw GPhotoError
     * @return Type of the widget
     */
    CameraWidgetType getType();

    /**
     * @brief Get the type of the provided widget
     * @throw GPhotoError
     * @return Type of the widget
     */
    static CameraWidgetType getType(CameraWidget* widget);

    /**
     * @brief Get the name of the widget
     * @return Name of the widget, or empty string
     */
    string getName();

    /**
     * @brief Get the possible values of the widget
     *
     * @return  Possible values, or empty vector
     */
    vector<string> getAvailableValues();

    bool isReadOnly();

    void apply();

protected:
    CameraWrapper& camera;
    CameraWidget* widget = nullptr;
    string widget_name;
};

/**
 * @brief Wrapper for GP_WIDGET_MENU, GP_WIDGET_TEXT, GP_WIDGET_RADIO
 */
class CameraWidgetString : public CameraWidgetBase
{
public:
    CameraWidgetString(CameraWrapper& camera, string config_name);
    CameraWidgetString(CameraWrapper& camera, CameraWidget* widget);

    string getValue();

    void setValue(string value);
};

/**
 * @brief Wrapper for GP_WIDGET_RANGE
 */
class CameraWidgetRange : public CameraWidgetBase
{
public:
    struct Range
    {
        float min;
        float max;
        float step;
    };

    CameraWidgetRange(CameraWrapper& camera, string config_name);
    CameraWidgetRange(CameraWrapper& camera, CameraWidget* widget);

    float getValue();
    Range getRange();

    void setValue(float value);
};

}  // namespace gphotow