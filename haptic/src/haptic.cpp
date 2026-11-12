// haptic.cpp
// Lua bindings of the haptic extension.
//
// The platform work lives in haptic_ios.mm, haptic_android.cpp and
// haptic_html5.cpp. On every other platform the functions are registered but
// do nothing, so game code can call them without platform checks.

#define LIB_NAME "HapticExt"
#define MODULE_NAME "haptic"

#include <dmsdk/sdk.h>

#include "haptic_private.h"

#if !defined(DM_PLATFORM_IOS) && !defined(DM_PLATFORM_ANDROID) && !defined(DM_PLATFORM_HTML5)
namespace dmHaptic
{
    bool PlatformIsSupported()              { return false; }
    void PlatformImpact(Impact)             {}
    void PlatformNotification(Notification) {}
    void PlatformSelection()                {}
    void PlatformVibrate(int)               {}
}
#endif

static int Haptic_IsSupported(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 1);
    lua_pushboolean(L, dmHaptic::PlatformIsSupported());
    return 1;
}

static int Haptic_Impact(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    int style = lua_isnoneornil(L, 1) ? (int) dmHaptic::IMPACT_LIGHT : (int) luaL_checkinteger(L, 1);
    if (style < 0 || style >= dmHaptic::IMPACT_COUNT)
    {
        return DM_LUA_ERROR("haptic.impact: unknown style %d, use haptic.IMPACT_*", style);
    }
    dmHaptic::PlatformImpact((dmHaptic::Impact) style);
    return 0;
}

static int Haptic_Notification(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    int type = lua_isnoneornil(L, 1) ? (int) dmHaptic::NOTIFICATION_SUCCESS : (int) luaL_checkinteger(L, 1);
    if (type < 0 || type >= dmHaptic::NOTIFICATION_COUNT)
    {
        return DM_LUA_ERROR("haptic.notification: unknown type %d, use haptic.NOTIFICATION_*", type);
    }
    dmHaptic::PlatformNotification((dmHaptic::Notification) type);
    return 0;
}

static int Haptic_Selection(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    dmHaptic::PlatformSelection();
    return 0;
}

static int Haptic_Vibrate(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    int duration_ms = lua_isnoneornil(L, 1) ? 400 : (int) luaL_checkinteger(L, 1);
    if (duration_ms <= 0)
    {
        return 0;
    }
    // A stuck motor is worse than a short buzz: cap a mistyped duration.
    if (duration_ms > 5000)
    {
        duration_ms = 5000;
    }
    dmHaptic::PlatformVibrate(duration_ms);
    return 0;
}

static const luaL_reg Module_methods[] =
{
    {"is_supported", Haptic_IsSupported},
    {"impact",       Haptic_Impact},
    {"notification", Haptic_Notification},
    {"selection",    Haptic_Selection},
    {"vibrate",      Haptic_Vibrate},
    {0, 0}
};

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);
    luaL_register(L, MODULE_NAME, Module_methods);

#define SETCONSTANT(name) \
    lua_pushnumber(L, (lua_Number) dmHaptic::name); \
    lua_setfield(L, -2, #name);

    SETCONSTANT(IMPACT_LIGHT)
    SETCONSTANT(IMPACT_MEDIUM)
    SETCONSTANT(IMPACT_HEAVY)
    SETCONSTANT(NOTIFICATION_SUCCESS)
    SETCONSTANT(NOTIFICATION_WARNING)
    SETCONSTANT(NOTIFICATION_ERROR)

#undef SETCONSTANT

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

static dmExtension::Result AppInitializeHaptic(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeHaptic(dmExtension::Params* params)
{
    LuaInit(params->m_L);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeHaptic(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeHaptic(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(HapticExt, LIB_NAME, AppInitializeHaptic, AppFinalizeHaptic, InitializeHaptic, 0, 0, FinalizeHaptic)
