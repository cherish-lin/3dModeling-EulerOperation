#include "EulerOperator.h"

HalfEdge* EulerOperator::mev(Vertex* sv, double point[3], Loop* loop)
{
	Solid* solid = loop->face->solid;
	Edge* edge = new Edge();
	HalfEdge* half_l = new HalfEdge();
	HalfEdge* half_r = new HalfEdge();
	Vertex* ev = new Vertex(point[0], point[1], point[2]);

	ev->id = solid->vnum;
	v_list.push_back(ev);
	solid->vnum += 1;
	half_l->startVertex = sv;
	half_l->endVertex = ev;
	half_r->startVertex = ev;
	half_r->endVertex = sv;

	edge->l = half_l;
	edge->r = half_r;
	half_l->edge = edge;
	half_r->edge = edge;

	half_r->brother = half_l;
	half_l->brother = half_r;

	half_l->lp = loop;
	half_r->lp = loop;

	if (loop->halfedges == NULL)
	{
		half_l->next = half_r;
		half_r->next = half_l;

		half_l->pre = half_r;
		half_r->pre = half_l;
		loop->halfedges = half_l;
	}
	else
	{
		HalfEdge* thalf = loop->halfedges;
		while (thalf->endVertex != sv) {
			thalf = thalf->next;
			//cout << "begin:" << thalf->endVertex->id << ' ' << sv->id << "!!!" << endl;
		}
		//cout << "end" << endl;
		half_r->next = thalf->next;
		thalf->next->pre = half_r;
		thalf->next = half_l;
		half_l->pre = thalf;
		half_l->next = half_r;
		half_r->pre = half_l;
	}
	
	addEdgeIntoSolid(edge, solid);
	return half_l;
}

Face* EulerOperator::mef(Vertex* sv, Vertex* ev, Loop* loop, bool flag)
{
	Solid* solid = loop->face->solid;
	Edge* edge = new Edge();
	HalfEdge* half_l = new HalfEdge();
	HalfEdge* half_r = new HalfEdge();
	Loop* newLoop = new Loop();

	half_l->startVertex = sv;
	half_l->endVertex = ev;
	half_r->startVertex = ev;
	half_r->endVertex = sv;

	half_r->brother = half_l;
	half_l->brother = half_r;

	half_l->edge = edge;
	half_r->edge = edge;
	edge->l = half_l;
	edge->r = half_r;


	HalfEdge* thalf = loop->halfedges;
	HalfEdge* tmpa, * tmpb, * tmpc;
	while (thalf->endVertex != sv)thalf = thalf->next;
	tmpa = thalf;

	while (thalf->endVertex != ev)thalf = thalf->next;
	tmpb = thalf;

	thalf = thalf->next;
	while (thalf->endVertex != ev)thalf = thalf->next;
	tmpc = thalf;

	half_r->next = tmpa->next;
	tmpa->next->pre = half_r;
	tmpa->next = half_l;
	half_l->pre = tmpa;

	half_l->next = tmpb->next;
	tmpb->next->pre = half_l;
	tmpb->next = half_r;
	half_r->pre = tmpb;
	loop->halfedges = half_l;
	newLoop->halfedges = half_r;
	half_l->lp = loop;
	half_r->lp = newLoop;

	Face* face = new Face();

	newLoop->id = solid->lnum;
	solid->lnum += 1;
	l_list.push_back(newLoop);
	addFaceIntoSolid(face, solid);

	addLoopIntoFace(newLoop, face);

	if (tmpc == tmpb)
	{
		if (flag)
		{
			sweep_list.push_back(half_l->lp->face);
		}
	}
	else
	{
		sweep_list.push_back(half_r->lp->face);
	}

	addEdgeIntoSolid(edge, solid);

	return face;
}

Solid* EulerOperator::mvfs(double point[3], Vertex*& vertex)
{
	Solid* solid = new Solid();
	Face* face = new Face();
	Loop* out_lp = new Loop();
	vertex = new Vertex(point[0], point[1], point[2]);

	vertex->id = solid->vnum;
	out_lp->id = solid->lnum;
	face->id = solid->fnum;

	l_list.push_back(out_lp);
	
	v_list.push_back(vertex);

	solid->vnum += 1;
	solid->fnum += 1;
	solid->lnum += 1;


	solid->faces = face;
	face->solid = solid;

	face->out_lp = out_lp;
	out_lp->face = face;

	return solid;

}

Loop* EulerOperator::kemr(Vertex* sv, Vertex* ev, Loop* loop)
{
	HalfEdge* tmpa, * tmpb, * hal;
	Face* face = loop->face;
	Loop* inlp = new Loop();
	Solid* solid = loop->face->solid;

	hal = loop->halfedges;

	while (hal->startVertex != sv || hal->endVertex != ev)hal = hal->next;
	tmpa = hal;

	while (hal->startVertex != ev || hal->endVertex != sv) {
		//cout << hal->startVertex->id << ' ' << hal->endVertex->id << endl;
		hal = hal->next;
	}
	tmpb = hal;
	tmpb->pre->next = tmpa->next;
	tmpa->pre->next = tmpb->next;

	loop->face->solid->faces->out_lp->halfedges = tmpa->pre;

	inlp->halfedges = tmpb->pre;
	tmpb->pre->lp = inlp;

	inlp->id = solid->lnum;
	solid->lnum += 1;
	l_list.push_back(inlp);

	addLoopIntoFace(inlp, tmpa->pre->brother->lp->face);
	delete tmpa;
	delete tmpb;

	return NULL;
}

void EulerOperator::kfmrh(Face* fa, Face* fb)
{
	Loop* loop = fb->out_lp;
	addLoopIntoFace(loop, fa);
	fa->solid->lnum -= 1;
	fa->solid->fnum -= 1;

	Solid* solid = fa->solid;
	Face* face = solid->faces;
	if (face == fb)
	{
		solid->faces = face->next;
	}
	else
	{
		Face* tf = face;
		while (face != fb && face != NULL)
		{
			tf = face;
			face = face->next;
		}
		tf->next = face->next;
	}
	delete fb;
}

void EulerOperator::sweep(double dir[3], double dist)
{
	Vertex* startv, * nextv, * upv, * upprev;
	HalfEdge* he, * suphe, * uphe;
	double point[3];

	vector<Face*>::iterator it;
	for (it = sweep_list.begin(); it != sweep_list.end(); it++)
	{
		Loop* loop = (*it)->out_lp;
		he = loop->halfedges;
		startv = he->startVertex;
		point[0] = startv->positions[0] + dist * dir[0];
		point[1] = startv->positions[1] + dist * dir[1];
		point[2] = startv->positions[2] + dist * dir[2];

		suphe = mev(startv, point, loop);
		upprev = suphe->endVertex;
		he = he->next;
		nextv = he->startVertex;

		Loop* lp = loop;
		//cout << "in sweep:";
		while (nextv != startv)
		{
			//cout << startv->id << ' ' << nextv->id << endl;
			point[0] = nextv->positions[0] + dist * dir[0];
			point[1] = nextv->positions[1] + dist * dir[1];
			point[2] = nextv->positions[2] + dist * dir[2];
			uphe = mev(nextv, point, lp);
			upv = uphe->endVertex;

			mef(upprev, upv, loop, false);

			upprev = upv;
			he = he->next;
			nextv = he->startVertex;
		}
		//cout << "----" << upprev->id << ' ' << suphe->endVertex->id << endl;
		mef(upprev, suphe->endVertex, loop, false);
		//kfmrh(loop->face,)
	}

}

void EulerOperator::addEdgeIntoSolid(Edge* edge, Solid*& solid)
{
	Edge* te = solid->edges;

	if (te == NULL)solid->edges = edge;
	else {
		while (te->next != NULL)te = te->next;
		te->next = edge;
		edge->pre = te;
	}

}

void EulerOperator::addFaceIntoSolid(Face* face, Solid*& solid)
{
	Face* tface = solid->faces;
	if (tface == NULL) //第一个面
	{
		solid->faces = face;
	}
	else //把当前面插在面链表的最后
	{
		while (tface->next != NULL)tface = tface->next;
		tface->next = face;
		face->pre = tface;
	}
	face->solid = solid;

	face->id = solid->fnum;

	solid->fnum += 1;
}

void EulerOperator::addLoopIntoFace(Loop* loop, Face*& face)
{
	loop->face = face;
	// 只有一个外环，有多个内环
	if (face->out_lp == NULL) //外环
	{
		face->out_lp = loop;
	}
	else //内环
	{
		//cout << "in loop" << endl;
		Loop* lp = face->inner_lp;
		if (lp == NULL)face->inner_lp = loop;
		else
		{
			while (lp->next != NULL)lp = lp->next;
			lp->next = loop;
			loop->pre = lp;
		}
		face->inum += 1;
		//cout << face->id << ' ' << face->inum << "!!!!!!!!" << endl;
	}

}
