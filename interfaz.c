#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "map.h"


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
	printf("Pid: %d \n",(int)getpid());
	//Se crea la tubería por la cual se van a enviar los parámetros de búsqueda
	int pipe_search,pipe_result;
	mkfifo("/tmp/pipe_search",0666);
	pipe_search = open("/tmp/pipe_search",O_WRONLY);
	//Se abre la tubería por la cual se van a recibir las respuestas de las búsquedas
	pipe_result = open("/tmp/pipe_result",O_RDONLY);
	//Variables de búsqueda
	int option,sourceid=0,dstid=0,hod=0,end=-1;
	float result;
	
	
	
	do{
		printf("-----------------------------------------------\n");
		printf("Origen: %d, Destino: %d, Hora: %d \n",sourceid,dstid,hod);
		printf("1 - Ingresar origen.\n");
		printf("2 - Ingresar destino.\n");
		printf("3 - Ingresar hora.\n");
		printf("4 - Buscar tiempo de viaje medio.\n");
		printf("5 - Salir.\n");
		scanf("%d",&option);
		
		switch(option){
			case 1:
				printf("Ingrese el id de origen: ");
				scanf("%d",&sourceid);
				printf("\n");
				break;
			case 2:
				printf("Ingrese el id de destino: ");
				scanf("%d",&dstid);
				printf("\n");
				break;
			case 3:
				printf("Ingrese la hora de viaje: ");
				scanf("%d",&hod);
				printf("\n");
				break;
			case 4:
				//Se envían los parámetros de búsqueda al otro proceso
				write(pipe_search,&sourceid,sizeof(int));
				write(pipe_search,&dstid,sizeof(int));
				write(pipe_search,&hod,sizeof(int));
				//Se recibe el resulrado de la búsqueda
				read(pipe_result,&result,sizeof(float));
				if(result==-1){
					printf("NA \n");
				}else{
					printf("Resultado: %f \n",result);
				}	
				break;
			case 5:
				write(pipe_search,&end,sizeof(int));
				printf("Adios, vuelva pronto.\n");
				break;
		}
	}while(option!=5);
	
	return 0;

}
