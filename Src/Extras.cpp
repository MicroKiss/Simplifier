#include "Extras.hpp"
#include <chrono>

long long TimeIt (std::function<void ()> const &func) {
    using namespace std::chrono;
    auto start = high_resolution_clock::now ();
    func ();
    auto stop = high_resolution_clock::now ();
    return duration_cast<milliseconds> (stop - start).count ();
}