package com.defold.haptic;

import android.content.Context;
import android.os.Build;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.os.VibratorManager;
import android.util.Log;

/**
 * Static bridge between the Defold haptic extension and the Android vibrator.
 *
 * Android has no equivalent of the iOS feedback generators, so each effect is
 * built from what the running API level offers:
 *   API 29+  predefined effects (tick / click / heavy click) for impacts
 *   API 26+  one-shots and waveforms with amplitude
 *   older    plain timed vibration
 *
 * Every method swallows Throwable: a vendor vibrator bug must not crash the game.
 */
public class HapticJNI {

    private static final String TAG = "HapticJNI";

    // Must match dmHaptic::Impact in haptic_private.h
    private static final int IMPACT_LIGHT = 0;
    private static final int IMPACT_MEDIUM = 1;
    private static final int IMPACT_HEAVY = 2;

    // Must match dmHaptic::Notification in haptic_private.h
    private static final int NOTIFICATION_SUCCESS = 0;
    private static final int NOTIFICATION_WARNING = 1;
    private static final int NOTIFICATION_ERROR = 2;

    // Fallback impacts for API < 29: {duration ms, amplitude 1..255}
    private static final int[][] IMPACT_ONE_SHOT = {
        {15, 80},
        {25, 150},
        {40, 255},
    };

    // Waveforms: timings alternate pause / buzz, starting with a pause.
    private static final long[][] NOTIFICATION_TIMINGS = {
        {0, 20, 80, 35},
        {0, 35, 100, 25},
        {0, 30, 60, 30, 60, 45},
    };
    private static final int[][] NOTIFICATION_AMPLITUDES = {
        {0, 120, 0, 255},
        {0, 255, 0, 150},
        {0, 200, 0, 200, 0, 255},
    };

    private static Vibrator sVibrator;
    private static boolean sResolved;

    private static Vibrator getVibrator(Context context) {
        if (!sResolved) {
            sResolved = true;
            try {
                Context app = context.getApplicationContext();
                if (Build.VERSION.SDK_INT >= 31) {
                    VibratorManager manager = (VibratorManager) app.getSystemService(Context.VIBRATOR_MANAGER_SERVICE);
                    sVibrator = manager != null ? manager.getDefaultVibrator() : null;
                } else {
                    sVibrator = (Vibrator) app.getSystemService(Context.VIBRATOR_SERVICE);
                }
                if (sVibrator != null && !sVibrator.hasVibrator()) {
                    sVibrator = null;
                }
            } catch (Throwable t) {
                Log.e(TAG, "failed to get vibrator", t);
                sVibrator = null;
            }
        }
        return sVibrator;
    }

    public static boolean isSupported(Context context) {
        return getVibrator(context) != null;
    }

    public static void impact(Context context, int style) {
        try {
            Vibrator vibrator = getVibrator(context);
            if (vibrator == null || style < 0 || style >= IMPACT_ONE_SHOT.length) {
                return;
            }
            if (Build.VERSION.SDK_INT >= 29) {
                int effect = VibrationEffect.EFFECT_TICK;
                if (style == IMPACT_MEDIUM) effect = VibrationEffect.EFFECT_CLICK;
                if (style == IMPACT_HEAVY) effect = VibrationEffect.EFFECT_HEAVY_CLICK;
                vibrator.vibrate(VibrationEffect.createPredefined(effect));
            } else {
                oneShot(vibrator, IMPACT_ONE_SHOT[style][0], IMPACT_ONE_SHOT[style][1]);
            }
        } catch (Throwable t) {
            Log.e(TAG, "impact failed", t);
        }
    }

    public static void notification(Context context, int type) {
        try {
            Vibrator vibrator = getVibrator(context);
            if (vibrator == null || type < 0 || type >= NOTIFICATION_TIMINGS.length) {
                return;
            }
            long[] timings = NOTIFICATION_TIMINGS[type];
            if (Build.VERSION.SDK_INT >= 26) {
                if (vibrator.hasAmplitudeControl()) {
                    vibrator.vibrate(VibrationEffect.createWaveform(timings, NOTIFICATION_AMPLITUDES[type], -1));
                } else {
                    vibrator.vibrate(VibrationEffect.createWaveform(timings, -1));
                }
            } else {
                vibrator.vibrate(timings, -1);
            }
        } catch (Throwable t) {
            Log.e(TAG, "notification failed", t);
        }
    }

    public static void selection(Context context) {
        try {
            Vibrator vibrator = getVibrator(context);
            if (vibrator == null) {
                return;
            }
            if (Build.VERSION.SDK_INT >= 29) {
                vibrator.vibrate(VibrationEffect.createPredefined(VibrationEffect.EFFECT_TICK));
            } else {
                oneShot(vibrator, 10, 60);
            }
        } catch (Throwable t) {
            Log.e(TAG, "selection failed", t);
        }
    }

    public static void vibrate(Context context, int durationMs) {
        try {
            Vibrator vibrator = getVibrator(context);
            if (vibrator == null || durationMs <= 0) {
                return;
            }
            oneShot(vibrator, durationMs, 255);
        } catch (Throwable t) {
            Log.e(TAG, "vibrate failed", t);
        }
    }

    private static void oneShot(Vibrator vibrator, int durationMs, int amplitude) {
        if (Build.VERSION.SDK_INT >= 26) {
            int effective = vibrator.hasAmplitudeControl() ? amplitude : VibrationEffect.DEFAULT_AMPLITUDE;
            vibrator.vibrate(VibrationEffect.createOneShot(durationMs, effective));
        } else {
            vibrator.vibrate(durationMs);
        }
    }
}
