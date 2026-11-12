// Web backend of the haptic extension: the Vibration API.
// navigator.vibrate() has no amplitude, so effects differ only in timing.
// Browsers without the API (Safari, including every browser on iOS) get no-ops.

var LibHaptic = {
    $HapticState: {
        // Indexes match dmHaptic::Impact and dmHaptic::Notification.
        impact: [10, 20, 35],
        notification: [
            [20, 80, 35],
            [35, 100, 25],
            [30, 60, 30, 60, 45],
        ],
        run: function (pattern) {
            try {
                if (typeof navigator !== "undefined" && typeof navigator.vibrate === "function") {
                    navigator.vibrate(pattern);
                }
            } catch (e) {
                // Blocked by permissions policy or missing user activation: nothing to do.
            }
        },
    },

    HapticJs_IsSupported: function () {
        return typeof navigator !== "undefined" && typeof navigator.vibrate === "function" ? 1 : 0;
    },

    HapticJs_Impact__deps: ["$HapticState"],
    HapticJs_Impact: function (style) {
        var duration = HapticState.impact[style];
        if (duration !== undefined) {
            HapticState.run(duration);
        }
    },

    HapticJs_Notification__deps: ["$HapticState"],
    HapticJs_Notification: function (type) {
        var pattern = HapticState.notification[type];
        if (pattern !== undefined) {
            HapticState.run(pattern);
        }
    },

    HapticJs_Selection__deps: ["$HapticState"],
    HapticJs_Selection: function () {
        HapticState.run(10);
    },

    HapticJs_Vibrate__deps: ["$HapticState"],
    HapticJs_Vibrate: function (duration_ms) {
        HapticState.run(duration_ms);
    },
};

mergeInto(LibraryManager.library, LibHaptic);
