//#include <iostream>


int fun(int a, int b){
    int c = 0;
    while( b >= 0){
        c = c + a;
        b = b - 1;
    }
    return c;
}