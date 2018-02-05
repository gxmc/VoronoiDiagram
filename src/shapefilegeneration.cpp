#include <ogrsf_frmts.h>
#include "AWDNdbRepository.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "Voronoi.h"
#include "VPoint.h"

using namespace AWDNdb;
using namespace std;
using namespace vor;

int main()
{
  // Get Meta from AWDNdb
  /*
  AWDNdbRepository dbr;
  map<string,string> input;
  vector<map<string, string> > data;

  input.insert(map<string,string>::value_type("read","meta"));
  input.insert(map<string,string>::value_type("table","meta"));
  input.insert(map<string,string>::value_type("selects","name;latitude;longitude"));
  input.insert(map<string,string>::value_type("wheres","state"));
  input.insert(map<string,string>::value_type("wvalues","ND"));
  data = dbr.read(input);
  */

  // create POINT shapefile
  const char *pszDriverName = "ESRI Shapefile";
  /*
  OGRSFDriver *poDriver;

  OGRRegisterAll();

  poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
  if(poDriver == NULL)
    {
      printf( "%s driver not available.\n", pszDriverName );
      exit( 1 );
    }

  OGRDataSource *poDS;

  poDS = poDriver->CreateDataSource( "/data/development/junkdrawer/NDstations.shp", NULL );
  if( poDS == NULL )
    {
      printf( "Creation of output file failed.\n" );
      exit( 1 );
    }

  OGRLayer *poLayer;

  poLayer = poDS->CreateLayer( "point_out", NULL, wkbPoint, NULL );
  if( poLayer == NULL )
    {
      printf( "Layer creation failed.\n" );
      exit( 1 );
    }

  OGRFieldDefn oField( "Name", OFTString );

  oField.SetWidth(100);

  if( poLayer->CreateField( &oField ) != OGRERR_NONE )
    {
      printf( "Creating Name field failed.\n" );
      exit( 1 );
    }

  for(auto a : data)
    {
      OGRFeature *poFeature;

      poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
      poFeature->SetField( "Name", a["name"].c_str() );

      OGRPoint pt;

      pt.setY( stof(a["latitude"]) );
      pt.setX( stof(a["longitude"]) );

      poFeature->SetGeometry( &pt );

      if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
	{
	  printf( "Failed to create feature in shapefile.\n" );
	  exit( 1 );
	}

      OGRFeature::DestroyFeature( poFeature );
    }

  OGRDataSource::DestroyDataSource( poDS );
  */

  // Voronoi
  //replacement for AWDNdb lat,lon
  std::vector<double> sitesLat{48.171,46.79,48.38,46.88,48.821,48.82,46.20,45.98,48.47,47.51,48.761,48.77,47.44,48.05,48.81,47.18,46.901,47.34,46.32,48.63,46.54,46.90,46.75,47.53,48.30,46.08,47.21,47.71,48.41,47.84,47.731,47.30,46.01,47.35,48.16,46.721,48.17,46.91,48.12,48.762,46.73,46.33,46.44,46.77,46.56,47.50,47.69,46.43,48.02,48.18,48.76,46.38,47.73,46.07,47.811,47.23,47.86,47.00,47.17,48.84,48.27,48.351,48.60,46.72,46.881,48.35,47.571,47.57,48.97,47.81,48.13,46.26,46.22,47.368352,48.430824,48.54651,46.442317};
  std::vector<double> sitesLon{-99.65,-103.97,-101.82,-100.99,-100.76,-102.24,-103.47,-97.82,-99.17,-99.13,-97.75,-103.20,-98.18,-98.61,-103.31,-98.14,-102.81,-102.63,-98.77,-99.21,-97.14,-96.81,-97.91,-97.85,-97.60,-100.68,-97.43,-101.68,-97.19,-97.07,-99.92,-101.68,-102.64,-96.92,-103.11,-97.44,-97.71,-98.93,-100.63,-98.35,-97.24,-100.28,-97.72,-100.91,-98.33,-97.26,-98.62,-97.64,-98.17,-101.29,-101.53,-102.32,-97.99,-98.09,-98.31,-97.79,-101.96,-97.12,-99.80,-99.61,-102.48,-100.06,-97.49,-99.45,-99.58,-100.77,-101.44,-100.92,-98.36,-103.23,-103.74,-99.67,-96.89,-97.640306,-98.514013,-103.471843,-101.37375};

  double w = 1000;
  Voronoi * v;
  Vertices * ver;
  Vertices * dir;
  Edges * edg;

  v = new Voronoi();
  ver = new Vertices();
  dir = new Vertices();

  /*1
  for(auto a : data)
    {
      // Initialize the points and store them in a list
      ver->push_back(new VPoint( w * stof(a["longitude"]), w * stof(a["latitude"])));
      //
      dir->push_back(new VPoint( stof(a["longitude"]), stof(a["latitude"])));
    }
  */

  int count = 0;
  for (auto a : sitesLat)
    {
      cout << a << "," << sitesLon[count] << endl;
      ver->push_back(new VPoint(w * sitesLon[count], w * a));
      dir->push_back(new VPoint(sitesLon[count], a));
      count++;
    }

  edg = v->GetEdges(ver, w, w);

  //create polygon shapefile
  OGRSFDriver *lsDriver;

  OGRRegisterAll();

  lsDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
  if(lsDriver == NULL)
    {
      printf( "%s driver not available.\n", pszDriverName );
      exit( 1 );
    }

  OGRDataSource *lsDS;

  lsDS = lsDriver->CreateDataSource( "/data/development/junkdrawer/NDedges.shp", NULL );
  if( lsDS == NULL )
    {
      printf( "Creation of output file failed.\n" );
      exit( 1 );
    }

  OGRLayer *lsLayer;

  lsLayer = lsDS->CreateLayer( "edges", NULL, wkbLineString, NULL );
  if( lsLayer == NULL )
    {
      printf( "Layer creation failed.\n" );
      exit( 1 );
    }

  for(vor::Edges::iterator i = edg->begin(); i!= edg->end(); ++i)
    {
      cout << (*i)->start->x/w << "," << (*i)->start->y/w << " " << (*i)->end->x/w << "," << (*i)->end->y/w << endl;

      if (to_string((*i)->start->x/w) != "-nan" && to_string((*i)->start->y/w) != "-nan" && to_string((*i)->end->x/w) != "-nan" && to_string((*i)->end->y/w) != "-nan")
	{
	  cout << "HERE" << endl;
	  OGRFeature *lsFeature;

	  lsFeature = OGRFeature::CreateFeature( lsLayer->GetLayerDefn() );
	  OGRLineString ls;
	  OGRPoint pt;

	  pt.setX( (*i)->start->x/w );
	  pt.setY( (*i)->start->y/w );

	  ls.setPoint(0, &pt);

	  pt.setX( (*i)->end->x/w );
	  pt.setY( (*i)->end->y/w );

	  ls.setPoint(1, &pt);
	  lsFeature->SetGeometry( &ls );

	  if( lsLayer->CreateFeature( lsFeature ) != OGRERR_NONE )
	    {
	      printf( "Failed to create feature in shapefile.\n" );
	      exit( 1 );
	    }

	  OGRFeature::DestroyFeature( lsFeature );
	}
    }
  OGRDataSource::DestroyDataSource( lsDS );
}
