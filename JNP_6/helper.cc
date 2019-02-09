#include "helper.h"

#include <algorithm>
#include <cassert>

BasicStarship::BasicStarship(ShieldPoints shield) : shield(shield) {
}

ShieldPoints BasicStarship::getShield() const {
    return shield;
}

// Returns 1 if ship exists 0 otherwise
size_t BasicStarship::count() const {
    if (getShield() > 0) {
        return 1;
    }
    return 0;
}

void BasicStarship::takeDamage(AttackPower damage) {
    shield -= std::min(shield, damage);
}

BasicWarship::BasicWarship(AttackPower power) : attack_power(power) {
}

AttackPower BasicWarship::getAttackPower() const {
    return attack_power;
}
