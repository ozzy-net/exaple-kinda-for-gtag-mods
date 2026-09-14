#include "GorillaLocomotion.hpp"
#include <iostream>

int main() {
    // Demonstration values, not official GorillaLocomotion defaults.
    gorilla::Settings player{6.5f, 1.1f, false};
    bool playerAvailable = true;

    gorilla::Locomotion locomotion({
        [&](gorilla::Settings& output) -> bool {
            if (!playerAvailable)
                return false;

            output = player;
            return true;
        },
        [&](const gorilla::Settings& input) -> bool {
            if (!playerAvailable)
                return false;

            player = input;
            return true;
        }
    });

    if (locomotion.connect() != gorilla::Result::Success)
        return 1;

    // Use these calls in your menu's action handlers.
    if (locomotion.setMaxJumpSpeed(8.0f)
        != gorilla::Result::Success)
        return 1;

    if (locomotion.setJumpMultiplier(1.25f)
        != gorilla::Result::Success)
        return 1;

    if (locomotion.setMovementEnabled(false)
        != gorilla::Result::Success)
        return 1;

    std::cout << "Max jump speed: " << player.maxJumpSpeed << '\n';

    if (locomotion.restoreDefaults() != gorilla::Result::Success)
        return 1;

    locomotion.disconnect();
    return 0;
}
