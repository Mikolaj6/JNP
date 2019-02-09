#include "rebelfleet.h"

#include <cassert>

#include "imperialfleet.h"

RebelStarship::RebelStarship(ShieldPoints shield, Speed speed, Speed upperLimit,
                             Speed lowerLimit)
    : BasicStarship(shield), speed(speed) {
    assert(speed <= upperLimit);
    assert(speed >= lowerLimit);
}

Speed RebelStarship::getSpeed() const {
    return speed;
}

RebelWarship::RebelWarship(ShieldPoints shield, Speed speed, AttackPower power,
                           Speed upperLimit, Speed lowerLimit)
    : RebelStarship(shield, speed, upperLimit, lowerLimit),
      BasicWarship(power) {
}

void RebelWarship::reactToAttack(ImperialStarship *ship) const {
    ship->takeDamage(getAttackPower());
}

XWing::XWing(ShieldPoints shield, Speed speed, AttackPower attack)
    : RebelWarship(shield, speed, attack, maxSpeed, minSpeed) {
}

StarCruiser::StarCruiser(ShieldPoints shield, Speed speed, AttackPower attack)
    : RebelWarship(shield, speed, attack, maxSpeed, minSpeed) {
}

Explorer::Explorer(ShieldPoints shield, Speed speed)
    : RebelStarship(shield, speed, maxSpeed, minSpeed) {
}

std::shared_ptr<XWing> createXWing(ShieldPoints shield, Speed speed,
                                   AttackPower attack) {
    return std::make_shared<XWing>(shield, speed, attack);
}

std::shared_ptr<Explorer> createExplorer(ShieldPoints shield, Speed speed) {
    return std::make_shared<Explorer>(shield, speed);
}

std::shared_ptr<StarCruiser> createStarCruiser(ShieldPoints shield, Speed speed,
                                               AttackPower attack) {
    return std::make_shared<StarCruiser>(shield, speed, attack);
}
