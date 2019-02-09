#include "battle.h"

#include <iostream>

#include "imperialfleet.h"
#include "rebelfleet.h"

ModuloClock::ModuloClock(Time startTime, Time maxTime)
    : time(startTime), maxTime(maxTime) {
}

void ModuloClock::tick(Time timestep) {
    time = (time + timestep) % (maxTime + 1);
}

Time ModuloClock::currentTime() {
    return time;
}

// Adding imperial ships to builder
SpaceBattle::Builder
SpaceBattle::Builder::ship(const std::shared_ptr<ImperialStarship> &ship) {
    imperial_ships.push_back(ship);
    return *this;
}

// Adding rebel ships to builder
SpaceBattle::Builder
SpaceBattle::Builder::ship(const std::shared_ptr<RebelStarship> &ship) {
    rebel_ships.push_back(ship);
    return *this;
}

SpaceBattle::Builder SpaceBattle::Builder::startTime(Time time) {
    startTime_ = time;
    return *this;
}

SpaceBattle::Builder SpaceBattle::Builder::maxTime(Time time) {
    maxTime_ = time;
    return *this;
}

// Constructing space battle
SpaceBattle SpaceBattle::Builder::build() {
    return SpaceBattle(std::move(rebel_ships), std::move(imperial_ships),
                       std::make_shared<ModuloClock>(startTime_, maxTime_));
}

// Constructing space battle
SpaceBattle::SpaceBattle(
    std::vector<std::shared_ptr<RebelStarship>> &&rebel_ships,
    std::vector<std::shared_ptr<ImperialStarship>> &&imperial_ships,
    std::shared_ptr<BasicClock> clock)
    : imperial_ships(std::move(imperial_ships)),
      rebel_ships(std::move(rebel_ships)), clock(clock) {
}

// Given attack strategy
bool SpaceBattle::isAttackSecond(Time t) {
    return ((t % 2 == 0) || (t % 3 == 0)) && (t % 5 != 0);
}

size_t SpaceBattle::countImperialFleet() const {
    size_t result = 0;
    for (const auto &ship : imperial_ships) {
        result += ship->count();
    }
    return result;
}

size_t SpaceBattle::countRebelFleet() const {
    size_t result = 0;
    for (const auto &ship : rebel_ships) {
        result += ship->count();
    }
    return result;
}

void SpaceBattle::tick(Time time) {
    const size_t imperial_size = countImperialFleet();
    const size_t rebel_size = countRebelFleet();
    if (rebel_size == 0 && imperial_size == 0) {
        std::cout << "DRAW\n";
        return;
    }
    if (rebel_size == 0) {
        std::cout << "IMPERIUM WON\n";
        return;
    }
    if (imperial_size == 0) {
        std::cout << "REBELLION WON\n";
        return;
    }

    if (isAttackSecond(clock->currentTime())) {
        for (auto ship1 : imperial_ships) {
            for (auto ship2 : rebel_ships) {
                if (ship1->getShield() > 0 && ship2->getShield() > 0) {
                    ship1->attack(ship2.get());
                }
            }
        }
    }
    clock->tick(time);
}
