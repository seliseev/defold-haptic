#if defined(DM_PLATFORM_IOS)

#import <UIKit/UIKit.h>
#import <AudioToolbox/AudioToolbox.h>

#include <dmsdk/sdk.h>

#include "haptic_private.h"

// Feedback generators are created once and kept: creating one per call adds
// latency to the first tap after the Taptic Engine has gone idle.

namespace dmHaptic
{
    static UIImpactFeedbackGenerator*       g_Impact[IMPACT_COUNT] = { nil, nil, nil };
    static UINotificationFeedbackGenerator* g_Notification         = nil;
    static UISelectionFeedbackGenerator*    g_Selection            = nil;

    bool PlatformIsSupported()
    {
        // UIKit has no public "has Taptic Engine" query. Every iPhone that the
        // engine can run on has one, and no iPad does.
        return [UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPhone;
    }

    void PlatformImpact(Impact style)
    {
        if (g_Impact[style] == nil)
        {
            UIImpactFeedbackStyle ios_style = UIImpactFeedbackStyleLight;
            if (style == IMPACT_MEDIUM) ios_style = UIImpactFeedbackStyleMedium;
            if (style == IMPACT_HEAVY)  ios_style = UIImpactFeedbackStyleHeavy;
            g_Impact[style] = [[UIImpactFeedbackGenerator alloc] initWithStyle:ios_style];
        }
        [g_Impact[style] impactOccurred];
    }

    void PlatformNotification(Notification type)
    {
        if (g_Notification == nil)
        {
            g_Notification = [[UINotificationFeedbackGenerator alloc] init];
        }
        UINotificationFeedbackType ios_type = UINotificationFeedbackTypeSuccess;
        if (type == NOTIFICATION_WARNING) ios_type = UINotificationFeedbackTypeWarning;
        if (type == NOTIFICATION_ERROR)   ios_type = UINotificationFeedbackTypeError;
        [g_Notification notificationOccurred:ios_type];
    }

    void PlatformSelection()
    {
        if (g_Selection == nil)
        {
            g_Selection = [[UISelectionFeedbackGenerator alloc] init];
        }
        [g_Selection selectionChanged];
    }

    void PlatformVibrate(int duration_ms)
    {
        // iOS only offers the fixed-length system vibration; the duration is ignored.
        AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
    }
}

#endif // DM_PLATFORM_IOS
