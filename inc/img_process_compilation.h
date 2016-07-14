#include <iostream>
using namespace std;
const int WIDTH = 80;
const int HEIGHT = 60;
const float PI = 3.142;
enum STATE {TurnRight, TurnLeft, GoStraight, TJunction, SPath, Triangle};
int servo_cap(int servo_angle){
	if(servo_angle > 1200){
		return 1200;
	}
	if(servo_angle < 600){
		return 600;
	}
	else return servo_angle;
}
// -1 means that edge is not visible in image OR not within range of camera
// midpoint_ending_index keeps track of up to which index in midpoints[] and edges[] are valid data values
// the first (HEIGHT) integers in edges[] keeps track of coordinates of left edges
// last_midpoint is the midpoint of the bottom row of the previous loop
int update_midpoints_and_edges(bool image[HEIGHT][WIDTH], int8_t midpoints[HEIGHT], int8_t& midpoint_ending_index, int8_t& absolute_midpoint_ending_index, int8_t& last_midpoint, int16_t& WCount){
	int ending_index = HEIGHT-1;
	int absolute_ending_index = HEIGHT - 1;
	// find midpoint of bottom row

	int start = 0;int end = 0; int TCount = 0;
	for(start = last_midpoint; image[HEIGHT-1][start] == 1 && start >= 0; start--);
	for(end = last_midpoint; image[HEIGHT-1][end] == 1 && end < WIDTH; end++);

	midpoints[HEIGHT-1] = double(0.5*(start+end)) + 0.5;
	last_midpoint = midpoints[HEIGHT-1];

	for(int i = 1; i<HEIGHT &&  ending_index >= 0 && image[HEIGHT-1-i][midpoints[ending_index]]; i++){
		absolute_ending_index--;
		// find left & right edge
		int left = 0; int right = 0;
		for(left = midpoints[ending_index];left >= 0 && image[HEIGHT-1-i][left] == 1 ; left--);
		for(right = midpoints[ending_index]; right < WIDTH && image[HEIGHT-1-i][right] == 1; right++);

		bool flag = 0;
		if(left < 3 && right > WIDTH-4 && i>1){ // whole row is white
			while( i<HEIGHT && ((image[HEIGHT-1-i][2] && image[HEIGHT-1-i][WIDTH-3])||(image[HEIGHT-1-i][1] && image[HEIGHT-1-i][WIDTH-2])||(image[HEIGHT-1-i][0] && image[HEIGHT-1-i][WIDTH-1])) ){
				i++;
				flag = 1;
				WCount++;
//				midpoints[ending_index - 1] = WIDTH/2 + 1;
//				ending_index--;
			}
			if(flag){
				i--;
			}
		}
		else{
			midpoints[--ending_index] = double(left+right)/2 + 0.5;
		}
		if(HEIGHT - 2 - i >= 0 && image[HEIGHT-2-i][left] && image[HEIGHT-2-i][right]){
			TCount++;
		}
//		midpoints[--ending_index] = double(left+right)/2 + 0.5;
	}
	midpoint_ending_index = ending_index;
	absolute_midpoint_ending_index = absolute_ending_index;
	return TCount;
}

// starting_index should be larger than ending_index, because closest midpoints are stored last
void regression_line(int8_t midpoints[HEIGHT],int starting_index, int ending_index, double& gradient){
	// computation of regression line
	double x_mean = 0;
	double y_mean = 0;
	double xy_mean = 0;
	double x2_mean = 0;
	double y2_mean = 0;
	for(int i=starting_index ; i>= ending_index ; i--){
		x_mean += midpoints[i];
		y_mean += -i;
		xy_mean += (midpoints[i])*(-i);
		x2_mean += midpoints[i]*midpoints[i];
		y2_mean += i*i;
	}
	x_mean /= (starting_index - ending_index + 1);
	y_mean /= (starting_index - ending_index + 1);
	xy_mean /= (starting_index - ending_index + 1);
	x2_mean /= (starting_index - ending_index + 1);
	y2_mean /= (starting_index - ending_index + 1);
	int size = (starting_index - ending_index + 1);
	// gradient of horizontal regression line
	gradient = ((y_mean*y_mean)-y2_mean)/((x_mean*y_mean)-xy_mean);
}

/*
int check_triangle(bool image[HEIGHT][WIDTH], int midpoint_ending_index, int column){
	if(image[HEIGHT - 1][column] == 0){return 0;} //Only continue if point of last (nearest) line is white
	int horizontal_length = 0, vertical_length = 0, area = 0, vertical_index = 0, distance = 0, left = 0, right = 0; //Value to be used
	int left_mid_point = 0, right_mid_point = 0, starting_black = HEIGHT - 1, ending_black = midpoint_ending_index;
	int k = 0;
	if(image[midpoint_ending_index][column] == 0){
		for(; ending_black > 0 && image[ending_black][column] == 0; ending_black++); //Find first white point for ending_black
	}
	//Find edge of vertical while line
	for(; starting_black < midpoint_ending_index - 1 && image[starting_black][column] == 1; starting_black--); starting_black++;
	for(; ending_black > 0 && image[ending_black][column] == 1; ending_black++); ending_black--;

	k = starting_black - ending_black + 1; //Length of white line

	if(k > 1 && k < 5){ //FIRST CONDITION TO ENTER THIS COMPLICATED SHIT (Length of first white line is within a certain range)
		horizontal_length++;
		area = k;
		vertical_length = k;
		vertical_index = column;
		distance = starting_black;
		left_mid_point = (starting_black + ending_black) / 2;
		right_mid_point = left_mid_point;

		for(left = column - 1; left >= 0 && image[left_mid_point][left] == 0; left --, left_mid_point = (starting_black + ending_black)/2){
			for(starting_black = left_mid_point; starting_black < midpoint_ending_index -  1 && image[starting_black][column] == 1; starting_black--); starting_black++;
			for(ending_black = left_mid_point; ending_black > 0 && image[ending_black][column] == 1; ending_black++); ending_black--;
			k = starting_black - ending_black + 1;
			if(k < 1 || k > 5){break;}
			horizontal_length++;
			area += k;
			if(k > vertical_length){
				vertical_length = k;
				vertical_index = left;
			}
			if(starting_black < distance){
				distance = starting_black;
			}
		}
		for(right = column + 1; right < WIDTH && image[right_mid_point][right] == 0; right ++, right_mid_point = (starting_black + ending_black)/2){
			for(starting_black = right_mid_point; starting_black < midpoint_ending_index - 1 && image[starting_black][column] == 1; starting_black--); starting_black++;
			for(ending_black = right_mid_point ; ending_black > 0 && image[ending_black][column] == 1; ending_black++); ending_black--;
			k = starting_black - ending_black + 1;
			if(k < 1 || k > 5){break;}
			horizontal_length++;
			area += k;
			if(k > vertical_length){
				vertical_length = k;
				vertical_index = right;
			}
			if(starting_black < distance){
				distance = starting_black;
			}
		}


		if(horizontal_length > 5){
			return 0; //Not Triangle
		}
		else if(1){    //Triangle pointing right
			return 1;
		}
		else if(1){    //Triangle pointing left
			return 2;
		}
		else if(1){    //Triangle pointing up
			return 3;
		}
	}

	return 0;   //Not triangle
}
*/
/*
void check_triangle(bool image[HEIGHT][WIDTH], int midpoint_ending_index, int column, int& left, int& right, int& area, int& nearest, int& furthest){
	if(image[HEIGHT - 1][column] == 0){return;} //Only continue if point of last (nearest) line is white
	int horizontal_length = 0, vertical_length = 0, vertical_index = 0, left_mid_point = 0, right_mid_point = 0; //Value to be used
	area = 0; left = 0; right = 0; nearest = 0; furthest = 0;
	int starting_black = HEIGHT - 1, ending_black = midpoint_ending_index;
	int k = 0;
	if(image[midpoint_ending_index][column] == 0){
		for(; ending_black < HEIGHT - 1 && image[ending_black][column] == 0; ending_black++); //Find first white point for ending_black
	}
	//Find edge of vertical while line
	for(; starting_black > midpoint_ending_index - 1  && image[starting_black][column] == 1; starting_black--); starting_black++;
	for(; ending_black < HEIGHT - 1 && image[ending_black][column] == 1; ending_black++); ending_black--;

	k = starting_black - ending_black + 1; //Length of white line

	if(k > 0 && k < 30){ //FIRST CONDITION TO ENTER THIS COMPLICATED SHIT (Length of first white line is within a certain range)
		horizontal_length++;
		area = k;
		vertical_length = k;
		vertical_index = column;
		nearest = starting_black;
		furthest = ending_black;
		left_mid_point = double(0.5*(starting_black + ending_black)) + 0.5;
		right_mid_point = left_mid_point;

		for(left = column - 1; left >= 0 && image[left_mid_point][left] == 0; left --, left_mid_point = double(0.5*(starting_black + ending_black)) + 0.5){
			for(starting_black = left_mid_point; starting_black < HEIGHT - 1 && image[starting_black][column] == 0; starting_black++); starting_black--;
			for(ending_black = left_mid_point; ending_black > midpoint_ending_index - 1 && image[ending_black][column] == 0; ending_black--); ending_black++;
			k = starting_black - ending_black + 1;
			if(k < 1 || k > 25){break;}
			horizontal_length++;
			area += k;
			if(k > vertical_length){
				vertical_length = k;
				vertical_index = left;
			}
			if(starting_black > nearest){
				nearest = starting_black;
			}
			if(ending_black < furthest){
				furthest = ending_black;
			}
		}
		if(left < 2){return;}
		for(right = column + 1; right < WIDTH && image[right_mid_point][right] == 0; right ++, right_mid_point = double(0.5*(starting_black + ending_black)) + 0.5){
			for(starting_black = right_mid_point; starting_black < HEIGHT - 1 && image[starting_black][column] == 0; starting_black++); starting_black--;
			for(ending_black = right_mid_point ; ending_black > midpoint_ending_index - 1 && image[ending_black][column] == 0; ending_black--); ending_black++;
			k = starting_black - ending_black + 1;
			if(k < 1 || k > 25){break;}
			horizontal_length++;
			area += k;
			if(k > vertical_length){
				vertical_length = k;
				vertical_index = right;
			}
			if(starting_black > nearest){
				nearest = starting_black;
			}
			if(ending_black < furthest){
				furthest = ending_black;
			}
		}
		if(nearest - furthest > 25){return;}
		if(right > WIDTH - 3){return;}
//		return left_mid_point;
	}
//	return ;
}
*/
int check_triangle(bool image[HEIGHT][WIDTH], int midpoint_ending_index, int column){
	if(image[HEIGHT - 1][column] == 0){return 0;} //Only continue if point of last (nearest) line is white
	int horizontal_length = 0, vertical_length = 0, vertical_index = 0, left_mid_point = 0, right_mid_point = 0; //Value to be used
	int area = 0, left = 0, right = 0, nearest = 0, furthest = 0;
	int starting_black = HEIGHT - 1, ending_black = midpoint_ending_index;
	int k = 0;
	if(image[midpoint_ending_index][column] == 0){
		for(; ending_black < HEIGHT - 1 && image[ending_black][column] == 0; ending_black++); //Find first white point for ending_black
	}
	//Find edge of vertical while line
	for(; starting_black > midpoint_ending_index - 1  && image[starting_black][column] == 1; starting_black--); starting_black++;
	for(; ending_black < HEIGHT - 1 && image[ending_black][column] == 1; ending_black++); ending_black--;

	k = starting_black - ending_black + 1; //Length of white line

	if(k > 0 && k < 30){ //FIRST CONDITION TO ENTER THIS COMPLICATED SHIT (Length of first white line is within a certain range)
		horizontal_length++;
		area = k;
		vertical_length = k;
		vertical_index = column;
		nearest = starting_black;
		furthest = ending_black;
		left_mid_point = double(0.5*(starting_black + ending_black)) + 0.5;
		right_mid_point = left_mid_point;

		for(left = column - 1; left >= 0 && image[left_mid_point][left] == 0; left --, left_mid_point = double(0.5*(starting_black + ending_black)) + 0.5){
			for(starting_black = left_mid_point; starting_black < HEIGHT - 1 && image[starting_black][column] == 0; starting_black++); starting_black--;
			for(ending_black = left_mid_point; ending_black > midpoint_ending_index - 1 && image[ending_black][column] == 0; ending_black--); ending_black++;
			k = starting_black - ending_black + 1;
			if(k < 1 || k > 25){break;}
			horizontal_length++;
			area += k;
			if(k > vertical_length){
				vertical_length = k;
				vertical_index = left;
			}
			if(starting_black > nearest){
				nearest = starting_black;
			}
			if(ending_black < furthest){
				furthest = ending_black;
			}
		}
		if(left < 2){return 0;}
		for(right = column + 1; right < WIDTH && image[right_mid_point][right] == 0; right ++, right_mid_point = double(0.5*(starting_black + ending_black)) + 0.5){
			for(starting_black = right_mid_point; starting_black < HEIGHT - 1 && image[starting_black][column] == 0; starting_black++); starting_black--;
			for(ending_black = right_mid_point ; ending_black > midpoint_ending_index - 1 && image[ending_black][column] == 0; ending_black--); ending_black++;
			k = starting_black - ending_black + 1;
			if(k < 1 || k > 25){break;}
			horizontal_length++;
			area += k;
			if(k > vertical_length){
				vertical_length = k;
				vertical_index = right;
			}
			if(starting_black > nearest){
				nearest = starting_black;
			}
			if(ending_black < furthest){
				furthest = ending_black;
			}
		}
		if(nearest - furthest > 25){return 0;}
		if(right > WIDTH - 3){return 0;}
		return area;
	}
	return 0;
}



STATE determine_state(int8_t midpoints[HEIGHT], int midpoint_ending_index, int absolute_midpoint_endnig_index, int TCount, int WCount){
	// 1. test whether it is a t-junction:
	//	if(TCount > 15 && WCount > 10){
	//		return TJunction;
	//	}

	// 2. test whether it is a straight line
	double gradient = 0; double gradient1 = 0; double gradient2 = 0; double gradient3 = 0;
	regression_line(midpoints,HEIGHT-1,midpoint_ending_index,gradient); // all points
	regression_line(midpoints,HEIGHT-1,midpoint_ending_index + (-midpoint_ending_index + HEIGHT-1)/3,gradient1); // closest one third
	regression_line(midpoints,HEIGHT-1 - (-midpoint_ending_index + HEIGHT-1)/3-1,HEIGHT-1 - 2*(-midpoint_ending_index + HEIGHT-1)/3,gradient2); // 1/3 till 2/3
	regression_line(midpoints,HEIGHT-1 - 2*(-midpoint_ending_index + HEIGHT-1)/3-1,midpoint_ending_index,gradient3); // 2/3 till 1
	// convert to angles
	int a = (int((180*atan(gradient)/PI)+180)%180);
	int b = (int((180*atan(gradient1)/PI)+180)%180);
	int c = (int((180*atan(gradient2)/PI)+180)%180);
	int d = (int((180*atan(gradient3)/PI)+180)%180);
	if(abs(a-90) <= 20){
		if(abs(b-c) < 30 && abs(c-d) < 30 && abs(b-d) < 30){
			return GoStraight;
		}
		else return SPath;
	}
	//
	// 3. test whether it is TurnLeft or TurnRight
	else{
		if(abs(b-c) < 15 && abs(c-d) < 15 && abs(b-d) < 15){
			return GoStraight;
		}
		else if(gradient > 0 && b >= c && c >= d){
			return TurnRight;
		}
		else if(gradient < 0 && b <= c && c <= d){
			return TurnLeft;
		}
	}
	return SPath;
}

int average_midpoint_error(const int8_t midpoints[HEIGHT],int starting_index, int ending_index){
	int sum = 0;
	for(int i=starting_index ; i>= ending_index ; i--){
		sum += midpoints[i];
	}
	sum /= (starting_index - ending_index + 1);
	return (sum-WIDTH/2);
}