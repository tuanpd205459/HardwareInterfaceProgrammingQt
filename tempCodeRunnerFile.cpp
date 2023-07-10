#include <iostream>
using namespace std;

int main() {
	
	// your code here
	int H,W,M,N,n;
    int sum =0, max =0;
    int i,j,a[100][100];
    cin >> n>> H >> W >>M>>N;
    for(i=0;i<M;i++)
        for(j=0;j<N;j++){
            cin >> a[i][j];
        }
    for(i=0;i<M-H;i++)
        for(j=0;j<N-W;j++){
         int   r= i;
         int   c = j;
         if(i==M-H-1 && j ==N-W-1) sum += a[i][j];
         if(sum> max) max = sum;
        }

cout >> sum;

	return 0;
}