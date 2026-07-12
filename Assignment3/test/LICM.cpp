
int LICM(){
    int b = 5;
    int c= 10;
    int i=20;
    do{
        int a = b + c;
        int g = 3 + 4;
        int e;
        if((i%2)==0){
            e = 2;
        }else {
            e = 3;
        }
        int d = a + 1;
        int f = e + 2;
        i=i-1;
    } while(i >= 0);
    return b;
}