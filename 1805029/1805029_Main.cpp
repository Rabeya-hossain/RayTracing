#include<bits/stdc++.h>
#include <GL/glut.h>
using namespace std;
#include <windows.h>
#include "1805029_Header.h"
#include "1805029_bitmap_image.hpp"

// position of camera

Point pos(0,-160,60);
Point l(0,2,0);
Point r(2,0,0);
Point u(0,0,2);



double nearPlane,farPlane,aspectRatio,fovy;
int recursionLevel,numPixel; // number of pixels
int windowWidth = 640, windowHeight = 640;

vector <Object*> objects;
vector <pointLight*> lights;
vector <spotLight*> spotlights;

int imageCount=1;

Point pointBuffer[1000][1000];
//Point textureBuffer[1000][1000];

vector<Object*> squareToTriangle(Point a,Point b,Point c,Point d)
{
	Object *tr1= new Triangle(a,b,d);
	Object *tr2=new Triangle(b,c,d);
	vector<Object*> vec;
	vec.push_back(tr1);
	vec.push_back(tr2);
	return vec;

}
vector<Object*> pyramidToTriangle(Point center,double height,double width)
{
	vector<Object*> vec;
	vector<Object*> Base;

    //base
	Point a(center.x,center.y,center.z);
	Point b(center.x+width,center.y,center.z);
	Point c(center.x+width,center.y+width,center.z);
	Point d(center.x,center.y+width,center.z);
	Base=squareToTriangle(a,b,c,d);
	for(Object* tr:Base) vec.push_back(tr);

	//faces
	Point peak(center.x+(width/2),center.y+(width/2),center.z+height);

	Object *tr1=new Triangle(a,b,peak);
	Object *tr2=new Triangle(b,c,peak);
	Object *tr3=new Triangle(c,d,peak);
	Object *tr4=new Triangle(d,a,peak);

	vec.push_back(tr1);
	vec.push_back(tr2);
	vec.push_back(tr3);
	vec.push_back(tr4);

	return vec;

}

vector<Object*> cubeToTriangle(Point center,double length)
{
	vector<Object*> vec;
	vector<Object*> Base;
	//base points
	Point a(center.x,center.y,center.z);
	Point b(center.x+length,center.y,center.z);
	Point c(center.x+length,center.y+length,center.z);
	Point d(center.x,center.y+length,center.z);

	Point h(center.x,center.y,center.z+length);
	Point g(center.x+length,center.y,center.z+length);
	Point f(center.x+length,center.y+length,center.z+length);
	Point e(center.x,center.y+length,center.z+length);


	Base=squareToTriangle(a,b,c,d);
	for(Object* tr:Base) vec.push_back(tr);

	Base=squareToTriangle(d,c,f,e);
	for(Object* tr:Base) vec.push_back(tr);

	Base=squareToTriangle(a,b,g,h);
	for(Object* tr:Base) vec.push_back(tr);

	Base=squareToTriangle(a,d,e,h);
	for(Object* tr:Base) vec.push_back(tr);

	Base=squareToTriangle(b,c,f,g);
	for(Object* tr:Base) vec.push_back(tr);

	Base=squareToTriangle(e,f,g,h);
	for(Object* tr:Base) vec.push_back(tr);
	return vec;
}

void loadData()
{
	ifstream cin("description.txt");
    cin>> nearPlane >> farPlane;
    cin>> fovy >> aspectRatio;
	cin >> recursionLevel >> numPixel;


    double cell,ambient,diffuse,reflection;
	cin >> cell >> ambient >> diffuse >>  reflection ;
	Object *flloor=new Floor(1200,cell);
	flloor->setCoEfficients(ambient,diffuse,0,reflection);
	objects.push_back(flloor);


	int objCount;
	cin >> objCount;

	for(int i=0;i<objCount;i++)
	{
		string objType;
		cin >> objType;

		if(objType == "sphere"){
			double x,y,z,radius,r,g,b,ambient,diffuse,specular,reflection,shininess;
            cin >> x >> y >> z >> radius >> r >> g >> b >> ambient >> diffuse >> specular >> reflection >> shininess;
            Point center(x,y,z);
            Color color(r,g,b);
			Object *obj;
            obj = new Sphere(center,radius,color);
            obj->setCoEfficients(ambient,diffuse,specular,reflection);
            obj->setShine(shininess);
			objects.push_back(obj);
        }
        else if(objType == "pyramid"){
            double x,y,z,width,height,r,g,b,ambient,diffuse,specular,reflection,shininess;
			cin >> x >> y >> z >> width >> height >> r >> g >> b >> ambient >> diffuse >> specular >> reflection >> shininess;
			Point center(x,y,z);
			Color color(r,g,b);

			vector<Object*> pyramid = pyramidToTriangle(center,height,width);
			for(Object* obj:pyramid)
			{
				obj->setColor(r,g,b);
				obj->setCoEfficients(ambient,diffuse,specular,reflection);
			    obj->setShine(shininess);
				objects.push_back(obj);

			}

        }
        else if(objType == "cube"){
			double x,y,z,length,r,g,b,ambient,diffuse,specular,reflection,shininess;
			cin >> x >> y >> z >> length >> r >> g ;
			cin >> b >> ambient >> diffuse >> specular >> reflection >> shininess;
			Point leftCorner(x,y,z);
			Color color(r,g,b);

			vector<Object*> cube = cubeToTriangle(leftCorner,length);
			for(Object* obj:cube)
			{
				obj->setColor(r,g,b);
				obj->setCoEfficients(ambient,diffuse,specular,reflection);
			    obj->setShine(shininess);
				objects.push_back(obj);

			}

        }
        else{
            cout<<objType<<" is not a valid object type"<<endl;

		}

	}

	int lightCount;
	cin >> lightCount;

	for(int i=0;i<lightCount;i++){
		double x,y,z,fallof;
		cin >> x >> y >> z >> fallof;
		Point pos(x,y,z);
		pointLight *pl = new pointLight(pos);
		pl->setFalloff(fallof);
		lights.push_back(pl);
	}

	int spotlightCount;
	cin >> spotlightCount;

	for(int i=0;i<spotlightCount;i++){
		double x,y,z,fallof,dirx,diry,dirz,cutoff;
		cin >> x >> y >> z >> fallof >> dirx >> diry >> dirz >> cutoff;
		Point pos(x,y,z);
		Point dir(dirx,diry,dirz);
		spotLight *sl = new spotLight(pos,dir-pos);
		sl->setFalloff(fallof);
		sl->setCutoff(cutoff);
		spotlights.push_back(sl);

	}
	cin.close();
	cout<<"input loaded"<<endl;


}

// void texture()
// {
// 	//load texture_b.bpm in a 2d array as r,g,b values of each pixel
// 	bitmap_image image("texture_b.bmp");
// 	unsigned char r,g,b;
// 	int height=image.height();
// 	int width=image.width();
// 	cout<<height<<" "<<width<<endl;
// 	bitmap_image* image1 = new bitmap_image(height,width);
// 	for(int i=0;i<height;i++)
// 	{
// 		for(int j=0;j<width;j++)
// 		{
// 			image.get_pixel(i,j,r,g,b);
// 			textureBuffer[i][j]=Point(r,g,b);
// 			image1->set_pixel(i, j, textureBuffer[i][j].x, textureBuffer[i][j].y, textureBuffer[i][j].z);

// 			//cout<<r<<" "<<g<<" "<<b<<endl;
// 		}
// 	}
// 	image1->save_image("Output.bmp");
// 	cout<<"texture loaded"<<endl;

// }


void init(){
	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	loadData();

	//give PERSPECTIVE parameters
	gluPerspective(fovy,aspectRatio,nearPlane,	farPlane);
	//gluPerspective(80,1,1,	1000);

	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance

}

void capture()
{
	 double planeDistance = (windowHeight / 2.0) / tan((pi * fovy) / (360.0));
	 Point look(l.x,l.y,l.z);
	 Point right(r.x,r.y,r.z);
	 Point up(u.x,u.y,u.z);
	 look.normalize();
	 right.normalize();
	 up.normalize();


	Point topLeft = pos + (look * planeDistance) + (up * (windowHeight / 2.0)) - (right * (windowWidth / 2.0));

	double du = windowWidth / (numPixel*1.0);
	double dv = windowHeight / (numPixel*1.0);

	// Choose middle of the grid cell
	topLeft = topLeft + (right * du / 2.0) - (up * dv / 2.0);
	 for(int i=0;i<numPixel;i++){
		for(int j=0;j<numPixel;j++){
			Point pixel=topLeft + (right*du*i) - (up*dv*j);
			pointBuffer[i][j]=pixel;
			//cout<<pixel.x<<' '<<pixel.y<<' '<<pixel.z<<endl;
		}
	 }
	 cout<<"point buffer done"<<endl;
	 //return;

     int nearest=-1;
	 double tmin,t;
	 bitmap_image* image = new bitmap_image(numPixel,numPixel);
	 for(int i=0;i<numPixel;i++)
		for(int j=0;j<numPixel;j++)
			image->set_pixel(i, j, 0, 0, 0);



	 for(int i=0;i<numPixel;i++){
		for(int j=0;j<numPixel;j++){

			Ray ray(pos,pointBuffer[i][j]-pos);
			nearest=-1;
			for(int k=0;k<objects.size();k++){
				//cout<<"checking object "<<k<<endl;
				t=objects[k]->intersectingParam(ray);
				//if(k==0) cout<<"t "<<t<<endl;
				if(t<0) continue;
				if(nearest==-1){
					tmin=t;
					nearest=k;
				}
				else if(t<tmin){
					tmin=t;
					nearest=k;
				}
			}
			//cout<<"nearest "<<nearest<<endl;
			if(nearest!=-1){
				Point intersectingPoint=ray.getPoint(tmin);
				//Color color=objects[nearest]->getColor(intersectingPoint);
				Color color(0,0,0);
				double t=objects[nearest]->intersect(ray,color,recursionLevel);
				if(color.r > 1) color.r = 1;
				if(color.g > 1) color.g = 1;
				if(color.b > 1) color.b = 1;

				if(color.r < 0) color.r = 0;
				if(color.g < 0) color.g = 0;
				if(color.b < 0) color.b = 0;
				// cout<<"nearest "<<nearest<<endl;
				// cout<<"color "<<color.r<<" "<<color.g<<" "<<color.b<<endl;

				image->set_pixel(i, j, 255*color.r, 255*color.g, 255*color.b);
			}
		}
	 }
	 image->save_image("out.bmp");
	imageCount++;
	cout<<"Saved Image"<<endl;
}



//drawAxes
void drawAxes()
{

        glLineWidth(3.0);
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINES);
		{
			glVertex3f( 200,0,0);
			glVertex3f(-200,0,0);

			glVertex3f(0,-200,0);
			glVertex3f(0, 200,0);

			glVertex3f(0,0, 200);
			glVertex3f(0,0,-200);
		}
		glEnd();

}

void keyboardListener(unsigned char key, int xx,int yy){
    double rate = 0.01;
	switch(key){
		case '0':
		    capture();
			break;
		// case '9':
		//     texture();
		// 	break;

		case '1':
			r.x = r.x*cos(rate)+l.x*sin(rate);
			r.y = r.y*cos(rate)+l.y*sin(rate);
			r.z = r.z*cos(rate)+l.z*sin(rate);

			l.x = l.x*cos(rate)-r.x*sin(rate);
			l.y = l.y*cos(rate)-r.y*sin(rate);
			l.z = l.z*cos(rate)-r.z*sin(rate);
			break;

        case '2':
			r.x = r.x*cos(-rate)+l.x*sin(-rate);
			r.y = r.y*cos(-rate)+l.y*sin(-rate);
			r.z = r.z*cos(-rate)+l.z*sin(-rate);

			l.x = l.x*cos(-rate)-r.x*sin(-rate);
			l.y = l.y*cos(-rate)-r.y*sin(-rate);
			l.z = l.z*cos(-rate)-r.z*sin(-rate);
			break;

        case '3':
			l.x = l.x*cos(rate)+u.x*sin(rate);
			l.y = l.y*cos(rate)+u.y*sin(rate);
			l.z = l.z*cos(rate)+u.z*sin(rate);

			u.x = u.x*cos(rate)-l.x*sin(rate);
			u.y = u.y*cos(rate)-l.y*sin(rate);
			u.z = u.z*cos(rate)-l.z*sin(rate);
			break;

        case '4':
			l.x = l.x*cos(-rate)+u.x*sin(-rate);
			l.y = l.y*cos(-rate)+u.y*sin(-rate);
			l.z = l.z*cos(-rate)+u.z*sin(-rate);

			u.x = u.x*cos(-rate)-l.x*sin(-rate);
			u.y = u.y*cos(-rate)-l.y*sin(-rate);
			u.z = u.z*cos(-rate)-l.z*sin(-rate);
			break;

        case '5':
			u.x = u.x*cos(rate)+r.x*sin(rate);
			u.y = u.y*cos(rate)+r.y*sin(rate);
			u.z = u.z*cos(rate)+r.z*sin(rate);

			r.x = r.x*cos(rate)-u.x*sin(rate);
			r.y = r.y*cos(rate)-u.y*sin(rate);
			r.z = r.z*cos(rate)-u.z*sin(rate);
			break;

        case '6':
			u.x = u.x*cos(-rate)+r.x*sin(-rate);
			u.y = u.y*cos(-rate)+r.y*sin(-rate);
			u.z = u.z*cos(-rate)+r.z*sin(-rate);

			r.x = r.x*cos(-rate)-u.x*sin(-rate);
			r.y = r.y*cos(-rate)-u.y*sin(-rate);
			r.z = r.z*cos(-rate)-u.z*sin(-rate);
			break;

		default:
			break;
	}
	glutPostRedisplay();
}


void specialKeyListener(int key, int x,int y)
{
	switch(key){
		case GLUT_KEY_UP:		//down arrow key
			pos.x+=l.x;
			pos.y+=l.y;
			pos.z+=l.z;
			break;
		case GLUT_KEY_DOWN:		// up arrow key
			pos.x-=l.x;
			pos.y-=l.y;
			pos.z-=l.z;
			break;

		case GLUT_KEY_RIGHT:
			pos.x+=r.x;
			pos.y+=r.y;
			pos.z+=r.z;
			break;
		case GLUT_KEY_LEFT :
			pos.x-=r.x;
			pos.y-=r.y;
			pos.z-=r.z;
			break;

		case GLUT_KEY_PAGE_UP:
		    pos.x+=u.x;
			pos.y+=u.y;
			pos.z+=u.z;
			break;
		case GLUT_KEY_PAGE_DOWN:
            pos.x-=u.x;
			pos.y-=u.y;
			pos.z-=u.z;
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
	glutPostRedisplay();
}
void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//  gluLookAt(0,0,200,	0,0,0,	0,1,0);
	//gluLookAt(0,100,0,	0,0,0,	0,0,1);

	gluLookAt(pos.x, pos.y, pos.z,
			pos.x + l.x, pos.y + l.y, pos.z + l.z,
			u.x, u.y, u.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);

	drawAxes();
	for(auto obj:objects){

		obj->draw();
	}
	for(auto light:lights){
		light->draw();
	}
	for(auto spotlight:spotlights){
		spotlight->draw();
	}

	glutSwapBuffers();
}


int main(int argc, char **argv){


	glutInit(&argc,argv);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Ray Tracing");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);

	glutMainLoop();		//The main loop of OpenGL


	return 0;
}
