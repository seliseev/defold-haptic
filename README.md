# defold-haptic

Haptic feedback for [Defold](https://defold.com) with one API for iOS, Android and HTML5.

On desktop and other platforms the module is still registered and every call is a no-op, so game code needs no platform checks.

## Installation

Add the library to `game.project` → `[project] dependencies`:

```
https://github.com/seliseev/defold-haptic/archive/refs/heads/main.zip
```

Then fetch libraries (`Project → Fetch Libraries`). The extension contains native code, so the game is built through the Defold build server.

## Usage

```lua
if haptic.is_supported() then
    haptic.impact(haptic.IMPACT_LIGHT)                   -- button press
    haptic.notification(haptic.NOTIFICATION_SUCCESS)     -- level complete
    haptic.selection()                                   -- picker tick
end
```

## API

| Function | Description |
|---|---|
| `haptic.is_supported()` | `true` if the device can play haptic feedback |
| `haptic.impact([style])` | Single tap. `haptic.IMPACT_LIGHT` (default), `IMPACT_MEDIUM`, `IMPACT_HEAVY` |
| `haptic.notification([type])` | Multi-tap pattern. `haptic.NOTIFICATION_SUCCESS` (default), `NOTIFICATION_WARNING`, `NOTIFICATION_ERROR` |
| `haptic.selection()` | The lightest tick |
| `haptic.vibrate([duration_ms])` | Plain vibration, 400 ms by default, capped at 5000 ms |

Calls are safe on unsupported devices: they do nothing. An unknown style or type raises a Lua error.

## Platform behaviour

| | iOS | Android | HTML5 |
|---|---|---|---|
| `impact` | `UIImpactFeedbackGenerator` | API 29+: predefined tick / click / heavy click. Older: one-shot with amplitude | `navigator.vibrate`, 10 / 20 / 35 ms |
| `notification` | `UINotificationFeedbackGenerator` | Waveform, with amplitude where the motor supports it | `navigator.vibrate` pattern |
| `selection` | `UISelectionFeedbackGenerator` | API 29+: predefined tick. Older: 10 ms one-shot | `navigator.vibrate`, 10 ms |
| `vibrate` | System vibration, duration is ignored | One-shot of the given duration | `navigator.vibrate` |
| `is_supported` | `true` on iPhone, `false` on iPad | The device has a vibrator | The browser has `navigator.vibrate` |

Notes:

- **iOS** plays nothing when the user has turned off System Haptics in the device settings.
- **Android** adds the `android.permission.VIBRATE` permission to the app manifest. It is a normal permission: no runtime prompt.
- **HTML5** has no amplitude control, so effects differ only in timing. Safari does not implement the Vibration API, which covers every browser on iOS.

## Example

The repository is a runnable Defold project: open `game.project`, bundle for a device and tap the screen to step through all effects.

## License

MIT, see [LICENSE](LICENSE).
