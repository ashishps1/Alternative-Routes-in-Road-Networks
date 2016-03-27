#include<iostream>
#include<stdio.h>
#include<GL/glut.h>
#include<GL/gl.h>
#include<stdlib.h>
#include<memory.h>
#include<vector>
#include<map>
#include<queue>
#include<stack>
#include<algorithm>
#include<ctime>
#include<unistd.h>

using namespace std;

#define p_b push_back
#define m_p make_pair
#define all(v) v.begin(),v.end()
#define INF 1000000000

int width=1250;		//width of the canvas
int height=700;		//height of the canvas
int road_width=110;	//width of the road
int hor_road_gap=150;	//gap in horizontal road
int ver_road_gap=100;	//gap in vertical road
int hor_gap=100;
int ver_gap=100;

int n=3;		//No. of rows
int m=4;		//No. of columns

double dist[5][5];	
bool used[5][5];

int no_of_vehicle=14;	//Stores the number of vehicles.

int x,y;
int dx[]={-1,1,0,0};	//To traverse along the grids
int dy[]={0,0,-1,1};	//To traverse along the grids

bool paus=false,draw=false;

stack<pair<int,int> > path;	//To store the path from source to destination.

vector<double> speeds;		//stores the speed of vehicles.
vector<pair<double,double> > positions;		//stores the position of vehicles.
vector<bool> directions;	//stores the direction of vehicles.
vector<char> orientations;	//stores the orientation of vehicles.

queue<double> hor_left;
queue<double> hor_right;
queue<double> ver_up;
queue<double> ver_down;

map<pair<pair<int,int>,pair<int,int> >,double > weights;	//stores the weights of different road sections.
map<pair<int,int>,pair<int,int> > parent;			//stores the parent of the current grid.
pair<int,int> start,dest;	
map<pair<int,int>,pair<double,double> > point_map;
map<pair<int,int>,double> map_speed;

double colors[20][3];

struct vehicles{		//stores the state of each vehicles.
	double vehicle_speedX;		
	double vehicle_speedY;
	double vehicle_posX;
	double vehicle_posY;
	char orientation;
	bool direction;

} vehicle[14];

double car_speedX;
double car_speedY;
double car_posX;
double car_posY;

void speed_shuffle(){			//for shuffling the speed of the vehicles.
	double d=0.005;
	for(int i=1;i<=no_of_vehicle;i++){
		speeds.p_b(d);
		d+=0.005;	
	}
	random_shuffle(all(speeds));		
}

void shuffle_directions(){		//for assigning the directions and orientations to the vehicles. 
	srand(time(0));
	for(int i=1;i<=no_of_vehicle;i++){
		if(i<=8){
			orientations.p_b('V');
			if(i%2)
				directions.p_b(1);
			else
				directions.p_b(0);
		}
		else{
			orientations.p_b('H');
			if(i%2)
				directions.p_b(1);
			else
				directions.p_b(0);
		}	
	}
}

void color_shuffle(){		//for shuffling the color of the vehicles.
	srand(time(0));
	for(int i=0;i<no_of_vehicle;i++){
		colors[i][0]=(rand()%10)*1.0/10;
		colors[i][1]=(rand()%10)*1.0/10;
		colors[i][2]=(rand()%10)*1.0/10;
	}
}

void assign_positions(){		//for assigning the positions to the vehicles.
	srand(time(0));
	int j;
	char k;
	hor_left.push(ver_gap+0.5*road_width-10);
	hor_left.push(ver_gap+1.5*road_width+ver_road_gap-10);
	hor_left.push(ver_gap+2.5*road_width+2*ver_road_gap-10);

	hor_right.push(ver_gap+0.5*road_width+10);
	hor_right.push(ver_gap+1.5*road_width+ver_road_gap+10);
	hor_right.push(ver_gap+2.5*road_width+2*ver_road_gap+10);

	ver_up.push(hor_gap+10);
	ver_up.push(hor_gap+road_width+hor_road_gap+10);
	ver_up.push(hor_gap+2*road_width+2*hor_road_gap+10);
	ver_up.push(hor_gap+3*road_width+3*hor_road_gap+10);

	ver_down.push(hor_gap+0.5*road_width+10);
	ver_down.push(hor_gap+1.5*road_width+hor_road_gap+10);
	ver_down.push(hor_gap+2.5*road_width+2*hor_road_gap+10);
	ver_down.push(hor_gap+3.5*road_width+3*hor_road_gap+10);

	for(int i=0;i<no_of_vehicle;i++){

		j=directions[i];
		k=orientations[i];
		
		vehicle[i].direction=j;
		vehicle[i].orientation=k;

		if(k=='H'&&j==0){
			vehicle[i].vehicle_posY=hor_left.front();
			vehicle[i].vehicle_posX=rand()%width;
			hor_left.pop();
		}
		else if(k=='H'&&j==1){
			vehicle[i].vehicle_posY=hor_right.front();
			vehicle[i].vehicle_posX=rand()%width;
			hor_right.pop();
		}
		else if(k=='V'&&j==0){
			vehicle[i].vehicle_posX=ver_down.front();
			vehicle[i].vehicle_posY=rand()%height;
			ver_down.pop();
		}
		else if(k=='V'&&j==1){
			vehicle[i].vehicle_posX=ver_up.front();
			vehicle[i].vehicle_posY=rand()%height;
			ver_up.pop();
		}
	}	
}

void draw_vehicles(){		//for drawing the vehicles.
	
	for(int i=0;i<no_of_vehicle;i++){
		glColor3f(colors[i][0],colors[i][1],colors[i][2]);

		if(vehicle[i].orientation=='H'&&vehicle[i].direction==0){
			glPushMatrix();
				glBegin(GL_POLYGON);
					glVertex2f(vehicle[i].vehicle_posX,vehicle[i].vehicle_posY);
					glVertex2f(vehicle[i].vehicle_posX,vehicle[i].vehicle_posY-40);
					glVertex2f(vehicle[i].vehicle_posX-65,vehicle[i].vehicle_posY-40);
					glVertex2f(vehicle[i].vehicle_posX-90,vehicle[i].vehicle_posY-20);
					glVertex2f(vehicle[i].vehicle_posX-65,vehicle[i].vehicle_posY);
				glEnd();
			glPopMatrix();
		}
		else if(vehicle[i].orientation=='H'&&vehicle[i].direction==1){
			glPushMatrix();
				glBegin(GL_POLYGON);
					glVertex2f(vehicle[i].vehicle_posX,vehicle[i].vehicle_posY);
					glVertex2f(vehicle[i].vehicle_posX,vehicle[i].vehicle_posY+40);
					glVertex2f(65+vehicle[i].vehicle_posX,vehicle[i].vehicle_posY+40);
					glVertex2f(90+vehicle[i].vehicle_posX,vehicle[i].vehicle_posY+20);
					glVertex2f(65+vehicle[i].vehicle_posX,vehicle[i].vehicle_posY);
				glEnd();
			glPopMatrix();
		}
		else if(vehicle[i].orientation=='V'&&vehicle[i].direction==0){
			glPushMatrix();
				glBegin(GL_POLYGON);
					glVertex2f(vehicle[i].vehicle_posX,vehicle[i].vehicle_posY);
					glVertex2f(40+vehicle[i].vehicle_posX,vehicle[i].vehicle_posY);
					glVertex2f(40+vehicle[i].vehicle_posX,vehicle[i].vehicle_posY-65);
					glVertex2f(20+vehicle[i].vehicle_posX,vehicle[i].vehicle_posY-90);
					glVertex2f(vehicle[i].vehicle_posX,vehicle[i].vehicle_posY-65);
				glEnd();
			glPopMatrix();			
		}
		else if(vehicle[i].orientation=='V'&&vehicle[i].direction==1){
			glPushMatrix();
				glBegin(GL_POLYGON);
					glVertex2f(vehicle[i].vehicle_posX,vehicle[i].vehicle_posY);
					glVertex2f(vehicle[i].vehicle_posX,vehicle[i].vehicle_posY+65);
					glVertex2f(20+vehicle[i].vehicle_posX,vehicle[i].vehicle_posY+90);
					glVertex2f(40+vehicle[i].vehicle_posX,vehicle[i].vehicle_posY+65);
					glVertex2f(40+vehicle[i].vehicle_posX,vehicle[i].vehicle_posY);
				glEnd();
			glPopMatrix();			
		}

	}

}

void draw_object(){		//for drawing the road

//road
	int i,x,y;
//horizontal_road
	y=ver_gap;
	glColor3f(0.2,0.2,0.2);
	for(i=1;i<=3;i++){
		glBegin(GL_POLYGON);
			glVertex2f(0,y);
			glVertex2f(0,y+road_width);
			glVertex2f(width,y+road_width);
			glVertex2f(width,y);
		glEnd();
		y+=(ver_road_gap+road_width);
	}	

//vertical_road
	x=hor_gap;
	for(i=1;i<=4;i++){
		glBegin(GL_POLYGON);
			glVertex2f(x,0);
			glVertex2f(x,height);
			glVertex2f(x+road_width,height);
			glVertex2f(x+road_width,0);
		glEnd();
		x+=(hor_road_gap+road_width);		
	}

//road_strip
	int j=0,gap=50,t,t1;
	double hh[3];
	hh[0]=ver_gap+road_width*1.0/2-2.5,hh[1]=hh[0]+road_width+ver_road_gap,hh[2]=hh[1]+road_width+ver_road_gap;

	glColor3f(1.0,1.0,1.0);

	vector<double> v;
	v.p_b(0.0);
	v.p_b(hor_gap+road_width);
	v.p_b(hor_gap+2*road_width+hor_road_gap);
	v.p_b(hor_gap+3*road_width+2*hor_road_gap);
	v.p_b(hor_gap+4*road_width+3*hor_road_gap);

for(i=0;i<5;i++){

	if(i==0)
		t=10,t1=75;
	else
		t=20,t1=100;
	
	for(j=0;j<3;j++){
		glBegin(GL_POLYGON);
			glVertex2f(v[i]+t,hh[j]);
			glVertex2f(v[i]+t,hh[j]+5);
			glVertex2f(v[i]+t+t1,hh[j]+5);
			glVertex2f(v[i]+t+t1,hh[j]);
		glEnd();
	}

}
	double vv[4];
	vv[0]=hor_gap+road_width*1.0/2-2.5,vv[1]=vv[0]+road_width+hor_road_gap,vv[2]=vv[1]+road_width+hor_road_gap,vv[3]=vv[2]+road_width+hor_road_gap;

	v.clear();
	v.p_b(0.0);
	v.p_b(ver_gap+road_width);
	v.p_b(ver_gap+2*road_width+ver_road_gap);
	v.p_b(ver_gap+3*road_width+2*ver_road_gap);

for(i=0;i<4;i++){

	if(i==0)
		t=10,t1=50;
	else
		t=10,t1=75;
	
	for(j=0;j<4;j++){
		glBegin(GL_POLYGON);
			glVertex2f(vv[j],v[i]+t);
			glVertex2f(vv[j],v[i]+t+t1);
			glVertex2f(vv[j]+5,v[i]+t+t1);
			glVertex2f(vv[j]+5,v[i]+t);
		glEnd();
	}

}

//car
	glPushMatrix();
		glColor3f(1.0,0.0,0.0);
		glBegin(GL_POLYGON);
			glVertex2f(10+car_speedX,ver_gap+0.5*road_width+10);
			glVertex2f(10+car_speedX,ver_gap+0.5*road_width+50);
			glVertex2f(75+car_speedX,ver_gap+0.5*road_width+50);
			glVertex2f(100+car_speedX,ver_gap+0.5*road_width+40);
			glVertex2f(100+car_speedX,ver_gap+0.5*road_width+20);
			glVertex2f(75+car_speedX,ver_gap+0.5*road_width+10);
		glEnd();
	glPopMatrix();

	draw_vehicles();

}

void draw_path(){		//for displaying the shortest path to be followed
	glColor3f(1.0,0.0,0.0);
	stack<pair<int,int> > st;
	
	pair<int,int> p=dest;
	pair<int,int> q;
	pair<int,int> pq1,pq2;

	while(p!=start){
		st.push(p);
		path.push(p);
		p=parent[p];
	}
	
	p=start;

	while(!st.empty()){
		q=st.top();
		pq1=point_map[m_p(p.first,p.second)];
		pq2=point_map[m_p(q.first,q.second)];

		glBegin(GL_LINES);
		    glVertex2f(pq1.first,pq1.second);
		    glVertex2f(pq2.first,pq2.second);
		glEnd();

		p=q;
		st.pop();
	}
	draw=true;
}

void idle()
{	
	//car_speedX+=1.0;
	if(car_speedX>=width)
		car_speedX=0;

	if(!paus){
		for(int i=0;i<no_of_vehicle;i++){
			if(vehicle[i].orientation=='H'&&vehicle[i].direction==0){
				vehicle[i].vehicle_posX-=speeds[i];
				vehicle[i].vehicle_speedX=speeds[i];
				vehicle[i].vehicle_speedY=0.0;
			}
			else if(vehicle[i].orientation=='H'&&vehicle[i].direction==1){
				vehicle[i].vehicle_posX+=speeds[i];
				vehicle[i].vehicle_speedX=speeds[i];
				vehicle[i].vehicle_speedY=0.0;
			}
			else if(vehicle[i].orientation=='V'&&vehicle[i].direction==0){
				vehicle[i].vehicle_posY-=speeds[i];
				vehicle[i].vehicle_speedX=0.0;
				vehicle[i].vehicle_speedY=speeds[i];
			}
			else if(vehicle[i].orientation=='V'&&vehicle[i].direction==1){
				vehicle[i].vehicle_posY+=speeds[i];
				vehicle[i].vehicle_speedX=0.0;
				vehicle[i].vehicle_speedY=speeds[i];
			}

		}
	}
		
	glutPostRedisplay();
}

void pointMap(){		//for mapping points to the coordinates.

	double hor=hor_gap+0.5*road_width,ver;
	int i,j;
	for(i=1;i<=m;i++){
		ver=ver_gap+0.5*road_width;
		for(j=1;j<=n;j++){
			point_map[m_p(i,j)]=m_p(hor,ver);
			ver+=(road_width+ver_road_gap);
		}
		hor+=(road_width+hor_road_gap);
	}
}


void find_weights(){		//for finding the weights across each road
	int i,j,k,x,y;
	srand(0);
	for(i=1;i<=m;i++){
		for(j=1;j<=n;j++){
			weights[m_p(m_p(i,j),m_p(i,j+1))]=speeds[rand()%14];
			weights[m_p(m_p(i,j),m_p(i,j-1))]=speeds[rand()%14];
			weights[m_p(m_p(i,j),m_p(i+1,j))]=speeds[rand()%14];
			weights[m_p(m_p(i,j),m_p(i-1,j))]=speeds[rand()%14];
		}
	}
}

bool isValid(pair<int,int> u) {
	int i = u.first, j = u.second;
	return (i>=1 && j>=1 && i<=m && j<=n);
}

void dijkstra(){		//dijkstra algorithm to find the shortest path.

	int i,j,x,y;
	
	for(i=0;i<5;i++)
		for(j=0;j<5;j++)
			dist[i][j]=INF;
	
	priority_queue<pair<double,pair<int,int> > > pq;
	pair<double,pair<int,int> > p;
	pq.push(m_p(0.0,start));
	dist[start.first][start.second]=0;

	while(!pq.empty()){
		p=pq.top();
		pq.pop();
		for(i=0;i<4;i++){
			x=p.second.first+dx[i];
			y=p.second.second+dy[i];
			if(isValid(m_p(x,y))){
				if(dist[p.second.first][p.second.second]+weights[m_p(m_p(p.second.first,p.second.second),m_p(x,y))]<dist[x][y]){
					dist[x][y]=dist[p.second.first][p.second.second]+weights[m_p(m_p(p.second.first,p.second.second),m_p(x,y))];
					pq.push(m_p(dist[x][y],m_p(x,y)));
					parent[m_p(x,y)]=m_p(p.second.first,p.second.second);									
				}
			}										
		}
	}
}

void mouse(int button,int state,int x,int y)
{
	
}

void keyboardFunc(unsigned char key, int x, int y )
{
	if(key==27){
		exit(0);
	}
	else if(key=='p'||key=='P'){
		paus=true;
	}
	else if(key=='r'||key=='R'){
		paus=false;
	}
}

void myinit()
{
	glClearColor(0.0,1.0,0.0,1.0);
	glColor3f(0.0,0.0,1.0);
	glPointSize(2.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0,1100.0,0.0,700.0);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	draw_object();
	draw_path();
	glFlush();
}

int main(int argc,char** argv){

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowSize(width,height);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Traffic");

	start=m_p(1,1);
	dest=m_p(m,n);

	speed_shuffle();
	shuffle_directions();
	color_shuffle();
	assign_positions();
	pointMap();
	find_weights();
	dijkstra();
	int j,k;
	j=4,k=3;

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouse);
	myinit();

	glutMainLoop();

	return 0;	
}
