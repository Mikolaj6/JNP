#ifndef IMPERIALFLEET_H
#define IMPERIALFLEET_H

#include "helper.h"
#include <memory>

#include <vector>

class RebelStarship;

// Basic class for imperial ships and groups
class ImperialStarship : public virtual BasicWarshipInterface {
  protected:
    ImperialStarship() = default;

  public:
    // Assumes both this and other ship are alive
    void attack(RebelStarship *ship);
};

// Basic class for imperial single ships
class ActualImperialStarship : public ImperialStarship,
                               public BasicWarship,
                               public BasicStarship {
  protected:
    ActualImperialStarship(ShieldPoints shield, AttackPower power);
};

class DeathStar : public ActualImperialStarship {
  public:
    DeathStar(ShieldPoints shield, AttackPower power);
};

class ImperialDestroyer : public ActualImperialStarship {
  public:
    ImperialDestroyer(ShieldPoints shield, AttackPower power);
};

class TIEFighter : public ActualImperialStarship {
  public:
    TIEFighter(ShieldPoints shield, AttackPower power);
};

class Squadron : public ImperialStarship {
  private:
    std::vector<std::shared_ptr<ImperialStarship>> ships;

  public:
    Squadron(
        const std::initializer_list<std::shared_ptr<ImperialStarship>> &ships);
    Squadron(const std::vector<std::shared_ptr<ImperialStarship>> &ships);
    ShieldPoints getShield() const override;
    AttackPower getAttackPower() const override;
    void takeDamage(AttackPower power) override;
    size_t count() const override;
};

// Functions create imperialShip and return its shared pointer
std::shared_ptr<DeathStar> createDeathStar(ShieldPoints shield,
                                           AttackPower attack);
std::shared_ptr<TIEFighter> createTIEFighter(ShieldPoints shield,
                                             AttackPower attack);
std::shared_ptr<ImperialDestroyer> createImperialDestroyer(ShieldPoints shield,
                                                           AttackPower attack);
// Functions create Squadron and return its shared pointer
std::shared_ptr<Squadron>
createSquadron(const std::vector<std::shared_ptr<ImperialStarship>> &ships);

std::shared_ptr<Squadron> createSquadron(
    const std::initializer_list<std::shared_ptr<ImperialStarship>> &ships);

#endif // IMPERIALFLEET_H
