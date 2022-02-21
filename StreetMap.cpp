#include "provided.h"
#include "ExpandableHashMap.h"
#include <string>
#include <cstring>
#include <vector>
#include <functional>
#include <fstream>

using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}
unsigned int hasher(const string& g)
{
    return std::hash<string>()(g);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment>> segs_Map;
    bool checkAlpha(string geoFile);
    void separateSegment(string& lat1, string& long1, string& lat2, string& long2, string geoFile);
};

StreetMapImpl::StreetMapImpl()
{

}

StreetMapImpl::~StreetMapImpl()
{

}

bool StreetMapImpl::load(string mapFile)
{
    ifstream infile(mapFile);
    string geoFile;
    if (!infile) {
        cerr << "Error: Cannot open " << mapFile << endl;
        return false;
    }
    string address;

    while (getline(infile, geoFile))
    {
        if (checkAlpha(geoFile))
            address = geoFile;
        else
        {
            vector<StreetSegment>* seg_ptr;           // Vector of pointer
            vector<StreetSegment> f_Segs, r_Segs; // Initializing vector forward segment and reverse segment
            // Ignoring the coordinate counter
            if (geoFile.length() > 3)
            {
                string lat1, long1, lat2, long2;
                separateSegment(lat1, long1, lat2, long2, geoFile);
                GeoCoord g1(lat1, long1),
                    g2(lat2, long2);
                StreetSegment forward_SS(g1, g2, address),
                    reverse_SS(g2, g1, address);
                // Extracting the start GeoCoord
                seg_ptr = segs_Map.find(g1);
                // Forward add item
                if (seg_ptr == nullptr)
                {
                    f_Segs.push_back(forward_SS);
                    segs_Map.associate(g1, f_Segs);
                }
                else
                    seg_ptr->push_back(forward_SS);

                // Extracting the ending GeoCoord
                seg_ptr = segs_Map.find(g2);
                // Reverse add item
                if (seg_ptr != nullptr)
                    seg_ptr->push_back(reverse_SS);
                else
                {
                    r_Segs.push_back(reverse_SS);
                    segs_Map.associate(g2, r_Segs);
                }

            }
        }
    }
    return true;  // Delete this line and implement this function correctly
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    const vector<StreetSegment>* seg_ptr = segs_Map.find(gc);
    if (seg_ptr != nullptr)
    {
        segs.clear();
        segs = *seg_ptr;
        return true;
    }
    return false;
}

bool StreetMapImpl::checkAlpha(string geoFile)
{
    for (int i = 0; i < geoFile.length(); i++)
    {
        if (isalpha(geoFile[i]))
            return true;
    }
    return false;
}
void StreetMapImpl::separateSegment(string& lat1, string& long1, string& lat2, string& long2, string geoFile)
{
    string str1, str2;
    int spcCntr(0);
    for (int i = 0; i < geoFile.length(); i++)
    {
        if (isspace(geoFile[i]))
        {
            spcCntr++;
            if (spcCntr == 2)
            {
                lat1 = str1;
                long1 = str2;
                str1 = str2 = "";
            }
        }
        else
        {
            if (spcCntr == 0 || spcCntr == 2)
                str1 += geoFile[i];
            else
                str2 += geoFile[i];
        }
    }
    lat2 = str1;
    long2 = str2;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    return m_impl->getSegmentsThatStartWith(gc, segs);
}

