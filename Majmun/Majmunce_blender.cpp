#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <iomanip>
using namespace std;
const double M_PI = 3.14159265358979323846;

struct Tocka3D{
    double x;
    double y;
    double z;
    double w;

    Tocka3D(double x_, double y_, double z_, double w_ = 1.0) {
        x = x_;
        y = y_;
        z = z_;
        w = w_;
    }
};

struct Matrica4x4
{
    double m[4][4];
};

Matrica4x4 mnoziMatrica(const Matrica4x4& A, const Matrica4x4& B)
{
    Matrica4x4 R = {};

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                R.m[i][j] += A.m[i][k] * B.m[k][j];

    return R;
}

Tocka3D primeniTransformacija(const Matrica4x4& T, const Tocka3D& p)
{
    double x =
        T.m[0][0] * p.x +
        T.m[0][1] * p.y +
        T.m[0][2] * p.z +
        T.m[0][3] * p.w;

    double y =
        T.m[1][0] * p.x +
        T.m[1][1] * p.y +
        T.m[1][2] * p.z +
        T.m[1][3] * p.w;

    double z =
        T.m[2][0] * p.x +
        T.m[2][1] * p.y +
        T.m[2][2] * p.z +
        T.m[2][3] * p.w;

    double w =
        T.m[3][0] * p.x +
        T.m[3][1] * p.y +
        T.m[3][2] * p.z +
        T.m[3][3] * p.w;

    if (w != 0)
    {
        x /= w;
        y /= w;
        z /= w;
    }

    return Tocka3D(x, y, z);
}

Matrica4x4 translacija(double tx, double ty, double tz)
{
    Matrica4x4 T =
    { {
        {1,0,0,tx},
        {0,1,0,ty},
        {0,0,1,tz},
        {0,0,0,1}
    } };

    return T;
}

Matrica4x4 skaliranje(double sx, double sy, double sz)
{
    Matrica4x4 S =
    { {
        {sx,0,0,0},
        {0,sy,0,0},
        {0,0,sz,0},
        {0,0,0,1}
    } };

    return S;
}

Matrica4x4 rotacijaX(double stepeni)
{
    double rad = stepeni * M_PI / 180.0;
    double c = cos(rad);
    double s = sin(rad);

    Matrica4x4 R =
    { {
        {1,0,0,0},
        {0,c,-s,0},
        {0,s,c,0},
        {0,0,0,1}
    } };

    return R;
}

Matrica4x4 rotacijaY(double stepeni)
{
    double rad = stepeni * M_PI / 180.0;
    double c = cos(rad);
    double s = sin(rad);

    Matrica4x4 R =
    { {
        {c,0,s,0},
        {0,1,0,0},
        {-s,0,c,0},
        {0,0,0,1}
    } };

    return R;
}

Matrica4x4 rotacijaZ(double stepeni)
{
    double rad = stepeni * M_PI / 180.0;
    double c = cos(rad);
    double s = sin(rad);

    Matrica4x4 R =
    { {
        {c,-s,0,0},
        {s,c,0,0},
        {0,0,1,0},
        {0,0,0,1}
    } };

    return R;
}

Matrica4x4 okoluTocka(const Matrica4x4& T, double cx, double cy, double cz)
{
    Matrica4x4 A = translacija(-cx, -cy, -cz);
    Matrica4x4 B = translacija(cx, cy, cz);

    return mnoziMatrica(B, mnoziMatrica(T, A));
}

void transformirajOBJ(const string& vlez, const string& izlez, const Matrica4x4& T)
{
    ifstream in(vlez);
    ofstream out(izlez);

    out << setprecision(15);

    string linija;

    while (getline(in, linija))
    {
        istringstream iss(linija);
        string tip;
        iss >> tip;

        if (tip == "v")
        {
            double x, y, z;

            if (iss >> x >> y >> z)
            {
                Tocka3D p(x, y, z);
                Tocka3D q = primeniTransformacija(T, p);

                out << "v " << q.x << " " << q.y << " " << q.z << "\n";
            }
            else
            {
                out << linija << "\n";
            }
        }
        else
        {
            out << linija << "\n";
        }
    }
}

int main()
{
    string vlezFajl = "monkey.obj";
    string izlezFajl = "monkey_transformiran.obj";

    double tx = 2, ty = 0, tz = 0;

    double rotX = 30;
    double rotY = 0;
    double rotZ = 45;

    double sx = 1.5, sy = 1.5, sz = 1.5;

    double rcx = 0, rcy = 0, rcz = 0;

    double scx = 0, scy = 0, scz = 0;

    Matrica4x4 T = translacija(tx, ty, tz);

    Matrica4x4 RX = rotacijaX(rotX);
    Matrica4x4 RY = rotacijaY(rotY);
    Matrica4x4 RZ = rotacijaZ(rotZ);

    Matrica4x4 rotacijaOsnovna = mnoziMatrica(RZ, mnoziMatrica(RY, RX));
    Matrica4x4 R = okoluTocka(rotacijaOsnovna, rcx, rcy, rcz);

    Matrica4x4 skaliranjeOsnovno = skaliranje(sx, sy, sz);
    Matrica4x4 S = okoluTocka(skaliranjeOsnovno, scx, scy, scz);

    Matrica4x4 vkupno = mnoziMatrica(S, mnoziMatrica(R, T));

    transformirajOBJ(vlezFajl, izlezFajl, vkupno);

    cout << "Gotovo. Zacuvano vo monkey_transformiran.obj\n";

    return 0;
}