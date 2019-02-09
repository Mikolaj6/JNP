#include "imperialfleet.h"

#include "rebelfleet.h"

ActualImperialStarship::ActualImperialStarship(ShieldPoints shield,
                                               AttackPower power)
    : BasicWarship(power), BasicStarship(shield) {
}

void ImperialStarship::attack(RebelStarship *ship) {
    ship->takeDamage(getAttackPower());
    ship->reactToAttack(this);
}

DeathStar::DeathStar(ShieldPoints shield, AttackPower power)
    : ActualImperialStarship(shield, power) {
}

ImperialDestroyer::ImperialDestroyer(ShieldPoints shield, AttackPower power)
    : ActualImperialStarship(shield, power) {
}

TIEFighter::TIEFighter(ShieldPoints shield, AttackPower power)
    : ActualImperialStarship(shield, power) {
}

Squadron::Squadron(const std::vector<std::shared_ptr<ImperialStarship>> &ships)
    : ships(ships) {
}

Squadron::Squadron(
    const std::initializer_list<std::shared_ptr<ImperialStarship>> &ships)
    : ships(ships) {
}

ShieldPoints Squadron::getShield() const {
    ShieldPoints result = 0;
    for (const auto &ship : ships) {
        result += ship->getShield();
    }
    return result;
}

AttackPower Squadron::getAttackPower() const {
    AttackPower result = 0;
    for (const auto &ship : ships) {
        if (ship->getShield() > 0) {
            result += ship->getAttackPower();
        }
    }
    return result;
}

void Squadron::takeDamage(AttackPower power) {
    for (auto &ship : ships) {
        ship->takeDamage(power);
    }
}

size_t Squadron::count() const {
    size_t result = 0;
    for (const auto &ship : ships) {
        result += ship->count();
    }
    return result;
}

std::shared_ptr<DeathStar> createDeathStar(ShieldPoints shield,
                                           AttackPower attack) {
    return std::make_shared<DeathStar>(shield, attack);
}

std::shared_ptr<TIEFighter> createTIEFighter(ShieldPoints shield,
                                             AttackPower attack) {
    return std::make_shared<TIEFighter>(shield, attack);
}

std::shared_ptr<ImperialDestroyer> createImperialDestroyer(ShieldPoints shield,
                                                           AttackPower attack) {
    return std::make_shared<ImperialDestroyer>(shield, attack);
}

std::shared_ptr<Squadron>
createSquadron(const std::vector<std::shared_ptr<ImperialStarship>> &ships) {
    return std::make_shared<Squadron>(ships);
}

std::shared_ptr<Squadron> createSquadron(
    const std::initializer_list<std::shared_ptr<ImperialStarship>> &ships) {
    return std::make_shared<Squadron>(ships);
}
