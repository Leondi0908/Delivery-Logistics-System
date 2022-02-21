#include "provided.h"
#include <iomanip>
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* str;
	string proceed_Angle(double angle) const;
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
    str = sm;
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
	vector<DeliveryRequest> temp_deliveries = deliveries;
	DeliveryOptimizer deliv_opt(str);
	double d1(0), d2(0);
	deliv_opt.optimizeDeliveryOrder(depot, temp_deliveries, d1, d2);
	
	cout << "OLD distance: " << d1 << endl;
	cout << "NEW distance: " << d2 << endl;

	GeoCoord start(depot), end;
	double total_distance(0.0);
	for (int i = 0; i <= temp_deliveries.size(); i++)
	{
		if (i == temp_deliveries.size())
			end = depot;
		else
			end = temp_deliveries[i].location;
		PointToPointRouter ppr(str);
		list<StreetSegment> route;
		DeliveryCommand dc;
		if (ppr.generatePointToPointRoute(start, end, route, total_distance) == 0)
		{
			std::cout << std::fixed;
			std::cout << std::setprecision(2);
			for(list<StreetSegment>::iterator it = route.begin(); it != route.end();)
			{
					
				// Finding the next iterator address
				list<StreetSegment>::iterator it3 = it;
				// If we find the same geo coordinate
				// Going out from iterator loop
				if (it->end == end)
					break;
				else
					it3++;
				//cout << it->end.latitudeText << "   " << it->end.longitudeText << endl;
				list<StreetSegment>::iterator it2 = it3;

				// Set found the same address as false
				bool found_Address(false);
				bool threeDiffSegs(false);

				// Get the angle
				double angle = angleOfLine(*it);
				// If the iterator not the end so we can add it
				if (it3->end != end)
					it3++;
				//cout << it3->end.latitudeText << "   " << it3->end.longitudeText << "    " << end.latitudeText << "  " << end.longitudeText << endl;
				// Comparing the different street segment with the current iterator with the next one
				// And comparing the next one with another next one iterator. If all the address are different 
				// We will set the boolean expresion into true
				if (it->name != it2->name && it2->name != it3->name || it->name != it2->name && it2->end == end)
				{
					threeDiffSegs = true;					
				}


				// Doing a loop for the temporary iterator   
				while (it2 != route.end() && it->name == it2->name)
				{

					found_Address = true;
					// End of the route
					if (it2->end == end)
						break;
					// Incrementing the temporary iterator 
					// If the name in current street segment is the same with the next street segment
					it2++;
				}

				total_distance = 0.0;
				// Merging the same address 
				if (found_Address)
				{
					if(it2->end != end)
						it2--;
					total_distance = distanceEarthMiles(it->start, it2->end);
					dc.initAsProceedCommand(proceed_Angle(angle), it->name, total_distance);
					commands.push_back(dc);
					it = it2;
				}
				else
				{
					// Calculating between 2 street segment
					double angleof2Streets = angleBetween2Lines(*it, *it2);					

					if (angleof2Streets < 1 || angleof2Streets > 359)
					{
						total_distance = distanceEarthMiles(it->start, it2->end);
						dc.initAsProceedCommand(proceed_Angle(angle), it2->name, total_distance);
						commands.push_back(dc);
					}
					else if (angleof2Streets >= 1 && angleof2Streets < 180)
					{
						dc.initAsTurnCommand("left", it2->name);
						commands.push_back(dc);
					}
					else if (angleof2Streets >= 180 && angleof2Streets <= 359)
					{
						dc.initAsTurnCommand("right", it2->name);
						commands.push_back(dc);
					}//End of if statement
					if (threeDiffSegs)
					{
						total_distance = distanceEarthMiles(it2->start, it2->end);
						dc.initAsProceedCommand(proceed_Angle(angleOfLine(*it2)), it2->name, total_distance);
						commands.push_back(dc);
					}
					it++;
				}//End of if found statement
				totalDistanceTravelled += total_distance;
			}//End of the for loop
			start = end;
			if (i != deliveries.size())
			{
				dc.initAsDeliverCommand(deliveries[i].item);
				commands.push_back(dc);
			}

		}
		else if (ppr.generatePointToPointRoute(start, end, route, total_distance) == 1)
			return NO_ROUTE;
		else
			return BAD_COORD;
	}//End of the loop delivery request 
}

string DeliveryPlannerImpl::proceed_Angle(double angle) const
{
	if (angle >= 0 && angle < 22.5)
		return "east";
	else if (angle >= 22.5 && angle < 67.5)
		return "northeast";
	else if (angle >= 67.5 && angle < 112.5)
		return "north";
	else if (angle >= 112.5 && angle < 157.5)
		return "northwest";
	else if (angle >= 157.5 && angle < 202.5)
		return "west";
	else if (angle >= 202.5 && angle < 247.5)
		return "southwest";
	else if (angle >= 247.5 && angle < 292.5)
		return "south";
	else if (angle >= 292.5 && angle < 337.5)
		return "southeast";
	else if (angle >= 337.5)
		return "east";
}
//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
