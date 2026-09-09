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
	int has_fired;
	double time_to_hit;
}EscortShip;
 
//Battleship structure
typedef struct {
	char type;
	char name[50];
	char gun_name[50];
	double x, y;
	double max_v;
	double firing_delay;
}Battleship;

double escort_TE[5];
int part2b_mode = 0;

double get_escort_TE(char type)
{
	return escort_TE[type - 'A'];
}

//calculate maximum horizontal range
double get_max_range(double max_v) {
       return(max_v * max_v * sin(2 * 45.0 * PI / 180.0)) / G;

}

//jammed_battleship_range
double get_jammed_battleship_range(double velocity, double theta_min)
{
    double angle_rad;

    angle_rad = theta_min * PI / 180.0;

    return (velocity * velocity *
            sin(2.0 * angle_rad)) / G;
}

//calculate minimum range for Eship
double get_min_range_escort(double min_v, double min_angle) {
	return (min_v * min_v * sin(2 * min_angle * PI / 180.0)) / G;
}

//calculate time_to_hit
double get_time_to_hit(double distance, double velocity)
{
	double angle = 45.0 * PI / 180.0;

	return distance / (velocity * cos(angle));
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
	e->is_destroyed = 0;
	e->has_fired = 0;
	e->time_to_hit = 0.0;

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

//save final conditions
void save_final_conditions(Battleship b, EscortShip e[], int n)
{
	FILE *file;
	int i;

	file = fopen("final_conditions.txt", "w");

	if (file == NULL)
	{
		printf("error opening final_conditions.txt\n");
		return;
	}
	fprintf(file, "FINAL BATTLEFIELD CONDITIONS\n\n");

	fprintf(file, "BATTLESHIP\n");
	fprintf(file, "Type: %c\n", b.type);
	fprintf(file, "name: %s\n", b.name);
	fprintf(file, "position: (%.2f, %.2f)\n", b.x, b.y);
	fprintf(file, "maximum velocity: %.2f\n\n", b.max_v);

	fprintf(file, "ESCORT SHIPS\n");

	for(int i = 0; i < n; i++)
	{
		fprintf(file, "\nEscort Ship ID: %d\n", e[i].id);
		fprintf(file, "type: %c\n", e[i].type);
		fprintf(file, "position: (%.2f, %.2f)\n", e[i].x, e[i].y);
		fprintf(file, "time to hit: %.2f seconds\n", e[i].time_to_hit);
	

		if(e[i].is_destroyed == 1)
		{
			fprintf(file, "status: DESTROYED\n");
		}
		else
		{
			fprintf(file, "status: ALIVE\n");
		}
	}
	
	fclose(file);
	printf("final conditions saved successfully.\n");
}



//part1b simulation
void save_simulation_results(Battleship b, EscortShip e[], int n,
                                  int hit_count, int b_destroyed,
                                  double battle_time, int destroyer_id)
{
    FILE *file;
    int i;

    file = fopen("simulation_results.txt", "w");

    if (file == NULL)
    {
        printf("error opening simulation_results.txt\n");
        return;
    }

    fprintf(
        file,
        "SIMULATION RESULTS\n\n"
    );


    fprintf(
        file,
        "Battleship: %s\n",
        b.name
    );


    if (b_destroyed == 1)
    {
        fprintf(
            file,
            "Result: BATTLESHIP DESTROYED\n"
        );


        fprintf(
            file,
            "Destroyed by Escort ID: %d\n",
            destroyer_id
        );
    }
    else
    {
        fprintf(
            file,
            "Result: BATTLESHIP SURVIVED\n"
        );


        fprintf(
            file,
            "Escort ships destroyed: %d\n",
            hit_count
        );


        fprintf(
            file,
            "Battle time: %.2f seconds\n\n",
            battle_time
        );


        fprintf(
            file,
            "DESTROYED ESCORT SHIPS\n"
        );


        for (int i = 0; i < n; i++)
        {
            if (e[i].is_destroyed == 1)
            {
                fprintf(
                    file,
                    "Escort ID: %d   Time: %.2f seconds\n",
                    e[i].id,
                    e[i].time_to_hit
                );
            }
        }
    }


    fclose(file);
}


void save_part1b_iteration_result(Battleship b,
                                  EscortShip e[],
                                  int n,
                                  int iteration,
                                  int hit_this_iteration,
                                  int total_hit_count,
                                  int b_destroyed,
                                  int destroyer_id,
                                  double iteration_battle_time)
{
    FILE *file;

    char filename[100];



    sprintf(
        filename,
        "part1b_sim1_iteration_%02d.txt",
        iteration
    );


    file =
        fopen(
            filename,
            "w"
        );


    if (file == NULL)
    {
        printf(
            "Error creating %s\n",
            filename
        );

        return;
    }


    fprintf(
        file,
        "PART 1B - SIMULATION 1\n\n"
    );


    fprintf(
        file,
        "Iteration: %d\n",
        iteration
    );


    fprintf(
        file,
        "Battleship position: (%.2f, %.2f)\n",
        b.x,
        b.y
    );


    fprintf(
        file,
        "Escort ships destroyed this iteration: %d\n",
        hit_this_iteration
    );


    fprintf(
        file,
        "Total escort ships destroyed: %d\n",
        total_hit_count
    );


    fprintf(
        file,
        "Iteration battle time: %.2f seconds\n\n",
        iteration_battle_time
    );


    if (b_destroyed == 1)
    {
        fprintf(
            file,
            "Battleship status: DESTROYED\n"
        );


        fprintf(
            file,
            "Destroyed by Escort ID: %d\n\n",
            destroyer_id
        );
    }
    else
    {
        fprintf(
            file,
            "Battleship status: ALIVE\n\n"
        );
    }


    fprintf(
        file,
        "ESCORT SHIP STATUS\n"
    );


    for (int i = 0; i < n; i++)
    {
        fprintf(
            file,
            "\nEscort ID: %d\n",
            e[i].id
        );


        fprintf(
            file,
            "Type: %c\n",
            e[i].type
        );


        fprintf(
            file,
            "Position: (%.2f, %.2f)\n",
            e[i].x,
            e[i].y
        );


        if (e[i].is_destroyed == 1)
        {
            fprintf(
                file,
                "Status: DESTROYED\n"
            );


            fprintf(
                file,
                "Time to hit: %.2f seconds\n",
                e[i].time_to_hit
            );
        }
        else
        {
            fprintf(
                file,
                "Status: ALIVE\n"
            );
        }
    }


    fclose(file);
}

void save_part1b_sim2_result(Battleship b,
                             EscortShip e[],
                             int n,
                             int iteration,
                             int hit_this_iteration,
                             int total_hit_count,
                             int b_destroyed,
                             int destroyer_id,
                             double iteration_battle_time,
                             int gun_jammed,
                             double theta_min)
{
    FILE *file;
    char filename[100];

    sprintf(filename,
            "part1b_sim2_iteration_%02d.txt",
            iteration);

    file = fopen(filename, "w");

    if (file == NULL)
    {
        printf("Error opening %s\n", filename);
        return;
    }

    fprintf(file, "PART 1B - SIMULATION 2\n\n");
    fprintf(file, "Iteration: %d\n", iteration);

    fprintf(file,
            "Battleship Position: (%.2f, %.2f)\n",
            b.x, b.y);

    if (gun_jammed == 1)
    {
        fprintf(file, "Gun Status: JAMMED\n");

        fprintf(file,
                "Allowed Vertical Angle: %.2f - 90 degrees\n",
                theta_min);
    }
    else
    {
        fprintf(file, "Gun Status: NORMAL\n");
    }

    fprintf(file,
            "Escort Ships Destroyed This Iteration: %d\n",
            hit_this_iteration);

    fprintf(file,
            "Total Escort Ships Destroyed: %d\n",
            total_hit_count);

    fprintf(file,
            "Iteration Battle Time: %.2f seconds\n",
            iteration_battle_time);

    if (b_destroyed == 1)
    {
        fprintf(file, "Battleship Status: DESTROYED\n");

        fprintf(file,
                "Destroyed by Escort ID: %d\n",
                destroyer_id);
    }
    else
    {
        fprintf(file, "Battleship Status: ALIVE\n");
    }

    fprintf(file, "\nESCORT SHIP STATUS\n");

    for (int i = 0; i < n; i++)
    {
        fprintf(file, "\nEscort ID: %d\n", e[i].id);
        fprintf(file, "Type: %c\n", e[i].type);

        fprintf(file,
                "Position: (%.2f, %.2f)\n",
                e[i].x, e[i].y);

        if (e[i].is_destroyed == 1)
        {
            fprintf(file, "Status: DESTROYED\n");

            fprintf(file,
                    "Time to Hit: %.2f seconds\n",
                    e[i].time_to_hit);
        }
        else
        {
            fprintf(file, "Status: ALIVE\n");
        }
    }

    fclose(file);
}

void save_part1c_result(
    const char *simulation_name,
    Battleship b,
    EscortShip e[],
    int n,
    int iteration,
    double cumulative_impact,
    int b_destroyed,
    int destroyer_id,
    int gun_jammed,
    double theta_min)
{
    FILE *file;

    file = fopen("part1c_results.txt", "a");

    if (file == NULL)
    {
        printf("Error opening part1c_results.txt\n");
        return;
    }

    fprintf(file,
            "\n========================================\n");

    fprintf(file,
            "%s\n",
            simulation_name);

    fprintf(file,
            "========================================\n");

    fprintf(file,
            "Iteration: %d\n",
            iteration);

    fprintf(file,
            "Battleship Position: (%.2f, %.2f)\n",
            b.x,
            b.y);


    if (gun_jammed == 1)
    {
        fprintf(file,
                "Gun Status: JAMMED\n");

        fprintf(file,
                "Allowed Vertical Angle: %.2f - 90 degrees\n",
                theta_min);
    }
    else
    {
        fprintf(file,
                "Gun Status: NORMAL\n");
    }


    fprintf(file,
            "Cumulative Impact on Battleship: %.2f\n",
            cumulative_impact);

    fprintf(file,
            "Battleship Damage Percentage: %.2f%%\n",
            cumulative_impact * 100.0);


    if (b_destroyed == 1)
    {
        fprintf(file,
                "Battleship Status: DESTROYED\n");

        fprintf(file,
                "Final damaging Escort ID: %d\n",
                destroyer_id);
    }
    else
    {
        fprintf(file,
                "Battleship Status: ALIVE\n");
    }


    fprintf(file,
            "\nESCORT SHIP STATUS\n");


    for (int i = 0; i < n; i++)
    {
        fprintf(file,
                "\nEscort ID: %d\n",
                e[i].id);

        fprintf(file,
                "Type: %c\n",
                e[i].type);

        fprintf(file,
                "Position: (%.2f, %.2f)\n",
                e[i].x,
                e[i].y);

        fprintf(file,
                "Impact Power: %.2f\n",
                e[i].impact_power);


        if (e[i].is_destroyed == 1)
        {
            fprintf(file,
                    "Status: DESTROYED\n");
        }
        else
        {
            fprintf(file,
                    "Status: ALIVE\n");
        }


        if (e[i].has_fired == 1)
        {
            fprintf(file,
                    "Gun Fired: YES\n");
        }
        else
        {
            fprintf(file,
                    "Gun Fired: NO\n");
        }
    }


    fclose(file);
}

int part1c_escort_attack(
    Battleship b,
    EscortShip e[],
    int n,
    double *cumulative_impact,
    int *destroyer_id)
{
    for (int i = 0; i < n; i++)
    {
        double min_range;
        double max_range;


        // Destroyed escort cannot attack 
        if (e[i].is_destroyed == 1)
        {
            continue;
        }


        /* Escort can attack only once */
        if (e[i].has_fired == 1)
        {
            continue;
        }


        min_range =
            get_min_range_escort(
                e[i].min_v,
                e[i].min_angle
            );


        max_range =
            get_max_range(
                e[i].max_v
            );


        if (is_in_range(
                e[i].x,
                e[i].y,
                min_range,
                max_range,
                b.x,
                b.y))
        {
            /*
               Escort fires once
            */

            e[i].has_fired = 1;

            *cumulative_impact +=
                e[i].impact_power;


           printf("Escort Ship %d hit Battleship\n",e[i].id);

            printf(
                "Impact Power: %.2f\n",
                e[i].impact_power
            );

            printf(
                "Cumulative Impact: %.2f (%.2f%%)\n",
                *cumulative_impact,
                *cumulative_impact * 100.0
            );

            if (*cumulative_impact >= 1.0)
            {
                *destroyer_id =
                    e[i].id;

                printf("battleship DESTROYED by cumulative damage!\n");
                                 

                return 1;
            }
        }
    }


    return 0;
}

// PART 2A - CREATE ATTACK ORDER      

int build_part2a_attack_order(
    Battleship b,
    EscortShip e[],
    int n,
    int order[],
    int gun_jammed,
    double theta_min)
{
    int count = 0;
    double b_max_range;

    if (gun_jammed == 1)
    {
        b_max_range =
            get_jammed_battleship_range(
                b.max_v,
                theta_min
            );
    }
    else
    {
        b_max_range =
            get_max_range(b.max_v);
    }


   for (int i = 0; i < n; i++)
    {
        double distance;

        if (e[i].is_destroyed == 1)
        {
            continue;
        }

        distance =
            get_distance(
                b.x,
                b.y,
                e[i].x,
                e[i].y
            );

        if (distance <= b_max_range)
        {
            order[count] = i;
            count++;
        }
    }

    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            int first = order[i];
            int second = order[j];

            double distance_first =
                get_distance(
                    b.x,
                    b.y,
                    e[first].x,
                    e[first].y
                );

            double distance_second =
                get_distance(
                    b.x,
                    b.y,
                    e[second].x,
                    e[second].y
                );

            int swap_needed = 0;


            if (e[second].impact_power >
                e[first].impact_power)
            {
                swap_needed = 1;
            }
            else if (
                e[second].impact_power ==
                e[first].impact_power &&
                distance_second < distance_first)
	    {
        
                swap_needed = 1;
            }


            if (swap_needed == 1)
            {
                int temp = order[i];

                order[i] = order[j];

                order[j] = temp;
            }
        }
    }


    return count;
}

int part2a_battleship_attack(
    Battleship b,
    EscortShip e[],
    int n,
    int gun_jammed,
    double theta_min,
    FILE *file,
    int iteration)
{
    int order[100];

    int target_count;

    double current_fire_time = 0.0;


    target_count =
        build_part2a_attack_order(
            b,
            e,
            n,
            order,
            gun_jammed,
            theta_min
        );


    fprintf(
        file,
        "\nATTACK ORDER - ITERATION %d\n",
        iteration
    );


    if (target_count == 0)
    {
        fprintf(
            file,
            "No escort ships inside Battleship attack range.\n"
        );

        return 0;
    }


    for (int a = 0; a < target_count; a++)
    {
        int i = order[a];

        double distance;

        double shell_time;

        double total_hit_time;


        distance =
            get_distance(
                b.x,
                b.y,
                e[i].x,
                e[i].y
            );


        shell_time =
            get_time_to_hit(
                distance,
                b.max_v
            );


        current_fire_time =
            a * b.firing_delay;


        total_hit_time =
            current_fire_time +
            shell_time;


        e[i].is_destroyed = 1;

        e[i].time_to_hit =
            total_hit_time;


        printf(
            "Attack %d -> Escort Ship %d"
            " | Fire time: %.2f"
            " | Hit time: %.2f\n",
            a + 1,
            e[i].id,
            current_fire_time,
            total_hit_time
        );


        fprintf(
            file,
            "%d. Escort ID %d"
            " | Type %c"
            " | Impact Power %.2f"
            " | Fire Time %.2f"
            " | Hit Time %.2f\n",
            a + 1,
            e[i].id,
            e[i].type,
            e[i].impact_power,
            current_fire_time,
            total_hit_time
        );
    }


    return target_count;
}
int part2b_escort_attack(
    Battleship b,
    EscortShip e[],
    int n,
    double battle_time,
    double *impact,
    int *destroyer_id,
    FILE *file
);

void run_part2a_scenario(
    const char *title,
    Battleship b,
    EscortShip original[],
    int n,
    double path_x[],
    double path_y[],
    int iterations,
    int use_jam,
    int t,
    double theta_min,
    int cumulative_mode,
    FILE *file)
{
    EscortShip temp[100];

    int b_destroyed = 0;
    int destroyer_id = -1;

    int total_attacked = 0;

    double cumulative_impact = 0.0;
    double battle_time = 0.0;


    /* Restore original escorts */

    for (int i = 0; i < n; i++)
    {
        temp[i] = original[i];

        temp[i].is_destroyed = 0;
        temp[i].has_fired = 0;
    }


    printf(
        "\n===== %s =====\n",
        title
	);
    

    fprintf(
        file,
        "%s\n",
	 title
    );


    for (int p = 0;
         p < iterations &&
         b_destroyed == 0;
         p++)
    {
        int gun_jammed = 0;
        int attacked_now = 0;


        /* Move Battleship */

        b.x = path_x[p];
        b.y = path_y[p];


        /* Simulation 2 gun jam */

        if (use_jam == 1 &&
            (p + 1) > t)
        {
            gun_jammed = 1;
        }


        printf(
            "\nIteration %d\n",
            p + 1
        );

        printf(
            "Battleship Position: (%.2f, %.2f)\n",
            b.x,
            b.y
        );


        fprintf(
            file,
            "\nIteration %d\n",
            p + 1
        );

        fprintf(
            file,
            "Battleship Position: (%.2f, %.2f)\n",
            b.x,
            b.y
        );


        if (gun_jammed == 1)
        {
            printf(
                "Gun Status: JAMMED\n"
            );

            fprintf(
                file,
                "Gun Status: JAMMED\n"
            );

            fprintf(
                file,
                "Allowed Vertical Angle: %.2f - 90 degrees\n",
                theta_min
            );
        }
        else
        {
            printf(
                "Gun Status: NORMAL\n"
            );

            fprintf(
                file,
                "Gun Status: NORMAL\n"
            );
        }

        attacked_now =
            part2a_battleship_attack(
                b,
                temp,
                n,
                gun_jammed,
                theta_min,
                file,
                p + 1
            );


        total_attacked +=
            attacked_now;


        for (int i = 0; i < n; i++)
        {
            if (temp[i].is_destroyed == 1 &&
                temp[i].time_to_hit > battle_time)
            {
                battle_time =
                    temp[i].time_to_hit;
            }
        }


        fprintf( file, "Escorts attacked this iteration: %d\n", attacked_now );

        if (cumulative_mode == 1)
        {
            if (part2b_mode == 1)

    b_destroyed = part2b_escort_attack(b, temp, n, battle_time, &cumulative_impact, &destroyer_id, file);
	    else
		    b_destroyed = part1c_escort_attack(b, temp, n, &cumulative_impact, &destroyer_id);
                        


            fprintf( file, "Cumulative Impact: %.2f\n", cumulative_impact );

            fprintf( file, "Damage Percentage: %.2f%%\n", cumulative_impact * 100.0 );
        }

        else
        {
            for (int i = 0; i < n; i++)
            {
                double min_range;
                double max_range;


                if (temp[i].is_destroyed == 1)
                {
                    continue;
                }


                if (temp[i].has_fired == 1)
                {
                    continue;
                }


                min_range =
                    get_min_range_escort(
                        temp[i].min_v,
                        temp[i].min_angle
                    );


                max_range =
                    get_max_range(
                        temp[i].max_v
                    );


                if (is_in_range(
                        temp[i].x,
                        temp[i].y,
                        min_range,
                        max_range,
                        b.x,
                        b.y))
                {
                    temp[i].has_fired = 1;

                    b_destroyed = 1;

                    destroyer_id =
                        temp[i].id;


                    printf(
                        "Escort Ship %d destroyed Battleship\n",
                        temp[i].id
                    );


                    fprintf(file, "Battleship destroyed by Escort ID: %d\n", temp[i].id );

                    break;
                }
            }
        }


        if (b_destroyed == 1)
        {
            fprintf( file, "Battleship Status: DESTROYED\n" );
        }
        else
        {
            fprintf( file, "Battleship Status: ALIVE\n" );
        }
    }


    /* Final scenario summary */

    fprintf(file, "\nScenario Summary\n" );

    fprintf(file,"Total Escorts Attacked: %d\n",total_attacked);

    fprintf(file,"Battle Time: %.2f seconds\n",battle_time);
    if (b_destroyed == 1)
    {
        fprintf(file,"Final Battleship Status: DESTROYED\n");

        fprintf(file,"Final damaging Escort ID: %d\n", destroyer_id);
    }
    else
    {
        fprintf(file, "Final Battleship Status: SURVIVED\n");
    }
}
int part2b_escort_attack(
    Battleship b,
    EscortShip e[],
    int n,
    double battle_time,
    double *impact,
    int *destroyer_id,
    FILE *file)
{
    for (int i = 0; i < n; i++)
    {
        if (e[i].is_destroyed)
            continue;

        double min_r = get_min_range_escort(
            e[i].min_v, e[i].min_angle);

        double max_r = get_max_range(e[i].max_v);

        if (is_in_range(
            e[i].x, e[i].y,
            min_r, max_r,
            b.x, b.y))
        {
            double TE = get_escort_TE(e[i].type);
            int shots = 1 + (int)(battle_time / TE);

            *impact += shots * e[i].impact_power;

            fprintf(file, "Escort %d | T_E %.2f | Shots %d | Damage %.2f%%\n", e[i].id, TE, shots, shots * e[i].impact_power * 100.0);
	    if (*impact >= 1.0)
            {
                *destroyer_id = e[i].id;
                return 1;
            }
        }
    }

    return 0;
}


//main function
int main()
{	
		        
    Battleship battleship;
    EscortShip escorts[100];
    EscortShip original_escorts[100];

    int n;
    
    double canvas_D;
    double v_max_b;
    char type;

    int k;
    int choice;
    int t;
    double theta_min;
    double path_x[100];
    double path_y[100];

    int hit_count = 0;
    int b_destroyed = 0;
    int destroyer_id = -1;

    double battle_time = 0.0;

    srand((unsigned int)time(NULL));

    printf("enter canvas size: ");
    scanf("%lf", &canvas_D);

    printf("enter number of escort ships: ");
    scanf("%d", &n);

    printf("enter number of path points (k): ");
    scanf("%d", &k);

    printf("enter battleship type (U/M/R/S): ");
    scanf(" %c", &type);

    printf("enter battleship max_v: ");
    scanf("%lf", &v_max_b);

    
   // INPUT VALIDATION
    if (canvas_D <= 0)
    {
        printf(
            "Canvas size must be greater than 0.\n"
        );

        return 1;
    }


    if (n < 1 || n > 100)
    {
        printf(
            "Number of escort ships must be between 1 and 100.\n"
        );

        return 1;
    }


    if (k < 1 || k > 100)
    {
        printf(
            "Number of path points must be between 1 and 100.\n"
        );

        return 1;
    }

 // Generate path points
   

    printf(
        "\nGENERATED BATTLESHIP PATH\n"
    );


    for (int i = 0; i < k; i++)
    {
        path_x[i] =
            (double)(
                rand() %
                (int)canvas_D
            );

        path_y[i] =
            (double)(
                rand() %
                (int)canvas_D
            );


        printf(
            "Path point %d: (%.2f, %.2f)\n",
            i + 1,
            path_x[i],
            path_y[i]
        );
    }


    

    init_battleship(
        &battleship,
        type,
        v_max_b,
        path_x[0],
        path_y[0]
    );
    printf("enter battleship firing interval T_B in seconds: ");
    scanf("%lf", &battleship.firing_delay);
    if (battleship.firing_delay < 0.0) {
	    printf("firing interval cant be negative.\n");
	    return 1; }
    printf("enter T_E for escort types A B C D E: ");
    for (int i = 0; i < 5; i++)
    {
	    scanf("%lf", &escort_TE[i]);
    }
    
    
  //CREATE ESCORT SHIPS

    for (int i = 0; i < n; i++)
    {
        init_escort_ship(
            &escorts[i],
            i + 1,
            canvas_D,
            v_max_b
        );
	original_escorts[i] = escorts[i];
    }

    save_initial_conditions(
        battleship,
        escorts,
        n,
        canvas_D
    );
    printf("\n NAVAL BATTLE SIMULATOR \n");
    printf("1. part1A\n");
    printf("2. part1B\n");
    printf("3. part1C\n");
    printf("4. part2A\n");
    printf("5. part2B\n");
    printf("6. run all\n");
    printf("0. exit\n");
    printf("enter choice: ");

    scanf("%d", &choice);

    switch (choice)
{
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        break;

    case 0:
        printf("exiting simulator.\n");
        return 0;

    default:
        printf("invalid choice.\n");
        return 1;
}
if (choice == 1 || choice == 6)
	{
    EscortShip part1a_escorts[100];
    int part1a_b_destroyed = 0;
    int part1a_destroyer_id = -1;
    int part1a_hit_count = 0;

    printf("\n===== PART 1A =====\n");

    for (int i = 0; i < n; i++)
    {
        part1a_escorts[i] = original_escorts[i];
    }

    battleship.x = path_x[0];
    battleship.y = path_y[0];

    for (int i = 0; i < n; i++)
    {
        double distance = get_distance(
            battleship.x,
            battleship.y,
            part1a_escorts[i].x,
            part1a_escorts[i].y
        );

        double max_range = get_max_range(battleship.max_v);

        if (distance <= max_range)
        {
            part1a_escorts[i].is_destroyed = 1;
            part1a_hit_count++;

            printf(
                "Battleship destroyed Escort Ship %d\n",
                part1a_escorts[i].id
            );
        }
    }

    for (int i = 0; i < n; i++)
    {
        if (part1a_escorts[i].is_destroyed == 1)
            continue;

        double min_range = get_min_range_escort(
            part1a_escorts[i].min_v,
            part1a_escorts[i].min_angle
        );

        double max_range = get_max_range(
            part1a_escorts[i].max_v
        );

        if (is_in_range(
            part1a_escorts[i].x,
            part1a_escorts[i].y,
            min_range,
            max_range,
            battleship.x,
            battleship.y))
        {
            part1a_b_destroyed = 1;
            part1a_destroyer_id = part1a_escorts[i].id;

            printf(
                "Escort Ship %d destroyed Battleship\n",
                part1a_destroyer_id
            );

            break;
        }
    }

    printf("\nPART 1A RESULT\n");

    if (part1a_b_destroyed == 1)
    {
        printf(
            "Battleship DESTROYED by Escort Ship %d\n",
            part1a_destroyer_id
        );
    }
    else
    {
        printf("Battleship SURVIVED\n");
        printf(
            "Escort Ships Destroyed: %d\n",
            part1a_hit_count
        );
    }
}

       if (choice == 2 || choice == 6)
    {


    printf(
        "\n PART 1B SIMULATION 1 START\n"
    );


    
    
    //PART 1B SIMULATION 1
    

    for (int p = 0;
         p < k && b_destroyed == 0;
         p++)
    {
        int hit_this_iteration = 0;

        double iteration_battle_time = 0.0;


        printf(
            "\n-----------------------------------\n"
        );

        printf(
            "ITERATION %d\n",
            p + 1
        );


        
       // Move Battleship to next point
        

        battleship.x =
            path_x[p];

        battleship.y =
            path_y[p];


        printf( "Battleship position: (%.2f, %.2f)\n", battleship.x, battleship.y);


        
       // BATTLESHIP ATTACKS FOR ESCOERTSHIP
        

        for (int i = 0; i < n; i++)
        {
            double distance;

            double max_range;

            double time_to_hit;

  if (escorts[i].is_destroyed == 1)
            {
                continue;
            }


            distance =
                get_distance(
                    battleship.x,
                    battleship.y,
                    escorts[i].x,
                    escorts[i].y
                );


            max_range =
                get_max_range(
                    battleship.max_v
                );


            printf(
                "Escort Ship %d distance: %.2f\n",
                escorts[i].id,
                distance
            );


            if (distance <= max_range)
            {
                escorts[i].is_destroyed = 1;


                time_to_hit =
                    get_time_to_hit(
                        distance,
                        battleship.max_v
                    );


                escorts[i].time_to_hit =
                    time_to_hit;


                hit_count++;

                hit_this_iteration++;


                if (
                    time_to_hit >
                    battle_time
                )
                {
                    battle_time =
                        time_to_hit;
                }


                if (
                    time_to_hit >
                    iteration_battle_time
                )
                {
                    iteration_battle_time =
                        time_to_hit;
                }


                printf( "Battleship destroyed Escort Ship %d\n", escorts[i].id );


                printf( "Time to hit: %.2f seconds\n", time_to_hit );
            }
            else
            {
                printf(
                    "Escort Ship %d is outside battleship range\n",
                    escorts[i].id
                );
            }
        }


        

        //REMAINING ESCORT SHIPS ATTACK BATTLESHIP
       

        for (int i = 0; i < n; i++)
        {
            double min_range;

            double max_range;


            
            //Destroyed escorts cannot attack.
            

            if (
                escorts[i].is_destroyed == 1
            )
            {
                continue;
            }


            min_range =
                get_min_range_escort(
                    escorts[i].min_v,
                    escorts[i].min_angle
                );


            max_range =
                get_max_range(
                    escorts[i].max_v
                );


            if (
                is_in_range(
                    escorts[i].x,
                    escorts[i].y,
                    min_range,
                    max_range,
                    battleship.x,
                    battleship.y
                )
            )
            {
                b_destroyed = 1;

                destroyer_id =
                    escorts[i].id;


                printf(
                    "Escort Ship %d destroyed the Battleship!\n",
                    escorts[i].id
                );


                break;
            }
        }


        //SAVE THIS ITERATION

        save_part1b_iteration_result(
            battleship,
            escorts,
            n,
            p + 1,
            hit_this_iteration,
            hit_count,
            b_destroyed,
            destroyer_id,
            iteration_battle_time
        );


        printf(
            "End of iteration %d\n",
            p + 1
        );


     
    }

    int t;
    double theta_min;


    printf(
        "\n PART 1B SIMULATION 2 \n"
    );


    printf(
        "Enter iteration t when gun becomes jammed: "
    );

    scanf("%d", &t);


    printf(
        "Enter theta_min (0 < theta_min < 30): "
    );

    scanf("%lf", &theta_min);


    /* Validate Simulation 2 input */

    if (t < 1 || t >= k)
    {
        printf(
            "Invalid t. t must satisfy 1 <= t < k.\n"
        );

        return 1;
    }


    if (theta_min <= 0 ||
        theta_min >= 30)
    {
        printf(
            "Invalid theta_min. "
            "It must be between 0 and 30.\n"
        );

        return 1;
    }
    

    for (int i = 0; i < n; i++)
    {
        escorts[i] =
            original_escorts[i];
    }


    hit_count = 0;
    b_destroyed = 0;
    destroyer_id = -1;
    battle_time = 0.0;


    /* Return Battleship to first path point */

    battleship.x = path_x[0];
    battleship.y = path_y[0];


    
   //SIMULATION 2 LOOP                         
    

    for (int p = 0;
         p < k && b_destroyed == 0;
         p++)
    {
        int hit_this_iteration = 0;

        int gun_jammed = 0;

        double iteration_battle_time = 0.0;


        // Move Battleship 

        battleship.x =
            path_x[p];

        battleship.y =
            path_y[p];

 if ((p + 1) > t)
        {
            gun_jammed = 1;
        }


        printf(
            "\n----------------------------------\n"
        );

        printf(
            "SIMULATION 2 - ITERATION %d\n",
            p + 1
        );


        printf(
            "Battleship position: (%.2f, %.2f)\n",
            battleship.x,
            battleship.y
        );


        if (gun_jammed == 1)
        {
            printf(
                "Gun Status: JAMMED\n"
            );

            printf(
                "Allowed vertical angle: "
                "%.2f - 90 degrees\n",
                theta_min
            );
        }
        else
        {
            printf(
                "Gun Status: NORMAL\n"
            );
        }


        
        //BATTLESHIP ATTACKS ESCORT SHIPS      
      

        for (int i = 0; i < n; i++)
        {
            double distance;
            double max_range;
            double time_to_hit;

      if (escorts[i].is_destroyed == 1)
            {
                continue;
            }


            distance =
                get_distance(
                    battleship.x,
                    battleship.y,
                    escorts[i].x,
                    escorts[i].y
                );


            
               //Normal gun uses normal maximum ran
               if (gun_jammed == 1)
            {
                max_range =
                    get_jammed_battleship_range(
                        battleship.max_v,
                        theta_min
                    );
            }
            else
            {
                max_range =
                    get_max_range(
                        battleship.max_v
                    );
            }


            if (distance <= max_range)
            {
                escorts[i].is_destroyed = 1;


                time_to_hit =
                    get_time_to_hit(
                        distance,
                        battleship.max_v
                    );


                escorts[i].time_to_hit =
                    time_to_hit;


                hit_count++;
                hit_this_iteration++;


                if (time_to_hit >
                    iteration_battle_time)
                {
                    iteration_battle_time =
                        time_to_hit;
                }


                if (time_to_hit >
                    battle_time)
                {
                    battle_time =
                        time_to_hit;
                }


                printf(
                    "Battleship destroyed "
                    "Escort Ship %d\n",
                    escorts[i].id
                );
            }
        }


        
         //REMAINING ESCORTS ATTACK BATTLESHIP  
              for (int i = 0; i < n; i++)
        {
            double min_range;
            double max_range;


            if (escorts[i].is_destroyed == 1)
            {
                continue;
            }


            min_range =
                get_min_range_escort(
                    escorts[i].min_v,
                    escorts[i].min_angle
                );


            max_range =
                get_max_range(
                    escorts[i].max_v
                );


            if (is_in_range(
                    escorts[i].x,
                    escorts[i].y,
                    min_range,
                    max_range,
                    battleship.x,
                    battleship.y))
            {
                b_destroyed = 1;

                destroyer_id =
                    escorts[i].id;


                printf(
                    "Escort Ship %d "
                    "destroyed the Battleship\n",
                    escorts[i].id
                );


                break;
            }
        }


        // SAVE SIMULATION 2 ITERATION           
         save_part1b_sim2_result(
            battleship,
            escorts,
            n,
            p + 1,
            hit_this_iteration,
            hit_count,
            b_destroyed,
            destroyer_id,
            iteration_battle_time,
            gun_jammed,
            theta_min
        );


        printf(
            "End of Simulation 2 iteration %d\n",
            p + 1
        );


        if (b_destroyed == 1)
        {
            printf(
                "\nBattleship destroyed. "
                "Simulation 2 stopped.\n"
            );

            break;
        }
    }

   // DISPLAY FINAL RESULT
       printf( "\n BATTLE RESULT \n" );
       if (b_destroyed == 1)
    {
        printf( "Battleship was destroyed by Escort Ship %d\n", destroyer_id );
    }
       else
    
    {
        printf(
            "Battleship survived all path points.\n"
        );


        printf(
            "Number of Escort Ships hit: %d\n",
            hit_count
        );


        printf(
            "Battle end time: %.2f seconds\n",
            battle_time
        );
    }


    
    //SAVE FINAL FILES
 
        save_final_conditions(
        battleship,
        escorts,
        n
    );


    save_simulation_results(
        battleship,
        escorts,
        n,
        hit_count,
        b_destroyed,
        battle_time,
        destroyer_id
    );

    }

    if (choice == 3 || choice == 6)
    {
    


     // PART 1C
FILE *part1c_file;

part1c_file =
    fopen("part1c_results.txt", "w");

if (part1c_file == NULL)
{
    printf( "Error creating part1c_results.txt\n");

    return 1;
}

fprintf(
    part1c_file,
    " PART 1C RESULTS\n"
);

fclose(part1c_file);

//PART 1C - REDO PART 1A                    


printf(
    "\n PART 1C - PART 1A \n"
);


Battleship c_battleship =
    battleship;

EscortShip c_escorts[100];

double cumulative_impact = 0.0;

int c_b_destroyed = 0;

int c_destroyer_id = -1;


// Restore original Escort conditions 

for (int i = 0; i < n; i++)
{
    c_escorts[i] =
        original_escorts[i];

    c_escorts[i].is_destroyed = 0;
    c_escorts[i].has_fired = 0;
}


// Use first path point as Part 1A position 

c_battleship.x =
    path_x[0];

c_battleship.y =
    path_y[0];


// Battleship attacks Escorts 

for (int i = 0; i < n; i++)
{
    double distance;
    double max_range;


    distance =
        get_distance(
            c_battleship.x,
            c_battleship.y,
            c_escorts[i].x,
            c_escorts[i].y
        );


    max_range =
        get_max_range(
            c_battleship.max_v
        );


    if (distance <= max_range)
    {
        c_escorts[i].is_destroyed = 1;

        printf( "Battleship destroyed Escort Ship %d\n", c_escorts[i].id );
    }
}


//Remaining Escorts attack Battleship 

c_b_destroyed =
    part1c_escort_attack(
        c_battleship,
        c_escorts,
        n,
        &cumulative_impact,
        &c_destroyer_id
    );

save_part1c_result(
    "PART 1C - PART 1A",
    c_battleship,
    c_escorts,
    n,
    1,
    cumulative_impact,
    c_b_destroyed,
    c_destroyer_id,
    0,
    0.0
);

// PART 1C - PART 1B SIMULATION 1            

printf( "\n PART 1C - PART 1B SIMULATION 1\n");


/* Reset initial conditions */

for (int i = 0; i < n; i++)
{
    c_escorts[i] =
        original_escorts[i];

    c_escorts[i].is_destroyed = 0;
    c_escorts[i].has_fired = 0;
}


cumulative_impact = 0.0;

c_b_destroyed = 0;

c_destroyer_id = -1;


/* Run same k path */

for (int p = 0;
     p < k && c_b_destroyed == 0;
     p++)
{
    c_battleship.x =
        path_x[p];

    c_battleship.y =
        path_y[p];


    printf( "\nPart 1C Simulation 1 - Iteration %d\n", p + 1 );


    printf(
        "Battleship Position: (%.2f, %.2f)\n",
        c_battleship.x,
        c_battleship.y
    );


    /* Battleship attacks Escorts */

    for (int i = 0; i < n; i++)
    {
        double distance;
        double max_range;


        if (c_escorts[i].is_destroyed == 1)
        {
            continue;
        }


        distance =
            get_distance(
                c_battleship.x,
                c_battleship.y,
                c_escorts[i].x,
                c_escorts[i].y
            );


        max_range =
            get_max_range(
                c_battleship.max_v
            );


        if (distance <= max_range)
        {
            c_escorts[i].is_destroyed = 1;

            printf(
                "Battleship destroyed Escort Ship %d\n",
                c_escorts[i].id
            );
        }
    }

    c_b_destroyed =
        part1c_escort_attack(
            c_battleship,
            c_escorts,
            n,
            &cumulative_impact,
            &c_destroyer_id
        );

    save_part1c_result(
        "PART 1C - PART 1B SIMULATION 1",
        c_battleship,
        c_escorts,
        n,
        p + 1,
        cumulative_impact,
        c_b_destroyed,
        c_destroyer_id,
        0,
        0.0
    );
}


// PART 1C - PART 1B SIMULATION 2 


printf( "\n PART 1C - PART 1B SIMULATION 2\n");

for (int i = 0; i < n; i++)
{
    c_escorts[i] =
        original_escorts[i];

    c_escorts[i].is_destroyed = 0;
    c_escorts[i].has_fired = 0;
}


cumulative_impact = 0.0;

c_b_destroyed = 0;

c_destroyer_id = -1;


for (int p = 0;
     p < k && c_b_destroyed == 0;
     p++)
{
    int gun_jammed = 0;


    c_battleship.x =
        path_x[p];

    c_battleship.y =
        path_y[p];

    if ((p + 1) > t)
    {
        gun_jammed = 1;
    }


    printf( "\nPart 1C Simulation 2 - Iteration %d\n", p + 1);


    if (gun_jammed == 1)
    {
        printf( "Gun Status: JAMMED\n");

        printf( "Allowed Vertical Angle: %.2f - 90 degrees\n", theta_min );
    }
    else
    {
        printf(  "Gun Status: NORMAL\n" );
    }


    /* Battleship attacks Escorts */

    for (int i = 0; i < n; i++)
    {
        double distance;
        double max_range;


        if (c_escorts[i].is_destroyed == 1)
        {
            continue;
        }


        distance =
            get_distance( c_battleship.x, c_battleship.y, c_escorts[i].x,  c_escorts[i].y );


        if (gun_jammed == 1)
        {
            max_range =
                get_jammed_battleship_range(
                    c_battleship.max_v,
                    theta_min
                );
        }
        else
        {
            max_range =
                get_max_range(
                    c_battleship.max_v
                );
        }


        if (distance <= max_range)
        {
            c_escorts[i].is_destroyed = 1;


            printf(  "Battleship destroyed Escort Ship %d\n",  c_escorts[i].id );
        }
    }

    c_b_destroyed =
        part1c_escort_attack(
			c_battleship,
			c_escorts,
			n,
			&cumulative_impact,
			&c_destroyer_id
			);
			

      save_part1c_result(
        "PART 1C - PART 1B SIMULATION 2",
        c_battleship,
        c_escorts,
        n,
        p + 1,
        cumulative_impact,
        c_b_destroyed,
        c_destroyer_id,
        gun_jammed,
        theta_min
    );
}
printf(
    "\nPart 1C completed.\n"
);

printf(
    "Results saved to part1c_results.txt\n"
);

}

if (choice == 4 || choice == 6)
{

FILE *part2a_file =
    fopen("part2a_results.txt", "w");

if (part2a_file == NULL)
{
    printf("Error creating part2a_results.txt\n");
    return 1;
}

fprintf(
    part2a_file,
    "PART 2A RESULTS\n"
    "T_B = %.2f seconds\n"
    "Strategy: higher impact power first, "
    "nearest escort used as tie breaker.\n",
    battleship.firing_delay
);

run_part2a_scenario( "PART 2A - PART 1A", battleship, original_escorts, n, path_x, path_y, 1, 0, t, theta_min, 0, part2a_file);

run_part2a_scenario( "PART 2A - PART 1B SIMULATION 1", battleship, original_escorts, n, path_x, path_y, k, 0, t, theta_min, 0, part2a_file);

run_part2a_scenario( "PART 2A - PART 1B SIMULATION 2", battleship, original_escorts, n, path_x, path_y, k, 1, t, theta_min, 0, part2a_file);

run_part2a_scenario( "PART 2A - PART 1C - PART 1A", battleship, original_escorts, n, path_x, path_y, 1, 0, t, theta_min, 1, part2a_file);

run_part2a_scenario( "PART 2A - PART 1C - PART 1B SIMULATION 1", battleship, original_escorts, n, path_x, path_y, k, 0, t, theta_min, 1, part2a_file);

run_part2a_scenario( "PART 2A - PART 1C - PART 1B SIMULATION 2", battleship, original_escorts, n, path_x, path_y, k, 1, t, theta_min, 1, part2a_file);

fclose(part2a_file);

printf("\npart 2A completed. \n");
printf("resutls saved to part2a_results.txt\n");
}

// PART 2B
if (choice == 5 || choice == 6)
{	

FILE *part2b_file = fopen("part2b_results.txt", "w");

if (part2b_file == NULL)
{
    printf("Error creating part2b_results.txt\n");
    return 1;
}

part2b_mode = 1;

fprintf(part2b_file,
        "PART 2B RESULTS\n"
        "T_E: A=%.2f B=%.2f C=%.2f D=%.2f E=%.2f\n",
        escort_TE[0], escort_TE[1], escort_TE[2],
        escort_TE[3], escort_TE[4]);

run_part2a_scenario("PART 2B - PART 1A", battleship, original_escorts, n, path_x, path_y, 1, 0, t, theta_min, 0, part2b_file);

run_part2a_scenario("PART 2B - PART 1B SIM 1", battleship, original_escorts, n, path_x, path_y, k, 0, t, theta_min, 0, part2b_file);

run_part2a_scenario("PART 2B - PART 1B SIM 2", battleship, original_escorts, n, path_x, path_y, k, 1, t, theta_min, 0, part2b_file);

run_part2a_scenario("PART 2B - PART 1C / 1A", battleship, original_escorts, n, path_x, path_y, 1, 0, t, theta_min, 1, part2b_file);

run_part2a_scenario("PART 2B - PART 1C / 1B SIM 1", battleship, original_escorts, n, path_x, path_y, k, 0, t, theta_min, 1, part2b_file);

run_part2a_scenario("PART 2B - PART 1C / 1B SIM 2", battleship, original_escorts, n, path_x, path_y, k, 1, t, theta_min, 1, part2b_file);

fclose(part2b_file);
part2b_mode = 0;

printf("\nPart 2B completed.\n");
printf("Results saved to part2b_results.txt\n");
}

return 0;

}





   


