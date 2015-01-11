#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define CHAIKIN 0
#define CORNER_CUTTING_1 1
#define CORNER_CUTTING_2 2
#define FOURPOINT 3
#define FOURPOINT_1 4
#define FOURPOINT_2 5
#define UNIFORM_SPLINE_3 6

#define FOURPOINT_GENERALIZED 10
#define CORNER_CUTTING_GENERALIZED 11

#define WEIGHT_B_PRESET_1 0.52
#define WEIGHT_A_PRESET_1 0.41

#define WEIGHT_B_PRESET_2 0.13
#define WEIGHT_A_PRESET_2 (WEIGHT_B_PRESET_2 + 0.5)

#define EPSILON_PRESET_1 0.74
#define EPSILON_PRESET_2 1.23


typedef struct s_Point {
  double x, y;
} Point;

#define MAX_VERTICES 10000

Point P1[MAX_VERTICES];
Point P2[MAX_VERTICES];

void PostscriptSetColor( FILE *f, double r, double g, double b)
{
  fprintf( f, "%lf %lf %lf setrgbcolor\n", r, g, b);
}

void PostscriptSetLineWidth( FILE *f, double w)
{
  fprintf( f, "%lf setlinewidth\n", w);
}

void PostscriptHeader( FILE *f)
{
  fprintf( f, "%%!PS\n");
}

void PostscriptEoF( FILE *f)
{
  fprintf( f, "showpage\n");
}

void PostscriptOutputClosedPolygon( FILE *f, int N, Point *P)
{
  int i;

  fprintf( f, "%lf %lf moveto\n", P[0].x, P[0].y);
  for (i=1; i<N; i++)
  {
    fprintf( f, "%lf %lf lineto\n", P[i].x, P[i].y);
  }
  fprintf( f, "%lf %lf lineto\n", P[0].x, P[0].y);
  fprintf( f, "stroke\n");
}


void GnuplotOutputClosedPolygon( char *filename, int N, Point *P)
{
  FILE *fout;

  int i;

  fout = fopen( filename, "w");

  for (i=0; i<N; i++)
  {
    fprintf( fout, "%lf %lf\n", P[i].x, P[i].y);
  }
  fprintf( fout, "%lf %lf\n", P[0].x, P[0].y);

  fclose(fout);
}

double AngleThreePoints(  Point *A, Point *B, Point *C)
{
  double V1X, V1Y, V2X, V2Y;
  double N1, N2, ScalarProduct, Angle;

  V1X = B->x - A->x;
  V1Y = B->y - A->y;
  N1 = sqrt( V1X * V1X + V1Y * V1Y);
  V1X /= N1;
  V1Y /= N1;


  V2X = C->x - B->x;
  V2Y = C->y - B->y;
  N2 = sqrt( V2X * V2X + V2Y * V2Y);
  V2X /= N2;
  V2Y /= N2;

  ScalarProduct = V1X * V2X + V1Y * V2Y;

  Angle = acos( ScalarProduct);
  if (ScalarProduct < 0.) {Angle = -Angle;}

  Angle = Angle / (N1 + N2);

  return Angle;
}


void ComputeAngle( int N, Point *P, double *Angle)
{
  int i;

  Angle[0] = AngleThreePoints( &P[N-1], &P[0], &P[1]);

  for (i=1; i<N-1; i++)
  {
    Angle[i] = AngleThreePoints( &P[i-1], &P[i], &P[i+1]);
  }

  Angle[i] = AngleThreePoints( &P[N-2], &P[N-1], &P[0]);
}

void GnuplotOutputAngle( char *filename, int N, Point *P)
{
  double Angle[MAX_VERTICES];
  FILE *fout;
  int i;

  ComputeAngle( N, P, Angle);


  fout = fopen( filename, "w");


  for (i=0; i<N; i++)
  {
    fprintf( fout, "%lf %lf\n", ((double) i)/(N-1), Angle[i]);
  }

  fclose(fout);

}

void CopyPolygon( int N, Point *P, Point *Q)
{
  int i;

  for (i=0; i<N; i++)
  {
    Q[i] = P[i];
  }
}

double PointDistance( Point *P, Point *Q)
{
  double vx, vy;

  vx = P->x - Q->x;
  vy = P->y - Q->y;

  return(sqrt(vx * vx + vy * vy));
}

double Max( double a, double b)
{
  if (a>b)
  {
    return a;
  }

  return b;
}

double LMaxDistance( int N, Point *P, Point *Q)
{
  double d, di;
  int i;
  Point MidPoint;

  d = 0.;

  for (i=0; i<N-1; i++)
  {
    di = PointDistance( &P[i], &Q[2*i]);
    d = Max( d, di);

    MidPoint.x = 0.5 * (P[i].x + P[i+1].x);
    MidPoint.y = 0.5 * (P[i].y + P[i+1].y);

    di = PointDistance( &MidPoint, &Q[2*i+1]);
    d = Max( d, di);
  }

// CLOSING EDGE

  di = PointDistance( &P[i], &Q[2*i]);
  d = Max( d, di);

  MidPoint.x = 0.5 * (P[i].x + P[0].x);
  MidPoint.y = 0.5 * (P[i].y + P[0].y);

  di = PointDistance( &MidPoint, &Q[2*i+1]);
  d = Max( d, di);

  return(d);
}

void CornerCuttingSubdivisionClosedPolygon( double A, double B, int N, Point *P, Point *Q)
{
  int i;

  for (i=0; i<N-1; i++)
  {
    Q[2*i].x = A * P[i].x + (1. - A) * P[i+1].x;
    Q[2*i].y = A * P[i].y + (1. - A) * P[i+1].y;
    Q[2*i+1].x = B * P[i].x + (1. - B) * P[i+1].x;
    Q[2*i+1].y = B * P[i].y + (1. - B) * P[i+1].y;
  }

  Q[2*i].x = A * P[i].x + (1. - A) * P[0].x;
  Q[2*i].y = A * P[i].y + (1. - A) * P[0].y;
  Q[2*i+1].x = B * P[i].x + (1. - B) * P[0].x;
  Q[2*i+1].y = B * P[i].y + (1. - B) * P[0].y;
}

void ChaikinSubdivisionClosedPolygon( int N, Point *P, Point *Q)
{
  int i;

  for (i=0; i<N-1; i++)
  {
    Q[2*i].x = 3./4. * P[i].x + 1./4. * P[i+1].x;
    Q[2*i].y = 3./4. * P[i].y + 1./4. * P[i+1].y;
    Q[2*i+1].x = 1./4. * P[i].x + 3./4. * P[i+1].x;
    Q[2*i+1].y = 1./4. * P[i].y + 3./4. * P[i+1].y;
  }

  Q[2*i].x = 3./4. * P[i].x + 1./4. * P[0].x;
  Q[2*i].y = 3./4. * P[i].y + 1./4. * P[0].y;
  Q[2*i+1].x = 1./4. * P[i].x + 3./4. * P[0].x;
  Q[2*i+1].y = 1./4. * P[i].y + 3./4. * P[0].y;
}

Point weightedSum (Point a, Point b, float normalizedWeight)
{
  Point ret;
  ret.x = normalizedWeight * a.x  + (1. - normalizedWeight) * b.x;
  ret.y = normalizedWeight * a.y  + (1. - normalizedWeight) * b.y;
  return ret;
}

int mod(int a, int b)
{
  int r = a % b;
  return r < 0 ? r + b : r;
}

void fourPointScheme (int N, Point *P, Point *Q, float epsilon)
{
  int i;

  for (i=0; i<N; i++)
  {
    Q[2*i].x = P[i].x;
    Q[2*i].y = P[i].y;
    Point tmp1 = weightedSum(P[mod(i-1, N)], P[(i+2)%N], 0.5);
    Point tmp2 = weightedSum(P[i], P[(i+1)%N], 0.5);
    Q[2*i+1] = weightedSum(tmp1, tmp2, -epsilon);
  }
}

void uniformSpline3 (int N, Point *P, Point *Q)
{
  int i;

  for (i=0; i<N; i++)
  {
    Q[2*i].x = (P[i].x + P[(i+1)%N].x) / 2.;
    Q[2*i].y = (P[i].y + P[(i+1)%N].y) / 2.;
    Q[2*i+1].x = 1./8. * (P[i].x + 6. * P[(i+1)%N].x + P[(i+2)%N].x);
    Q[2*i+1].y = 1./8. * (P[i].y + 6. * P[(i+1)%N].y + P[(i+2)%N].y);
  }
}

void printSelectedSubdivision (int code, float epsilon, float A, float B) {
  switch(code) {
  case CHAIKIN:
    printf("Chaikin\n");
    break;
  case CORNER_CUTTING_1:
    printf("Corner Cutting preset 1: A=%f B=%f\n", WEIGHT_A_PRESET_1, WEIGHT_B_PRESET_1);
    break;
  case CORNER_CUTTING_2:
    printf("Corner Cutting preset 2: A=%f B=%f\n", WEIGHT_A_PRESET_2, WEIGHT_B_PRESET_2);
    break;
  case FOURPOINT:
    printf("Four Point\n");
    break;
  case FOURPOINT_1:
    printf("Four Point preset 1: epsilon=%f\n", EPSILON_PRESET_1);
    break;
  case FOURPOINT_2:
    printf("Four Point preset 2: epsilon=%f\n", EPSILON_PRESET_2);
    break;
  case FOURPOINT_GENERALIZED:
    printf("Generalized four points epsilon=%f\n", epsilon);
    break;
  case CORNER_CUTTING_GENERALIZED:
    printf("Generalized corner cutting A=%f B=%f\n", A, B);
    break;
  case UNIFORM_SPLINE_3:
    printf("Uniform spline 3\n");
    break;
  default:
    printf("Subdivision code does not exist\n");
    break;
  }
}
int main(int argc, char *argv[])
{
  int subdivisionToPlot = CHAIKIN;
  float epsilon, weightA, weightB;
  if (argc > 1) {
    subdivisionToPlot = atoi(argv[1]);
    if (subdivisionToPlot == FOURPOINT_GENERALIZED) {
      if (argc <= 2) {
        printf("Generalized fourpoint required an additional float argument epsilon\n");
        return;
      }
      epsilon = atof(argv[2]);
    }
    else if (subdivisionToPlot == CORNER_CUTTING_GENERALIZED) {
      if (argc <= 3) {
        printf("Generalized corner cutting required two additional float arguments A and B\n");
        return;
      }

      weightA = atof(argv[2]);
      weightB = atof(argv[3]);
    }
  }

  FILE *fout;
  char FileName[100];

  int N, i;
  int totalIteration = 7;


  fout = fopen( "results/PostscriptOutput.ps", "w");

  N = 4;

  P1[0].x = 100.;
  P1[0].y = 300.;
  P1[1].x = 500.;
  P1[1].y = 300.;
  P1[2].x = 500.;
  P1[2].y = 500.;
  P1[3].x = 300.;
  P1[3].y = 700.;

  PostscriptHeader( fout);
  PostscriptSetLineWidth( fout, 1.0);
  PostscriptSetColor( fout, 1., 0., 0.);

  PostscriptOutputClosedPolygon( fout, 4, P1);
  GnuplotOutputClosedPolygon( "results/Subdivision0.txt", 4, P1);

  printSelectedSubdivision(subdivisionToPlot, epsilon, weightA, weightB);
  for (i=0; i<totalIteration; i++) {
    switch(subdivisionToPlot) {
    case CHAIKIN:
      ChaikinSubdivisionClosedPolygon( N, P1, P2);
      break;
    case CORNER_CUTTING_1:
      CornerCuttingSubdivisionClosedPolygon( WEIGHT_A_PRESET_1, WEIGHT_B_PRESET_1 , N, P1, P2);
      break;
    case CORNER_CUTTING_2:
      CornerCuttingSubdivisionClosedPolygon( WEIGHT_A_PRESET_2, WEIGHT_B_PRESET_2 , N, P1, P2);
      break;
    case FOURPOINT:
      fourPointScheme(N, P1, P2, 1./8.);
      break;
    case FOURPOINT_1:
      fourPointScheme(N, P1, P2, EPSILON_PRESET_1);
      break;
    case FOURPOINT_2:
      fourPointScheme(N, P1, P2, EPSILON_PRESET_2);
      break;
    case UNIFORM_SPLINE_3:
      uniformSpline3 (N, P1, P2);
      break;
    case FOURPOINT_GENERALIZED:
      fourPointScheme(N, P1, P2, epsilon);
      break;
    case CORNER_CUTTING_GENERALIZED:
      CornerCuttingSubdivisionClosedPolygon( weightA, weightB , N, P1, P2);
      break;
    default:
      ChaikinSubdivisionClosedPolygon( N, P1, P2);
      break;
    }


    PostscriptOutputClosedPolygon( fout, 2 * N, P2);

    sprintf( FileName, "results/Subdivision%d.txt", i);
    GnuplotOutputClosedPolygon( FileName, 2 * N, P2);
    sprintf( FileName, "results/AngleSubdivision%d.txt", i);
    GnuplotOutputAngle( FileName, 2 * N, P2);

    printf( "Distance Subdivision: %lf\n", LMaxDistance( N, P1, P2));

    N = N * 2;

    CopyPolygon( N, P2, P1);
  }

  fclose( fout);
}
