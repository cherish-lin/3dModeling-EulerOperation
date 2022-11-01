#include <gl/GLUT.H>

#include "HalfEdge.h"
#include "EulerOperator.h"


void CALLBACK PolyLine3DBegin(GLenum type)
{
	glBegin(type);
}

void CALLBACK PolyLine3DVertex(GLdouble* vertex)
{
	const GLdouble* pointer = (GLdouble*)vertex;
	
	glVertex3dv(pointer);
}

void CALLBACK PolyLine3DEnd()
{
	glEnd();
}

GLfloat r = 1, theta = 90, phi = 0;
Face* faces;
Solid *solid;
void Display()
{
	faces = solid->faces;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 5);
	gluLookAt(r * sin(theta * 3.14159 / 180) * sin(phi * 3.14159 / 180), r * cos(theta * 3.14159 / 180), r * sin(theta * 3.14159 / 180) * cos(phi * 3.14159 / 180), 0, 0, 0, 0, 1, 0);
	//cout << "TOP: " << faces->id << endl;
	Face* t = faces;
	double increment = 0.1;
	double r = 0.0;
	while (t != NULL) {
		cout << "this face->id:" << t->id << endl;
		GLUtesselator* tess = gluNewTess();
		gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK*)())PolyLine3DVertex);
		gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK*)())PolyLine3DBegin);
		gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK*)())PolyLine3DEnd);
		r += increment;
		glColor3d(r, 0.5, 0.8);//在此设置颜色  
		gluTessBeginPolygon(tess, NULL);

		Loop* out_lp = t->out_lp;
		Loop* in_lp = t->inner_lp;
		HalfEdge* he1 = out_lp->halfedges;
		Vertex* start = he1->startVertex;
		gluTessBeginContour(tess);
		while (he1->endVertex != start) {
			
			gluTessVertex(tess, he1->startVertex->positions, he1->startVertex->positions);
			for (int i = 0; i < 3; i++) {
				cout << he1->startVertex->positions[i] << ' ';
			}
			cout << endl;
			he1 = he1->next;
		}
		for (int i = 0; i < 3; i++) {
			cout << he1->startVertex->positions[i] << ' ';
		}
		cout << endl;
		cout << "---------------" << endl;
		
		gluTessVertex(tess, he1->startVertex->positions, he1->startVertex->positions);
		gluTessEndContour(tess);



		while (in_lp != NULL) {
			cout << "this has a in_lp " << endl;
			HalfEdge* he2 = in_lp->halfedges;
			Vertex* start2 = he2->startVertex;
			gluTessBeginContour(tess);
			while (he2->endVertex != start2) {

				gluTessVertex(tess, he2->startVertex->positions, he2->startVertex->positions);
				for (int i = 0; i < 3; i++) {
					cout << he2->startVertex->positions[i] << ' ';
				}
				cout << endl;
				he2 = he2->next;
			}
			for (int i = 0; i < 3; i++) {
				cout << he2->startVertex->positions[i] << ' ';
			}
			cout << endl;
			cout << "---------------" << endl;

			gluTessVertex(tess, he2->startVertex->positions, he2->startVertex->positions);
			gluTessEndContour(tess);
			in_lp = in_lp->next;

		}

		gluTessEndPolygon(tess);

		gluDeleteTess(tess);
		
		t = t->next;
		
	}
	
	glutSwapBuffers();
}


void myKeyboard(unsigned char key, int x, int y) {//键盘控制函数，控制视角变化

	switch (key)
	{
	case'A':
	case'a':
		if (phi - 1 > -180)
			phi = phi - 1;
		else phi = 179;
		break;
	case'D':
	case'd':
		if (phi + 1 < 180)
			phi = phi + 1;
		else phi = -179;
		break;
	case'S':
	case's':
		if (theta + 1 < 180)
			theta = theta + 1;
		break;
	case'W':
	case'w':
		if (theta - 1 > 0)
			theta = theta - 1;
		break;
	}
	glutPostRedisplay();
}


vector<Vertex*>vec_V;
vector<Loop*>vec_L;
vector<Face*>vec_S;
void TestEuler() {
	/*double arr[3] = { 0,0,0 };
	double arr1[3] = { 0,0,0 };
	double arr2[3] = { 200,0,0 };
	double arr3[3] = { 0,200,0 };
	double arr4[3] = { 50,50,0 };
	double arr5[3] = { 100,50,0 };
	double arr6[3] = { 50,100,0 };
	double arr7[3] = { 200,200,0 };
	for (int i = 0; i < 3; i++) {
		arr1[i] /= 400;
		arr2[i] /= 400;
		arr3[i] /= 400;
		arr4[i] /= 400;
		arr5[i] /= 400;
		arr6[i] /= 400;
		arr7[i] /= 400;
	}*/
	EulerOperator op;
	double arr[3];
	vector<Face*>deleteFace;
	// 用户输入底面 中每一个环中所有点的位置
	bool flag = true;
	Vertex* startvertex = new Vertex(0, 0, 0);
	Vertex* firstvertex = new Vertex(0, 0, 0);
	while (1) {
		cout << "请输入" << (flag ? "外": "内") << "环中点的个数，以及所有点的位置坐标。" << endl;
		int n; cin >> n;
		if (n == 0) { //底面构造完成，输入扫掠方向和扫掠长度
			cout << "请输入扫掠轴的方向以及扫掠长度" << endl;
			double dist;
			for (int i = 0; i < 3; i++)cin >> arr[i];
			cin >> dist;
			op.sweep(arr, dist);
			for (auto deleteface : deleteFace) {
				op.kfmrh(solid->faces,deleteface);
			}
			break;
		}
		Vertex* now = new Vertex(0,0,0);
		if (flag) { //外环
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < 3; j++) {
					cin >> arr[j];
				}
				if (i == 0) {
					solid = op.mvfs(arr, startvertex);
					now = startvertex;
				}
				else {
					HalfEdge *he = op.mev(now, arr, solid->faces->out_lp);
					now = he->endVertex;
				}
			}
			op.mef(now, startvertex, solid->faces->out_lp, true);

			flag = false;
		}
		else { //内环
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < 3; j++) {
					cin >> arr[j];
				}
				if (i == 0) {
					//cout << startvertex->id << "!!" << endl;
					HalfEdge* he = op.mev(startvertex, arr, solid->faces->out_lp);
					now = he->endVertex;
					firstvertex = now;
				}
				else {
					HalfEdge* he = op.mev(now, arr, solid->faces->out_lp);
					now = he->endVertex;
				}

			}
			Face* deleteface = op.mef(now, firstvertex, solid->faces->out_lp, false);
			//cout << startvertex->id << ' ' << firstvertex->id << endl;
			op.kemr(startvertex, firstvertex, solid->faces->out_lp);

			// 存储多余的面，待扫掠操作完成后进行kfmrh操作来完成模型的构建
			/*Face* deleteface = solid->faces->next;
			while (deleteface->next != NULL) {
				deleteface = deleteface->next;
			}*/
			deleteFace.push_back(deleteface);
		}
	}
/*
3
-0.5 -0.5 0
0.5 -0.5 0
0 0.5 0

3
-0.25 -0.25 0
0.25 -0.25 0
0 0.25 0

0
0 0 -1 0.5

//home model -- 5 edges of outloop,4 holes in it
5
-1 -1 0
1 -1 0
1 0 0
0 1 0
-1 0 0
4
-0.75 -0.75 0
-0.25 -0.75 0
-0.25 -0.25 0
-0.75 -0.25 0
4
0.25 -0.75 0
0.75 -0.75 0
0.75 -0.25 0
0.25 -0.25 0
3
-0.75 0.2 0
-0.25 0.2 0
-0.25 0.7 0
3
0.25 0.2 0
0.75 0.2 0
0.25 0.7 0
0
0 0 -1 0.5
*/
	//while (1) {
	//	//if (solid!= NULL) {

	//	//	Face* now = solid->faces;
	//	//	cout << "现在有" << now->inum << "个内环" << endl;
	//	//	/*if (now->inner_lp) {
	//	//		Loop* lp = now->inner_lp;
	//	//		cout << now->id << ' ' << lp->id << "id" << endl;
	//	//		
	//	//	}*/
	//	//}
	//	cout << "请输入欧拉操作及对应的参数:" << endl;
	//	string str; cin >> str;
	//	if (str == "sweep") {
	//		for (int i = 0; i < 3; i++)cin >> arr[i];//扫掠的方向
	//		//for (int i = 0; i < 3; i++)arr[i] /= 2;
	//		double dist = 1;//延扫掠方向扫掠的距离
	//		cin >> dist;
	//		
	//		op.sweep(arr, dist);
	//		//调用kfmrh，完成模型的构造
	//		for (auto deleteface : deleteFace) {
	//			op.kfmrh(solid->faces,deleteface);
	//		}
	//		//if (solid != NULL) {

	//		//	Face* now = solid->faces;
	//		//	cout << "现在有" << now->inum << "个内环" << endl;
	//		//	/*if (now->inner_lp) {
	//		//		Loop* lp = now->inner_lp;
	//		//		cout << now->id << ' ' << lp->id << "id" << endl;

	//		//	}*/
	//		//}
	//		break;
	//	}
	//	else if (str == "mvfs") {
	//		for (int i = 0; i < 3; i++)cin >> arr[i]; //输入初始顶点的位置
	//		for (int i = 0; i < 3; i++)arr[i] /= 2;
	//		Vertex* start;
	//		solid = op.mvfs(arr, start);
	//	}
	//	else if (str == "mev") {
	//		int u; cin >> u;//旧顶点的编号
	//		for (int i = 0; i < 3; i++)cin >> arr[i]; //新顶点的位置，与给定顶点进行相连，形成一条边
	//		for (int i = 0; i < 3; i++)arr[i] /= 2;
	//		Vertex* start = new Vertex(0,0,0);
	//		vector<Vertex*>vec = op.getVertex_list();
	//		for (auto x : vec) {
	//			if (x->id == u) {
	//				start = x;
	//				break;
	//			}
	//		}
	//		//cout << "end";
	//		op.mev(start, arr, solid->faces->out_lp);
	//	}
	//	else if (str == "mef") { //连接两个编号为u和v的顶点,flag代表新生成的面参不参与扫掠操作？？
	//		Vertex* start = new Vertex(0, 0, 0), * end = new Vertex(0, 0, 0);
	//		int u, v, flag; cin >> u >> v >> flag;
	//		vector<Vertex*>vec = op.getVertex_list();
	//		for (auto x : vec) {
	//			if (x->id == u) {
	//				start = x;
	//				break;
	//			}
	//		}
	//		for (auto x : vec) {
	//			if (x->id == v) {
	//				end = x;
	//				break;
	//			}
	//		}
	//		op.mef(start, end, solid->faces->out_lp, flag);
	//	}
	//	else if (str == "kemr") { //删除给定两个顶点相连的边，构造一个内环
	//		int u, v; cin >> u >> v;
	//		vector<Vertex*>vec = op.getVertex_list();
	//		Vertex* start = new Vertex(0, 0, 0), * end = new Vertex(0, 0, 0);
	//		for (auto x : vec) {
	//			if (x->id == u) {
	//				start = x;
	//				break;
	//			}
	//		}
	//		for (auto x : vec) {
	//			if (x->id == v) {
	//				end = x;
	//				break;
	//			}
	//		}
	//		op.kemr(start, end, solid->faces->out_lp);
	//		Face* deleteface = solid->faces->next;
	//		while (deleteface->next != NULL) {
	//			deleteface = deleteface->next;
	//		}
	//		deleteFace.push_back(deleteface);
	//	}
	//}

// test
	/*
mvfs 0 0 0
mev 0 1 0 0
mev 1 1 1 0
mev 2 0 1 0
mef 3 0 1
mev 0 0.25 0.25 0
mev 4 0.75 0.25 0
mev 5 0.25 0.75 0
mef 6 4 0
kemr 0 4
sweep 0 0 1 0.5



// two hole
mvfs 0 0 0
mev 0 1 0 0
mev 1 1 1 0
mev 2 0 1 0
mef 3 0 1

mev 0 0.25 0.25 0
mev 4 0.75 0.25 0
mev 5 0.25 0.75 0
mef 6 4 0
kemr 0 4

mev 0 0.9 0.9 0
mev 7 0.7 0.9 0
mev 8 0.9 0.7 0
mef 9 7 0
kemr 0 7

sweep 0 0 1 0.5


// two quad holes
mvfs 0 0 0
mev 0 1 0 0
mev 1 1 1 0
mev 2 0 1 0
mef 3 0 1

mev 0 0.25 0.25 0
mev 4 0.5 0.25 0
mev 5 0.5 0.5 0
mev 6 0.25 0.5 0
mef 7 4 0
kemr 0 4

mev 0 0.9 0.9 0
mev 8 0.7 0.9 0
mev 9 0.7 0.7 0
mev 10 0.9 0.7 0
mef 11 8 0
kemr 0 8

sweep 0 0 1 0.5
	*/

}

int main(int argc, char* argv[])
{
	
    TestEuler();
	//GLFWwindow* window;

    glutInit(&argc, argv);

    glutInitWindowSize(640, 480);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("EulerOperation");

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//glViewport(0, 0, 640, 480);
	glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(Display);

    glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 5);
	//glOrtho(0, 640, 0, 480, 0, 400);
	gluLookAt(r * sin(theta * 3.14159 / 180) * sin(phi * 3.14159 / 180), r * cos(theta * 3.14159 / 180), r * sin(theta * 3.14159 / 180) * cos(phi * 3.14159 / 180),
		0, 0, 0, 
		0, 1, 0);
	//gluLookAt()
    //gluOrtho2D(0.0, 200.0, 0.0, 150.0);
	glutKeyboardFunc(myKeyboard); //指定键盘回调函数
    glutMainLoop();
    return 0;
}

