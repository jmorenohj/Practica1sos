#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "map.h"


#define ARR_SIZE 1200

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





int table[ARR_SIZE];




int main(){
	printf("Pid: %d \n",(int)getpid());
	//Se abre la tubería por la cual se van a recibir los datos de búsqueda
	int pipe_search,pipe_result;
	pipe_search=open("/tmp/pipe_search",O_RDONLY);
	
	//Se crea la tubería por la cual se va a retornar el resultado
	mkfifo("/tmp/pipe_result",0666);
	pipe_result = open("/tmp/pipe_result",O_WRONLY);
	
	//Se abre el archivo auxiliar donde está el arreglo y el archivo indexado
	FILE *aux=fopen("aux.txt","rb");
	FILE *indexed=fopen("indexed.txt","rb");
	int val;
	hashmap* m = hashmap_create();
	
	//Se crea el arreglo o la tabla hash (dependiendo de lo que se quiera usar) 
	char key[6];
	for(int i=0;i<ARR_SIZE;i++){
		fread(&val,sizeof(int),1,aux);
		//printf("%d: %d \n",i,val);
		table[i]=val;
		if(val!=0){
			sprintf(key,"%d",i);
			hashmap_set(m,key,sizeof(key)-1,val);
		}
	
	}
	
	int source_id,dstid,hod,result;
	
	while(1){
	
		//se reciben los parámetros de búsqueda, si la primera es -1 es porque el usuario digitó 5 en la interfaz
		read(pipe_search,&source_id,sizeof(int));
		if(source_id==-1){
			printf("No hay mas búsquedas \n");
			return 0;
		}
		read(pipe_search,&dstid,sizeof(int));
		read(pipe_search,&hod,sizeof(int));
		
		//Se pasa el source_id a una string la cual es la llave de búsqueda
		sprintf(key,"%d",source_id);
		
		//Registro en el cual se guarda la búsqueda realizada
		Register prb;
		
		//Se accede a la primera primera posición de la lista encadenada en el archivo indexado con el source_id dado y se guarda en res
		uintptr_t res;
		hashmap_get(m,key,sizeof(key)-1,&res);
		int pos = table[source_id];
		//Variables para medir el tiempo de la búsqueda
		clock_t t_inicio,t_final;
		double segundos;
		t_inicio=clock();
		
		//se busca la primera posición de la lista encadenada en el archivo indexado
		fseek(indexed,pos,SEEK_SET);
		fread(&prb,sizeof(Register),1,indexed);
		
		//Se itera en la lista encadenada hasta que se encuentra el registro buscado
		int finded=0;
		while(prb.next_source_id!=0){
			//printf("%d %d %d %f\n ",prb.sourceid,prb.dstid,prb.hod,prb.mean_travel_time);
			fseek(indexed,prb.next_source_id,SEEK_SET);
			fread(&prb,sizeof(Register),1,indexed);
			if(prb.sourceid==source_id && prb.dstid==dstid && prb.hod==hod){
				finded=1;
				break;
			}
		}
		
		//Se calcula el tiempo de la búsqueda
		t_final=clock();
		segundos = (double)(t_final-t_inicio)/CLOCKS_PER_SEC;
		printf("\nBúsqueda realizada en: %f s\n",segundos);
		
		//Si se encontró, se retorna el valor de la búsqueda, si no se retorna -1
		if(finded==1){
			write(pipe_result,&prb.mean_travel_time,sizeof(float));
			printf("%d %d %d %f \n",prb.sourceid,prb.dstid,prb.hod,prb.mean_travel_time);
			printf("Se encontró \n");
		}else{
			prb.mean_travel_time=-1;
			write(pipe_result,&prb.mean_travel_time,sizeof(float));
			printf("No se encontró \n");
		}
	}
	

	
	

	return 0;
}
