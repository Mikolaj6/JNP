#ifndef PROJECT_4_BATTLE_H
#define PROJECT_4_BATTLE_H
#include <utility>
#include <cassert>
#include <exception>
#include <tuple>
#include <algorithm>
#include <iostream>
#include "imperialfleet.h"

using namespace std;

/**
 * @brief      Oblicza kwadraty liczb od 0 do max
 *
 * @tparam     T     Typ parametru
 * @tparam     i     Numer obecnej iteracji
 * @tparam     max   Maksymalna podstawa kwadratu
 * @tparam     Args  Wczesniej policzone kwadraty
 *
 * @return     Tablica kwadratów liczb naturalnych nie większy niż max*max
 */
template<typename T, T i, T max, T... Args>
constexpr static auto calculateSquares(){
    if constexpr (i > max/i){
        return std::array<T, sizeof...(Args)> {{Args...}};
    }
    else return calculateSquares<T,i+1,max, Args..., i*i>();
};


template<typename T, T t0, T t1, typename... S>
class SpaceBattle{
    static_assert(std::is_arithmetic<T>::value, "T musi być typem liczbowym");

    std::tuple<S...> tupleShips;///< Krotka zawierająca statki imperium i rebelii
    size_t now;///<Obecna wartość czasu
    size_t rebelCount;
    size_t imperialCount;
    
    
    constexpr static auto squares = 
        calculateSquares<T,T{1}, t1, T{0}>();///< Tablica kwadratów

    /**
     * @brief      Funcja sprawdza czy podany czas T jest kwadratem
     *
     * @param[in]  time  The time
     *
     * @return     True if square, False otherwise.
     */
    inline bool isSquare(T time) { 
        return std::binary_search(squares.begin(), squares.end(), time); 
    };


    void manageBattle();

public:
    SpaceBattle(const S... ships);

    /**
     * @brief      Zlicza liczbę staków imperium
     *
     * @return     Liczba statków imperium
     */
    size_t countImperialFleet();

    /**
     * @brief      Zwraca liczbę staków rebelii
     *
     * @return     Liczba statków rebelii.
     */
    size_t countRebelFleet();

    /**
     * @brief      Funkcja sprawdza aktualny czas; jeśli jest to czas ataku, to
     *             następuje atak statków Imperium na statki Rebelii; na koniec
     *             czas przesuwa się o timeStep
     *
     * @param[in]  timeStep  Ile czasu ma upłynąć.
     */
    void tick(T timeStep);
};

/**
 * @brief      Strultura służy do iteracji po krotce staków poprzez użycie
 *             operatora ()
 *
 * @tparam     index  Indeks elementu w krotce
 * @tparam     Ts     Typy elementów z krotki
 */
template<int index, typename... Ts>
struct iterate {
    void operator() (size_t *rebel, size_t *imp, tuple<Ts...>& t) {
        iterate<index - 1, Ts...>{}(rebel, imp, t);

        if (isImperial(get<index>(t))) {
            if(get<index>(t).getShield())
                ++(*imp);
        }
        else {
            if(get<index>(t).getShield())
                ++(*rebel);
        }
    }
};


/**
 * @brief      Ta wersja struktury iterate, służy zatrzymaniu zliczania
 *             elementów. Jest wywoływana, gdy index = 0
 *
 * @tparam     Ts    Typy elementów z krotki
 */
template<typename... Ts>
struct iterate<0, Ts...> {
    void operator() (size_t *rebel, size_t *imp, tuple<Ts...>& t) {

        if (isImperial(get<0>(t))) {
            if(get<0>(t).getShield())
                ++(*imp);
        }
        else {
            if(get<0>(t).getShield())
                ++(*rebel);
        }
    }
};


/**
 * @brief      Funckja zlicza rozmiar florty.
 *
 * @param      t     Krotka zawierająca statki
 *
 * @tparam     Ts    Typy statków przetrzymywanych w krotce
 *
 * @return     Zwraca parę elmentów typu size_t. Pierszy element oznacza liczbę
 *             staków reblii, drugi liczbę statków imperium
 */
template<typename... Ts>
pair<size_t, size_t> getFleetsSizes(tuple<Ts...>& t) {
    const auto size = tuple_size<tuple<Ts...>>::value;
    size_t rebel{0}, imp{0};
    iterate<size - 1, Ts...>{}(&rebel, &imp, t);
    return make_pair(rebel, imp);
}

/**
 * @brief       Służy do iteracji po krotce statków, w formacie
 *              dla każdego statku imperium, dla każdego rebelianckiego
 *              i atakuje w razie potrzeby, oraz zlicza zniszczone statki
 *
 * @tparam     max        rozmiar krotki minus 1
 * @tparam     index_imp  indeks imperialnego statku
 * @tparam     index_reb  indeks rebelianckiego statku
 * @tparam     Ts         krotka
 */
template<int max, int index_imp, int index_reb, typename... Ts>
struct iterate_both {
    void operator() (tuple<Ts...>& t, size_t* iCount, size_t *rCount)
    {
        iterate_both<max, index_imp, index_reb - 1, Ts...>{}(t, iCount,
                                                            rCount);

        auto imp = &get<index_imp>(t);
        auto reb = &get<index_reb>(t);

        if (isImperial(*imp) && !isImperial(*reb) && reb->getShield() > 0 
            && imp->getShield() > 0){
            attack(*imp, *reb);
            if(imp->getShield() <= 0) ++*iCount;
            if(reb->getShield() <= 0) ++*rCount;
        }
    }
};

/**
 * @brief       Gdy id imp to max
 *
 * @tparam      max         rozmiar krotki minus 1
 * @tparam      index_imp   indeks imerialnego
 * @tparam      Ts          krotka
 */
template<int max, int index_imp, typename... Ts>
struct iterate_both<max, index_imp, max, Ts...> {
    void operator() (tuple<Ts...>& t, size_t* iCount, size_t *rCount) 
    {
        iterate_both<max, index_imp - 1, max, Ts...>{}(t, iCount, rCount);
        iterate_both<max, index_imp, max - 1, Ts...>{}(t, iCount, rCount);

        auto imp = &get<index_imp>(t);
        auto reb = &get<max>(t);

        if (isImperial(*imp) && !isImperial(*reb) && reb->getShield() > 0 
            && imp->getShield() > 0) {
            attack(*imp, *reb);
            if(imp->getShield() <= 0) ++*iCount;
            if(reb->getShield() <= 0) ++*rCount;
        }
    }
};

/**
 * @brief       Gdy id imp to max a rebelianckiego 0
 *
 * @tparam      max     rozmiar krotki minus 1
 * @tparam      Ts      krotka
 */
template<int max, typename... Ts>
struct iterate_both<max, 0, max, Ts...> {
    void operator() (tuple<Ts...>& t, size_t* iCount, size_t *rCount)
    {
        iterate_both<max, 0, max - 1, Ts...>{}(t, iCount, rCount);

        auto imp = &get<0>(t);
        auto reb = &get<max>(t);

        if (isImperial(*imp) && !isImperial(*reb) && reb->getShield() > 0 
            && imp->getShield() > 0) {
            attack(*imp, *reb);
            if(imp->getShield() <= 0) ++*iCount;
            if(reb->getShield() <= 0) ++*rCount;
        }
    }
};

/**
 * @brief       Gdy id imp to 0 a rebelianckiego 0
 *
 * @tparam      max
 * @tparam      index_imp
 * @tparam      Ts
 */
template<int max, int index_imp, typename... Ts>
struct iterate_both<max, index_imp, 0, Ts...> {
    void operator() (tuple<Ts...>& t, size_t* iCount, size_t *rCount){

        auto imp = &get<index_imp>(t);
        auto reb = &get<0>(t);

        if (isImperial(*imp) && !isImperial(*reb) && reb->getShield() > 0 
            && imp->getShield() > 0){
            attack(*imp, *reb);
            if(imp->getShield() <= 0) ++*iCount;
            if(reb->getShield() <= 0) ++*rCount;
        }
    }
};

/**
 * @brief      Funkcja wykonuję bitwę między flotami rebelii i imperium
 */
template<typename T, T t0, T t1, typename... S>
void SpaceBattle<T, t0, t1, S...>::manageBattle() {

    const auto size = tuple_size<tuple<S...>>::value;
    size_t desImp = 0;
    size_t desrRebel = 0;
    iterate_both<size - 1, size - 1, size - 1, S...>{}(tupleShips, &desImp, &desrRebel);
    this->imperialCount -= desImp;
    this->rebelCount -= desrRebel;
}
/**
 * @brief      Konstruktor klasy SpaceBattle
 *
 * @param[in]  ships  Statki, które będą toczyły ze sobą bitwę
 *
 * @tparam     T      
 * @tparam     t0     Czas początkowy
 * @tparam     t1     Czas końcowy
 * @tparam     S      Typy statków przekazanych w argumencie ships
 */
template<typename T, T t0, T t1, typename... S>
SpaceBattle<T, t0, t1, S...>::SpaceBattle(const S... ships)
: tupleShips(ships...), now(t0) 
{
    auto tmp = getFleetsSizes(tupleShips);
    this->rebelCount   = tmp.first; 
    this->imperialCount = tmp.second;
}


/**
 * @brief      Zlicza liczbę statków imperium
 *
 * @return     Liczba statków imperium
 */
template<typename T, T t0, T t1, typename... S>
size_t SpaceBattle<T, t0, t1, S...>::countImperialFleet() {
    return getFleetsSizes(tupleShips).second;
}
/**
 * @brief      Zlicza liczbę statków rebelii
 *
 * @return     Liczba statków rebelii
 */
template<typename T, T t0, T t1, typename... S>
size_t SpaceBattle<T, t0, t1, S...>::countRebelFleet() {
    return getFleetsSizes(tupleShips).first;
}

/**
* @brief      Funkcja sprawdza aktualny czas; jeśli jest to czas ataku, to
*             następuje atak statków Imperium na statki Rebelii; na koniec
*             czas przesuwa się o timeStep
*
* @param[in]  timeStep  Ile czasu ma upłynąć.
*/
template<typename T, T t0, T t1, typename... S>
void SpaceBattle<T, t0, t1, S...>::tick(T timeStep) {
    if(this->imperialCount == 0 && this->rebelCount == 0) {
        std::cout << "DRAW\n";
    } 
    else if (this->imperialCount == 0) {
        std::cout << "REBELLION WON\n";
    }
    else if (this->rebelCount == 0) {
        std::cout << "IMPERIUM WON\n";
    }
    else{
        if(isSquare(now)) manageBattle();

    }
    
    now+=timeStep;
    now%=t1;
}

/**
 * @brief      Sprawdza czy obiekt podany jako argument jest statkiem imperium
 *
 * @param      <unnamed>  Stała referencja do jakiegoś obiektu typu T
 *
 * @tparam     T          Typ elementu
 *
 * @return     Fałsz
 */
template <typename T>
constexpr bool isImperial (T const &)
{ return false; }

/**
 * @brief      Sprawdza czy obiekt podany jako argument jest statkiem imperium 
 *
 * @param      <unnamed>  Stała referencja do jakiegoś obiektu typu ImperialStarship
 *
 *
 * @return     Prawda
 */
template <typename T>
constexpr bool isImperial (ImperialStarship<T> const &)
{ return true; }


#endif //PROJECT_4_BATTLE_H