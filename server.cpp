#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <typeinfo>
#include <thread>
#include <mutex>

using namespace std;

struct rmRef_h {
    int referencias;
    int dato;
    int bytes;
    string key;
    struct rmRef_h *siguiente;
};

typedef struct rmRef_h* Tlista;


void reportarLista(Tlista lista){
     while(lista != NULL){
        cout<<lista->key<<"  Numero de referencias: "<<lista->referencias<<endl;
          lista = lista->siguiente;
     }
}

void insertar(Tlista &lista, int valor, string llave, int largo){
    Tlista t, q = new(struct rmRef_h);

    q->dato = valor;
    q->key = llave;
    q->bytes = largo;
    q->siguiente = NULL;

    if(lista==NULL){
        lista = q;
    }else{
        t = lista;
        while(t->siguiente!=NULL){
            t = t->siguiente;
        }
        t->siguiente = q;
    }
}

rmRef_h* buscarElemento(Tlista lista, char* valor, int num){
    Tlista q = lista;
    int i = 1, varAux = 0;
    while(q!=NULL){
        if(q->key==valor){
            q->referencias+=num;
            return q;
            varAux = 1;
        }
        q = q->siguiente;
        i++;
    }
    if(varAux==0)
        return NULL;
}

bool recolectorBasura(Tlista &lista){
    Tlista p, ant;
    p = lista;

    if(lista!=NULL){
        do{
            if(p->referencias<=0){
                if(p==lista){
                    lista = lista->siguiente;
                }else{
                    ant->siguiente = p->siguiente;
                    }

                delete(p);
                return true;
            }
            ant = p;
            p = p->siguiente;
        }while(p!=NULL);
    }
    else{
        false;}
}

int largoLista(Tlista lista){
    int n = 0;
    while(lista != NULL){
        n++;
        lista = lista->siguiente;
     }
     return n;
}

Tlista lista = NULL;
rmRef_h cache[5];

string itoa(int n){
    	string rtn;
    	for(rtn="";n>0;rtn.insert(rtn.begin(),n%10+'0'),n/=10);
    	return rtn;
    }

string buscaCaracter (char *cadena, char caracter, int num){
    int i = 0;
    int conteo = 0;
    int *ubicacion = (int*)malloc(2 * sizeof(int));

    while (cadena[i]!='\0'){
        if((cadena[i] == caracter)){
            ubicacion[conteo]=i;
            conteo++;
        }
        i++;
    }

    switch(num){
        case 0:{
            string str(cadena);
            str=str.substr(0,ubicacion[0]);
            return str;
        }
        case 1:{
            string str(cadena);
            str=str.substr(ubicacion[0]+1, ubicacion[1]-ubicacion[0]-1);
            return str;
        }
        case 2:{
            string str(cadena);
            str=str.substr(ubicacion[1]+1, ubicacion[2]-ubicacion[1]-1);
            return str;
        }
        case 3:{
            string str(cadena);
            str=str.substr(ubicacion[2]+1, ubicacion[3]-ubicacion[2]-1);
            return str;
        }
        case 4:{
            string str(cadena);
            str=str.substr(ubicacion[3]+1, str.size());
            return str;
        }
    }
}

void rm_new(string llave, int valor, int largo, int seguir, int client){
    int i;
    char dato_concat[500];
    dato_concat[0]=NULL;
    char *buffer = (char*)malloc(1024 * sizeof(char));

    if(seguir==0){
        strcpy(buffer, "*");
        send(client,buffer, 1024, 0);
        close(client);
        return;
    }

    char* bytes = (char*) malloc(i+1);
    char* dato = (char*) malloc(i+1);

    strcat(dato_concat,"e");
    strcat(dato_concat,"-");
    strcat(dato_concat,llave.c_str());
    strcat(dato_concat,"-");
    dato = (char*)itoa(valor).c_str();
    strcat(dato_concat, dato);
    strcat(dato_concat,"-");
    bytes = (char*)itoa(largo).c_str();
    strcat(dato_concat,bytes);

    strcpy(buffer, dato_concat);

    cout<<buffer<<endl;
    send(client, buffer, 1024, 0);
    free(buffer);
}

void sincronizarServer(Tlista lista, int cliente){
    while(lista != NULL){
        rm_new(lista->key, lista->dato, lista->bytes, 1, cliente);
        lista = lista->siguiente;
     }
     rm_new("",0,0,0, cliente);
}

bool conectarServer(int port){
    int client;

    struct sockaddr_in server_addr;

    client = socket(AF_INET, SOCK_STREAM, 0);

    if (client < 0){
        cout << "\nError" << endl;
        return false;
        exit(1);
    }

    cout << "\Cliente creado" << endl;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (connect(client,(struct sockaddr *)&server_addr, sizeof(server_addr)) == 0){
        cout << "Conectado al puerto: " << port << endl;
        sincronizarServer(lista, client);
        close(client);
        return true;
    }else{
        close(client);
        return false;}
}

void atenderCliente(int server){

    while(true){
        int i=0;
        int bufsize = 1024;
        char buffer[bufsize];

        recv(server, buffer, bufsize, 0);

        string tipoStr = buscaCaracter(buffer, '-', 0);
        string keyStr = buscaCaracter(buffer, '-', 1);
        string datoStr = buscaCaracter(buffer, '-', 2);
        string largoStr = buscaCaracter(buffer, '-', 3);

        char* tipo = (char*) malloc(i+1);
        char* key = (char*) malloc(i+1);
        char* datoChar = (char*) malloc(i+1);
        char* largoChar = (char*) malloc(i+1);

        tipo = (char *)tipoStr.c_str();
        key = (char *)keyStr.c_str();
        datoChar = (char *)datoStr.c_str();
        largoChar = (char*)largoStr.c_str();

        int dato = atoi(datoChar);
        int largo = atoi(largoChar);

        if(*buffer=='*'){
            break;
        }

        if(*tipo=='a'){
            insertar(lista, dato, key, largo);
        }else if(*tipo=='b'){
            if(buscarElemento(lista, key, 0)!=NULL){
                char dato_concat[500];
                dato_concat[0]=NULL;
                rmRef_h* nodo=buscarElemento(lista, key, 1);
                int num=nodo->dato;
                int largo=nodo->bytes;

                strcat(dato_concat, (char*)itoa(num).c_str());
                strcat(dato_concat, "-");
                strcat(dato_concat, (char*)itoa(largo).c_str());
                strcpy(buffer, dato_concat);
                send(server, buffer, 1024, 0);
            }
        }else if(*tipo=='c'){
            string var = buscarElemento(lista, key, 0)->key;
            strcpy(buffer, (char*)var.c_str());
            send(server, buffer, 1024, 0);
        }else if(*tipo=='d'){
            buscarElemento(lista, key, -1);
        }else if(*tipo=='e'){
            insertar(lista, dato, key, largo);
            buscarElemento(lista, key, 1);
        }
    }
}

int conectarCliente(){
    int client, server;
    int portNum = 9000;

    struct sockaddr_in server_addr;
    socklen_t size;

    client = socket(AF_INET, SOCK_STREAM, 0);

    if (client < 0){
        cout << "\nError" << endl;
        exit(1);
    }

    cout << "Socket creado" << endl;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(portNum);

    if ((bind(client, (struct sockaddr*)&server_addr,sizeof(server_addr))) < 0){
        cout << "El socket ya está abierto" << endl;
        close(client);
        exit(1);
        return -1;
    }

    size = sizeof(server_addr);

    cout << "Esperando clientes" << endl;

    listen(client, 1);

    int clientCount = 1;
    server = accept(client,(struct sockaddr *)&server_addr,&size);

    if (server < 0)
        cout << "Error al aceptar" << endl;

    while (true){
        cout << "\nConectado con el cliente #" << clientCount << endl;

        atenderCliente(server);

        cout << "\nConexión terminada en la IP " << inet_ntoa(server_addr.sin_addr)<<"\n\n\n";

        close(client);
        close(server);
        close(client);
        close(server);
        break;
    }
}

int main(){
    while(true){
    /*    int sinc;
        cout<<"Sincronizar server: "<<endl;
        cin >> sinc;
        if(sinc==1){
            conectarServer(7500);
        }*/
        conectarCliente();
        reportarLista(lista);
        recolectorBasura(lista);
    }
    return 0;
}


