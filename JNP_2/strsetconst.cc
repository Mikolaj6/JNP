#include "strsetconst.h"
#include "strset.h"
#include <climits>

#ifdef NDEBUG
    const bool debug{ false };
#else
    const bool debug{ true };
#endif

namespace {
    bool first = true;
    unsigned long id = ULONG_MAX;

    /**
     * Inicjalizuje strset42.
     */
    void init42(){
        if (debug) {
            std::cerr << "strsetconst init invoked" << std::endl;
        }

        id = jnp1::strset_new();
        jnp1::strset_insert(id, "42");

        if (debug) {
            std::cerr << "strsetconst init finished" << std::endl;
        }
    }

}

namespace jnp1 {

    unsigned long strset42() {

        if (first) {
            first = false;
            init42();
        }
        return id;

    }
}