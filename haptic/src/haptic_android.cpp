#if defined(DM_PLATFORM_ANDROID)

#include <jni.h>

#include <dmsdk/sdk.h>
#include <dmsdk/dlib/android.h>

#include "haptic_private.h"

// JNI bridge to com.defold.haptic.HapticJNI. The class reference and method
// ids are resolved once and reused: haptics fire from button handlers, so the
// lookup must not be repeated on every tap.

namespace dmHaptic
{
    static const char* HAPTIC_JNI_CLASS = "com.defold.haptic.HapticJNI";

    static jclass    g_Class        = 0;
    static jmethodID g_IsSupported  = 0;
    static jmethodID g_Impact       = 0;
    static jmethodID g_Notification = 0;
    static jmethodID g_Selection    = 0;
    static jmethodID g_Vibrate      = 0;
    static bool      g_LoadFailed   = false;

    static bool EnsureLoaded(JNIEnv* env)
    {
        if (g_Class != 0) return true;
        if (g_LoadFailed) return false;

        jclass local = dmAndroid::LoadClass(env, HAPTIC_JNI_CLASS);
        if (local == 0 || env->ExceptionCheck())
        {
            env->ExceptionClear();
            dmLogError("haptic: failed to load %s", HAPTIC_JNI_CLASS);
            g_LoadFailed = true;
            return false;
        }

        g_IsSupported  = env->GetStaticMethodID(local, "isSupported",  "(Landroid/content/Context;)Z");
        g_Impact       = env->GetStaticMethodID(local, "impact",       "(Landroid/content/Context;I)V");
        g_Notification = env->GetStaticMethodID(local, "notification", "(Landroid/content/Context;I)V");
        g_Selection    = env->GetStaticMethodID(local, "selection",    "(Landroid/content/Context;)V");
        g_Vibrate      = env->GetStaticMethodID(local, "vibrate",      "(Landroid/content/Context;I)V");

        if (env->ExceptionCheck() || !g_IsSupported || !g_Impact || !g_Notification || !g_Selection || !g_Vibrate)
        {
            env->ExceptionClear();
            env->DeleteLocalRef(local);
            dmLogError("haptic: %s is missing expected methods", HAPTIC_JNI_CLASS);
            g_LoadFailed = true;
            return false;
        }

        g_Class = (jclass) env->NewGlobalRef(local);
        env->DeleteLocalRef(local);
        return true;
    }

    bool PlatformIsSupported()
    {
        dmAndroid::ThreadAttacher thread;
        JNIEnv* env = thread.GetEnv();
        if (!env || !EnsureLoaded(env)) return false;
        return env->CallStaticBooleanMethod(g_Class, g_IsSupported, thread.GetActivity()->clazz) == JNI_TRUE;
    }

    void PlatformImpact(Impact style)
    {
        dmAndroid::ThreadAttacher thread;
        JNIEnv* env = thread.GetEnv();
        if (!env || !EnsureLoaded(env)) return;
        env->CallStaticVoidMethod(g_Class, g_Impact, thread.GetActivity()->clazz, (jint) style);
    }

    void PlatformNotification(Notification type)
    {
        dmAndroid::ThreadAttacher thread;
        JNIEnv* env = thread.GetEnv();
        if (!env || !EnsureLoaded(env)) return;
        env->CallStaticVoidMethod(g_Class, g_Notification, thread.GetActivity()->clazz, (jint) type);
    }

    void PlatformSelection()
    {
        dmAndroid::ThreadAttacher thread;
        JNIEnv* env = thread.GetEnv();
        if (!env || !EnsureLoaded(env)) return;
        env->CallStaticVoidMethod(g_Class, g_Selection, thread.GetActivity()->clazz);
    }

    void PlatformVibrate(int duration_ms)
    {
        dmAndroid::ThreadAttacher thread;
        JNIEnv* env = thread.GetEnv();
        if (!env || !EnsureLoaded(env)) return;
        env->CallStaticVoidMethod(g_Class, g_Vibrate, thread.GetActivity()->clazz, (jint) duration_ms);
    }
}

#endif // DM_PLATFORM_ANDROID
