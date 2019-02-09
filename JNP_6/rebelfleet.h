#ifndef REBELFLEET_H
#define REBELFLEET_H
#include "helper.h"
#include <memory>

class ImperialStarship;

// Rebel warship without ablity to defend itself
class RebelStarship : public BasicStarship {
  private:
    const Speed speed;

  protected:
    RebelStarship(ShieldPoints shield, Speed speed, Speed upperLimit,
                  Speed lowerLimit);

  public:
    // Assumes both this and other ship are alive.
    // Is const, because this is what happns after this ship is attacked.
    virtual void reactToAttack(ImperialStarship *) const {
    }
    Speed getSpeed() const;
};

// Rebel warship can fire back at imperial ships
class RebelWarship : public RebelStarship, public BasicWarship {
  protected:
    RebelWarship(ShieldPoints shield, Speed speed, AttackPower power,
                 Speed upperLimit, Speed lowerLimit);

  public:
    virtual void reactToAttack(ImperialStarship *) const override;
};

class XWing : public RebelWarship {
  private:
    static constexpr Speed maxSpeed = 2997960;
    static constexpr Speed minSpeed = 299796;

  public:
    XWing(ShieldPoints shield, Speed speed, AttackPower attack);
};

class StarCruiser : public RebelWarship {
  private:
    static constexpr Speed maxSpeed = 299795;
    static constexpr Speed minSpeed = 99999;

  public:
    StarCruiser(ShieldPoints shield, Speed speed, AttackPower attack);
};

class Explorer : public RebelStarship {
  private:
    static constexpr Speed maxSpeed = 2997960;
    static constexpr Speed minSpeed = 299796;

  public:
    Explorer(ShieldPoints shield, Speed speed);
};

// Functions create rebelShip and return its shared pointer
std::shared_ptr<XWing> createXWing(ShieldPoints shield, Speed speed,
                                   AttackPower attack);
std::shared_ptr<Explorer> createExplorer(ShieldPoints shield, Speed speed);
std::shared_ptr<StarCruiser> createStarCruiser(ShieldPoints shield, Speed speed,
                                               AttackPower attack);

#endif // REBELFLEET_H
