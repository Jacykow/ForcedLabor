#include <iostream>

using namespace std;

int main()
{
    int matrixSize;
    cin>>matrixSize;
    double matrixA[matrixSize][matrixSize];
    double matrixB[matrixSize][matrixSize];
    for(int y=0;y<matrixSize;y++){
        for(int x=0;x<matrixSize;x++){
            cin>>matrixA[y][x];
        }
    }
    for(int y=0;y<matrixSize;y++){
        for(int x=0;x<matrixSize;x++){
            cin>>matrixB[y][x];
        }
    }
    for(int y=0;y<matrixSize;y++){
        for(int x=0;x<matrixSize;x++){
            if(x != 0)cout<<" ";
            cout<<matrixA[y][x]+matrixB[y][x];
        }
        cout<<endl;
    }
    return 0;
}