//#include <iostream>
//
//using namespace std;
//
//const int WIDTH = 40;
//const int HEIGHT = 30;
//const float PI = 3.142;
//
//enum STATE {TurnRight, TurnLeft, GoStraight, TJunction, SPath};
//
//int servo_cap(int servo_angle){
//	if(servo_angle > 1350){
//		return 1250;
//	}
//	if(servo_angle < 450){
//		return 550;
//	}
//	else return servo_angle;
//}
//
//// -1 means that edge is not visible in image OR not within range of camera
//// midpoint_ending_index keeps track of up to which index in midpoints[] and edges[] are valid data values
//// the first (HEIGHT) integers in edges[] keeps track of coordinates of left edges
//void update_midpoints_and_edges(bool image[HEIGHT][WIDTH], int edges[2*HEIGHT], int midpoints[HEIGHT], int& midpoint_ending_index){
//	int ending_index = HEIGHT;
//	// find midpoint of bottomest row
//	int start = 0;int end = WIDTH-1;
//	for(int i=start; i<WIDTH && !image[HEIGHT-1][i]; start = ++i);
//	for(int i=end; i >= 0 && !image[HEIGHT-1][i]; end = --i);
//
//	edges[2*HEIGHT-1] = end;
//	edges[HEIGHT-1] = start;
//	midpoints[HEIGHT-1] = double(0.5*(start+end)) + 0.5;
//	ending_index--;
//
//
//	for(int i=1; i<HEIGHT && image[HEIGHT-1-i][midpoints[HEIGHT-i]]; i++){
//		// find left & right edge
//		int left = 0; int right = 0;
//
//		for(left = midpoints[HEIGHT-i]; image[HEIGHT-1-i][left] == 1 && left >= 0; left--);
//		for(right = midpoints[HEIGHT-i]; image[HEIGHT-1-i][right] == 1 && right < WIDTH; right++);
//
//		// check if there is a change in trend, if yes check whether this row has two midpoints
//		// i did this because checking whehter there is a change in trend for each row takes less time than looping through the whole row every time to check
//		if(i > 1 && ((double(left+right)/2 + 0.5) - midpoints[HEIGHT-i])*(midpoints[HEIGHT-i]-midpoints[HEIGHT-1-i]) < 0){
//			int counter = 0;
//			for(int j=0; j<WIDTH-1 && counter < 3 ; j++){
//				if(image[HEIGHT-i-1][j] != image[HEIGHT-1-i][j+1]){
//					counter++;
//				}
//			}
//			if(counter > 2){
//				break;
//			}
//		}
//
//		edges[2*HEIGHT-1-i] = right;
//		edges[HEIGHT-1-i] = left;
//		midpoints[HEIGHT-1-i] = double(left+right)/2 + 0.5;
//
//		ending_index--;
//
//	}
//
//	midpoint_ending_index = ending_index;
//}
//
//void calculate_arc_length(int edges[2*HEIGHT], int midpoint_ending_index, double& left_edge, double& right_edge){
//	for(int i=1; HEIGHT - i >= midpoint_ending_index; i++){
//		if(edges[HEIGHT-i] != -1 && edges[HEIGHT-1-i] != -1){
//			switch(abs(edges[HEIGHT-i]-edges[HEIGHT-1-i])){
//				case 0: left_edge += 1; break;
//				case 1: left_edge += 1.414; break;
//				case 2: left_edge += 2.236; break;
//				case 3: left_edge += 3.162; break;
//				case 4: left_edge += 4.123; break;
//				case 5: left_edge += 5.099; break;
//				case 6: left_edge += 6.083; break;
//				case 7: left_edge += 7.071; break;
//				default: left_edge += abs(edges[HEIGHT-i]-edges[HEIGHT-1-i]);
//			}
//		}
//		if(edges[2*HEIGHT-i] != -1 && edges[2*HEIGHT-1-i] != -1){
//			switch(abs(edges[2*HEIGHT-i]-edges[2*HEIGHT-1-i])){
//				case 0: right_edge += 1; break;
//				case 1: right_edge += 1.414; break;
//				case 2: right_edge += 2.236; break;
//				case 3: right_edge += 3.162; break;
//				case 4: right_edge += 4.123; break;
//				case 5: right_edge += 5.099; break;
//				case 6: right_edge += 6.083; break;
//				case 7: right_edge += 7.071; break;
//				default: left_edge += abs(edges[2*HEIGHT-i]-edges[2*HEIGHT-1-i]);
//			}
//		}
//	}
//}
//
//// starting_index should be larger than ending_index, because closest midpoints are stored last
//void regression_line(int midpoints[HEIGHT],int starting_index, int ending_index, double& gradient){
//	// computation of regression line
//	double x_mean = 0;
//	double y_mean = 0;
//	double xy_mean = 0;
//	double x2_mean = 0;
//	double y2_mean = 0;
//
//	for(int i=starting_index ; i>= ending_index ; i--){
//		x_mean += midpoints[i];
//		y_mean += -i;
//		xy_mean += (midpoints[i])*(-i);
//		x2_mean += midpoints[i]*midpoints[i];
//		y2_mean += i*i;
//	}
//	x_mean /= (starting_index - ending_index + 1);
//	y_mean /= (starting_index - ending_index + 1);
//	xy_mean /= (starting_index - ending_index + 1);
//	x2_mean /= (starting_index - ending_index + 1);
//	y2_mean /= (starting_index - ending_index + 1);
//
//	int size = (starting_index - ending_index + 1);
//	// gradient of horizontal regression line
//	gradient = ((y_mean*y_mean)-y2_mean)/((x_mean*y_mean)-xy_mean);
//
//}
//
//int internal_angle(int x1, int y1, int x2, int y2, int x3, int y3){
//	int vec1_x = x2 - x1;
//	int vec1_y = y2 - y1;
//	int vec2_x = x3 - x2;
//	int vec2_y = y3 - y2;
//
//	return (180/PI)*acos((vec1_x*vec2_x + vec1_y*vec2_y)/sqrt((vec1_x*vec1_x + vec1_y*vec1_y)*(vec2_y*vec2_y + vec2_x*vec2_x)));
//}
//
//STATE determine_state(int midpoints[HEIGHT], int midpoint_ending_index, int edges[2*HEIGHT]){
//	// 1. test whether it is a t-junction:
//	int counter = 0;
//	for(int i=0; HEIGHT-i >= midpoint_ending_index; i++){
//		if(edges[HEIGHT-1-i] == 0 && edges[2*HEIGHT-1-i] == WIDTH-1){
//			counter++;
//		}
//		if(counter > 1){
//			return TJunction;
//		}
//	}
//
//	// 2. test whether it is a straight line
//	double gradient = 0; double gradient1 = 0; double gradient2 = 0;
//
//	regression_line(midpoints,HEIGHT-1,midpoint_ending_index,gradient);
//	regression_line(midpoints,HEIGHT-1,0.5*(midpoint_ending_index + HEIGHT-1),gradient1);
//	regression_line(midpoints,0.5*(midpoint_ending_index + HEIGHT-1) - 1,midpoint_ending_index,gradient2);
//	// convert to angles
//	int a = (int((180*atan(gradient)/PI)+180)%180);
//	int b = (int((180*atan(gradient1)/PI)+180)%180);
//	int c = (int((180*atan(gradient2)/PI)+180)%180);
//	if(abs(a-90) < 10){
//		if(abs(b-c) < 15 && ((b >= c && b >= a && a >= c)||(c >= b && c >= a && a >= b))){
//			return GoStraight;
//		}
//		else return SPath;
//	}
//
//	// 3. test whether it is TurnLeft or TurnRight
//	if(abs(a-90) > 20){
//		if(gradient > 0){
//			return TurnRight;
//		}
//		else return TurnLeft;
//	}
//
//	// 4. remaining possibility
//	return SPath;
//}
//
//double variance(int midpoints[HEIGHT], int midpoint_ending_index){
//	double mean = 0;
//	for(int i=HEIGHT-1; i>= midpoint_ending_index; i--){
//		mean += midpoints[i];
//	}
//	double squared_differences = 0;
//	mean /= (HEIGHT - midpoint_ending_index);
//	for(int i=HEIGHT-1; i>= midpoint_ending_index; i--){
//		squared_differences += (midpoints[i] - mean)*(midpoints[i] - mean);
//	}
//	squared_differences /= (HEIGHT - midpoint_ending_index);
//	return sqrt(squared_differences);
//}
//
//int average_midpoint_error(const midpoints[HEIGHT],int starting_index, int ending_index){
//	int sum = 0;
//	for(int i=starting_index ; i>= ending_index ; i--){
//		sum += midpoints[i];
//	}
//	sum /= (starting_index - ending_index + 1);
//	return (sum-WIDTH/2);
//}
