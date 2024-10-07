#include <bits/stdc++.h>
#include <GL/glut.h>
using namespace std;

#define pi (2 * acos(0.0))
#define ee 2.71828182845904523536
#define epsilon 0.00001

class Point
{
public:
    double x, y, z;
    Point()
    {
        x = 0;
        y = 0;
        z = 0;
    }
    Point(double a, double b, double c)
    {
        x = a;
        y = b;
        z = c;
    }

    void normalize()
    {
        double sqr = sqrt(x * x + y * y + z * z);
        x /= sqr;
        y /= sqr;
        z /= sqr;
    }
    double dot(Point b)
    {
        return x * b.x + y * b.y + z * b.z;
    }
    double length() { return sqrt(x * x + y * y + z * z); }

    Point operator+(Point b) { return Point(x + b.x, y + b.y, z + b.z); }
    Point operator-(Point b) { return Point(x - b.x, y - b.y, z - b.z); }
    double operator*(Point b) { return x * b.x + y * b.y + z * b.z; }
    Point operator+(double b) { return Point(x + b, y + b, z + b); }
    Point operator-(double b) { return Point(x - b, y - b, z - b); }
    Point operator*(double b) { return Point(x * b, y * b, z * b); }
    Point operator/(double b) { return Point(x / b, y / b, z / b); }

    Point operator-() { return Point(-x, -y, -z); }
};

class Line
{
public:
    Point a, b;

    Line() {}
    Line(Point a, Point b)
    {
        this->a = a;
        this->b = b;
    }
    void setPoint(Point a, Point b)
    {
        this->a = a;
        this->b = b;
    }
    double angle(Line l)
    {
        Point vect1 = b - a;
        Point vect2 = l.b - l.a;
        double dot = vect1.x * vect2.x + vect1.y * vect2.y + vect1.z * vect2.z;
        double mag1 = sqrt(vect1.x * vect1.x + vect1.y * vect1.y + vect1.z * vect1.z);
        double mag2 = sqrt(vect2.x * vect2.x + vect2.y * vect2.y + vect2.z * vect2.z);

        dot /= (mag1 * mag2);
        double theta = acos(dot);
        return min(theta, 2.0 * pi - theta);
    }
    pair<int, Point> intersection(Line l)
    {
        /// equation a+t(b-a)
        double a1 = b.x - a.x, b1 = -(l.b.x - l.a.x), c1 = a.x - l.a.x;
        double a2 = b.y - a.y, b2 = -(l.b.y - l.a.y), c2 = a.y - l.a.y;
        //        cout<<"a1 "<<a1<<gap<<"b1 "<<b1<<gap<<"c1 "<<c1<<endl;
        //        cout<<"a2 "<<a2<<gap<<"b2 "<<b2<<gap<<"c2 "<<c2<<endl;

        Point bad;
        if (fabs(a1 * b2 - a2 * b1) < epsilon)
            return {0, bad};

        double t = (b1 * c2 - b2 * c1) / (a1 * b2 - a2 * b1);
        Point tmp(a.x + t * (b.x - a.x), a.y + t * (b.y - a.y), a.z + t * (b.z - a.z));
        return {1, tmp};
    }
};

class Color
{
public:
    double r, g, b;
    Color()
    {
        r = 0;
        g = 0;
        b = 0;
    }
    Color(double a, double c, double d)
    {
        r = a;
        g = c;
        b = d;
    }
    Color operator+(Color c) { return Color(r + c.r, g + c.g, b + c.b); }
    Color operator*(double a) { return Color(r * a, g * a, b * a); }
};

class Ray
{
public:
    Point start;
    Point dir;
    Ray(Point start, Point dir)
    {
        this->start.x = start.x;
        this->start.y = start.y;
        this->start.z = start.z;
        this->dir.x = dir.x;
        this->dir.y = dir.y;
        this->dir.z = dir.z;

        // normalize
        double sqr = sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
        this->dir.x /= sqr;
        this->dir.y /= sqr;
        this->dir.z /= sqr;
    }
    Point getPoint(double t)
    {
        return start + dir * t;
    }
    double operator*(Ray r)
    {
        return dir * r.dir;
    }
};

class pointLight
{
public:
    Point reference_point;
    double falloff;
    Color color;
    pointLight() {}
    pointLight(Point center)
    {
        reference_point.x = center.x;
        reference_point.y = center.y;
        reference_point.z = center.z;
        color.r = 1;
        color.g = 1;
        color.b = 1;
    }
    void setFalloff(double f)
    {
        falloff = f;
    }
    void draw()
    {
        glPushMatrix();
        glTranslatef(reference_point.x, reference_point.y, reference_point.z);
        glColor3f(0.5, 0.5, 0.5);
        glutSolidSphere(3, 20, 20);
        glPopMatrix();
    }
};
class spotLight
{
public:
    Point reference_point;
    Point direction;
    Color color;
    double falloff;
    double cutoff;
    spotLight() {}
    spotLight(Point center, Point direction)
    {
        reference_point.x = center.x;
        reference_point.y = center.y;
        reference_point.z = center.z;
        this->direction.x = direction.x;
        this->direction.y = direction.y;
        this->direction.z = direction.z;

        double sqr = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
        this->direction.x /= sqr;
        this->direction.y /= sqr;
        this->direction.z /= sqr;

        color.r = 1;
        color.g = 1;
        color.b = 1;
    }
    void setFalloff(double f)
    {
        falloff = f;
    }
    void setCutoff(double c)
    {
        cutoff = c;
    }
    void draw()
    {
        glPushMatrix();
        glTranslatef(reference_point.x, reference_point.y, reference_point.z);

        double angleX = std::acos(direction.z);
        double angleY = std::atan2(direction.y, direction.x);
        glRotatef(-angleX * (180.0f / M_PI), 1.0f, 0.0f, 0.0f);
        glRotatef(angleY * (180.0f / M_PI), 0.0f, 0.0f, 1.0f);

        glColor3f(1, 0.5, 0.5);           // Green color for the cone
        glutSolidCone(2, 10, 20, 20); // Draw the cone
        glPopMatrix();
    }
};


class Object;
class pointLight;
class spotLight;
extern vector<Object *> objects;
extern vector<pointLight *> lights;
extern vector<spotLight *> spotlights;

extern Point pos;

class Object
{
public:
    Point reference_point;
    double height, width, length;
    Color color;
    double coEfficients[4];
    int shine;
    Object()
    {
        reference_point.x = 0;
        reference_point.y = 0;
        reference_point.z = 0;
        height = 0;
        width = 0;
        length = 0;
        color.r = 0;
        color.g = 0;
        color.b = 0;
        coEfficients[0] = 0;
        coEfficients[1] = 0;
        coEfficients[2] = 0;
        coEfficients[3] = 0;
        shine = 0;
    }
    void setColor(double r, double g, double b)
    {
        color.r = r;
        color.g = g;
        color.b = b;
    }
    void setShine(int s)
    {
        shine = s;
    }
    void setCoEfficients(double a, double d, double s, double r)
    {
        coEfficients[0] = a;
        coEfficients[1] = d;
        coEfficients[2] = s;
        coEfficients[3] = r;
    }

    virtual void draw() {}
    virtual double intersectingParam(Ray ray) {}
    virtual Color getColor(Point point) {}
    virtual Ray getNormal(Point intersectionPoint) {}

    double intersect(Ray ray, Color &color, int depthOfRecursion)
    {
        double t = intersectingParam(ray);
        if (t < 0)
            return -1;
        if (depthOfRecursion == 0)
        {
            color.r = 0;
            color.g = 0;
            color.b = 0;
            return t;
        }
        Point intersectionPoint = ray.getPoint(t);
        Color objectColor = getColor(intersectionPoint);

        color = objectColor * coEfficients[0];

        double lambert = 0.0, phong = 0.0;

        for (int i = 0; i < lights.size(); i++)
        {

            Point position = lights[i]->reference_point;
            if (position.z * pos.z < 0)
                continue; // checkinig if camera and light source are in the opposite side of the object
            Point direction = position - intersectionPoint; // direction is from intersection point to light source
            direction.normalize();
            Ray lightRay(intersectionPoint, direction); // changed the direction
            double dis = (intersectionPoint - position).length();
            if (dis < 1e-5)
                continue;

            int obscured = 0;
            for (Object *object : objects)
            {
                Ray newRay(position, -direction);
                double curT = object->intersectingParam(newRay);
                if (curT > 0 && curT + 1e-5 < dis)
                    obscured = 1;
            }
            if (obscured)
                continue;

            double scaling_factor = pow(ee, -(lights[i]->falloff * dis * dis));

            Ray normal = getNormal(intersectionPoint);
            lambert += max(0.0, lightRay.dir * normal.dir * scaling_factor);
            Ray reflection = Ray(intersectionPoint, ray.dir - normal.dir * 2 * (ray.dir * normal.dir));
            phong += max(0.0, (pow(lightRay.dir * reflection.dir, shine) * scaling_factor));
        }
        color = color + (objectColor * coEfficients[1] * lambert);
        color = color + (objectColor * coEfficients[2] * phong);

        // spotlight

        lambert = 0.0;
        phong = 0.0;

        for (int i = 0; i < spotlights.size(); i++)
        {

            Point position = spotlights[i]->reference_point;
            if (position.z * pos.z < 0)
                continue; // checkinig if camera and light source are in the opposite side of the object
            Point direction = position - intersectionPoint;
            direction.normalize();

            double dot = -direction * spotlights[i]->direction;
            double angle = acos(dot / (direction.length() * spotlights[i]->direction.length())) * (180.0 / pi);

            double dis = (position - intersectionPoint).length();
            double scaling_factor = pow(ee, -(spotlights[i]->falloff * dis * dis));
            if (dis < epsilon)
                continue;

            if (fabs(angle) < spotlights[i]->cutoff)
            {

                Ray lightRay = Ray(intersectionPoint, direction);
                if (dis < 1e-5)
                    continue;

                int obscured = 0;

                for (Object *obj : objects)
                {
                    Ray newRay(position, -lightRay.dir);
                    double curT = obj->intersectingParam(newRay);
                    if (curT > 0 && curT + 1e-5 < dis)
                    {
                        obscured = 1;
                        break;
                    }
                }
                if (obscured)
                    continue;

                Ray normal = getNormal(intersectionPoint);

                // Ray reflection = Ray(intersectionPoint, lightRay.dir - normal.dir * 2 * (lightRay.dir * normal.dir));
                Ray reflection = Ray(intersectionPoint, ray.dir - normal.dir * 2 * (ray.dir * normal.dir));

                lambert += max(0.0, lightRay.dir * normal.dir * scaling_factor);
                phong += max(0.0, (pow(lightRay.dir * reflection.dir, shine) * scaling_factor));
            }
        }

        color = color + (objectColor * coEfficients[1] * lambert);
        color = color + (objectColor * coEfficients[2] * phong);

        // reflection
        Ray normal = getNormal(intersectionPoint);
        Ray reflectionRay = Ray(intersectionPoint, ray.dir - normal.dir * 2 * (ray.dir * normal.dir));

        // to avoid self intersection
        reflectionRay.start = reflectionRay.start + reflectionRay.dir * 1e-4; // 1e-5 chilo

        int nearest = -1;
        double tr = -1, tMin = 1e9;

        for (int k = 0; k < (int)objects.size(); k++)
        {
            tr = objects[k]->intersectingParam(reflectionRay);
            if (tr > epsilon && tr < tMin)
                tMin = tr, nearest = k;
        }

        if (nearest != -1)
        {
            Color colorTemp(0, 0, 0); // refelction color
            double t = objects[nearest]->intersect(reflectionRay, colorTemp, depthOfRecursion - 1);

            color = color + (colorTemp * coEfficients[3]);
        }

        return t;
    }
};

class Sphere : public Object
{
public:
    Sphere() {}
    Sphere(Point center, double radius, Color color)
    {
        reference_point.x = center.x;
        reference_point.y = center.y;
        reference_point.z = center.z;
        length = radius;
        width = radius;
        height = radius;
        this->color = color;
    }
    void draw()
    {
        int slices = 24, stacks = 20;
        Point points[stacks + 1][slices + 1];
        for (int j = 0; j <= stacks; j++)
        {
            double phi = -M_PI / 2.0 + j * M_PI / stacks;
            double r = length * cos(phi);
            double h = length * sin(phi);
            for (int i = 0; i < slices + 1; i++)
            {
                double theta = i * 2.0 * M_PI / slices;
                points[j][i].x = r * cos(theta);
                points[j][i].y = r * sin(theta);
                points[j][i].z = h;
            }
        }

        glPushMatrix();
        glTranslatef(reference_point.x, reference_point.y, reference_point.z);
        glBegin(GL_QUADS);
        for (int j = 0; j < stacks; j++)
        {
            for (int i = 0; i < slices; i++)
            {
                // GLfloat c = (2+cos((i+j) * 2.0 * M_PI / slices)) / 3;
                glColor3f(color.r, color.g, color.b);
                glVertex3f(points[j][i].x, points[j][i].y, points[j][i].z);
                glVertex3f(points[j][i + 1].x, points[j][i + 1].y, points[j][i + 1].z);

                glVertex3f(points[j + 1][i + 1].x, points[j + 1][i + 1].y, points[j + 1][i + 1].z);
                glVertex3f(points[j + 1][i].x, points[j + 1][i].y, points[j + 1][i].z);
            }
        }
        glEnd();
        glPopMatrix();
    }
    Ray getNormal(Point intersectionPoint)
    {
        Point normal = intersectionPoint - reference_point;
        Ray normalRay(intersectionPoint, normal);
        return normalRay;
    }
    Color getColor(Point point)
    {
        return color;
    }

    double intersectingParam(Ray ray)
    {
        double a, b, c, d, t1, t2, t;
        a = ray.dir * ray.dir;
        Point temp = ray.start - reference_point;
        b = ray.dir * temp;
        b *= 2;
        c = temp * temp - length * length;
        d = b * b - 4 * a * c;
        // cout<<"in sphere intersectingParam "<<b<<" "<<" "<<c<<"  "<<d<<endl;
        if (d < 0)
            return -1;
        else
        {
            t1 = (-b + sqrt(d)) / (2 * a);
            t2 = (-b - sqrt(d)) / (2 * a);
            if (t1 < 0 && t2 < 0)
                return -1;
            else if (t1 < 0)
                t = t2;
            else if (t2 < 0)
                t = t1;
            else
                t = min(t1, t2);
            return t;
        }
    }
};

class Floor : public Object
{
public:
    int preI, preJ;
    Floor(double floorWidth, double tileWidth)
    {
        reference_point.x = -floorWidth / 2;
        reference_point.y = -floorWidth / 2;
        reference_point.z = 0;
        length = floorWidth;
        width = tileWidth;
        height = 0;
        setColor(1, 1, 1);
        setCoEfficients(0.4, 0.2, 0.2, 0.2);
        setShine(5);

        preI = (int)pos.x;
        preI -= (preI % ((int)width));

        preJ = (int)pos.y;
        preJ -= (preJ % ((int)width));
    }
    void draw()
    {
        // cout << "drawing floor" << endl;
        int i, j;
        bool flag = true;

        i = (int)pos.x;
        i -= (i % ((int)width));
        j = (int)pos.y;
        j -= (j % ((int)width));

        int tilesNum = abs((i - preI)) / width;
        int sign = (i > preI) ? 1 : -1;
        // cout<<tilesNum<<"\n";
        if (tilesNum >= 2)  // if more than two tiles have been shifted , shift the reference point
        {
            reference_point.x = preI - length / 2 + 2 * sign * width;
            preI = i;
        }
        
        tilesNum = abs((j - preJ)) / width;
        sign = (j > preJ) ? 1 : -1;
        if (tilesNum >= 2)
        {
            reference_point.y = preJ - length / 2 + 2 * sign * width;
            preJ = j;
        }
        

        glPushMatrix();
        // glTranslatef(pos.x,pos.y,0);
        double x, y;
        for (x = reference_point.x; x <= reference_point.x + length; x += width)
        {
            for (y = reference_point.y; y <= reference_point.y + length; y += width)
            {

                if (flag)
                    glColor3f(1, 1, 1);
                else
                    glColor3f(0, 0, 0);
                flag = !flag;
                glBegin(GL_QUADS);
                // glNormal3f(0,0,1);
                glVertex3f(x, y, 0);
                glVertex3f(x + width, y, 0);
                glVertex3f(x + width, y + width, 0);
                glVertex3f(x, y + width, 0);
                // glVertex3f(x,y,0);
                glEnd();
            }
            // break;
            // flag=!flag;
        }
        glPopMatrix();

    }
    Ray getNormal(Point intersectionPoint)
    {
        Point normal(0, 0, 1);
        Ray normalRay(intersectionPoint, normal);
        return normalRay;
    }

    Color getColor(Point point)
    {
        int x = (point.x - reference_point.x) / width;
        int y = (point.y - reference_point.y) / width;

        // int tiles=length/width;
        // if(x<0 || x>=tiles || y<0 || y>=tiles){
        //     return Color(0,0,0);
        // }
        // return Color(1,1,1);

        if ((x + y) % 2 == 0)
            return Color(1, 1, 1);
        else
            return Color(0, 0, 0);
    }

    double intersectingParam(Ray ray)
    {
        // return -1;
        if (ray.dir.z == 0)
            return -1;
        double t = -1 * ray.start.z / ray.dir.z;
        Point p = ray.start + ray.dir * t;

        // if (p.x <= reference_point.x || p.x >= abs(reference_point.x) && p.y <= reference_point.y && p.y >= abs(reference_point.y))
        // {
        //     return -1;
        // }
        if (t < 0)
            return -1;
        else
            return t;

    }
};

class Triangle : public Object
{
public:
    double a, b, c, d; // stores triangle as parametric equation
    double angleA, angleB, angleC;
    Point A, B, C;
    Triangle() {}

    Triangle(Point A, Point B, Point C)
    {
        this->A = A;
        this->B = B;
        this->C = C;
        Point vect1 = B - A;
        Point vect2 = C - A;

        a = vect1.y * vect2.z - vect1.z * vect2.y;
        b = vect1.z * vect2.x - vect1.x * vect2.z;
        c = vect1.x * vect2.y - vect1.y * vect2.x;
        d = -a * A.x - b * A.y - c * A.z;

        Line line1(A, B);
        Line line2(A, C);
        angleA = line1.angle(line2);

        line1.setPoint(B, A);
        line2.setPoint(B, C);
        angleB = line1.angle(line2);

        line1.setPoint(C, A);
        line2.setPoint(C, B);
        angleC = line1.angle(line2);

        this->color = color;
    }
    int isInside(Point p)
    {
        Line line1(A, p);
        Line line2(A, B);
        Line line3(A, C);

        double theta1 = line1.angle(line2);
        double theta2 = line1.angle(line3);

        if (max(theta1, theta2) - angleA > epsilon)
            return 0;

        line1.setPoint(B, p);
        line2.setPoint(B, A);
        line3.setPoint(B, C);

        theta1 = line1.angle(line2);
        theta2 = line1.angle(line3);

        if (max(theta1, theta2) - angleB > epsilon)
            return 0;
        return 1;
    }
    void draw()
    {
        glBegin(GL_TRIANGLES);

        glColor3f(color.r, color.g, color.b);

        glVertex3f(A.x, A.y, A.z);
        glVertex3f(B.x, B.y, B.z);
        glVertex3f(C.x, C.y, C.z);
        glEnd();
    }
    double intersectingParam(Ray ray)
    {
        Point temp(a, b, c);
        double dot = temp * ray.start;
        double dotDir = temp * ray.dir;
        if (dotDir == 0)
            return -1;
        double t = (-dot - d) / dotDir;

        Point intersectionPoint = ray.getPoint(t);
        if (isInside(intersectionPoint))
            return t;
        return -1;
    }
    Ray getNormal(Point p)
    {
        Point dir(a, b, c);
        Ray normal(p, dir);
        return normal;
    }
    Color getColor(Point p) { return color; }
};
