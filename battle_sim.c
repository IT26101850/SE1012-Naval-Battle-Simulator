#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define PI 3.14159265358979323846
#define G 9.81

//Escort ship structure
typedef struct {
	int id;
	int type; //0 to 4
        double x,y;
	double min_angle, max_angle;
	double min_v, max_v;
	double impact_power;
	int is_destroyed;
}EscortShip;
 
//Battleship structure
typedef struct {
	int type;
	double x, y;
	double max_v;
}Battleship;

int main() {
    printf("Battle Simulator Initialized...\n");

    return 0;
}


