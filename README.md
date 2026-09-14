# GorillaLocomotion C++ Adapter

A small, header-only adapter for connecting C++ menus to
GorillaLocomotion player settings.

Unofficial community integration. Not affiliated with Another Axiom.

## Features

- Set maximum jump speed.
- Set jump multiplier.
- Enable or disable movement.
- Capture and restore settings from the connected player.
- Reject negative, infinite, and NaN numeric values.
- Use any menu framework.
- No dependencies beyond the C++17 standard library.

## Scope

This is an adapter, not a locomotion engine or an automatic IL2CPP
binding. You supply two callbacks that read and write the local
player through your project's existing Unity integration.

It does not include a player resolver, memory offsets, hooks,
BNM bindings, or a prebuilt Android library.

The example uses a simulated player and does not connect to Unity.

## Installation

Copy `GorillaLocomotion.hpp` into your project's include directory.
Enable C++17 or newer.

```cpp
#include "GorillaLocomotion.hpp"
```

## Connect your player

Construct `gorilla::Locomotion` with two callbacks:

| Callback | Required behavior |
| --- | --- |
| `read(Settings&)` | Read all three settings from the current local player. |
| `write(const Settings&)` | Validate the player, then write all three settings. |

Both callbacks return `true` on success and `false` when the
player is unavailable or the operation fails.

Map the adapter's settings to the corresponding fields in your
GorillaLocomotion version:

| Adapter setting | Intended player field |
| --- | --- |
| `maxJumpSpeed` | `maxJumpSpeed` |
| `jumpMultiplier` | `jumpMultiplier` |
| `disableMovement` | `disableMovement` |

Verify these names and types against your project's Player script.
Forks can change them.

Keep callbacks valid for the adapter's entire lifetime. They must
resolve or validate the live player on every call and must not throw.

## Menu actions

After the player is ready, call `connect()` once to capture defaults.

```cpp
auto result = locomotion.connect();

// Inside menu action handlers:
result = locomotion.setMaxJumpSpeed(8.0f);
result = locomotion.setJumpMultiplier(1.25f);
result = locomotion.setMovementEnabled(false);
result = locomotion.restoreDefaults();
```

Check each returned result in your actual menu:

- `Success`: operation completed.
- `NotConnected`: call connect after your player is ready.
- `Unavailable`: the backend could not access or update the player.
- `InvalidValue`: a numeric value is negative or non-finite.

Choose sensible slider limits for your game. The adapter does not
enforce a maximum.

## Threading and player lifecycle

Run all adapter operations on the Unity main thread. If your menu
renders on another thread, queue its actions for the Unity thread.

This class is not thread-safe.

Before the player is destroyed or replaced, call `disconnect()`.
Call `connect()` again once the replacement player is ready.

`disconnect()` does not restore settings. To restore settings when
closing a menu, call `restoreDefaults()` while the player is still
available, then disconnect.

Do not call `connect()` every frame: it captures new defaults.

## Build the standalone example

GCC or Clang:

```sh
c++ -std=c++17 -Wall -Wextra -pedantic example.cpp -o example
```

Visual Studio Developer Command Prompt:

```bat
cl /std:c++17 /EHsc /W4 example.cpp
```

## Unity compatibility

The C++ adapter has no Unity-version-specific offsets.

Actual Unity 2021/2022 and Mono/IL2CPP compatibility depends on the
backend you supply. This package has not been compiled or tested
inside Unity in this session.

For a Unity project you control, a managed-to-native bridge is
another way to connect your player to native code.

## Credits

Original locomotion project:
https://github.com/Another-Axiom/GorillaLocomotion

Unity native plug-in documentation:
https://docs.unity3d.com/2021.3/Documentation/Manual/NativePlugins.html
