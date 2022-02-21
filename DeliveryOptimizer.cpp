#include "provided.h"
#include <vector>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
private:
    const StreetMap* str;

};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
    str = sm;
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    vector<DeliveryRequest>  temp_deliveries = deliveries;
    GeoCoord start(depot), end, end2;
    PointToPointRouter ppr(str);
    double distance1(0.0), distance2(0.0);

    // Finding the old value;
    for (int l = 0; l < deliveries.size(); l++)
    {
        list<StreetSegment> route;
        end = temp_deliveries[l].location;
        if (ppr.generatePointToPointRoute(start, end, route, distance1) == 0)
        {           
            for (list<StreetSegment>::iterator it = route.begin(); it != route.end(); it++)
            {
                distance1 = distanceEarthMiles(it->start, it->end);
                oldCrowDistance = oldCrowDistance + distance1;
            }
        }
        else
            return;
        start = end;
    }
    // We start k from negative one because we want to consider the depot geo coordinate
    int size = deliveries.size();
    int k=-1;
    while (k < size)
    {
        list<StreetSegment> route1;
        list<StreetSegment> route2;
        // If k is less than, the start will be set to the depot coordinate
        if (k>=0)
            start = deliveries[k].location;
        else
            start = depot;

        // Bubble sort with comparing the shortest distance
        for (int i = k+1; i < size-1; i++)
        {
            for (int j = k+1; j < size - i -1; j++)
            {
                distance1 = distance2 = 0.0;
                end = deliveries[j].location;
                end2 = deliveries[j+1].location;        
                if (ppr.generatePointToPointRoute(start, end, route1, distance1) == 0 && ppr.generatePointToPointRoute(start, end2, route2, distance2) == 0)
                {
                    if (distance1 > distance2)
                    {
                        cout << "Swap " << j << "  " << j + 1 << endl;
                        DeliveryRequest temp = deliveries[j + 1];
                        deliveries[j + 1] = deliveries[j];
                        deliveries[j] = temp;
                    }
                }
                

            }
        }
        k++;
    }

    // The distance for new deliveries plan
    distance2 = 0.0;
    start = depot;
    for (int l = 0; l < deliveries.size(); l++)
    {
        list<StreetSegment> route;
        end = deliveries[l].location;
        if (ppr.generatePointToPointRoute(start, end, route, distance2) == 0)
        {
            for (list<StreetSegment>::iterator it = route.begin(); it != route.end(); it++)
            {
                distance2 = distanceEarthMiles(it->start, it->end);
                newCrowDistance = newCrowDistance + distance2;
            }
                
        }
        start = end;
    }
}


//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
