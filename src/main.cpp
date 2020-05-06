#include<iostream>
#include<vector>
#include"pythonarrays.h"
#include"c_arrays.h"
#include<fstream>

// void E_from_H()

void Linspace(vector<double> &vec, double min, double max)
{
  int N = vec.size();
  double length = max - min;
  double dx = length / N;
  double val = min;
  for(int i=0; i < vec.size(); i++){
    vec[i]=val;
    val+=dx;
  }
}

void MakeGaussian(array2d<double> &gaussian){
  // linspace x and y
  vector<double>x(gaussian.size_0, 0);
  Linspace(x,-1,1);

  vector<double>y(gaussian.size_1, 0);
  Linspace(y,-1,1);

  double w=0.01;
  for(int i=0; i < gaussian.size_0; i++){
    for(int j=0; j < gaussian.size_1; j++){
      gaussian(i,j) = exp(-pow(x[i],2)/w)*exp(-pow(y[j],2)/w);
    }
  }
}
struct E_s{
  array3d<double> E_x;
  array3d<double> E_y;
  array3d<double> E_z;
  int N_x;
  int N_y;
  int N_z;
  E_s(int N_x, int N_y, int N_z):
    N_x(N_x),
    N_y(N_y),
    N_z(N_z),
    E_x(N_x,N_y+1,N_z+1),
    E_y(N_x+1,N_y,N_z+1),
    E_z(N_x+1,N_y+1,N_z)
  {
    E_x.init(0);
    E_y.init(0);
    E_z.init(0);
  }
};
struct H_s{
  array3d<double> H_x;
  array3d<double> H_y;
  array3d<double> H_z;
  int N_x;
  int N_y;
  int N_z;
  H_s(int N_x, int N_y, int N_z):
    N_x(N_x),
    N_y(N_y),
    N_z(N_z),
    H_x(N_x+1,N_y,N_z),
    H_y(N_x,N_y+1,N_z),
    H_z(N_x,N_y,N_z+1)
    {
      H_x.init(0);
      H_y.init(0);
      H_z.init(0);
    }
};

struct Params{
  double dt;
  double eps0;
  double mu0;
  double dy;
  double dx;
  double dz;
};
struct J_s{
  array3d<double> J_x;
  array3d<double> J_y;
  array3d<double> J_z;
  int N_x;
  int N_y;
  int N_z;
  J_s(int N_x, int N_y, int N_z):
    N_x(N_x),
    N_y(N_y),
    N_z(N_z),
    J_x(N_x+1,N_y,N_z),
    J_y(N_x,N_y+1,N_z),
    J_z(N_x,N_y,N_z+1)
    {
      J_x.init(0);
      J_y.init(0);
      J_z.init(0);
    }

};

void E_from_H(E_s &E, H_s &H, J_s &J, Params &P){

  for(int i=1; i < E.N_x-1; i++){
    for(int j=1; j < E.N_y-1; j++){
      for(int k=1; k < E.N_z-1; k++){

        E.E_x(i,j,k) = E.E_x(i,j,k) +
          (P.dt/P.eps0)*(
            (H.H_z(i,j+1,k) - H.H_z(i,j,k))/P.dy
          - (H.H_y(i,j,k+1) - H.H_y(i,j,k))/P.dz
          - J.J_x(i,j,k)
          );

        E.E_y(i-1,j+1,k) = E.E_y(i-1,j+1,k) +
          (P.dt/P.eps0)*(
            (H.H_x(i-1,j+1,k+1) - H.H_x(i-1,j+1,k))/P.dz
          - (H.H_z(i,j+1,k) - H.H_z(i-1,j+1,k))/P.dx
          -J.J_y(i-1,j+1,k)
          );

        E.E_z(i-1,j,k+1) = E.E_z(i-1,j,k+1) +
          (P.dt*P.eps0)*(
            (H.H_y(i,j,k+1) - H.H_y(i-1,j,k+1))/P.dx
          - (H.H_x(i-1,j+1,k+1) - H.H_x(i-1,j,k+1))/P.dy
          -J.J_z(i-1,j,k+1)
          );

        // if(E.E_z(i-1,j,k+1) > 0){
        //   std::cout << "E_Z is greater than 0" << std::endl;
        //   std::cout << "E.E_z(i-1,j,k+1)" << " => " << E.E_z(i-1,j,k+1) << std::endl;
        // }
        // if(E.E_y(i-1,j+1,k) > 0){
        //   cout<<"E_Y is greater than 0"<<endl;
        //   std::cout << "E.E_y(i-1,j+1,k)" << " => " << E.E_y(i-1,j+1,k) << std::endl;
        // }
        // if(E.E_x(i,j,k)>0){
        //   cout<<"E_x is greater than 0"<<endl;
        //   std::cout << "E.E_x(i,j,k)" << " => " << E.E_x(i,j,k) << std::endl;
        //   std::cout << "(P.dt/P.eps0)" << " => " << (P.dt/P.eps0) << std::endl;
        //   // cout << "P.dx => " << P.dx << endl;
        //   // cout << "P.dy => " << P.dy << endl;
        //   // cout << "P.dz => " << P.dz << endl;

        // }
      }
    }
  }
}
void H_from_E(H_s &H, E_s &E, Params &P){

  for(int i=1; i < E.N_x-1; i++){
    for(int j=1; j < E.N_y-1; j++){
      for(int k=1; k < E.N_z-1; k++){

        H.H_x(i-1,j+1,k+1) = H.H_x(i-1,j+1,k+1) +
          (P.dt/P.mu0)*(
            ((E.E_y(i-1,j+1,k+1) - E.E_y(i-1,j+1,k))/P.dz)
          - ((E.E_z(i-1,j+1,k+1) - E.E_z(i-1,j,k+1))/P.dy)
          );

        H.H_y(i,j,k+1) = H.H_y(i,j,k+1) +
          (P.dt/P.mu0)*(
            ((E.E_z(i,j,k+1) - E.E_z(i-1,j,k+1))/P.dx)
          - ((E.E_x(i,j,k+1) - E.E_x(i,j,k))/P.dz)
          );

        H.H_z(i,j+1,k) = H.H_z(i,j+1,k) +
          (P.dt/P.mu0)*(
            ((E.E_x(i,j+1,k) - E.E_x(i,j,k))/P.dy)
          - ((E.E_y(i,j+1,k) - E.E_y(i-1,j+1,k))/P.dx)
          );

        // if(H.H_x(i-1,j+1,k+1)>0){
        //   cout << "H.H_x(i-1,j+1,k+1) => " << H.H_x(i-1,j+1,k+1) << endl;
        //   cout << "(P.dt/P.mu0) => " << (P.dt/P.mu0) << endl;
        //   // cout << "P.dx => " << P.dx << endl;
        //   // cout << "P.dy => " << P.dy << endl;
        //   // cout << "P.dz => " << P.dz << endl;
        // }
        // if(H.H_y(i,j,k+1)>0){
        //   cout << "H.H_y(i,j,k+1) => " << H.H_y(i,j,k+1) << endl;
        //   cout << "(P.dt/P.mu0) => " << (P.dt/P.mu0) << endl;
        //   // cout << "P.dx => " << P.dx << endl;
        //   // cout << "P.dy => " << P.dy << endl;
        //   // cout << "P.dz => " << P.dz << endl;
        // }
        // if(H.H_z(i,j+1,k)){
        //   cout << "H.H_z(i,j+1,k) => " << H.H_z(i,j+1,k) << endl;
        //   cout << "(P.dt/P.mu0) => " << (P.dt/P.mu0) << endl;
        //   // cout << "P.dx => " << P.dx << endl;
        //   // cout << "P.dy => " << P.dy << endl;
        //   // cout << "P.dz => " << P.dz << endl;
        // }




      }
    }
  }
}
int main()
{
  std::cout << "hello" << std::endl;
  // make arrays
  int N_x = 201;
  int N_y = 201;
  int N_z = 3;

  E_s E(N_x, N_y, N_z);
  H_s H(N_x, N_y, N_z);
  J_s J(N_x, N_y, N_z);

  double c=2.998e8;
  Params P;
  P.mu0 = 4*M_PI*10e-7;
  P.eps0 = 8.85e-12;
  P.dx = 50e-9;
  P.dy = 50e-9;
  P.dz = 50e-9;
  P.dt = (50e-9)/2*c;

  // double c=1;
  // Params P;
  // P.mu0 = 1;
  // P.eps0 = 1;
  // P.dx = 1;
  // P.dy = 1;
  // P.dz = 1;
  // P.dt = 0.1;

  PythonInterp Python("/home/zom/Projects/diffraction_net/venv/", "utility");

  array2d<double> gaussian(N_x+1,N_y);
  MakeGaussian(gaussian);


  // Python.call_function_np("plotgaussian", gaussian.data, vector<int>{gaussian.size_0,gaussian.size_1}, PyArray_FLOAT64);
  // exit(0);

  double t=0;
  for(int i=0;i<300;i++){
    t+=P.dt*1e-10;

    std::cout << "made it here" << std::endl;
    for(int i=0; i < J.J_x.size_0; i++){
      for(int j=0; j < J.J_x.size_1; j++){
        for(int k=0; k < J.J_x.size_2; k++){
          // J.J_x(i,j,k) = cos(t)*gaussian(i,j);
          J.J_x(i,j,k) = 1*gaussian(i,j);
        }
      }
    }
    std::cout << "made it here 2" << std::endl;

    E_from_H(E, H, J, P);
    H_from_E(H, E, P);


    Python.call_function_np("plot", H.H_z.data, vector<int>{H.H_z.size_0,H.H_z.size_1,H.H_z.size_2}, PyArray_FLOAT64);
    Python.call("show");

    // getchar();

  }




}



