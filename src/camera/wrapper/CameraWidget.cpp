#include "CameraWidget.h"

#include "CameraExceptions.h"
#include "CameraWrapper.h"
#include "utils/logger.h"

namespace gphotow
{

CameraWidgetBase::CameraWidgetBase(CameraWrapper& camera, string config_name)
    : camera(camera), widget_name(config_name)
{
    int result = gp_camera_get_single_config(camera.camera, config_name.c_str(),
                                             &widget, camera.context);

    if (result != GP_OK)
    {
        throw GPhotoError(result);
    }
}

CameraWidgetBase::CameraWidgetBase(CameraWrapper& camera, CameraWidget* widget)
    : camera(camera), widget(widget)
{
    widget_name = getName();
}

CameraWidgetBase::~CameraWidgetBase()
{
    if (widget != nullptr)
        gp_widget_free(widget);
}

CameraWidgetType CameraWidgetBase::getType() { return getType(widget); }

CameraWidgetType CameraWidgetBase::getType(CameraWidget* widget)
{
    CameraWidgetType type;
    int result = gp_widget_get_type(widget, &type);
    if (result != GP_OK)
    {
        throw GPhotoError(result);
    }
    return type;
}

string CameraWidgetBase::getName()
{
    const char* value;
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

bool CameraWidgetBase::isReadOnly()
{
    int value;
    int result = gp_widget_get_readonly(widget, &value);

    if (result != GP_OK)
    {
        return true;
    }
    else
    {
        return value != 0;
    }
}

vector<string> CameraWidgetBase::getAvailableValues()
{
    int choices_num = gp_widget_count_choices(widget);
    const char* choice;
    vector<string> choices;

    for (int i = 0; i < choices_num; i++)
    {
        if (gp_widget_get_choice(widget, i, &choice) == GP_OK)
        {
            choices.push_back(string{choice});
        }
        else
        {
            return {};
        }
    }
    return choices;
}

void CameraWidgetBase::apply()
{
    if (!isReadOnly())
    {
        int result = gp_camera_set_single_config(
            camera.camera, widget_name.c_str(), widget, camera.context);

        if (result != GP_OK)
        {
            throw GPhotoError(result);
        }
    }
    else
    {
        throw WidgetReadOnlyError(getName());
    }
}

CameraWidgetString::CameraWidgetString(CameraWrapper& camera,
                                       string config_name)
    : CameraWidgetBase(camera, config_name)
{
    CameraWidgetType type = CameraWidgetBase::getType();
    if (type != GP_WIDGET_MENU && type != GP_WIDGET_RADIO &&
        type != GP_WIDGET_TEXT)
    {
        throw GPhotoError(GP_ERROR_BAD_PARAMETERS);
    }
}

CameraWidgetString::CameraWidgetString(CameraWrapper& camera,
                                       CameraWidget* widget)
    : CameraWidgetBase(camera, widget)
{
    CameraWidgetType type = CameraWidgetBase::getType();
    if (type != GP_WIDGET_MENU && type != GP_WIDGET_RADIO &&
        type != GP_WIDGET_TEXT)
    {
        throw GPhotoError(GP_ERROR_BAD_PARAMETERS);
    }
}

string CameraWidgetString::getValue()
{
    char* value;
    int result = gp_widget_get_value(widget, &value);

    if (result != GP_OK)
    {
        throw GPhotoError(result);
    }
    else
    {
        return string(value);
    }
}

void CameraWidgetString::setValue(string value)
{
    Log.d("[CameraWidget] %s = '%s'", widget_name.c_str(), value.c_str());
    gp_widget_set_value(widget, value.c_str());
}

CameraWidgetRange::CameraWidgetRange(CameraWrapper& camera, string config_name)
    : CameraWidgetBase(camera, config_name)
{
    CameraWidgetType type = CameraWidgetBase::getType();
    if (type != GP_WIDGET_RANGE)
    {
        throw GPhotoError(GP_ERROR_BAD_PARAMETERS);
    }
}

CameraWidgetRange::CameraWidgetRange(CameraWrapper& camera,
                                     CameraWidget* widget)
    : CameraWidgetBase(camera, widget)
{
    CameraWidgetType type = CameraWidgetBase::getType();
    if (type != GP_WIDGET_RANGE)
    {
        throw GPhotoError(GP_ERROR_BAD_PARAMETERS);
    }
}

float CameraWidgetRange::getValue()
{
    float value;
    int result = gp_widget_get_value(widget, &value);

    if (result != GP_OK)
    {
        throw GPhotoError(result);
    }
    else
    {
        return value;
    }
}

void CameraWidgetRange::setValue(float value)
{
    Log.d("[CameraWidget] %s = '%f'", widget_name.c_str(), value);
    gp_widget_set_value(widget, &value);
}

CameraWidgetRange::Range CameraWidgetRange::getRange()
{
    Range range;
    int result =
        gp_widget_get_range(widget, &range.min, &range.max, &range.step);

    if (result != GP_OK)
    {
        throw GPhotoError(result);
    }
    else
    {
        return range;
    }
}

}  // namespace gphotow