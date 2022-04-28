#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define ARR_SIZE 1200

//Formato de la struct que se almacena en el indexado
typedef struct Register{
	int sourceid; 
	int dstid;
	int hod;
	float mean_travel_time;
	float standard_deviation_travel_time;
	float geometric_mean_travel_time;
	float geometric_standard_deviation_travel_time;
	int next_source_id;
}Register;

int main(){
	//Se abre el archivo que contiene los datos
	FILE *data = fopen("datos.csv","r");
	//Se crea el archivo indexado
	FILE *indexed = fopen("indexed.txt","wb");
	//Las siguientes variables permiten la lectura del fichero
	char *line = NULL;
	size_t len=0;
	getline(&line,&len,data);
	int cont=0;
	char delimiter[]=",";
	//Este arreglo se crea para almacenar la primera posición de cada id en la linked list
	int *ids =(int*)calloc(ARR_SIZE,sizeof(int));

	//Se situa el puntero del archivo indexado en 1 para que ningún registro quede guardado en la posición 0 
	fseek(indexed,1,0);
	//Se itera sobre los registros para procesarlos
	while(getline(&line,&len,data)!=-1 && cont<10000000){
		cont++;
		//Las siguientes lineas almacenan los valores de cada registro en un struct
		Register regist;
		char *value = strtok(line,delimiter);regist.sourceid=atoi(value);
		value=strtok(NULL,delimiter);regist.dstid=atoi(value);
		value=strtok(NULL,delimiter);regist.hod=atoi(value);
		value=strtok(NULL,delimiter);regist.mean_travel_time=atof(value);
		value=strtok(NULL,delimiter);regist.standard_deviation_travel_time=atof(value);
		value=strtok(NULL,delimiter);regist.geometric_mean_travel_time=atof(value);
		value=strtok(NULL,delimiter);regist.geometric_standard_deviation_travel_time=atof(value);
		//Se guarda la posición del registro anterior con el mismo id añadiendo nodos a la lista 
		regist.next_source_id=*(ids+regist.sourceid);
		//Se guarda en el arreglo la posición en que queda guardado el registro
		*(ids+regist.sourceid)=(int)ftell(indexed);
		//Se guarda el registro en el archivo
		fwrite(&regist,sizeof(Register),1,indexed);
	}

	fclose(indexed);
	//Se guarda el arreglo en un archivo auxiliar para su uso posterior (crear tabla hash o hacer el papel de la tabla hash) 
	FILE *aux=fopen("aux.txt","wb");
	for(int i=0;i<ARR_SIZE;i++){
		fwrite(&*(ids+i),sizeof(int),1,aux);
	}
	
	return 0;
}



