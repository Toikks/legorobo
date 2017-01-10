// This is a program for driving obstacle course with LEGO Mindstorms NXT robot tank for school project
// 1st place against 7 other robots was achieved with this program. Not only was it the fastest, but also the most reliable.
// Language is "Not Exactly C" which is same syntax as C, but has minor differences
// The cannon attached to the robot, sadly was not used in the competition.

int color;              // Color to follow
int turn;               // Amount of turn
int threshold;          // Comparison point for color
int power = 100;        // Default power from motors
int min = 999;          // Minimum color sensor value
int max = -1;           // Maximum color sensor value
int dist;               // Distance to obstacle
int obstacle = 0;       // Obstacle sightings
int currentphase = 0;   // Progress on track
int timenow = 0;        // Logic
int cannonshots = 0;    // Amount of cannon fires wanted
int motorrotation = 0;  // Keeps track of cannon motor rotation
unsigned long end_time; // For time comparisons


// Keep track of progress, for troubleshooting purposes
// Phase number is printed to onboard screen
void phase(){
     currentphase++;
     ClearLine(LCD_LINE3);
     NumOut(40, LCD_LINE3, currentphase);
}

// Color sensor calibration
void calibrate(){
    SetSensorColorRed(IN_3);
    Wait(200);

    // Calibrate color sensor while turning left
    end_time = CurrentTick() + 500;
    OnFwd(OUT_B, 50);
    OnRev(OUT_C, 50);
    while (CurrentTick() < end_time) {
        color = Sensor(IN_3);
        if (color > max) {
            max = color;
        } else if (color < min) {
            min = color;
        }
    }

    // Calibrate color sensor while turning right
    if(obstacle > 1){
        end_time = CurrentTick() + 2000;
    }else{
        end_time = CurrentTick() + 1000;
    }
    OnFwd(OUT_C, 50);
    OnRev(OUT_B, 50);
    while (CurrentTick() < end_time) {
        color = Sensor(IN_3);
        if (color > max) {
            max = color;
        }else if (color < min) {
            min = color;
        }
    }
    threshold = (min+max)/2;
}

// Find edge of the track
void findtrack(){
    OnFwd(OUT_B, 50);
    OnRev(OUT_C, 50);
    while (Sensor(IN_3) > threshold) {
    }
    Off(OUT_BC);
}


// Find track from white area
void findgap(){
    OnFwd(OUT_BC, power);
    while (Sensor(IN_3) > threshold) {
    }
    Off(OUT_BC);
}

// Follow tracks edge
void follow(){
    color = Sensor(IN_3);
    turn = power+200*(threshold-color)/(max-min);
    if (turn > 100)
        turn = 100;
    if (turn < 0)
        turn = 0;
    OnFwd(OUT_C, turn);
    turn = power-200*(threshold-color)/(max-min);
    if (turn > 100)
        turn = 100;
    if (turn < 0)
        turn = 0;
    OnFwd(OUT_B, turn);}

// Prepare ultrasonic sensor
void distance(){
    SetSensorLowspeed(IN_4);
    //dist = SensorUS(IN_4);   // testing
    //ClearLine(LCD_LINE3);  // testing
    //NumOut(40, LCD_LINE3, dist);  // testing
}

// Turn left
void turning(){
    Off(OUT_BC);
    end_time = CurrentTick() + 400
    while(CurrentTick() < end_time){
        OnRev(OUT_B, 50);
        OnFwd(OUT_C, 50);
    }
    Off(OUT_BC);
}

// Fire cannon
void cannon(int cannonshots){
    motorrotation = MotorRotationCount(OUT_A);
    while(motorrotation < cannonshots){
        OnFwd(OUT_A, 100);
    }
    Off(OUT_A);
}

task main() {

    // Preparations
    distance();
    calibrate();
    findtrack();

    // Follow track until certain time has passed of face an obstacle
    end_time = CurrentTick()+18000;
    // end_time = CurrentTick()+1000;  // testing
    while (obstacle == 0 || timenow == 0){
        follow();
        if (SensorUS(IN_4) < 30 && CurrentTick() > end_time){
            obstacle++;
        }
        if(CurrentTick() > end_time){
            timenow = 1;
        }
    }

    // Move forward
    power = 75;

    // Find track and calibrate color sensor again
    while (obstacle == 1){
        turning();
        findgap();
        calibrate();
        findtrack();
        obstacle++;
    }

    // Lower speed for optimal turning
    power = 50;

    // Follow track before turning
    end_time = CurrentTick() + 3300;
    while (CurrentTick() < end_time){
        follow();
    }
    Off(OUT_BC);

    // Find track after driving over a gap
    findgap();
    calibrate();
    findtrack();

    // Stop after finding different color at end of the track
    end_time = CurrentTick() + 2000;
    while(CurrentTick() < end_time && Sensor(IN_3) < treshold){
        follow();
        power = 60;
    }
    Off(OUT_BC);
}
