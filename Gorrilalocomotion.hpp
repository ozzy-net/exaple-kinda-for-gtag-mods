#pragma once

#include <cmath>
#include <functional>
#include <optional>
#include <utility>

namespace gorilla {

struct Settings {
    float maxJumpSpeed{};
    float jumpMultiplier{};
    bool disableMovement{};
};

// Return false when the player is unavailable or an operation fails.
// Write must validate the player before changing any fields.
struct Backend {
    std::function<bool(Settings&)> read;
    std::function<bool(const Settings&)> write;
};

enum class Result {
    Success,
    NotConnected,
    Unavailable,
    InvalidValue
};

class Locomotion {
public:
    explicit Locomotion(Backend backend)
        : backend_(std::move(backend)) {}

    // Call on the Unity thread after the local player becomes available.
    Result connect() {
        defaults_.reset();

        if (!backend_.read || !backend_.write)
            return Result::NotConnected;

        Settings current{};
        if (!backend_.read(current))
            return Result::Unavailable;

        if (!valid(current))
            return Result::InvalidValue;

        defaults_ = current;
        return Result::Success;
    }

    // Call before the player is destroyed or replaced.
    void disconnect() {
        defaults_.reset();
    }

    bool connected() const {
        return defaults_.has_value();
    }

    Result read(Settings& output) const {
        if (!connected())
            return Result::NotConnected;

        Settings current{};
        if (!backend_.read(current))
            return Result::Unavailable;

        if (!valid(current))
            return Result::InvalidValue;

        output = current;
        return Result::Success;
    }

    Result setMaxJumpSpeed(float value) {
        return change([value](Settings& s) {
            s.maxJumpSpeed = value;
        });
    }

    Result setJumpMultiplier(float value) {
        return change([value](Settings& s) {
            s.jumpMultiplier = value;
        });
    }

    Result setMovementEnabled(bool enabled) {
        return change([enabled](Settings& s) {
            s.disableMovement = !enabled;
        });
    }

    Result restoreDefaults() {
        if (!connected())
            return Result::NotConnected;

        return backend_.write(*defaults_)
            ? Result::Success
            : Result::Unavailable;
    }

private:
    static bool valid(const Settings& s) {
        return std::isfinite(s.maxJumpSpeed)
            && std::isfinite(s.jumpMultiplier)
            && s.maxJumpSpeed >= 0.0f
            && s.jumpMultiplier >= 0.0f;
    }

    template<class Edit>
    Result change(Edit edit) {
        Settings current{};
        const auto result = read(current);

        if (result != Result::Success)
            return result;

        edit(current);

        if (!valid(current))
            return Result::InvalidValue;

        return backend_.write(current)
            ? Result::Success
            : Result::Unavailable;
    }

    Backend backend_;
    std::optional<Settings> defaults_;
};

} // namespace gorilla
