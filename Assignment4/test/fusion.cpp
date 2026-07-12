
int Fusion(int a, int b){
    int result= 0;
    if(a > b){
        for(int i = 0; i < a; i++){
            result = result +  b;
        }
    }else{
        for(int i = 0; i < b; i++){
            result = result + a;
        }
    }
    int c = 0;
    int d = 0;
    for(int i = 0; i < 10; i++){
            c = a + b;
        }
    for(int i = 0; i < 10; i++){
            d = a * b;
        }
    
    return result;
}