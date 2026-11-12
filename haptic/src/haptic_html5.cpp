#if defined(DM_PLATFORM_HTML5)

#include <dmsdk/sdk.h>

#include "haptic_private.h"

// Implemented in lib/web/lib_haptic.js on top of navigator.vibrate().
extern "C"
{
    int  HapticJs_IsSupported();
    void HapticJs_Impact(int style);
    void HapticJs_Notification(int type);
    void HapticJs_Selection();
    void HapticJs_Vibrate(int duration_ms);
}

namespace dmHaptic
{
    bool PlatformIsSupported()                   { return HapticJs_IsSupported() != 0; }
    void PlatformImpact(Impact style)            { HapticJs_Impact((int) style); }
    void PlatformNotification(Notification type) { HapticJs_Notification((int) type); }
    void PlatformSelection()                     { HapticJs_Selection(); }
    void PlatformVibrate(int duration_ms)        { HapticJs_Vibrate(duration_ms); }
}

#endif // DM_PLATFORM_HTML5
