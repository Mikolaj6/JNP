#ifndef BATTLE_H
#define BATTLE_H

#include "imperialfleet.h"
#include "rebelfleet.h"
#include <memory>
#include <utility>

// Clock interface used holding time
class BasicClock {
  public:
    virtual void tick(Time timestep) = 0;
    virtual Time currentTime() = 0;
    virtual ~BasicClock() = default;
};

// Implementation of modulo clock
class ModuloClock : public BasicClock {
  private:
    Time time;
    Time maxTime;

  public:
    ModuloClock(Time startTime, Time maxTime);
    virtual void tick(Time timestep) override;
    virtual Time currentTime() override;
};

class SpaceBattle {
  public:
    // Builder class for creating spaceBattles
    class Builder {
      private:
        std::vector<std::shared_ptr<ImperialStarship>> imperial_ships;
        std::vector<std::shared_ptr<RebelStarship>> rebel_ships;
        Time startTime_;
        Time maxTime_;

      public:
        Builder ship(const std::shared_ptr<ImperialStarship> &ship);
        Builder ship(const std::shared_ptr<RebelStarship> &ship);
        Builder startTime(Time time);
        Builder maxTime(Time time);
        SpaceBattle build();
    };

  private:
    std::vector<std::shared_ptr<ImperialStarship>> imperial_ships;
    std::vector<std::shared_ptr<RebelStarship>> rebel_ships;
    std::shared_ptr<BasicClock> clock;

  protected:
    // Check whether attack should occur in second t.
    // Virtual and protected so that it can be overridden in subclasses.
    virtual bool isAttackSecond(Time t);

  public:
    SpaceBattle(std::vector<std::shared_ptr<RebelStarship>> &&rebel_ships,
                std::vector<std::shared_ptr<ImperialStarship>> &&imperial_ships,
                std::shared_ptr<BasicClock> clock);
    size_t countImperialFleet() const;
    size_t countRebelFleet() const;
    void tick(Time timeStep);
    virtual ~SpaceBattle() = default;
};

#endif // BATTLE_H
