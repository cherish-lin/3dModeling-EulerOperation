#pragma once
#include<iostream>

struct Vertex;
struct HalfEdge;
struct Edge;
struct Loop;
struct Face;
struct Solid;

struct Vertex {
	int id;
	double positions[3]; //每个点的位置
	Vertex* pre; //前驱顶点
	Vertex* next; //后继顶点
	Vertex(double x, double y, double z) :id(0), next(NULL), pre(NULL) {
		positions[0] = x;
		positions[1] = y;
		positions[2] = z;
	}
};

struct HalfEdge {
	Edge* edge; //这条半边属于哪一条边
	Vertex* startVertex; //半边的起始顶点
	Vertex* endVertex; //半边的终止顶点
	Loop* lp;
	HalfEdge* pre;//上一条半边
	HalfEdge* next; //下一条半边
	HalfEdge* brother;

	HalfEdge() :edge(NULL), startVertex(NULL), endVertex(NULL), lp(NULL), pre(NULL), next(NULL), brother(NULL) {}
};

struct Edge {
	HalfEdge* l; //左半边
	HalfEdge* r; //右半边
	Edge* pre;
	Edge* next;
	Edge() :l(NULL), r(NULL), pre(NULL), next(NULL) {}
};

struct Loop {
	int id;
	HalfEdge* halfedges;//组成loop的所有半边
	Face* face;
	Loop* pre;
	Loop* next;
	Loop() :id(0), halfedges(NULL), face(NULL), pre(NULL), next(NULL) {}
};

struct Face {
	int id;
	Solid* solid; //该面属于的体
	Loop* out_lp;
	Loop* inner_lp;
	Face* pre;
	Face* next;
	int inum;
	Face() :id(0), solid(NULL), out_lp(NULL), inner_lp(NULL), pre(NULL), next(NULL), inum(0) {}
};

struct Solid {
	int id;
	Face* faces; //组成该体的所有面
	Edge* edges; //组成该体的所有边
	Solid* pre;
	Solid* next;
	int vnum; //vertex number
	int fnum; //face number
	int lnum; //loop number
	Solid() :id(0), faces(NULL), edges(NULL), pre(NULL), next(NULL), vnum(0), fnum(0), lnum(0) {}
};
