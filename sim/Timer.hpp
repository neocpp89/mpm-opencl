#ifndef TIMER_HPP
#define TIMER_HPP
#include <chrono>
#include <ratio>
#include <iostream>
#include <string>

class Timer
{
    std::string prefix;
    std::chrono::system_clock::time_point start;

    public:
        Timer(std::string s = "") :
            prefix(s),
            start(std::chrono::system_clock::now())
        {
            return;
        }

        ~Timer()
        {
            if (prefix != "") {
                std::cout << prefix << " took " << elapsedTime<double, std::milli>() << " ms.\n";
            }
            return;
        }

        // default to doubles and milliseconds
        template <typename Numeric = double, typename Ratio = std::milli>
        Numeric elapsedTime()
        {
            using duration_type = std::chrono::duration<Numeric, Ratio>;
            auto current = std::chrono::system_clock::now();
            return std::chrono::duration_cast<duration_type>(current - start).count();
        }

        void reset()
        {
            start = std::chrono::system_clock::now();
            return;
        }
};

#ifndef NDEBUG
using DebugTimer = Timer;
#else
/* Compile away to nothing if we are not debugging. */
class DebugTimer
{
    public:
        DebugTimer(std::string s = "") { return; }
};
#endif //NDEBUG

#endif //TIMER_HPP
