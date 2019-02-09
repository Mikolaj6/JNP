#ifndef PROJECT3_WALLET_H
#define PROJECT3_WALLET_H

#include <vector>
#include <chrono>
#include <ostream>
#include <boost/operators.hpp>

// KONWENCJA KOMENTARZY - PO ANG
/* Do zrobienia:
 * DONE- komentarze, ujednolicić język
 * - dodaj komentarz do parseBString
 * - static unitCount i limity maja byc w klasie statyczne
 * DONE - usunąć using namespace std i zamienić na parę  małych using::fun
 * DONE - usunąć w .cpp consty dla konstruktorów
 * - jak masz czas potestuj se konstruktory co biorą stringi
 * DONE - Jak będzie czas o w funkcji combine zrobić na inplace_merga
 *
 *
 * - odpalić FORMAT CODE w CLionie i sprawdzić żeby każda inijka miała mniej niż 80 znaków
 * - NA KOŃCU ZMINIC NA .cc i SPRAWDZIĆ CZY SIE KOMPILUJE NA STUDENTSIE BEZ WARNINGOW I DZIŁA PRZYKLADOWY
 */


class Operation
        : private boost::equality_comparable<Operation>,
          boost::less_than_comparable<Operation> {
private:
    std::chrono::system_clock::time_point time;
    unsigned long long balance;

public:
    Operation(std::chrono::system_clock::time_point time,
              unsigned long long balance);

    std::chrono::system_clock::time_point getTime() const;

    unsigned long long getUnits() const;

    bool operator==(const Operation &other) const;

    bool operator<(const Operation &other) const;

    friend std::ostream &operator<<(std::ostream &os, const Operation &op);

};


class Wallet
        : private boost::equality_comparable<Wallet>,
          boost::less_than_comparable<Wallet> {
    std::vector<Operation> history;

    void addToHistory();

    void addToHistory(unsigned long long val);

    unsigned long long getWalletVal() const;

    Wallet(Wallet &w1) noexcept;

    Wallet(const unsigned long long &b, const unsigned long long &units);

public:

    static const unsigned long long unitsInB;
    static const unsigned long long limitB;
    static const unsigned long long limit;
    static unsigned long long unitsInCirculation;
    
    ~Wallet();

    Wallet();

    template<class T>
    Wallet(T) = delete;

    Wallet(short b);

    Wallet(unsigned short b);

    Wallet(int b);

    Wallet(unsigned int b);

    Wallet(long b);

    Wallet(unsigned long b);

    Wallet(long long b);

    Wallet(unsigned long long b);

    explicit Wallet(const std::string &str);

    explicit Wallet(const char *str);

    Wallet(Wallet &&w2) noexcept;

    Wallet(Wallet &&w1, Wallet &&w2);

    static Wallet fromBinary(const std::string &str);

    Wallet &operator=(Wallet &&w1) noexcept;

    unsigned long long getUnits() const;

    unsigned long long opSize() const;

    const Operation &operator[](int k) const;

    Wallet &operator*=(unsigned long long val);

    Wallet &operator+=(Wallet &&w1);

    Wallet &operator+=(Wallet &w1);

    Wallet &operator-=(Wallet &&w1);

    Wallet &operator-=(Wallet &w1);

    friend Wallet operator+(Wallet &&w1, Wallet &&w2);

    friend Wallet operator+(Wallet &&w1, Wallet &w2);

    friend Wallet operator-(Wallet &&w1, Wallet &&w2);

    friend Wallet operator-(Wallet &&w1, Wallet &w2);

    friend bool operator==(const Wallet &w1, const Wallet &w2);

    friend bool operator<(const Wallet &w1, const Wallet &w2);

    friend Wallet operator*(unsigned long long val, const Wallet &w);

    friend Wallet operator*(const Wallet &w, unsigned long long val);

    friend std::ostream &operator<<(std::ostream &, const Wallet &w);

    friend const Wallet Empty();
};

const Wallet Empty();

#endif //PROJECT3_WALLET_H
