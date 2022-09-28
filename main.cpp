#include <iostream>
#include <random>
#include <vector>
#include <fstream>
#include <string>
#include <tuple>
#include <sstream>
#include <cmath>
#include <iterator>


const int first_inserted_atom = 96;
const double box_size = 10.260;


std::random_device rd;  // Will be used to obtain a seed for the random number engine.
std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd().


typedef std::tuple<double, double, double> data_tuple;


std::vector<data_tuple> coordinates_read (const std::string & name);

double min (std::vector<data_tuple> & data, const int & first_insert);

void fix (std::vector<data_tuple> & data, const int & first_insert, const double & box_size);

void data_file_creation (const std::string & name, std::vector<data_tuple> & data);


int main () {
    std::vector<data_tuple> data = coordinates_read("coordinates");
    fix(data, first_inserted_atom, box_size);
    std::cout << "Minimal_distance:\t" << min(data, first_inserted_atom) << '\n';
    data_file_creation("coordinates", data);
    return 0;
}


namespace std {
    istream& operator >> (istream& in, data_tuple & coordinates) {
        double first, second, third;
        in >> first >> second >> third;
        coordinates = {first, second, third};
        return in;
    }

    ostream& operator << (ostream& out, const data_tuple & coordinates) {
        auto [first, second, third] = coordinates;
        out << first << ' ' << second << ' ' << third << ' ';
        return out;
    }
}


std::vector<data_tuple> coordinates_read (const std::string & name) {
    std::ifstream fin(name);
    if (!fin.is_open()) throw std::runtime_error("Error opening file.");
    std::vector<data_tuple> tuples_vector;
    copy(std::istream_iterator<data_tuple> {fin},
         std::istream_iterator<data_tuple> {},
         back_inserter(tuples_vector));
    //copy(tuples_vector.begin(), tuples_vector.end(), std::ostream_iterator<data>(std::cout, "\n"));
    return tuples_vector;
}


template<typename T, size_t... Is>
double distance_impl (T const& t, T const& t1, std::index_sequence<Is...>, std::index_sequence<Is...>) {
    return (std::sqrt((std::pow(std::get<Is>(t) - std::get<Is>(t1), 2) + ...)));
}

template <class Tuple>
double distance (const Tuple & t, const Tuple & t1) {
    constexpr auto size = std::tuple_size<Tuple>{};
    return distance_impl(t, t1, std::make_index_sequence<size>{}, std::make_index_sequence<size>{});
}


int problem_atom (std::vector<data_tuple> & data, const int & first_insert) {
    int ans = data.size();
    double buf = distance(data[1], data[0]);
    for (int i = first_insert; i < data.size(); ++i)
        for (int j = 0; j < i; ++j)
            if (distance(data[i], data[j]) < buf) ans = i;
    return ans;
}


template<size_t Is = 0, typename... Tp>
void new_coordinates (std::tuple<Tp...>& coordinate, const double & box_size) {
    std::uniform_real_distribution<> dis(0, box_size);
    std::get<Is>(coordinate) = dis(gen);
    if constexpr(Is + 1 != sizeof...(Tp))
        new_coordinates<Is + 1>(coordinate, box_size);
}


void fix (std::vector<data_tuple> & data, const int & first_insert, const double & box_size) {
    int i;
    do {
        i = problem_atom(data, first_insert);
        new_coordinates(data[i], box_size);
    } while (i > first_insert && distance(data[1], data[0]) > min(data, first_insert));
}


double min (std::vector<data_tuple> & data, const int & first_insert) {
    double buf = distance(data[1], data[0]);
    for (int i = first_insert; i < data.size(); ++i)
        for (int j = 0; j < i; ++j) {
            double dist = distance(data[i], data[j]);
            if (dist < buf) buf = dist;
        }
    return buf;
}


// std::to_string not safe enough. It will be used everywhere instead of std::to_string.
template <typename T>
std::string toString (T val) {
    std::ostringstream oss;
    oss << val;
    return oss.str();
}


// Returns string contains tuple content.
template<typename T, size_t... Is>
std::string tuple_to_string_impl (T const& t, std::index_sequence<Is...>) {
    return ((toString(std::get<Is>(t)) + '\t') + ...);
}

template <class Tuple>
std::string tuple_to_string (const Tuple& t) {
    constexpr auto size = std::tuple_size<Tuple>{};
    return tuple_to_string_impl(t, std::make_index_sequence<size>{});
}


void data_file_creation (const std::string & name, std::vector<data_tuple> & data) {
    std::ofstream fout;
    fout.open(name, std::ios::trunc);
    for (auto & i : data)
        fout << tuple_to_string(i) << '\n';
    fout.close();
}