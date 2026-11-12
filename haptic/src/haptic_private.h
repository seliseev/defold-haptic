#ifndef DM_HAPTIC_PRIVATE_H
#define DM_HAPTIC_PRIVATE_H

// Platform layer of the haptic extension. Every platform file implements all
// five functions; platforms without haptics get the no-op stubs in haptic.cpp.

namespace dmHaptic
{
    enum Impact
    {
        IMPACT_LIGHT  = 0,
        IMPACT_MEDIUM = 1,
        IMPACT_HEAVY  = 2,
        IMPACT_COUNT
    };

    enum Notification
    {
        NOTIFICATION_SUCCESS = 0,
        NOTIFICATION_WARNING = 1,
        NOTIFICATION_ERROR   = 2,
        NOTIFICATION_COUNT
    };

    bool PlatformIsSupported();
    void PlatformImpact(Impact style);
    void PlatformNotification(Notification type);
    void PlatformSelection();
    void PlatformVibrate(int duration_ms);
}

#endif // DM_HAPTIC_PRIVATE_H
