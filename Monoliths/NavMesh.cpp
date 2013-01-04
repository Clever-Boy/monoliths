#include "stdafx.h"
#include "NavMesh.h"

using namespace p2t;

const float NAVMESH_POLY_SCALE = 1;

typedef boost::shared_ptr<Point> PointPtr;
typedef std::vector<Point*> PointVector;

void NavMesh::addPolygon(Ogre::Vector2* polygonVertices, int vertexCount)
{
	ClipperLib::Polygon polygon;
	for (int i = 0; i < vertexCount; i++)
	{
		Vector2 v = polygonVertices[i]*NAVMESH_POLY_SCALE;
		v.y *= -1;
		IntPoint p = IntPoint((long64)v.x, (long64)v.y);
		polygon.push_back(p);
	}
	_polygons.push_back(polygon);
}

class PointCollection 
{
	typedef std::map<Point*, int> MapType;
	MapType map;
	int _counter;

public:

	PointCollection()
		: _counter(0)
	{
	}

	Point* createPoint(double x, double y)
	{
		Point* point = new Point(x,y);
		map[point] = _counter++;
		return point;
	}

	int indexOf(Point* point)
	{
		return map[point];
	}

	MapType::iterator begin()
	{
		return map.begin();
	}

	MapType::iterator end()
	{
		return map.end();
	}

	~PointCollection()
	{
	}
};

void NavMesh::init(const Ogre::Vector2& mapSize)
{
	Clipper c;
	c.AddPolygons(_polygons, ptSubject);
	
	Polygons result;

	c.Execute(ctUnion, result);
	
	PointVector contour;
	
	float a = mapSize.x*0.5;
	float b = mapSize.y*0.5;
	
	PointCollection points;

	contour.push_back(points.createPoint(a,b));
	contour.push_back(points.createPoint(-a,b));
	contour.push_back(points.createPoint(-a,-b));
	contour.push_back(points.createPoint(a,-b));

	//return;

	CDT* cdt = new CDT(contour);

	for (auto i = result.begin(); i != result.end(); i++)
	{
		if (Area(*i) > 0)
		{
			PointVector hole;
			for (auto j = i->begin(); j != i->end(); j++)
			{
				Point* p = points.createPoint(j->X, j->Y);
				(*p)*=(1/NAVMESH_POLY_SCALE);
				p->y*=-1;
				hole.push_back(p);
			}
			std::reverse(hole.begin(), hole.end());
			cdt->AddHole(hole);
		}
	}

	cdt->Triangulate();
	std::vector<Triangle*> triangles = cdt->GetTriangles();

	for (auto i = points.begin(); i != points.end(); i++)
	{
		Point& p = *(i->first);
		_vertices.push_back(Ogre::Vector2(p.x, p.y));
	}

	for (auto i = triangles.begin(); i != triangles.end(); i++)
	{
		int idx0 = points.indexOf((*i)->GetPoint(0));
		int idx1 = points.indexOf((*i)->GetPoint(1));
		int idx2 = points.indexOf((*i)->GetPoint(2));
		_triangles.push_back(idx0);
		_triangles.push_back(idx1);
		_triangles.push_back(idx2);
	}

	delete cdt;
	//ClipperLib::Polygon

	//Point* contour = new Point[

	
}