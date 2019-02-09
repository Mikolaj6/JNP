#ifndef IMPERIALFLEET_H
#define IMPERIALFLEET_H

#include <utility>
#include <cassert>
#include <exception>
#include "rebelfleet.h"

template<typename U>
class ImperialStarship {
    U shield;
    U attack;

public:

    static_assert(std::is_arithmetic<U>::value);

    /**
     * @brief      Constructs the object.
     *
     * @param[in]  shield_  Początkowa wartość tarcz
     * @param[in]  attack_  Wartość atatku
     */
    ImperialStarship(U shield_, U attack_);

    /**
     * @brief      Funkcja zwraca obecną wartość tarcz
     *
     * @return     Obecna wartość tarcz
     */
    inline U getShield() { return shield; };

    /**
     * @brief      Funkcja zwraca siłę ataku statku
     *
     * @return     Siła ataku statku
     */
    inline U getAttackPower() { return attack; };

    /**
     * @brief      Funkcja poprzez którą statek otrzymuje obrażenia
     *
     * @param[in]  damage  Otrzymane obrażenia
     */
    void takeDamage(U damage);

    typedef U valueType;///< Typ atrybutu speed i shield

    /**
     * @brief      Wymiana ognia między statkami
     *
     * @param[in]  ImperialShip  Statek imperium
     * @param[in]  RebelShip     Statek rebeliantów
     *
     * @tparam     I             Typ statku imperium
     * @tparam     R             Typ statku RebelShip
     */
    template<typename I, typename R>
    friend void attack(I ImperialShip, R RebelShip);
};


template<typename U>
void ImperialStarship<U>::takeDamage(U damage) {
    assert(damage > 0);
    if (shield > 0) {
        if (shield < damage)
            shield = 0;
        else
            shield -= damage;
    }
}

template<typename U>
ImperialStarship<U>::ImperialStarship(U shield_, U attack_)
: shield(shield_), attack(attack_) {
    assert(shield_ > 0);
    assert(attack_ > 0);
}

template <typename U>
using DeathStar = ImperialStarship<U>;

template <typename U>
using TIEFighter = ImperialStarship<U>;

template <typename U>
using ImperialDestroyer = ImperialStarship<U>;


template<typename I, typename R>
void attack(I imperialShip, R rebelShip) {
    throw std::exception();
}

/**
 * @brief      Wymiana ognia między statkiem imperium a statkiem klasy Expolorer
 *
 * @param      imperialShip  Statek imperium
 * @param      rebelShip     The rebel ship
 *
 * @tparam     I             Typ potrzebny do szbalonu
 * @tparam     R             Typ potrzebny do szbalonu
 */
template<typename I, typename R>
void attack(ImperialStarship<I> &imperialShip, Explorer<R> &rebelShip) {
    rebelShip.takeDamage(imperialShip.getAttackPower());
}

/**
 * @brief      { function_description }
 *
 * @param      imperialShip  Statek imperium
 * @param      rebelShip     Statek rebeliantów
 *
 * @tparam     I             Typ potrzebny do szbalonu
 * @tparam     R             Typ potrzebny do szbalonu
 */
template<typename I, typename R>
void attack(ImperialStarship<I> &imperialShip, StarCruiser<R> &rebelShip) {
    rebelShip.takeDamage(imperialShip.getAttackPower());
    imperialShip.takeDamage(rebelShip.getAttackPower());
}

/**
 * @brief      { function_description }
 *
 * @param      imperialShip  Statek imperium
 * @param      rebelShip     Statek rebeliantów
 *
 * @tparam     I             Typ potrzebny do szbalonu
 * @tparam     R             Typ potrzebny do szbalonu
 */
template<typename I, typename R>
void attack(ImperialStarship<I> &imperialShip, XWing<R> &rebelShip) {
    rebelShip.takeDamage(imperialShip.getAttackPower());
    imperialShip.takeDamage(rebelShip.getAttackPower());
}

#endif //IMPERIALFLEET_H