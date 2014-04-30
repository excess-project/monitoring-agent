
/**
 * \file
 * \brief monitoring - excess project 
 * Salam Traboulsi 
 *
 * ============================================================================
 * ============================================================================
 */


#include "monitoring-excess.h"    /* types and structures specific to the component */



/* Global Var */





/*****************************************************************
 * defined functions to get CPU and memory collected information 
 *****************************************************************/


int32_t get_mem_info(mem_i *infos_mem)
{
  int i;
  char buffer[INPUT_SIZE+1];
  FILE *tmp=NULL;
  char *ptr;

  char *com="top -b -n1|grep --regexp=Mem --regexp=Swap ";
  char ** str_ptr = (char **) alloca(sizeof(char*)*100);
   
  


  infos_mem->memory_total=0;
  infos_mem->memory_used=0;
  infos_mem->memory_free=0;
  infos_mem->memory_buffers=0;
  infos_mem->swap_total=0;
  infos_mem->swap_used=0;
  infos_mem->swap_free=0;
  infos_mem->swap_cached=0;




  struct timeval tv1,tv2;
  struct timezone tz;
  long long diff;


  gettimeofday(&tv1, &tz);

           		
         

 



   if ((tmp = popen(com, "r")) == NULL)
     {
       perror("popen");
       return -1;
     }




  gettimeofday(&tv2, &tz);
  diff=(tv2.tv_sec-tv1.tv_sec) * 1000000L + \
           (tv2.tv_usec-tv1.tv_usec);    

  infos_mem->diff_mem=diff;
  infos_mem->mem_time= tv1 ; 


  fgets(buffer,INPUT_SIZE,tmp);
  ptr=strtok_r(buffer," ",str_ptr );
  i=0;
  while (i<4)
    {
      ptr=strtok_r(*str_ptr," ",str_ptr );
      if( strchr (ptr, 'k')!=NULL)
	  {
	      if (ptr!=NULL )
	        {
	          switch (i)
	            {
		            case(0) : infos_mem->memory_total=atoi(ptr);
			      break;
		            case(1) : infos_mem->memory_used=atoi(ptr);
			      break ;
		            case(2) : infos_mem->memory_free=atoi(ptr);
      			      break;
	    		    case(3) : infos_mem->memory_buffers=atoi(ptr);
      			      break;
            		    default:
              			;
            	    }
          		i++;
        	}
  	}
    }

  	fgets(buffer,INPUT_SIZE,tmp);
  	ptr=strtok_r(buffer," ",str_ptr );

 	 i=0;
 	 while (i<4)
    	{
      		ptr=strtok_r(*str_ptr," ",str_ptr );
      		if( strchr (ptr, 'k')!=NULL)
  		{
      			if (ptr!=NULL )
        		{	
          			switch (i)
            			{
            				case(0) : infos_mem->swap_total=atoi(ptr);
      					break;
            				case(1) : infos_mem->swap_used=atoi(ptr);
      					break ;
            				case(2) : infos_mem->swap_free=atoi(ptr);
      					break;
	   				case(3) : infos_mem->swap_cached=atoi(ptr);
      					break;
            				default:
              				;
            			}
          				i++;
        			}
  		}
    	}


  	pclose(tmp);
	

  return 0;
	}



int32_t get_cpu_info(cpu_i *infos_cpu)
{
  int i;
  char buffer[INPUT_SIZE+1];
  FILE *tmp=NULL;
  char *ptr;

  char *com="top  -b";
  char ** str_ptr = (char **) alloca(sizeof(char*)*100);
   

	/*** Timer */
	struct timeval tv1,tv2;
        struct timezone tz;
	long long diff;


     
  infos_cpu->cpu_user=0;
  infos_cpu->cpu_sys=0;
  infos_cpu->cpu_nice=0;
  infos_cpu->cpu_idle=0;
  infos_cpu->cpu_wa_io=0 ; 

gettimeofday(&tv1, &tz);

           		
         

  if ((tmp = popen(com, "r")) == NULL)
    {
      perror("popen");
      return -1;

    }

		gettimeofday(&tv2, &tz);
  		diff=(tv2.tv_sec-tv1.tv_sec) * 1000000L + \
           (tv2.tv_usec-tv1.tv_usec);    

 	infos_cpu->diff_cpu=diff;
	
  	infos_cpu->cpu_time=tv1 ; 


  fgets(buffer,INPUT_SIZE,tmp);
  fgets(buffer,INPUT_SIZE,tmp);
  fgets(buffer,INPUT_SIZE,tmp);
  ptr=strtok_r(buffer," ",str_ptr );
  i=0;

  while (i<5)
    {
      ptr=strtok_r(*str_ptr," ",str_ptr ); 
      if( strchr (ptr, '.')!=NULL)
  	{
      		if (ptr!=NULL )
        	{
          		switch (i)
            		{
            			case(0) : infos_cpu->cpu_user=strtod(ptr,NULL); 
      				break;
            			case(1) : infos_cpu->cpu_sys=strtod(ptr,NULL); 
      				break ;
            			case(2) : infos_cpu->cpu_nice=strtod(ptr,NULL); 
      				break;
	  			case(3) : infos_cpu->cpu_idle=strtod(ptr,NULL); 
      				break;
				case(4) : infos_cpu->cpu_wa_io=strtod(ptr,NULL); 
      				break;
            			default:
             			 ;
            		}
         	 i++;
        	}
  	}
    }




   pclose(tmp);
  

  return 0;
}










/*****************************************************************
 *  excess_sensor : is the monitoring information gatherer 
 *****************************************************************/ 

int  excess_sensor( sensor_msg_t * info)
{


  		
  		struct timeval tv1, tv2;
		struct timezone tz;
		double diff =0;
  		
  		int r1, r2 ;
  		
		info = malloc(sizeof(sensor_msg_t)); 
		
  
       

  
   		/* les infos de CPU  et de memory */
	 	cpu_i infocpu;
		mem_i infomem ;

    		

	
     	  

	   	
		while (CURRENT_INDEX >=0)
        	{

			gettimeofday(&tv1, &tz);

           		//mem et swap
          		r1= get_mem_info(&infomem);
             		if (r1 !=0)
             		fprintf(stderr, "%s\n", "ERROR: Sensor could'nt collect memory info");

         	
          		//cpu  info 
          		r2= get_cpu_info(&infocpu);
             		if ( r2 !=0)
              		fprintf(stderr, "%s\n", "ERROR: Sensor could'nt collect cpu info");
        
	  	
			gettimeofday(&tv2, &tz);
			diff=diff+(tv2.tv_sec-tv1.tv_sec) * 1000000L + \
           (tv2.tv_usec-tv1.tv_usec);    
     
    
        		 
        		 to_send_msg[CURRENT_INDEX ].cpu_used= infocpu.cpu_user +  infocpu.cpu_sys + infocpu.cpu_nice  ; //load
        		 to_send_msg[CURRENT_INDEX ].cpu_avail= infocpu.cpu_idle;
			       to_send_msg[CURRENT_INDEX ].cpu_wa_io= infocpu.cpu_wa_io;
			       to_send_msg[CURRENT_INDEX ].cpu_time =  infocpu.cpu_time; 
        		 
             to_send_msg[CURRENT_INDEX ].ram_used= (infomem.memory_used + (infomem.memory_buffers+infomem.swap_cached))*100/(infomem.memory_total+infomem.swap_total); 
        		 to_send_msg[CURRENT_INDEX ].ram_avail= (infomem.memory_used - (infomem.memory_buffers + infomem.swap_cached))*100/(infomem.memory_total+infomem.swap_total);
        		 to_send_msg[CURRENT_INDEX ].mem_time = infomem.mem_time; 
        	
        		 to_send_msg[CURRENT_INDEX ].time_to_get_cpu_info= infocpu.diff_cpu; 
        		 to_send_msg[CURRENT_INDEX ].time_to_get_mem_info= infomem.diff_mem; 
        
			
	

   
	     CURRENT_INDEX--; 
	
	     if(diff  >= TIME_OUT)  
             goto client; 
          
         

        }  

   // mettre a jour la structure d partage des infomations dans la structure share message 
        
     if(CURRENT_INDEX ==0) 
	   {
		    fprintf(stdout, "%s\n"," the send messag array is full .... you will lost your data .... sensor will restart ");
		    sleep(5);
		    BEGIN_INDEX = 0; 
  		  END_INDEX = BUFFER_SIZE -1 ; 
		    CURRENT_INDEX = BUFFER_SIZE -1; 
		
		    excess_sensor(to_send_msg); 
                
		
	   }
     

			
    client : 

    excess_client_part ( to_send_msg) ;


    end : 



    return 0 ; 

  }



/*****************************************************************
 *  excess_client_part : used to communicate with the server part (distant) 
 *****************************************************************/ 


int  excess_client_part ( void * info)
{

    
      
    // here to add the code related to th command line to send message to the server part 



    // send info vers le server part 

    
   // after sending the message, here call again   excess sensor (is not anymore threads communications)
	
	 excess_sensor(to_send_msg);
    

    return 0; 

}



// Main 




int main( int argc, char *argv[] )
{
  

   int err = 0; 
   sensor_msg_t info;
 
   // initialize of the sent message to the server part 
    
	   BEGIN_INDEX = 0; 
  	 END_INDEX = BUFFER_SIZE -1 ; 
	   CURRENT_INDEX = BUFFER_SIZE -1; 
   
  /*
   * Start of the components as thread 
   * First Start: monitoring info collector as thread  
   * 2: client communicator info as thread after the monitoring info collector  
   * 3: benchmark collector info will start as thread also to collect info according to the benchmark description 
   */


    // err= excess_sensor_as_thread( dcomponent description  )
    fprintf(stdout, "%s\n", " sensor ...");
    err= excess_sensor(to_send_msg); 
 
   
    // bench_sensor();  // this thread waits till he receives signal to monitor  the benchmark execution during a time interval !!!

    /* this is the end ... */
    
    return ( err!=0 ? EXIT_FAILURE : EXIT_SUCCESS );


}







