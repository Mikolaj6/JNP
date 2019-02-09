#ifndef CITATION_GRAPH_H
#define CITATION_GRAPH_H

#include <set>
#include <memory>
#include <map>
#include <vector>

// Byliśmy na konsultacjach i powiedział Pan, że może być struct
// zamiast class dla tych wyjątków, jak i Node. Taka decyzja,
// bo struct ma domyślnie wszystko public, co jest wygodne.

// Wyjątek rzucany, kiedy publikacja została już utworzona
// i istnieje w grafie, a nie powinna.
struct PublicationAlreadyCreated : public std::exception {
    const char *what() const throw() {
        return "PublicationAlreadyCreated";
    }
};

// Wyjątek rzucany, kiedy publikacja nie istnieje w grafie, a powinna.
struct PublicationNotFound : public std::exception {
    const char *what() const throw() {
        return "PublicationNotFound";
    }
};

// Wyjątek rzucany, przy próbie usunięcia pierwotnej publikacji,
// czyli korzenia drzewa
struct TriedToRemoveRoot : public std::exception {
    const char *what() const throw() {
        return "TriedToRemoveRoot";
    }
};

// Graf cytowanań to struktura, która umożliwia przeprowadzenie analizy
// wpływowości publikacji. Jest to graf skierowany acykliczny. Każdy
// wierzchołek odpowiada jednej publikacji. Graf ma jedno źródło, które
// reprezentuje pierwotną publikację, której wpływ chcemy analizować.
// Krawędź łączy publikację cytowaną z publikacją cytującą.
template<class Publication>
class CitationGraph {
private:
    using id_type = typename Publication::id_type;   
    struct Node;
    using map_type = std::map<id_type, std::weak_ptr<Node>>;

    // Struktura reprezentująca pojedynczy wierzchołek w tym grafie.
    struct Node : public std::enable_shared_from_this<Node> {
        std::set<std::weak_ptr<Node>,
                std::owner_less<std::weak_ptr<Node>>> parents;
        std::set<std::shared_ptr<Node>> children;
        std::weak_ptr<map_type> nodes_ptr;
        Publication publication;
        typename map_type::iterator position;

        // Tworzy nowy wierzchołek w grafie z podanym id, i wskaźnikiem na mapę
        // nodes, gdzie jest on trzymany. Uwaga, po skonstruowaniu obiektu
        // konieczne jest wstawić go do mapy nodes i ustawić position jako
        // iterator jego wystąpienia w niej.
        Node(id_type const &id, std::shared_ptr<map_type> nodes)
                : nodes_ptr(nodes), publication(Publication(id)),
                  position(nodes->end()) {}

        // Usuwa wierzchołek z grafu, dba o to, by został usunięty z rodziców
        // swoich dzieci (ale nie z dzieci rodziców), jak i z mapy nodes.
        // Jeśli usunięcie tego wierzchołka spowodowałoby, że jego dziecko
        // pozostanie bez żadnego ojca, to jest również usuwane.
        ~Node() {
            for (auto const &elem: children)
                elem->parents.erase(this->weak_from_this());

            auto map_shared = nodes_ptr.lock();
            if (position != map_shared->end())
                map_shared->erase(position);
        }
    };

    std::shared_ptr<map_type> nodes;
    std::shared_ptr<Node> root;

    // Zwraca iterator na miesce, gdzie występuje id w mapie nodes.
    // Jeśli id w tej mapie nie występuje to rzuca wyjątek PublicationNotFound.
    typename map_type::iterator findNode(id_type const &id) const {
        auto it = nodes->find(id);

        if (it == nodes->end())
            throw PublicationNotFound();

        return it;
    }

public:
    // Tworzy nowy graf. Tworzy też węzeł publikacji o identyfikatorze stem_id.
    CitationGraph(id_type const &stem_id);

    CitationGraph(CitationGraph<Publication> &&other) noexcept;

    CitationGraph<Publication> &
    operator=(CitationGraph<Publication> &&other) noexcept;

    CitationGraph(CitationGraph<Publication> const &other) = delete;

    CitationGraph<Publication> &
    operator=(CitationGraph<Publication> const &other) = delete;

    // Zwraca identyfikator źródła. Metoda ta powinna być noexcept wtedy i tylko
    // wtedy, gdy metoda Publication::get_id jest noexcept.
    id_type get_root_id() const noexcept(noexcept(root->publication.get_id()));

    // Zwraca listę identyfikatorów publikacji cytujących publikację o podanym
    // identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana
    // publikacja nie istnieje.
    std::vector<id_type> get_children(id_type const &id) const;

    // Zwraca listę identyfikatorów publikacji cytowanych przez publikację o
    // podanym identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana
    // publikacja nie istnieje.
    std::vector<id_type> get_parents(id_type const &id) const;

    // Sprawdza, czy publikacja o podanym identyfikatorze istnieje.
    bool exists(id_type const &id) const;

    // Zwraca referencję do obiektu reprezentującego publikację o podanym
    // identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli żądana
    // publikacja nie istnieje.
    Publication &operator[](id_type const &id) const;

    // Tworzy węzeł reprezentujący nową publikację o identyfikatorze id cytującą
    // publikacje o podanym identyfikatorze parent_id lub podanych
    // identyfikatorach parent_ids. Zgłasza wyjątek PublicationAlreadyCreated,
    // jeśli publikacja o identyfikatorze id już istnieje. Zgłasza wyjątek
    // PublicationNotFound, jeśli któryś z wyspecyfikowanych poprzedników nie
    // istnieje albo lista poprzedników jest pusta.
    void create(id_type const &id, std::vector<id_type> const &parent_ids);

    void create(id_type const &id, id_type const &parent_id);

    // Dodaje nową krawędź w grafie cytowań. Zgłasza wyjątek PublicationNotFound,
    // jeśli któraś z podanych publikacji nie istnieje.
    void add_citation(id_type const &child_id, id_type const &parent_id);

    // Usuwa publikację o podanym identyfikatorze. Zgłasza wyjątek
    // PublicationNotFound, jeśli żądana publikacja nie istnieje. Zgłasza
    // wyjątek TriedToRemoveRoot przy próbie usunięcia pierwotnej publikacji.
    // W wypadku rozspójnienia grafu, zachowujemy tylko spójną składową
    // zawierającą źródło.
    void remove(id_type const &id);
};

template<class Publication>
CitationGraph<Publication>::CitationGraph(id_type const &stem_id) :
        nodes(std::make_shared<std::map<id_type, std::weak_ptr<Node>>>()),
        root(std::make_shared<Node>(stem_id, nodes)) {
    root->position = nodes->emplace(stem_id, root).first;
}

template<class Publication>
CitationGraph<Publication> &CitationGraph<Publication>::operator=(
        CitationGraph<Publication> &&other) noexcept {
    std::swap(nodes, other.nodes);
    std::swap(root, other.root);

    return (*this);
}

template<class Publication>
CitationGraph<Publication>::CitationGraph(
        CitationGraph<Publication> &&other) noexcept
        : nodes(std::move(other.nodes)), root(std::move(other.root)) {}

template<class Publication>
typename Publication::id_type CitationGraph<Publication>::get_root_id()
const noexcept(noexcept(root->publication.get_id())) {
    return root->publication.get_id();
}

template<class Publication>
std::vector<typename Publication::id_type>
CitationGraph<Publication>::get_children(id_type const &id) const {
    auto it = findNode(id);
    std::vector<id_type> children_list;

    for (auto child : it->second.lock()->children) {
        id_type child_id = child->publication.get_id();
        children_list.push_back(child_id);
    }

    return children_list;
}


template<class Publication>
std::vector<typename Publication::id_type>
CitationGraph<Publication>::get_parents(id_type const &id) const {
    auto it = findNode(id);
    std::vector<id_type> parents_list;

    for (auto const &parent : it->second.lock()->parents)
        parents_list.push_back(parent.lock()->publication.get_id());

    return parents_list;
}

template<class Publication>
bool CitationGraph<Publication>::exists(id_type const &id) const {
    return nodes->find(id) != nodes->end();
}

template<class Publication>
Publication &CitationGraph<Publication>::operator[](id_type const &id) const {
    auto it = findNode(id);
    return it->second.lock()->publication;
}

template<class Publication>
void CitationGraph<Publication>::create(id_type const &id,
                                        std::vector<id_type> const &parent_ids) {
    if (nodes->find(id) != nodes->end())
        throw PublicationAlreadyCreated();
    if (parent_ids.empty())
        throw PublicationNotFound();
    for (auto const &parent_id: parent_ids)
        findNode(parent_id);

    auto node_ptr = std::make_shared<Node>(id, nodes);
    std::vector<std::weak_ptr<Node>> tmp;
    typename map_type::iterator it;
    bool inserted = false;

    try {
        for (auto const &parent_id: parent_ids) {
            auto parent = nodes->find(parent_id)->second;
            tmp.push_back(parent);
            parent.lock()->children.insert(node_ptr);
        }
        for (auto const &parent_id: parent_ids)
            node_ptr->parents.insert(nodes->find(parent_id)->second);
        it = nodes->emplace(id, node_ptr).first;
        inserted = true;
        node_ptr->position = it;
    } catch (...) {
        for (auto const &tmp_member: tmp)
            tmp_member.lock()->children.erase(node_ptr);
        if (inserted)
            nodes->erase(it);
        throw;
    }
}

template<class Publication>
void CitationGraph<Publication>::create(id_type const &id,
                                        id_type const &parent_id) {
    std::vector<id_type> vec{parent_id};
    create(id, vec);
}

template<class Publication>
void CitationGraph<Publication>::add_citation(id_type const &child_id,
                                              id_type const &parent_id) {
    auto it_child = findNode(child_id);
    auto it_parent = findNode(parent_id);

    auto child = it_child->second.lock();
    auto parent = it_parent->second;

    auto insert_val = child->parents.insert(parent);

    try {
        parent.lock()->children.insert(child);
    } catch (...) {
        if (insert_val.second)
            child->parents.erase(insert_val.first);
        throw;
    }
}


template<class Publication>
void CitationGraph<Publication>::remove(const id_type &id) {
    auto it = findNode(id);
    auto node_shared = it->second.lock();
    if (node_shared.get() == root.get())
        throw TriedToRemoveRoot();

    for (auto parent: node_shared->parents)
        parent.lock()->children.erase(node_shared);
}


#endif //CITATION_GRAPH_H
