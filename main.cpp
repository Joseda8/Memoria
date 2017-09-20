#include <iostream>
#include "rmlib.h"

using namespace std;

int main(){

    rm_init(7500, 9000, "217.0.0.1", "217.0.0.1");
    rm_new("Palabra", 25, 8,1);
    rm_new("Nueva", 78, 3,1);

/*
    bool valor = (instancia!="Nueva");
    cout<<valor<<endl;
*/


    rm_get instancia("Nueva");
    int num = (instancia=6);
    cout<<num<<endl;

    rm_delete(instancia);

    cout<<instancia.getKey()<<endl;

    rm_new("",0,0,0);

    return 0;
}

//Crear logica del segundo servidor
