#include "gridsubinterval.h"

#include "iostream"

GridSubInterval::GridSubInterval(double l, double u)
{
    lowerBound = l;
    upperBound = u;
}


GridSubInterval::~GridSubInterval()
{

}

bool GridSubInterval::belongsToSubinterval(double value, bool last)
{
    if (last)
    {
        std::cout << "last" << "\n";

        std::cout << "lowerBound: " << lowerBound << "value: " << value << "upperBound" << upperBound << "\n";
        if ( (value >= lowerBound) && (value <= upperBound) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }else
    {
        std::cout << " no last" << "\n";
        if ( (value >= lowerBound) && (value < upperBound) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

}

double GridSubInterval::getLowerBound()
{
    return lowerBound;
}

double GridSubInterval::getUpperBound()
{
    return upperBound;
}


void GridSubInterval::printSubInterval()
{

}
