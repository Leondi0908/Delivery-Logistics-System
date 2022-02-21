#include "provided.h"
#include "ExpandableHashMap.h"
#include <list>
#include <queue>

using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* str;
    bool success(const GeoCoord gc, const GeoCoord endc) const;
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    str = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
    const GeoCoord& start,
    const GeoCoord& end,
    list<StreetSegment>& route,
    double& totalDistanceTravelled) const
{
    GeoCoord start_Coord = start;
    bool pathFound = false;
    vector <StreetSegment> str_Segs1;
    ExpandableHashMap<GeoCoord, GeoCoord> locationOfPreviousWayPoint;
    ExpandableHashMap<GeoCoord, StreetSegment> get_SS;
    queue<GeoCoord>visitedCoords;

    // If the start and end coordinate are the same position
    if (success(start, end))
    {
        totalDistanceTravelled = 0.0;
        return DELIVERY_SUCCESS;
    }

    // If the start coordinate is not found
    if (!str->getSegmentsThatStartWith(start, str_Segs1))
        return BAD_COORD;

    // If the end coordinate is not found    
    if (!str->getSegmentsThatStartWith(end, str_Segs1))
        return NO_ROUTE;

    // Pushing the starting Point
    visitedCoords.push(start_Coord);

    // If the queue is not empty or the coordinate has not been found. 
    // It will be looping 
    while (!visitedCoords.empty() || pathFound != true)
    {
        // showing it the front GeoCoord from the queue
        start_Coord = visitedCoords.front();
        // Sucess to find arrive at the final destination
        if (success(start_Coord, end))
        {
            pathFound = true;
            break;
        }
        else
        {
            vector <StreetSegment> str_Segs2;
            // deleting the front queue after getting the data
            visitedCoords.pop();
            // If we can get the some street segements that has our start GeoCoord as the parameter 
            // and the next GeoCoord exists
            str->getSegmentsThatStartWith(start_Coord, str_Segs2);
            for (unsigned int i = 0; i < str_Segs2.size(); i++)
                {
                    //GeoCoord g1(str_Segs[i].start.latitudeText, str_Segs[i].start.longitudeText);
                    //GeoCoord g2(str_Segs[i].end.latitudeText, str_Segs[i].end.longitudeText);

                    // some path has been found, it is going to be the shortest path. 
                    if (locationOfPreviousWayPoint.find(str_Segs2[i].end) == nullptr)
                    {
                        //Getting the streetsegment
                        get_SS.associate(str_Segs2[i].end, str_Segs2[i]);
                        //Pushing another possible geo coordinate
                        visitedCoords.push(str_Segs2[i].end);
                        //Saving the previous geocoord
                        locationOfPreviousWayPoint.associate(str_Segs2[i].end, str_Segs2[i].start);
                    }
                }
            
        }

    }
    // If path has been found
    if (pathFound)
    {
        // initializing some of the geocoord
        GeoCoord g1, g2(end);

        while (!success(g2, start))
        {
            // Finding the previous geo coord and push it to the list
            // where it will make a path way
            GeoCoord* temp_geocoord = locationOfPreviousWayPoint.find(g2);
            // Finding the street segment
            StreetSegment* temp_SS = get_SS.find(g2);
            g1 = *temp_geocoord;
            StreetSegment SS; 
            SS= *temp_SS;
            totalDistanceTravelled += distanceEarthMiles(g2, g1);
            route.push_front(SS);
            g2 = g1;
        }
        return DELIVERY_SUCCESS;
    }
    else
        return NO_ROUTE;
}

bool PointToPointRouterImpl::success(const GeoCoord gc, const GeoCoord endc) const
{
    if (gc.latitude == endc.latitude && gc.longitude == endc.longitude)
        return true;
    else
        return false;
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
    const GeoCoord& start,
    const GeoCoord& end,
    list<StreetSegment>& route,
    double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
