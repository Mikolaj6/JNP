#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <set>
#include <map>
#include <sstream>
#include <algorithm>
#include <vector>
#include <unordered_map>

using namespace std;

const string REGEX_STR = "^[\\s]*[TDRCE](0|[1-9][0-9]{0,8})[\\s]+[A-Z0-9][a-zA-Z\\d,/-]*([\\s]+(0|[1-9][0-9]{0,8})){2,3}[\\s]*$";

using Type_elem_pair = pair <string, set<int>>;
using String_set = set<string>;
using Int_set = set<int>;
using String_to_int_set_map = unordered_map<string, Int_set>;
using String_int_set_pair = pair<string, Int_set>;

void read_input_send_err(
        String_set &circut_elements,
        map<char, String_to_int_set_map> &types_into_elements);

void print_everything(map<char, String_to_int_set_map> &types_into_elements);

int main() {
    String_set circut_elements;
    map<int, int> node_count;
    map<char, String_to_int_set_map> types_into_elements;

    read_input_send_err(circut_elements, types_into_elements);
    print_everything(types_into_elements);

    return 0;
}

/**
 * Compares two pairs of type Type_elem_pair, by the first element
 * in the int set in ascending order
 */
bool compare_pairs(Type_elem_pair pair_1, Type_elem_pair pair_2) {
    int int_1 = *pair_1.second.begin();
    int int_2 = *pair_2.second.begin();

    return int_1 < int_2;
}

/**
 * Takes reference to a map of type String_to_int_set_map and makes
 * it a vector of pairs of type String_int_set_pair, which it later returns
 */
vector <String_int_set_pair> mapToVector(String_to_int_set_map &map) {
    vector <String_int_set_pair> v;
    v.reserve(map.size());

    for (auto const &single_pair: map)
        v.emplace_back(single_pair);

    return v;
}

/**
 * It takes a vector of pairs of type String_int_set_pair and char that
 * represents the family of element (ex 'D') than it prints all the elements
 * of type. All elements are printed in specified ascending order
 */
void print_type(vector <String_int_set_pair> &vec_to_print, char node) {
    for (auto el = vec_to_print.begin(); el != vec_to_print.end(); ++el) {
        for (auto it = (*el).second.begin(); it != --(*el).second.end(); ++it)
            cout << node << *it << ", ";

        cout << node << *--(*el).second.end() << ": ";
        cout << (*el).first << endl;
    }
}

/**
 * Executes print type for all kinds of elements (from 'T' to 'E')
 */
void print_everything(map<char, String_to_int_set_map> &types_into_elements) {
    char tab[5] = {'T', 'D', 'R', 'C', 'E'};

    for (auto i: tab) {
        vector <String_int_set_pair> vec_of_pairs =
                mapToVector(types_into_elements[i]);

        sort(vec_of_pairs.begin(), vec_of_pairs.end(), compare_pairs);

        print_type(vec_of_pairs, i);
    }
}

/**
 * Converts String to int
 */
int st_to_int(string s) {
    int to_ret = 2;
    stringstream ss;

    ss << s;
    ss >> to_ret;

    return to_ret;
}

/**
 * This function checks whether given line has a correct signature if so
 * it sets nodes to given values and returns true, otherwise it returns false
 * nodes - stringstream that is used for reading line
 * is_transistor - given line represents transistor
 * node_tuple - tuple which holds references to values in the nodes
 */
bool check_nodes(stringstream &nodesStream,
                 bool is_transistor, tuple<int, int, int> &nodes) {

    auto node_1 = get<0>(nodes);
    auto node_2 = get<1>(nodes);
    auto node_3 = get<2>(nodes);

    string node_3s;

    nodesStream >> node_1;
    nodesStream >> node_2;
    nodesStream >> node_3s;
    node_3 = st_to_int(node_3s);

    get<0>(nodes) = node_1;
    get<1>(nodes) = node_2;
    get<2>(nodes) = node_3;

    if ((is_transistor && node_3s.empty())
        || (!is_transistor && !node_3s.empty()))
        return false;

    if (node_1 == node_2) {
      if (!is_transistor)
        return false;
      else if (node_2 == node_3)
        return false;
    }

    return true;
}

/**
 * Returns true when element is a transistor, false otherwise
 */
bool check_trasnistor(string element) {
    if (!element.empty() && element.at(0) == 'T')
        return true;
    else
        return false;
}

/**
 * Prints errors that appeared when reading lines and sends warnings if needed
 * errors - vector of strings that holds lines with errors inside
 * node_count - count number of times vertex was connected
 * Its long for a reason.
 */
void send_err(
        vector <string> const &errors,
        map<int, int> &node_count) {

    string unconnected_nodes;
    bool first = true;

    for (auto tuple: node_count) {
        if (tuple.second < 2) {
            if (first) {
                unconnected_nodes.append(to_string(tuple.first));
                first = false;
            } else {
                unconnected_nodes.append(", " +to_string(tuple.first));
            }
        }
    }

    for (auto it = errors.begin(); it != errors.end(); ++it) {
        if (it != errors.end())
            cerr << (*it) << endl;
    }

    if (!unconnected_nodes.empty())
        cerr << "Warning, unconnected node(s): " << unconnected_nodes <<endl;
}


/**
 * Reads element from line using stringstream
 */
void get_elem(stringstream &convert_line, string &line, string &elem) {
    convert_line << line;
    convert_line >> elem;
}

/**
 * Clears stringstream
 */
void clear_stringstream(stringstream &stream) {
    stream.str("");
    stream.clear();
}


/**
 * In case of a successful regex match reads element
 * checks for transistor and determines element type
 * (doesn't read and validate nodes)
 */
void read_input_matched_successfully (
        stringstream &convert_line,
        string &line,
        tuple<string, string, int> &elem_info,
        bool &is_transistor) {

    auto elem = get<0>(elem_info);
    auto elem_type = get<1>(elem_info);

    clear_stringstream(convert_line);
    get_elem(convert_line, line, elem);

    is_transistor = check_trasnistor(elem);
    convert_line >> elem_type;

    get<0>(elem_info) = elem;
    get<1>(elem_info) = elem_type;
    get<2>(elem_info) = st_to_int(elem.substr(1));
}

/**
 * In case of invalid line it converts line into
 * an error message an pushes it into an error vector
 */
void read_input_if_invalid_line(
        stringstream &convert_error,
        string &tmp,
        string &line,
        int line_counter,
        vector <string> &errors) {

    clear_stringstream(convert_error);
    tmp.clear();

    convert_error << "Error in line " << line_counter << ": " << line;
    tmp = convert_error.str();
    errors.push_back(tmp);
}

/**
 * In case of a valid line updates node_count and types_into_elements
 */
void read_input_if_valid_line(
        tuple<int, int, int> &nodes,
        String_set & circut_elements,
        map<int, int> &node_count,
        tuple<string, string, int> &elem_info,
        map<char, String_to_int_set_map> &types_into_elements,
        bool is_transistor) {

    auto node1 = get<0>(nodes);
    auto node2 = get<1>(nodes);

    auto elem = get<0>(elem_info);
    auto elem_type = get<1>(elem_info);
    auto elem_num = get<2>(elem_info);

    circut_elements.insert(get<0>(elem_info));
    node_count[node1]++;

    if (node2 != node1)
        node_count[node2]++;

    types_into_elements[elem[0]][elem_type].insert(elem_num);

    auto node3 = get<2>(nodes);

    if (is_transistor &&node1 != node3 && node2 != node3)
        node_count[node3]++;
}

/**
 * Main function that reads input and prints errors
 * Its long for a reason.
 */
void read_input_send_err(
        String_set &circut_elements,
        map<char, String_to_int_set_map> &types_into_elements) {

    map<int, int> node_count;
    string line, elem, elem_type, tmp;
    smatch base_match;
    regex base_regex(REGEX_STR);
    stringstream convert_line, convert_error;
    vector <string> errors;
    int line_counter = 0, elem_num, node1, node2, node3;
    auto nodes = tuple<int, int, int>(node1, node2, node3);
    auto elem_w_type = pair<string, string>(elem, elem_type);
    bool invalid_line, is_transistor;

    node_count.insert(pair<int, int>(0, 0));

    while (getline(cin, line)) {
        line_counter++;
        invalid_line = false;

        elem_w_type.first.clear();
        elem_type.clear();

        if (line.size() == 0)
            continue;

        //Reads line
        if (regex_match(line, base_match, base_regex)) {
            
            auto elem_info = make_tuple(elem, elem_type, elem_num);

            read_input_matched_successfully(convert_line, line, elem_info, 
                                            is_transistor);

            elem = get<0>(elem_info);
            elem_type = get<1>(elem_info);
            elem_num = get<2>(elem_info);

            //Reads nodes and checks if they are ok
            if (circut_elements.find(elem) != circut_elements.end())
                invalid_line = true;

            if (!check_nodes(convert_line, is_transistor, nodes))
                invalid_line = true;

        } else {
            invalid_line = true;
        }

        //Manages input
        if (invalid_line) {
            read_input_if_invalid_line(convert_error, tmp, line, line_counter,
                                       errors);
        } else {
            auto elem_info = make_tuple(elem, elem_type, elem_num);

            read_input_if_valid_line(nodes, circut_elements, node_count,
                                     elem_info, types_into_elements, 
                                     is_transistor);
        }
    }

    send_err(errors, node_count);
}
