#include "stdafx.h"
#include "NavMesh.h"

using namespace p2t;

const float NAVMESH_POLY_SCALE = 16;

typedef boost::shared_ptr<Point> PointPtr;
typedef std::vector<Point*> PointVector;

TriangleConnection::TriangleConnection(NavMeshTriangle* tri0, int vIdxA, int vIdxB)
	: tri0(tri0), tri1(NULL), vIdxA(vIdxA), vIdxB(vIdxB)
{
}

void TriangleConnection::setTri1(NavMeshTriangle* tri)
{
	tri1 = tri;
	tri0->addConnection(this);
	tri1->addConnection(this);
	distance = tri0->center.distance(tri1->center);
}

const Ogre::Vector2& TriangleConnection::getPointA(const NavMesh* navMesh) const
{
	return navMesh->getVertices()[vIdxA];
}

const Ogre::Vector2& TriangleConnection::getPointB(const NavMesh* navMesh) const
{
	return navMesh->getVertices()[vIdxB];
}

TriangleConnection* TriangleConnection::getConnectionBeetween(const NavMeshTriangle* tri1, const NavMeshTriangle* tri2)
{
	for (auto i = tri1->connections.begin(); i != tri1->connections.end(); i++)
	{
		for (auto j = tri2->connections.begin(); j != tri2->connections.end(); j++)
		{
			if (*i == *j) return *i;
		}
	}
	
	throw std::exception("TriangleConnection::getConnectionBeetween(): connection not found between the given triangles");
}

NavMeshTriangle::NavMeshTriangle(const NavMesh& navMesh, int i0, int i1, int i2)
	: idx0(i0), idx1(i1), idx2(i2)
{
	center = navMesh.getVertex(i0)+navMesh.getVertex(i1)+navMesh.getVertex(i2);
	center /= 3;
}

const Ogre::Vector2& NavMeshTriangle::getPosition(int i, const NavMesh& navMesh)
{
	int idx;
	if (i == 0) idx = idx0;
	else if (i == 1) idx = idx1;
	else idx = idx2;
	return navMesh.getVertices()[idx];
}

bool NavMeshTriangle::containsPoint(const Ogre::Vector2& p, const NavMesh& navMesh)
{
	Vector2 v0 = getPosition(0, navMesh);
	Vector2 v1 = getPosition(1, navMesh);
	Vector2 v2 = getPosition(2, navMesh);
		
	Vector2 p0 = p-v0;
	Vector2 p1 = p-v1;
	Vector2 p2 = p-v2;

	Vector2 d0 = v1-v0;
	Vector2 d1 = v2-v1;
	Vector2 d2 = v0-v2;

	if (d0.crossProduct(p0) > 0) return false;
	if (d1.crossProduct(p1) > 0) return false;
	if (d2.crossProduct(p2) > 0) return false;
	return true;
}

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

float NavmeshGraphFunctionContainer::getHeuristics(NavMeshTriangle*& n1, NavMeshTriangle*& n2)
{
	return n1->center.distance(n2->center);
}

void NavmeshGraphFunctionContainer::getSuccessors(NavMeshTriangle*& n, std::vector<NavMeshTriangle*>* s, std::vector<float>* c)
{
	for (auto i = n->connections.begin(); i != n->connections.end(); i++)
	{
		NavMeshTriangle* succ = (*i)->getOppositeTriangle(n);
		s->push_back(succ);
		c->push_back((*i)->distance);
	}
}

class PointCollection 
{
	//typedef std::map<Point*, int> MapType;
	//MapType map;
	std::map<Point*, int> _points2Indices;
	std::vector<Point*> _points;
	//int _counter;

public:

	PointCollection()
	{
	}

	Point* createPoint(double x, double y)
	{
		Point* point = new Point(x,y);
		_points2Indices[point] = _points.size();
		_points.push_back(point);
		return point;
	}

	int indexOf(Point* point)
	{
		return _points2Indices[point];
	}

	int size() 
	{ 
		return _points.size(); 
	}

	Point* operator[] (int idx) 
	{ 
		return _points[idx];
	}

	~PointCollection()
	{
	}
};

struct TriangleConnectionData
{
	int idx0;
	int idx1;
	
	TriangleConnectionData(int i0, int i1)
		: idx0(i0), idx1(i1)
	{
	}

	bool operator ==(const TriangleConnectionData& other) const
	{
		return idx0 == other.idx0 && idx1 == other.idx1 ||
			idx1 == other.idx0 && idx0 == other.idx1;
	}

	

	//static bool operator==(const TriangleConnectionData& d0, const TriangleConnectionData& d1) { return true;}
	//static bool operator[]
};

class TriangleConnectionDataHasher
{
public:
	size_t operator()(const TriangleConnectionData& data) const
	{
		int a = std::min(data.idx0, data.idx1);
		int b = std::max(data.idx0, data.idx1);
		size_t hash = a*31 + b;
		return hash;
	}
};

//typedef std::unordered_map<TriangleConnectionData, TriangleConnection*, TriangleConnectionDataHasher> ConnectionMapType;
class ConnectionMap 
{
	typedef std::unordered_map<TriangleConnectionData, TriangleConnection*, TriangleConnectionDataHasher> TMap;

	TMap _map;
	std::vector<TriangleConnection*> _connections;

public:

	void addConnection(NavMeshTriangle* triangle, int i0, int i1)
	{
		TriangleConnectionData data = TriangleConnectionData(i0, i1);
		TMap::iterator bitch = _map.find(data);
		if (bitch == _map.end())
		{
			TriangleConnection* conn = new TriangleConnection(triangle, i0, i1);
			_connections.push_back(conn);
			_map[data] = conn;
		}
		else
		{
			bitch->second->setTri1(triangle);
		}
	}

	std::vector<TriangleConnection*> getConnections() const
	{
		std::vector<TriangleConnection*> result;
		for (auto i = _connections.begin(); i != _connections.end(); i++)
		{
			if ((*i)->tri1 != NULL)
				result.push_back(*i);
		}
		return result;
	}
};

void NavMesh::init(const Ogre::Vector2& mapSize)
{
	Clipper c;
	c.AddPolygons(_polygons, ptSubject);
	
	Polygons result;

	c.Execute(ctUnion, result, pftNonZero, pftNonZero);
	
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

	//Point

	for (int i = 0; i < points.size(); i++)
	{
		Point* p = points[i];
		_vertices.push_back(Ogre::Vector2(p->x, p->y));
	}

	for (auto i = triangles.begin(); i != triangles.end(); i++)
	{
		Point* p0 = (*i)->GetPoint(0);
		Point* p1 = (*i)->GetPoint(1);
		Point* p2 = (*i)->GetPoint(2);

		int idx0 = points.indexOf(p0);
		int idx1 = points.indexOf(p1);
		int idx2 = points.indexOf(p2);
		_triangles.push_back(NavMeshTriangle(*this, idx2, idx1, idx0));
	}

	delete cdt;

	ConnectionMap connectionMap;

	for (auto i = _triangles.begin(); i != _triangles.end(); i++)
	{
		NavMeshTriangle& tri = *i;
		
		connectionMap.addConnection(&tri, tri.idx0, tri.idx1);
		connectionMap.addConnection(&tri, tri.idx1, tri.idx2);
		connectionMap.addConnection(&tri, tri.idx2, tri.idx0);
		//NavMeshTriangle* apad = kalauzlany->second->tri0;
	}

	_triangleConnections = connectionMap.getConnections();

	/*
	Vector2 p = Vector2(10,5);
	Vector2 v0 = Vector2(0,0);
	Vector2 v1 = Vector2(10,0);
	Vector2 v2 = Vector2(5,10);
		
	Vector2 p0 = p-v0;
	Vector2 p1 = p-v1;
	Vector2 p2 = p-v2;

	Vector2 d0 = v1-v0;
	Vector2 d1 = v2-v1;
	Vector2 d2 = v0-v2;

	bool contains = true;
	float c0 = d0.crossProduct(p0);
	float c1 = d1.crossProduct(p1);
	float c2 = d2.crossProduct(p2);

	if (c0 < 0) contains = false;
	if (c1 < 0) contains = false;
	if (c2 < 0) contains = false;
	
	if (contains)
		OutputDebugString("jeeee\n");
	else
		OutputDebugString("Neeeee\n");*/
}

std::vector<NavMeshTriangle*> NavMesh::findPathBetween(const NavMeshTriangle* a, const NavMeshTriangle* b)
{
	GenericSearchGraphDescriptor<NavMeshTriangle*, float> graph;
	graph.func_container = &_functionContainer;
	graph.hashTableSize = 128;
	graph.SeedNode = const_cast<NavMeshTriangle*>(a);
	graph.TargetNode = const_cast<NavMeshTriangle*>(b);

	A_star_planner<NavMeshTriangle*, float> planner;

	planner.init(graph);
	planner.plan();

	std::vector<std::vector<NavMeshTriangle*>> paths = planner.getPlannedPaths();
	return paths[0];
	//std::vector<NavMeshTriangle*> result;
}
