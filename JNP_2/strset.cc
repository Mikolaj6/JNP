#include "strset.h"
#include "strsetconst.h"
#include <unordered_map>
#include <string.h>
#include <set>
#include <stdlib.h>
#include <climits>
#include <time.h>
#include <sstream>

#ifdef NDEBUG
    const bool debug{ false };
#else
    const bool debug{ true };
#endif

namespace {

    using UL = unsigned long;

    std::unordered_map<unsigned long, std::set<std::string>> &mapa() {
        static auto *aux = new std::unordered_map<UL, std::set<std::string>>();
        return *aux;
    };

    unsigned long x = 1;
    using strset_it = std::set<std::string>::iterator;
    using strset_p = std::pair<strset_it, strset_it>;
    using map_it = std::unordered_map<UL, std::set<std::string>>::iterator;
    using const_s = const std::string;

    bool already_created = false;

    /**
     * Sprawdza, który zbiór jest pierwszy leksykograficznie po
     * osiągnięciu końca któregoś z nich.
     */
    int similar_sets(strset_p &set_1, strset_p &set_2) {
        if (set_1.first == set_1.second && set_2.first == set_2.second) {
            return 0;
        }
        if (set_1.first == set_1.second) {
            return -1;
        }
        return 1;
    }

    /**
     * Sprawdza, który zbiór jest pierwszy leksykograficznie przy założeniu,
     * że obydwa istnieją. Argumentami są referencje par iteratorów wskazujących
     * na początek i koniec odpowiednich zbiorów.
     */
    int both_sets_exist(strset_p &set_1, strset_p &set_2) {
        while (set_1.first != set_1.second && set_2.first != set_2.second) {
            int cmp = set_1.first->compare(*set_2.first);

            if (cmp < 0) {
                return -1;
            }
            else if (cmp > 0) {
                return 1;
            }

            ++set_1.first;
            ++set_2.first;
        }

        return similar_sets(set_1, set_2);
    }

    /**
     * Sprawdza, który zbiór jest pierwszy leksykograficznie przy założeniu,
     * że przynajmniej jeden z nich jest pusty.
     */
    int lacking_sets(map_it map_it1, map_it map_it2) {
        if (map_it1 == mapa().end() && map_it2 == mapa().end()) {
            return 0;
        }
        if (map_it1 == mapa().end()) {
            return -1;
        }
        return 1;
    }

    void print_func_name_no_arg(const_s &function) {
        std::cerr << function << "()" << std::endl;
    }

    void print_func_name_1arg(const_s &function, UL i1) {
        std::cerr << function << "(" << i1 << ")" << std::endl;
    }

    void print_func_name_2arg(const_s &function, UL i1, const_s &str2) {
        std::cerr << function << "(" << i1 << ", " << str2 <<")" << std::endl;
    }

    void print_func_name_2arg(const_s &function, UL i1, UL i2) {
        std::cerr << function << "(" << i1 << ", " << i2 <<")" << std::endl;
    }

    std::string pr_str(const char *value) {
        std::string to_ret ="\"";
        to_ret.append(value);
        to_ret.append("\"");
        return to_ret;
    }
}

namespace jnp1 {

    /**
     * Tworzy nowy zbiór i zwraca jego identyfikator.
     */
    unsigned long strset_new() {
        mapa().insert(make_pair(x, std::set<std::string>()));
        x++;

        if (debug) {
            print_func_name_no_arg(__func__);
            std::cerr << __func__ << ": set " << (x - 1) << " created"
                      << std::endl;
        }

        return (x - 1);
    }

    /**
     * Jeżeli istnieje zbiór o identyfikatorze id, usuwa go, a w przeciwnym
     * przypadku nie robi nic.
     */
    void strset_delete(unsigned long id) {
        if (debug) {
            print_func_name_1arg(__func__, id);
        }

        if (id != jnp1::strset42()) {
            if (id != 0){
                mapa().erase(id);
                if (debug) {
                    std::cerr << __func__ << ": set " << id << " deleted"
                              << std::endl;
                }
            } else if (debug) {
                std::cerr << __func__ << ": id does not exist" << std::endl;
            }
        } else if (debug) {
            std::cerr << __func__<< ": attempt to remove the 42 Set"
                      << std::endl;
        }
    }

    /**
     * Jeżeli istnieje zbiór o identyfikatorze id, zwraca liczbę jego elementów,
     * a w przeciwnym przypadku zwraca 0.
     */
    size_t strset_size(unsigned long id) {
        if (debug) {
            print_func_name_1arg(__func__, id);
        }

        size_t size = 0;
        auto it = mapa().find(id);

        if (it != mapa().end()) {
            size = it->second.size();
        } else if (debug) {
            std::cerr << __func__<< ": set " << id << " does not exist"
                      << std::endl;

            return 0;
        }

        if (debug) {
            std::string id_set = "set " + std::to_string(id);
            if (id == jnp1::strset42()) {
                id_set = "the 42 Set";
            }

            std::cerr << __func__<< ": " << id_set << " contains " << size
                      << " element(s)" << std::endl;
        }

        return size;
    }

    /**
     * Jeżeli istnieje zbiór o identyfikatorze id i element value nie należy do
     * tego zbioru, to dodaje element do zbioru, a w przeciwnym przypadku nie
     * robi nic.
     */
    void strset_insert(unsigned long id, const char *value) {
        if (value == nullptr) {
            if (debug) {
                print_func_name_2arg(__func__, id, "NULL");
                std::cerr << __func__<< ": invalid value (NULL)" << std::endl;
            }
            return;
        }

        if (debug) {
            print_func_name_2arg(__func__, id, pr_str(value));
        }

        if (id == jnp1::strset42()) {
            if (already_created) {
                if (debug) {
                    std::cerr << __func__
                              << ": attempt to insert into the 42 Set"
                              << std::endl;
                }
                return;
            }
            else {
                already_created = true;
            }
        }

        auto it = mapa().find(id);
        if (it != mapa().end()) {
            if (it->second.find(std::string(value)) == it->second.end()) {
                it->second.insert(std::string(value));
                if (debug) {
                    std::cerr << __func__<< ": set " << id << ", element "
                              << pr_str(value) << " inserted" << std::endl;
                }
            } else if (debug) {
                std::cerr << __func__<< ": set " << id << ", element "
                          << pr_str(value) << " was already present"
                          << std::endl;
            }
        } else if (debug) {
            std::cerr << __func__<< ": set " << id << " does not exist"
                      << std::endl;
        }
    }

    /**
     * Jeżeli istnieje zbiór o identyfikatorze id i element value należy do
     * tego zbioru, to usuwa element ze zbioru, a w przeciwnym przypadku
     * nie robi nic.
     */
    void strset_remove(unsigned long id, const char *value) {
        if (value == nullptr) {
            if (debug) {
                print_func_name_2arg(__func__, id, "NULL");
                std::cerr << __func__<< ": invalid value (NULL)" << std::endl;
            }
            return;
        }

        if (debug) {
            print_func_name_2arg(__func__, id, pr_str(value));
        }

        if (id == jnp1::strset42()) {
            if (debug) {
                std::cerr << __func__ << ": attempt to remove from the 42 Set"
                          << std::endl;
            }
            return;
        }

        auto it = mapa().find(id);
        if (it != mapa().end()) {
            auto str = it->second.find(value);
            if (str != it->second.end()){
                if (debug) {
                    std::cerr << __func__<< ": set " << id << ", element "
                              << pr_str(value) << " removed" << std::endl;
                }
                it->second.erase(str);
            } else if (debug) {
                std::cerr << __func__<< ": set " << id
                          << " does not contain the element "
                          << pr_str(value) << std::endl;
            }
        } else if (debug) {
            std::cerr << __func__<< ": set " << id << " does not exist"
                      << std::endl;
        }
    }

    /**
     * Jeżeli istnieje zbiór o identyfikatorze id i element value należy do
     * tego zbioru, to zwraca 1, a w przeciwnym przypadku 0.
     */
    int strset_test(unsigned long id, const char *value) {
        if (value == nullptr) {
            if (debug) {
                print_func_name_2arg(__func__, id, "NULL");
                std::cerr << __func__<< ": invalid value (NULL)" << std::endl;
            }
            return 0;
        }

        if (debug) {
            print_func_name_2arg(__func__, id, pr_str(value));
        }

        int result = 0;
        auto it = mapa().find(id);
        if (it != mapa().end()) {
            if (it->second.count(std::string(value))) {
                result = 1;
            }
        } else if (debug) {
            std::cerr << __func__<< ": set " << id << " does not exist"
                      << std::endl;

            return 0;
        }

        if (debug) {
            std::string id_set = "set " + std::to_string(id);
            if (id == jnp1::strset42()) {
                id_set = "the 42 Set";
            }

            std::cerr << __func__<< ": " << id_set;
            if (result == 1) {
                std::cerr << " contains";
            } else {
                std::cerr << " does not contain";
            }
            std::cerr << " the element " << pr_str(value) << std::endl;
        }

        return result;
    }

    /**
     * Jeżeli istnieje zbiór o identyfikatorze id, usuwa wszystkie jego
     * elementy, a w przeciwnym przypadku nie robi nic.
     */
    void strset_clear(unsigned long id) {
        if (debug) {
            print_func_name_1arg(__func__, id);
        }

        if (id == jnp1::strset42()) {
            if (debug) {
                std::cerr << __func__ << ": attempt to clear the 42 Set"
                          << std::endl;
            }
            return;
        }

        auto it = mapa().find(id);
        if (it != mapa().end()) {
            if (debug) {
                std::cerr << __func__<< ": set " << id << " cleared"
                          << std::endl;
            }
            it->second.clear();
        } else if (debug) {
            std::cerr << __func__<< ": set " << id << " does not exist"
                      << std::endl;
        }
    }

    /**
     * Porównuje zbiory o identyfikatorach id1 i id2. Niech sorted(id) oznacza
     * posortowany leksykograficznie zbiór o identyfikatorze id. Takie ciągi
     * już porównujemy naturalnie: pierwsze miejsce, na którym się różnią,
     * decyduje o relacji większości. Jeśli jeden ciąg jest prefiksem drugiego,
     * to ten będący prefiks jest mniejszy. Funkcja strset_comp(id1, id2)
     * powinna zwrócić
     * -1, gdy sorted(id1) < sorted(id2),
     * 0, gdy sorted(id1) = sorted(id2),
     * 1, gdy sorted(id1) > sorted(id2).
     * Jeżeli zbiór o którymś z identyfikatorów nie istnieje, to jest traktowany
     * jako równy zbiorowi pustemu.
     */
    int strset_comp(unsigned long id1, unsigned long id2) {
        jnp1::strset42();

        int result;

        auto it1 = mapa().find(id1);
        auto it2 = mapa().find(id2);

        bool first_exists = it1 != mapa().end();
        bool second_exists = it2 != mapa().end();

        if (first_exists && second_exists) {
            strset_p set_1 = make_pair(it1->second.begin(), it1->second.end());
            strset_p set_2 = make_pair(it2->second.begin(), it2->second.end());

            result = both_sets_exist(set_1, set_2);
        } else {
            result = lacking_sets(it1, it2);
        }

        if (debug) {
            print_func_name_2arg(__func__, id1, id2);

            std::string id1_set = "set " + std::to_string(id1);
            std::string id2_set = "set " + std::to_string(id2);
            if (id1 == jnp1::strset42()) {
                id1_set = "the 42 Set";
            }
            if (id2 == jnp1::strset42()) {
                id2_set = "the 42 Set";
            }

            std::cerr << __func__<< ": result of comparing " << id1_set
                      << " to " << id2_set << " is " << result << std::endl;
            if (!first_exists) {
                std::cerr << __func__<< ": set " << id1 << " does not exist"
                          << std::endl;
            }
            if (!second_exists) {
                std::cerr << __func__<< ": set " << id2 << " does not exist"
                          << std::endl;
            }
        }

        return result;
    }
}
