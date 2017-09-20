#include <iostream>
#include "rmlib.h"

using namespace std;



int main(){

    cout<<rm_init(7500, 9000, "217.0.0.1", "217.0.0.1");
    rm_new("Jejeje", 777, 8,1);

    rm_delete("Nueva");

    rm_new("",0,0,0);

    return 0;
}
