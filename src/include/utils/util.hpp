#pragma once
#include <time.h>
#include <string>
#include <iostream>

namespace ss 
{

/**
 * A timer object measures elapsed time, and it is very similar to boost::timer.
 */
class timer
{
public:
    timer(): time(double(clock())) {};
    ~timer() {};
    /**
     * Restart the timer.
     */
    void restart()
    {
        time = double(clock());
    }
    /**
     * Measures elapsed time.
     *
     * @return The elapsed time
     */
    double elapsed()
    {
        return (double(clock()) - time) / CLOCKS_PER_SEC;
    }
private:
    double time;
};
}
