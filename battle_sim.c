#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define PI 3.14159265358979323846
#define G 9.81

//Escort ship structure
typedef struct {
	int id;
	char type; // 'A', 'B', 'C', 'D', 'E'
	char name[50];
	char gun_name[50];
        double x, y;
	double min_angle, max_angle;
	double min_v, max_v;
	double impact_power;
	int is_destroyed;
}EscortShip;
 
//Battleship structure
typedef struct {
	char type;
	char name[50];
	char gun_name[50];
	double x, y;
	double max_v;
}Battleship;

//calculate maximum horizontal range
double get_max_range(double max_v) {
       return(max_v * max_v * sin(2 * 45.0 * PI / 180.0)) / G;
}

//calculate minimum range for Eship
double get_min_range_escort(double min_v, double min_angle) {
	return (min_v * min_v * sin(2 * min_angle * PI / 180.0)) / G;
}	

//calculate distance between ships
double get_distance(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}
//check whether target is within the range
int is_in_range(double shooter_x, double shooter_y, double min_r, double max_r, double target_x, double target_y) {
  double dist = get_distance(shooter_x, shooter_y, target_x, target_y);
  return (dist >= min_r && dist <= max_r);
} 

//Battleship setup logic
void init_battleship(Battleship *b, char type, double max_v, double x, double y) {
	b->type = type;
	b->max_v = max_v;
	b->x = x;
	b->y = y;

	switch (type) {
		case 'U': case 'u':
			strcpy(b->name, "USS Iowa (BB-61)");
			strcpy(b->gun_name, "50-caliber Mark 7 gun");
			break;
		case 'M': case 'm':
			strcpy(b->name, "MS king George V");
			strcpy(b->gun_name, "MARK 7 gun");
			break;
		case 'R': case 'r':
			strcpy(b->name, "Richelieu");
			strcpy(b->gun_name, "Mle 1935 gun");
			break;
		case 'S': case 's':
		       strcpy(b->name, "Sovetsky Soyuz-class");
		       strcpy(b->gun_name, "B-37 gun");
		       break;
		default:
		       strcpy(b->name, "unknown battleship");
		       strcpy(b->gun_name, "Standard Gun");
		       break;

		}      
}

//Escortship setup logic
void init_escort_ship(EscortShip *e, int id, double canvas_D, double v_max_b) {
	e->id = id;
	e->x = (double)(rand() % (int)canvas_D);
	e->y = (double)(rand() % (int)canvas_D);

	int type_idx = rand() % 5;

	switch (type_idx) {
		case 0:
			e->type = 'A';
			strcpy(e->name, "1936A-class Destroyer");
			strcpy(e->gun_name, "SK C/34 naval gun");
			e->impact_power = 0.08;
			e->min_angle = (rand() % 15) + 5.0;
			e->max_angle = e->min_angle + 20.0;
			e->min_v = 10.0 + (rand()%10);
			e->max_v = 1.2 * v_max_b;
			break;
		case 1:
			e->type = 'B';
			strcpy(e->name, "Gabbiano-class Corvette");
			strcpy(e->gun_name, "L/47 dual-purpose gun");
			e->impact_power = 0.06;
			e->min_angle = (rand() % 15) + 5.0;
			e->max_angle = e->min_angle + 30.0;
			e->min_v = 5.0 + (rand() % 10);
			e->max_v = v_max_b * 0.8;
			break;
		case 2:
			e->type = 'C';
			strcpy(e->name, "Matsu-class Destroyer");
			strcpy(e->gun_name, "Type 89 dual-purpose gun");
			e->impact_power = 0.07;
			e->min_angle = (rand() % 15) + 5.0;
			e->max_angle = e->min_angle + 25.0;
			e->min_v = 5.0 + (rand() % 10);
			e->max_v = v_max_b * 0.85;
			break;
		case 3:
	        	e->type = 'D';
			strcpy(e->name, "F-class Escort Ships");
			strcpy(e->gun_name, "SK C/32 naval gun");
			e->impact_power = 0.05;
			e->min_angle = (rand() % 15) + 5.0;
			e->max_angle = e->min_angle + 50.0;
			e->min_v = 5.0 + (rand() % 10);
			e->max_v = v_max_b * 0.75;
			break;
		case 4:
			e->type = 'E';
			strcpy(e->name, "Japanese Kaibokan");
			strcpy(e->gun_name, "4.7 inch naval gun");
			e->impact_power = 0.04;
			e->min_angle = (rand() % 10) + 5.0;
			e->max_angle = e->min_angle + 70.0;
			e->min_v = 5.0 + (rand() % 10);
			e->max_v = v_max_b * 0.7;
			break;
	}
}
//save initial conditions to text file
void  save_initial_conditions(Battleship b, EscortShip escorts[], int n, double canvas_D) {
	FILE *file;
	int i;

	file = fopen("initial_conditions.txt", "w");

	if (file == NULL) {
		printf("Error creating initial_condition.txt!\n");
		return; 
	}
	fprintf(file, "INITIAL BATTLEFEILD CONDITIONS\n\n");
	fprintf(file, "Canvas size: %.2f x %.2f\n\n" , canvas_D, canvas_D);

	fprintf(file, "BATTLESHIP\n");
	fprintf(file, "Type: %c\n", b.type);
	fprintf(file, "Name: %s\n", b.name);
	fprintf(file, "Gun: %s\n", b.gun_name);
	fprintf(file, "position: (%.2f, %.2f)\n", b.x, b.y);
	fprintf(file, "max_v: %.2f\n\n", b.max_v);

	fprintf(file, "ESCORT SHIPS\n");

	for (i = 0; i < n; i++) {
		fprintf(file, "\nEscort Ship ID: %d\n", escorts[i].id);
		fprintf(file, "type: %c\n", escorts[i].type);
		fprintf(file, "Name: %s\n", escorts[i].name);
		fprintf(file, "Gun: %s\n", escorts[i].gun_name);
		fprintf(file, "Position: (%.2f, %.2f)\n", escorts[i].x, escorts[i].y);
		fprintf(file, "minimum angle: %.2f\n", escorts[i].min_angle);
		fprintf(file, "maximum angle: %.2f\n", escorts[i].max_angle);
		fprintf(file, "maximum velocity: %.2f\n", escorts[i].max_v);
		fprintf(file, "minimum velocity: %.2f\n", escorts[i].min_v);
		fprintf(file, "impact power: %.2f\n", escorts[i].impact_power);

	}


		
					
	fclose(file);
	printf("initial conditions saved successfully.\n");
}




	


	      
			
int main() {
    Battleship battleship;
    EscortShip escorts[100];

    int n;
    double canvas_D;
    double v_max_b;
    char type;

    srand((unsigned int)time(NULL));

    printf("enter canvas size: ");
    scanf("%lf", &canvas_D);

    printf("enter number of escort ships: ");
    scanf("%d", &n);

    printf("enter battleship type (U/M/R/S): ");
    scanf(" %c", &type);

    printf("enter battleship max_v: ");
    scanf("%lf", &v_max_b);

    //create battleship
    init_battleship(&battleship, type, v_max_b, 50, 50);

    //create escort ships
    for (int i = 0; i < n; i++)
    {
	    init_escort_ship(
			    &escorts[i],
			    i + 1,
			    canvas_D,
			    v_max_b
			    );
    }
    //call file handling function
    save_initial_conditions(
		    battleship,
		    escorts,
		    n,
		    canvas_D
		    );



    return 0;
}


