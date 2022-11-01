#pragma once
#include <vector>
#include "HalfEdge.h"
using namespace std;
class EulerOperator {
private:
	vector<Vertex*> v_list;
	vector<Loop*> l_list;
	vector<Face*> sweep_list;
public:
	EulerOperator() {
		v_list.clear();
		sweep_list.clear();
		l_list.clear();
	}
	~EulerOperator() {}
	vector<Vertex*> getVertex_list() { return v_list; }
	vector<Face*> getSweep_list() { return sweep_list; }
	vector<Loop*> getLoop_list() { return l_list; }
	// euler operation
	HalfEdge* mev(Vertex* sv, double point[3], Loop* loop);
	Face* mef(Vertex* sv, Vertex* ev, Loop* loop, bool mark);
	Solid* mvfs(double point[3], Vertex*& vertex);
	Loop* kemr(Vertex* sv, Vertex* ev, Loop* loop);
	void kfmrh(Face* fa, Face* fb);
	// 扫掠操作
	void sweep(double dir[3], double dist);

	void addEdgeIntoSolid(Edge* edge, Solid*& solid);
	void addFaceIntoSolid(Face* face, Solid*& solid);
	void addLoopIntoFace(Loop* loop, Face*& face);
};
