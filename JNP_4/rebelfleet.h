#ifndef REBELFEET_H_
#define REBELFEET_H_
#include <utility>
#include <cassert>
#include <type_traits>
#include <iostream>
#include <string>

/**
 * @brief      Class for rebel starship.
 *
 * @tparam     U           Typ wartości
 * @tparam     starting    Minimalna szykbość
 * @tparam     ending      Masksymalna szybkość
 * @tparam     isExplorer  Czy jest Explorerem
 */
template<typename U, int starting, int ending, bool isExplorer>
class RebelStarship {
    U shield;///< Tarcze
    U speed;///< Szczybkość statku
    U attackPower;///< Moc ataku

public:

    /**
     * @brief      Tworzy instancję szablony RebelStarship zdolną do ataku
     *
     * @param[in]  shield_       Tarcza
     * @param[in]  speed_        Szybkość
     * @param[in]  attackPower_  Moc ataku
     *
     * @tparam     shipType      Zmienna boolowa odpowiadająca za typ statku.
     *                           Jeśli true to Explorer
     * @tparam     <unnamed>     Parametr bez nazwy, który pozwala jedynie
     *                           statkom nie będącym Explorerem na korzystanie z
     *                           tego konstruktora
     */
    template<bool shipType = isExplorer, 
            typename = typename std::enable_if<!shipType>::type>
    RebelStarship(U shield_, U speed_, U attackPower_) :
        shield(shield_), speed(speed_), attackPower(attackPower_) 
    {
        assert(shield_ >= 0);
        assert(attackPower_ >= 0);
        assert(speed_ >= starting);
        assert(speed_ <= ending);
    }


    /**
     * @brief      Tworzy statke, będący Explorerem
     *
     * @param[in]  shield_    Tarcze
     * @param[in]  speed_     Szybkość
     *
     * @tparam     shipType   Zmienna boolowa odpowiadająca za typ statku. 
     * @tparam     <unnamed>  Parameter, który pozwala na specajlizację szablonu
     */
    template<bool shipType = isExplorer, typename = typename std::enable_if<shipType>::type>
    RebelStarship(U shield_, U speed_) :shield(shield_), speed(speed_) {
        assert(shield_ >= 0);
        assert(speed_ >= starting);
        assert(speed_ <= ending);
    }

    /**
     * @brief      Funkcja zwracająca moc ataku
     *
     * @tparam     shipType   Zmienna boolowa odpowiadająca za typ statku.
     * @tparam     <unnamed>  Zmienna boolowa odpowiadająca za typ statku. Jeśli
     *                        statek jest Explorerem, wówczas ten szablon jest
     *                        dla niego nie dostępny
     *
     * @return     Zwraca moc ataku
     */
    template<bool shipType = isExplorer, typename = typename std::enable_if<!shipType>::type>
    U getAttackPower(){ return attackPower; }

    /**
     * @brief      Funkcja zwracająca wartość tarcza
     *
     * @return     Zwraca wartość tarcz
     */
    inline U getShield(){return shield;};


    /**
     * @brief      Funkcja zwracająca wartość parametru szybkość
     *
     * @return     Szybkość statku
     */
    inline U getSpeed(){return speed;};

    /**
     * @brief      Statke pobiera obrażenia w wielkośći damage
     *
     * @param[in]  damage  Wielkość obrażeń
     */
    void takeDamage(U damage);
    
    typedef U valueType;///<Jakiego typu są wartości klasy

    /**
     * @brief      Funckja odpowiadająca za walkę dwóch statków
     *
     * @param[in]  ImperialShip  Statek atakujący
     * @param[in]  RebelShip     Statek broniący się
     *
     * @tparam     I             Typ statku imperium
     * @tparam     R             Typ statku rebelii
     */
    template<typename I, typename R>
    friend void attack(I ImperialShip, R RebelShip);
};

template <typename U>
using Explorer = RebelStarship<U, 299796, 2997960, true>;

template <typename U>
using StarCruiser = RebelStarship<U, 99999, 299795, false>;

template <typename U>
using XWing = RebelStarship<U, 299796, 2997960, false>;


template<typename U, int starting, int ending, bool isExplorer>
void RebelStarship<U, starting, ending, isExplorer>::takeDamage(U damage) {
    assert(damage > 0);
    if (shield > 0) {
        if (shield <= damage)
            shield = 0;
        else
            shield -= damage;
    }
}

#endif ///REBELFLEET_H