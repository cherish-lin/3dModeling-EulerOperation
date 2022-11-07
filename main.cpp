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
		//cout << "this face->id:" << t->id << endl;
		GLUtesselator* tess = gluNewTess();
		gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK*)())PolyLine3DVertex);
		gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK*)())PolyLine3DBegin);
		gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK*)())PolyLine3DEnd);
		r += increment;
		if (r > 1.0)r = 1.0;
		glColor3d(r, 0.4, 0.7);//在此设置颜色  
		gluTessBeginPolygon(tess, NULL);

		Loop* out_lp = t->out_lp;
		Loop* in_lp = t->inner_lp;
		HalfEdge* he1 = out_lp->halfedges;
		Vertex* start = he1->startVertex;
		gluTessBeginContour(tess);
		while (he1->endVertex != start) {
			
			gluTessVertex(tess, he1->startVertex->positions, he1->startVertex->positions);
			/*for (int i = 0; i < 3; i++) {
				cout << he1->startVertex->positions[i] << ' ';
			}
			cout << endl;*/
			he1 = he1->next;
		}
		/*for (int i = 0; i < 3; i++) {
			cout << he1->startVertex->positions[i] << ' ';
		}
		cout << endl;
		cout << "---------------" << endl;*/
		
		gluTessVertex(tess, he1->startVertex->positions, he1->startVertex->positions);
		gluTessEndContour(tess);



		while (in_lp != NULL) {
			//cout << "this has a in_lp " << endl;
			HalfEdge* he2 = in_lp->halfedges;
			Vertex* start2 = he2->startVertex;
			gluTessBeginContour(tess);
			while (he2->endVertex != start2) {

				gluTessVertex(tess, he2->startVertex->positions, he2->startVertex->positions);
				/*for (int i = 0; i < 3; i++) {
					cout << he2->startVertex->positions[i] << ' ';
				}
				cout << endl;*/
				he2 = he2->next;
			}
			/*for (int i = 0; i < 3; i++) {
				cout << he2->startVertex->positions[i] << ' ';
			}
			cout << endl;
			cout << "---------------" << endl;*/

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
	case's': //这里只能上下反转180度
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
			double mon = sqrt(arr[0] * arr[0] + arr[1] * arr[1] + arr[2] * arr[2]);
			for (int i = 0; i < 3; i++) arr[i] /= mon; //将扫掠方向单位化

			cin >> dist;
			op.sweep(arr, dist);
			//对多余的面采用kfmrh操作，生成内环
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
					solid = op.mvfs(arr, startvertex); //构建模型的第一部操作
					now = startvertex;
				}
				else {
					HalfEdge *he = op.mev(now, arr, solid->faces->out_lp); //对外环的每一个点，依次进行mev操作
					now = he->endVertex;
				}
			}
			op.mef(now, startvertex, solid->faces->out_lp, true); //将最后一个点与第一个点相连，形成一个新面

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
			op.kemr(startvertex, firstvertex, solid->faces->out_lp); //运用kemr操作来生成内环

			// 存储多余的面，待扫掠操作完成后进行kfmrh操作来完成模型的构建
			/*Face* deleteface = solid->faces->next;
			while (deleteface->next != NULL) {
				deleteface = deleteface->next;
			}*/
			deleteFace.push_back(deleteface);
		}
	}
/*
triangle with a triangle hole 
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
	//cout << r * sin(theta * 3.14159 / 180) * sin(phi * 3.14159 / 180) << ' ' << r * cos(theta * 3.14159 / 180) << ' ' << r * sin(theta * 3.14159 / 180) * cos(phi * 3.14159 / 180) << endl;
	gluLookAt(r * sin(theta * 3.14159 / 180) * sin(phi * 3.14159 / 180), r * cos(theta * 3.14159 / 180), r * sin(theta * 3.14159 / 180) * cos(phi * 3.14159 / 180),
		0, 0, 0, 
		0, 1, 0);
	//gluLookAt()
    //gluOrtho2D(0.0, 200.0, 0.0, 150.0);
	glutKeyboardFunc(myKeyboard); //指定键盘回调函数
    glutMainLoop();
    return 0;
}

