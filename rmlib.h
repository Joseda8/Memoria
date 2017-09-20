#ifndef rmlib
#define rmlib

#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <typeinfo>


using namespace std;

char* ip;
char* ipPasivo;
int puerto;
int puertoPasivo;
bool server;
int client;

struct rmRef_h {
    int dato;
    int bytes;
    string key;
    struct rmRef_h *siguiente;
};

bool conectar(int port){
    int bufsize = 1024;
    char buffer[bufsize];

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
        return true;
        }else{
            close(client);
            return false;}

    recv(client, buffer, bufsize, 0);
    cout << "Conexión confirmada"<<endl;
}

bool rm_init (int port, int portHA, char* ip, char* ipHA){
    ip = ip;
    puerto = port;
    ipPasivo = ipHA;
    puertoPasivo = portHA;
    server = conectar(port);
    if(server==0){
        server=conectar(portHA);
    }
    return server;
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

string itoa(int n){
    	string rtn;
    	for(rtn="";n>0;rtn.insert(rtn.begin(),n%10+'0'),n/=10);
    	return rtn;
    }

void rm_new(char* llave, int valor, int largo, int seguir){

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

    strcat(dato_concat,"a");
    strcat(dato_concat,"-");
    strcat(dato_concat,llave);
    strcat(dato_concat,"-");
    dato = (char*)itoa(valor).c_str();
    strcat(dato_concat, dato);
    strcat(dato_concat,"-");
    bytes = (char*)itoa(largo).c_str();
    strcat(dato_concat,bytes);

    strcpy(buffer, dato_concat);

    send(client, buffer, 1024, 0);
    free(buffer);
}

class rm_get{
    private:
        char* key;
        int dato;
        int largo;
    public:
        rm_get(char *llave){
            int i;
            this->key=llave;
            char dato_concat[500];
            dato_concat[0]=NULL;
            char *buffer = (char*)malloc(1024 * sizeof(char));
            strcat(dato_concat, "b");
            strcat(dato_concat, "-");
            strcat(dato_concat, key);

            strcpy(buffer, dato_concat);

            send(client, buffer, 1024, 0);

            recv(client, buffer, 1024, 0);

            string datoStr = buscaCaracter(buffer, '-', 0);
            string largoStr = buscaCaracter(buffer, '-', 1);

            char* datoChar = (char*) malloc(i+1);
            char* largoChar = (char*) malloc(i+1);
            datoChar = (char *)datoStr.c_str();
            largoChar = (char*)largoStr.c_str();

            this->dato = atoi(datoChar);
            this->largo = atoi(largoChar);

            free(buffer);
    }

        ~rm_get(){
        }

        char* getKey(){
            return this->key;
        }

    int operator=(int var){
        return this->dato;
	}

	bool operator==(char* llave){
        if(strcmp(this->key,llave)==0){
            return true;
        }else{
            return false;
        }
	}

	bool operator!=(char* llave){
        if(strcmp(this->key,llave)==0){
            return false;
        }else{
            return true;
        }
	}
};

void rm_delete(rm_get nodo){
    char dato_concat[500];
    dato_concat[0]=NULL;
    char *buffer = (char*)malloc(1024 * sizeof(char));

    strcat(dato_concat, "d");
    strcat(dato_concat, "-");
    strcat(dato_concat, nodo.getKey());

    strcpy(buffer, dato_concat);

    send(client, buffer, 1024, 0);
    free(buffer);
}

#endif
