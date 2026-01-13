#include <psp2/kernel/modulemgr.h>
#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <psp2/motion.h>
#include <psp2/power.h>

// Module Info
// SCE_MODULE_INFO(vita_input, 0, 1, 1);

// Standard entry points
int _vsh_module_start(SceSize args, void *argp) { return SCE_KERNEL_START_SUCCESS; }
int _vsh_module_stop(SceSize args, void *argp) { return SCE_KERNEL_STOP_SUCCESS; }
int _vsh_module_exit(void) { return SCE_KERNEL_STOP_SUCCESS; }
int _start(SceSize args, void *argp) { return SCE_KERNEL_START_SUCCESS; }
int _stop(SceSize args, void *argp) { return SCE_KERNEL_STOP_SUCCESS; }
int _exit(void) { return SCE_KERNEL_STOP_SUCCESS; }

static SceCtrlData pad_data[4];
static unsigned int last_buttons[4];
static SceTouchData touch_front;
static SceTouchData touch_back;
static char string_buffer[256];

// Helper: Custom integer to string
char *itoa_simple(int num, char *str)
{
    char temp[12];
    int i = 0;
    if (num == 0)
    {
        *str++ = '0';
        *str = '\0';
        return str;
    }
    if (num < 0)
    {
        *str++ = '-';
        num = -num;
    }
    while (num)
    {
        temp[i++] = (num % 10) + '0';
        num /= 10;
    }
    while (i--)
    {
        *str++ = temp[i];
    }
    *str = '\0';
    return str;
}

// Fixed-point float to string
char *ftoa_fixed(int total_milli, char *str)
{
    if (total_milli < 0)
    {
        *str++ = '-';
        total_milli = -total_milli;
    }
    int ipart = total_milli / 1000;
    int fpart = total_milli % 1000;
    str = itoa_simple(ipart, str);
    *str++ = '.';
    if (fpart < 100)
        *str++ = '0';
    if (fpart < 10)
        *str++ = '0';
    str = itoa_simple(fpart, str);
    return str;
}

double vita_init()
{
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);
    sceMotionStartSampling();
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, 1);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, 1);
    for (int i = 0; i < 4; i++)
        last_buttons[i] = 0;
    return 1.0;
}

double vita_update()
{
    for (int i = 0; i < 4; i++)
    {
        last_buttons[i] = pad_data[i].buttons;
        sceCtrlPeekBufferPositive(i, &pad_data[i], 1);
    }
    sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch_front, 1);
    sceTouchPeek(SCE_TOUCH_PORT_BACK, &touch_back, 1);
    return 1.0;
}

double vita_is_connected(double port)
{
    SceCtrlPortInfo info;
    sceCtrlGetControllerPortInfo(&info);
    int p = (int)port;
    if (p < 0 || p > 3)
        return 0.0;
    return (info.port[p] != SCE_CTRL_TYPE_UNPAIRED) ? 1.0 : 0.0;
}

double vita_check(double port, double mask, double type)
{
    int p = (int)port;
    unsigned int m = (unsigned int)mask;
    int t = (int)type;
    if (t == 0)
        return (pad_data[p].buttons & m) ? 1.0 : 0.0;
    if (t == 1)
        return ((pad_data[p].buttons & m) && !(last_buttons[p] & m)) ? 1.0 : 0.0;
    if (t == 2)
        return (!(pad_data[p].buttons & m) && (last_buttons[p] & m)) ? 1.0 : 0.0;
    return 0.0;
}

double vita_axis(double port, double axis_idx)
{
    int p = (int)port;
    int val = 128;
    if (axis_idx == 0)
        val = pad_data[p].lx;
    else if (axis_idx == 1)
        val = pad_data[p].ly;
    else if (axis_idx == 2)
        val = pad_data[p].rx;
    else if (axis_idx == 3)
        val = pad_data[p].ry;
    return ((double)val - 128.0) / 128.0;
}

char *vita_get_quat()
{
    SceMotionState ms;
    sceMotionGetState(&ms);
    char *s = string_buffer;
    s = ftoa_fixed((int)(ms.deviceQuat.x * 1000.0f), s);
    *s++ = ',';
    s = ftoa_fixed((int)(ms.deviceQuat.y * 1000.0f), s);
    *s++ = ',';
    s = ftoa_fixed((int)(ms.deviceQuat.z * 1000.0f), s);
    *s++ = ',';
    s = ftoa_fixed((int)(ms.deviceQuat.w * 1000.0f), s);
    return string_buffer;
}

char *vita_get_touch(double port)
{
    SceTouchData *td = ((int)port == 0) ? &touch_front : &touch_back;
    char *s = string_buffer;
    s = itoa_simple(td->reportNum, s);
    *s++ = ',';
    int x = (td->reportNum > 0) ? (td->report[0].x * 1000) / 1919 : -1000;
    int y = (td->reportNum > 0) ? (td->report[0].y * 1000) / 1087 : -1000;
    s = ftoa_fixed(x, s);
    *s++ = ',';
    s = ftoa_fixed(y, s);
    return string_buffer;
}

char *vita_get_battery()
{
    char *s = string_buffer;
    s = itoa_simple(scePowerGetBatteryLifePercent(), s);
    *s++ = ',';
    s = itoa_simple(scePowerIsBatteryCharging(), s);
    *s++ = ',';
    s = itoa_simple(scePowerGetBatteryTemp(), s);
    return string_buffer;
}

double vita_get_type(double port)
{
    SceCtrlPortInfo info;
    sceCtrlGetControllerPortInfo(&info);
    int p = (int)port;
    if (p == 0 && info.port[0] == SCE_CTRL_TYPE_UNPAIRED)
        return 1.0;
    if (info.port[p] == SCE_CTRL_TYPE_DS3)
        return 2.0;
    if (info.port[p] == SCE_CTRL_TYPE_DS4)
        return 3.0;
    return 0.0;
}