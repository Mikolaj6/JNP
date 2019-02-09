#include "wallet.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <exception>


const unsigned long long Wallet::unitsInB = 100000000;
const unsigned long long Wallet::limitB = 21000000;
const unsigned long long Wallet::limit = Wallet::limitB * Wallet::unitsInB;
unsigned long long Wallet::unitsInCirculation = 0;


namespace {
    using std::vector;
    using std::pair;
    using std::string;
    using std::move;
    using std::ostream;
    using std::to_string;
    using SysClock = std::chrono::system_clock;
    using SysT = SysClock::time_point;

    SysT timeNow() {
        return SysClock::now();
    }

    // Combines histories
    vector<Operation> combine(
            vector<Operation> &&his1,
            vector<Operation> &&his2) {
        auto mid = his1.size();
        vector<Operation> a;
        a.reserve(his1.size() + his2.size());
        a.insert(a.end(), make_move_iterator(his1.begin()),
                 make_move_iterator(his1.end()));
        a.insert(a.end(), make_move_iterator(his2.begin()),
                 make_move_iterator(his2.end()));
        std::inplace_merge(a.begin(), a.begin() + mid, a.end());
        return a;
    }

    
    string alignFracZeroes(unsigned long long value){
      string ret = to_string(value);
      while(ret.size() + 1 < to_string(Wallet::unitsInB).size()){
        ret = '0' + ret;
      }
      while(ret.back() == '0'){
        ret.pop_back();
      }
      if(!ret.empty()){
        ret = "," + ret;
      }
      return ret;
    }
    
    size_t findDotPosition(const string& str){
      size_t ret = str.size();
      bool found = false;
      for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '.' || str[i] == ',') {
          if(found){ 
            //found more than one dot
            throw std::exception();
          }
          else{
            found = true;
            ret = i;
          }
        }
      }
      
      return ret;
    }
    
    
    long long parseBString(const string &str) {
        size_t dotPos = findDotPosition(str);
        unsigned long long wholePart = 0;
        unsigned long long fractionPart = 0;
        string copy = str;
        wholePart = std::stoll(copy, nullptr, 10);
        if (dotPos < str.size()) {
          copy[dotPos] = ' ';
          copy = copy.substr(dotPos + 1);
          dotPos = 0;
          fractionPart = std::stoll(copy, &dotPos, 10);
          if (dotPos > 8) {
            throw std::exception();
          }
          while (dotPos < 8) {
            dotPos++;
            fractionPart *= 10;
          }
        }
        
        return wholePart * Wallet::unitsInB + fractionPart;
    }
}


void Wallet::addToHistory() {
    unsigned long long val = getWalletVal();
    history.emplace_back(Operation(timeNow(), val));
}

void Wallet::addToHistory(unsigned long long val) {
    history.emplace_back(Operation(timeNow(), val));
}

unsigned long long Wallet::getWalletVal() const {
    unsigned long long size = history.size();
    if (size == 0)
        return 0;
    else
        return history.back().getUnits();
}

// Special private constructor that takes Wallet's history
Wallet::Wallet(Wallet &w1) noexcept
        : history(move(w1.history)) {
    w1.history = vector<Operation>();
}

Wallet::Wallet() : history(vector<Operation>()) {
    addToHistory();
}

Wallet::Wallet(const string &str)
        : Wallet(0, parseBString(str)) {}

//Private constructor for calling other constructors
Wallet::Wallet(const unsigned long long &b, const unsigned long long &units)
        : history({Operation(timeNow(), b * Wallet::unitsInB + units)}) {
    if ((units > Wallet::limit)
        || (b > limitB)
        || (b * Wallet::unitsInB + units > Wallet::limit)) {
        throw std::exception();
    }


    if (Wallet::unitsInCirculation + b * Wallet::unitsInB + units > Wallet::limit) {
        throw std::exception();
    }

    Wallet::unitsInCirculation += b * Wallet::unitsInB + units;
}

// All 'number' constructors (for implicit convertions)
Wallet::Wallet(long long b)
        : Wallet(b, 0) {}

Wallet::Wallet(unsigned long long b)
        : Wallet((long long) b) {}

Wallet::Wallet(short b)
        : Wallet((long long) b) {}

Wallet::Wallet(unsigned short b)
        : Wallet((long long) b) {}

Wallet::Wallet(int b)
        : Wallet((long long) b) {}

Wallet::Wallet(unsigned int b)
        : Wallet((long long) b) {}

Wallet::Wallet(long b)
        : Wallet((long long) b) {}

Wallet::Wallet(unsigned long b)
        : Wallet((long long) b) {}


Wallet::Wallet(Wallet &&w2) noexcept
        : history(move(w2.history)) {
    w2.history = vector<Operation>();
    addToHistory();
}

Wallet Wallet::fromBinary(const string &str) {
    long long wholePart = std::stoll(str, nullptr, 2);
    return Wallet(wholePart);
}

Wallet::Wallet(const char *str)
        : Wallet(string(str)) {}


Wallet::Wallet(Wallet &&w1, Wallet &&w2) {
    unsigned long long sum = w2.getWalletVal() + w1.getWalletVal();
    if (sum > Wallet::limit)
        throw std::exception();
    history = combine(move(w2.history), move(w1.history));
    w1.history = vector<Operation>();
    w2.history = vector<Operation>();
    addToHistory(sum);
}


Wallet &Wallet::operator=(Wallet &&w1) noexcept {
    if (this != &w1) {
        history = move(w1.history);
        w1.history = vector<Operation>();
        addToHistory();
    }

    return *this;
}

Wallet operator+(Wallet &&w1, Wallet &&w2) {
    return Wallet(w1) += w2;
}

Wallet operator+(Wallet &&w1, Wallet &w2) {
    return Wallet(w1) += w2;
}

Wallet operator-(Wallet &&w1, Wallet &&w2) {
    return Wallet(w1) -= w2;
}

Wallet operator-(Wallet &&w1, Wallet &w2) {
    return Wallet(w1) -= w2;
}

Wallet &Wallet::operator+=(Wallet &&w1) {
    if (this == &w1)
        return *this;

    addToHistory(getWalletVal() + w1.getWalletVal());
    w1.addToHistory(0);

    return *this;
}

Wallet &Wallet::operator-=(Wallet &&w1) {
    if (this == &w1)
        return *this;

    if (getWalletVal() < w1.getWalletVal())
        throw std::exception();

    unsigned long long val_w1 = w1.getWalletVal();
    w1.addToHistory(val_w1 + val_w1);
    addToHistory(getWalletVal() - val_w1);
    return *this;
}

Wallet &Wallet::operator+=(Wallet &w1) {
    return (*this) += (move(w1));
}

Wallet &Wallet::operator-=(Wallet &w1) {
    return (*this) -= (move(w1));
}

Wallet &Wallet::operator*=(const unsigned long long val) {

    if (Wallet::unitsInCirculation + ((val - 1) * (getWalletVal())) > Wallet::limit)
        throw std::exception();
    Wallet::unitsInCirculation += (val - 1) * (getWalletVal());

    addToHistory(val * getWalletVal());
    return *this;
}


Wallet operator*(const unsigned long long val, const Wallet &w) {
    if (Wallet::unitsInCirculation + (val * w.getWalletVal()) > Wallet::limit)
        throw std::exception();

    return Wallet(val * (w.getWalletVal()/Wallet::unitsInB));
}

Wallet operator*(const Wallet &w, unsigned long long val) {
    return val * w;
}

unsigned long long Wallet::getUnits() const {
    return getWalletVal();
}

unsigned long long Wallet::opSize() const {
    return history.size();
}

ostream &operator<<(ostream &os, const Wallet &w) {
    std::stringstream ss;
    unsigned long long trailing = w.getWalletVal() % Wallet::unitsInB;
    if (trailing != 0)
        ss << "." << trailing;
    string to_put = ss.str();
    to_put.erase(to_put.find_last_not_of('0') + 1, string::npos);

    os << "Wallet[" << std::to_string(w.getWalletVal() / Wallet::unitsInB) << to_put
       << " B]";
    return os;
}

bool operator==(const Wallet &w1, const Wallet &w2) {
    return w1.getWalletVal() == w2.getWalletVal();
}

bool operator<(const Wallet &w1, const Wallet &w2) {
    return w1.getWalletVal() < w2.getWalletVal();
}

const Wallet Empty() {
    return Wallet();
}

const Operation &Wallet::operator[](int k) const {
    if ((k < 0) || ((unsigned long long)(k) >= history.size()))
        throw std::exception();
    return history[k];
}

Wallet::~Wallet() {
    Wallet::unitsInCirculation -= getWalletVal();
}

unsigned long long Operation::getUnits() const {
    return balance;
}

std::chrono::system_clock::time_point Operation::getTime() const {
    return std::chrono::system_clock::time_point(time);
}

bool Operation::operator==(const Operation &other) const {
    return time == other.time;
}

bool Operation::operator<(const Operation &other) const {
    return time < other.time;
}

ostream &operator<<(ostream &os, const Operation &op) {
    time_t opTimeT = SysClock::to_time_t(op.getTime());
    os << "Wallet balance is " << op.balance / Wallet::unitsInB
       << alignFracZeroes(op.balance % Wallet::unitsInB) << " B after operation made at day "
       << std::put_time(localtime(&opTimeT), "%F");
    return os;
}

Operation::Operation(std::chrono::system_clock::time_point time,
                     unsigned long long balance)
        : time(time), balance(balance) {}