#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "stats.h"


//struct stats
typedef struct{
    int Made;
    int Eaten;
    double Min_Delay;
    double Max_Delay;
    double Total_Delay;
} candy_stats;

//global vairables
int ProducerSize;
candy_stats *stats;

//init stats
void stats_init(int num_producers){
    ProducerSize = num_producers;
    stats = malloc(sizeof(candy_stats)* num_producers);

    for (int i = 0; i < num_producers; i++)
    {
        stats[i].Made = 0;
        stats[i].Eaten = 0;
        stats[i].Min_Delay = DBL_MAX;
        stats[i].Max_Delay = 0;
        stats[i].Total_Delay = 0;
    }
}

//clean up the stats and all allocate memory
void stats_cleanup(void){
    if (stats)
    {
        free(stats);
    }
}

//stats add
void stats_record_produced(int factory_number){
    if (factory_number >= ProducerSize)
    {
        printf("STATS: Stats error\n");
        exit(0);
    }
    stats[factory_number].Made++;
}

void stats_record_consumed(int factory_number, double delay_in_ms){
    if (factory_number >= ProducerSize)
    {
        printf("STATS: Stats error\n");
        exit(0);
    }

    stats[factory_number].Eaten ++;
    stats[factory_number].Total_Delay += delay_in_ms;

    if (delay_in_ms < stats[factory_number].Min_Delay)
    {
        stats[factory_number].Min_Delay = delay_in_ms;
    }

    if (delay_in_ms > stats[factory_number].Max_Delay)
    {
        stats[factory_number].Max_Delay = delay_in_ms;
    }
}

void stats_display(void){

    int mismatch = 0;

    if (stats == NULL) return;
    printf("Statistics:\n");
    printf("   Factory# \t #Made \t #Eaten \t  Min_Delay(ms) \t\t Max_Delay(ms)  \t Avg_Delay(ms)\n");
    //printf(" \t Avg_Delay(ms)\n");
    for (int i=0; i<ProducerSize; i++)
        printf("    %6d \t %4d \t %5d  \t  %10.5f \t  %10.5f \t  %10.5f\n", 
            i, stats[i].Made, stats[i].Eaten, 
            stats[i].Min_Delay, stats[i].Max_Delay,
        stats[i].Total_Delay/stats[i].Eaten);

    for (int i = 0; i < ProducerSize; i++)
    {
       if (stats[i].Made != stats[i].Eaten)
       {
           mismatch = 1;
       }
    }
    if (mismatch == 1)
    {
        printf("Error: Mismatch between number made and eaten.\n");
    }

}