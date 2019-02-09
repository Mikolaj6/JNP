#ifndef HELPER_H
#define HELPER_H

#include <cstdint>
#include <vector>

using ShieldPoints = uint32_t;
using Speed = uint32_t;
using AttackPower = uint32_t;
using Time = uint32_t;

// Basic ship interface
class BasicStarshipInterface {
  protected:
    BasicStarshipInterface() = default;

  public:
    virtual ShieldPoints getShield() const = 0;
    virtual void takeDamage(AttackPower damage) = 0;
    virtual size_t count() const = 0;
    virtual ~BasicStarshipInterface() = default;
};

// Basic ship class
class BasicStarship : public virtual BasicStarshipInterface {
  private:
    ShieldPoints shield;

  protected:
    BasicStarship(ShieldPoints shield);

  public:
    virtual ShieldPoints getShield() const override;
    virtual void takeDamage(AttackPower damage) override;
    virtual size_t count() const override;
};

// Warship interface
class BasicWarshipInterface : public virtual BasicStarshipInterface {
  protected:
    BasicWarshipInterface() = default;

  public:
    virtual AttackPower getAttackPower() const = 0;
};

// Warship class (can deal damage)
class BasicWarship : public virtual BasicWarshipInterface {
  private:
    AttackPower attack_power;

  protected:
    BasicWarship(AttackPower power);

  public:
    virtual AttackPower getAttackPower() const override;
};

#endif // HELPER_H
