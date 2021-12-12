/*code for virtual bowling*/

#include <ssl_client.h>
#include <cmath>
#include <Arduino.h>
#include <M5Core2.h>

ButtonColors on_clrs = { WHITE, RED, BLUE }; //set of colors for button to display on screen
ButtonColors off_clrs = { WHITE, WHITE, WHITE }; //set of colors for button to hide on screen
//Button* button1 = NULL; //https://forum.m5stack.com/topic/2971/solution-to-make-button-disappear

//button is used as pointer otherwise the button would continue to appear on the screen when you click at its position
//the following are pointers to buttons that are used in the program
Button* tl;
Button* single;   
Button* multi;
Button* twop;
Button* threep;
Button* bhold;

//varables for controlling the excution of while loop
int Pressing1 = 0;
int Pressing2 = 0;

//varables to store the IMU data
float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;

//difficulty level (hard,middle,easy)
int mode = 0;

//array to store the accleration data in x and z direction
double velocityaluea[1000] = { 0.0 };
double velocityaluec[1000] = { 0.0 };


void setup() {
    // put your setup code here, to run once:
    M5.begin();  //initialize the M5stack
    M5.IMU.Init(); //initialize the IMU
}


void loop() {
    // put your main code here, to run repeatedly:

    M5.update();  //retrieve data from the stack (check whether buttun is touched or not)
    //int gamerun = 0;
    int diffmode = 0; //difficulty level (hard,middle,easy)
    //int countinuegame = 0;
    M5.Lcd.fillScreen(WHITE); //set the LCD screen color to white
    M5.Lcd.setTextColor(BLACK);//set text to black
    int touchp = 0; //varable for controlling the excution of while loop
    int checktl = 0; //varable for controlling the excution of while loop

    while (Pressing1 == 0) {//game status: 0 = is gaming , 1 = exit

        while (touchp == 0) { //make sure this loop run once only
            M5.Lcd.setCursor(30, 70); 
            M5.Lcd.println("Welcome to virtual bowling!"); //print statements
            delay(500); //unit is ms
            tl = new Button(10, 100, 300, 100, false, "START"); //create a pointer to button
            tl->draw({ YELLOW, BLACK, WHITE }); //button show on screen
            M5.update();
            while (checktl == 0) { //waiting for user to hit the start 
                M5.update();
                if ((tl->pressedFor(1000, 200) || tl->wasReleased())) {
                    checktl = 1; //after the button is hit, exit the inner while loop
                    touchp = 1;  //after the button is hit, exit the outer while loop
                    tl->hide();  //hide the button
                    delete tl;  //deallocate the button so it won't appear on screen anymore
                }

            }
        }


        while (checktl == 1) {
            M5.update();

            M5.Lcd.clear(WHITE);  //clear screen to white color
            M5.Lcd.setCursor(10, 50);
            M5.Lcd.println("Please select the gamemode: ");
            single = new Button(10, 120, 120, 100, false, "Single player");  //create a pointer to button
            multi = new Button(150, 120, 120, 100, false, "Multiple players");  //create a pointer to button
            M5.Lcd.setCursor(10, 70);
            M5.Lcd.println("Please select leftmost button if you     want to exit. ");
            single->draw({ WHITE, BLACK, BLUE });   //button show on screen
            multi->draw({ WHITE, BLACK, BLUE });    //button show on screen
            delay(500);
            if (Pressing2 != 3) { Pressing2 = 0; }   //if the user do not want to quit the game, then excute the following while loop again
            while (Pressing2 == 0) {

                M5.update();

                //cases for single player
                if (single->isPressed()) {
                    Pressing2 = 1;  //button is pressed
                    M5.Lcd.clear(WHITE);  //clear screen to white color
                    single->hide();  //hide the button
                    delete single;  //deallocate the button so it won't appear on screen anymore
                    delete multi;  //deallocate the button so it won't appear on screen anymore
                    diffmode = gamestart();  //choosing the difficulty level
                    int hit = 0;  //record how many balls are hit
                    hit = recordingdata(diffmode);  //records accleration data and returns how many balls are hit
                    M5.Lcd.clear(WHITE);
                    M5.Lcd.setCursor(10, 20);
                    M5.Lcd.print("Total hit:");
                    M5.Lcd.println(hit);
                    M5.Lcd.setCursor(10, 40);
                    M5.Lcd.println("Please click the button to return   to main menu.");  //create a pointer to button
                    multi = new Button(150, 100, 120, 100, false, "Return");  
                    multi->draw({ WHITE, BLACK, BLUE });
                    delay(500);
                    int modenow = 0;
                    while (modenow == 0) {  //wait for player to hit button
                        M5.update();
                        if ((multi->pressedFor(1000, 200) || multi->wasReleased())) {
                            M5.Lcd.clear(WHITE);
                            delete multi;
                            modenow = 1;  //exit the loop
                        }
                    }


                }
                M5.update();

                //cases for double player
                if (multi->isPressed()) {
                    M5.update();
                    Pressing2 = 1;  //button is pressed

                    M5.Lcd.clear(WHITE);
                    single->hide();
                    delete single;
                    delete multi;
                    M5.Lcd.setCursor(10, 50);
                    M5.Lcd.println("Please select the number of players. ");
                    twop = new Button(10, 120, 120, 100, false, "2");
                    threep = new Button(150, 120, 120, 100, false, "3");
                    twop->draw({ WHITE, BLACK, BLUE });
                    threep->draw({ WHITE, BLACK, BLUE });
                    int nummode = 0;
                    M5.update();
                    while (nummode == 0) {
                        M5.update();
                        
                        //excutions when there are 2 players
                        if (twop->isPressed()) {
                            
                            nummode = 1;   //button is pressed
                            M5.Lcd.clear(WHITE);

                            delete twop;
                            delete threep;
                            int players[2];  //array players[] record the balls each player hit
                            diffmode = gamestart();

                            //for loop records the data for 2 players
                            for (int ii = 0; ii < 2; ii++) {
                                M5.Lcd.setCursor(10, 20);
                                M5.Lcd.print("Round for player ");
                                M5.Lcd.println(ii + 1);
                                delay(500);

                                //ask if the player is ready to go
                                M5.Lcd.println("Are you ready?");
                                multi = new Button(150, 100, 120, 100, false, "Ready");
                                multi->draw({ WHITE, BLACK, BLUE });
                                delay(500);
                                int snow = 0;
                                while (snow == 0) { //wait for button to be pressed
                                    M5.update();
                                    if (multi->isPressed()) {
                                        M5.Lcd.clear(WHITE);
                                        delete multi;
                                        snow = 1;
                                    }
                                }


                                int hit = 0;
                                
                                players[ii] = recordingdata(diffmode);  //record hit 
                                M5.Lcd.clear(WHITE);
                                M5.Lcd.setCursor(10, 20);
                                M5.Lcd.print("Total hit:");
                                M5.Lcd.println(players[ii]);
                                M5.Lcd.setCursor(10, 40);
                                M5.Lcd.println("Please click the button to continue.");
                                //continue will take players to the next step (next player or display winner)
                                multi = new Button(150, 100, 120, 100, false, "Continue");
                                multi->draw({ WHITE, BLACK, BLUE });
                                delay(500);
                                int modenew = 0;
                                while (modenew == 0) { //wait for button to be pressed
                                    M5.update();
                                    if ((multi->pressedFor(1000, 200) || multi->wasReleased())) {
                                        M5.Lcd.clear(WHITE);
                                        delete multi;
                                        modenew = 1;
                                    }
                                }
                            }

                            //conditions for winners and draw round
                            if (players[0] > players[1]) { //Player 1 wins
                                M5.Lcd.clear(WHITE);
                                M5.Lcd.setCursor(10, 20);
                                M5.Lcd.println("Player 1 wins with ");
                                M5.Lcd.setCursor(10, 40);
                                M5.Lcd.print(players[0]);
                                M5.Lcd.print("  to  ");
                                M5.Lcd.println(players[1]);
                                delay(1000);
                            }
                            else if (players[0] < players[1]) { //Player 2 wins
                                M5.Lcd.clear(WHITE);
                                M5.Lcd.setCursor(10, 20);
                                M5.Lcd.println("Player 2 wins with ");
                                M5.Lcd.setCursor(10, 40);
                                M5.Lcd.print(players[1]);
                                M5.Lcd.print("  to  ");
                                M5.Lcd.println(players[0]);
                                delay(1000);

                            }
                            else { //Players draw
                                M5.Lcd.clear(WHITE);
                                M5.Lcd.setCursor(10, 20);
                                M5.Lcd.println("Player 1 and 2 draw with");
                                M5.Lcd.setCursor(10, 40);
                                M5.Lcd.println(players[1]);
                                delay(1000);
                            }
                            M5.update();

                            //ask for user to return
                            M5.Lcd.println("Please click the button to return.");
                            multi = new Button(150, 100, 120, 100, false, "Return");
                            multi->draw({ WHITE, BLACK, BLUE });
                            delay(500);
                            int modenow = 0;
                            while (modenow == 0) {  //wait for button to be pressed
                                M5.update();
                                if ((multi->pressedFor(1000, 200) || multi->wasReleased())) {
                                    M5.Lcd.clear(WHITE);
                                    delete multi;
                                    modenow = 1;
                                }
                            }

                        }

                        ////excutions when there are 3 players
                        else if (multi->isPressed()) {

                            nummode = 1;
                            M5.Lcd.clear(WHITE);

                            delete twop;
                            delete threep;
                            int players[3];  //array players[] record the balls each player hit
                            diffmode = gamestart();

                            //for loop records the data for 3 players
                            for (int ii = 0; ii < 3; ii++) {
                                M5.Lcd.setCursor(10, 20);
                                M5.Lcd.print("Round for player ");
                                M5.Lcd.println(ii + 1);
                                delay(500);

                                //ask if the player is ready to go
                                M5.Lcd.println("Are you ready?");
                                multi = new Button(150, 100, 120, 100, false, "Ready");
                                multi->draw({ WHITE, BLACK, BLUE });
                                delay(500);
                                int snow = 0;
                                while (snow == 0) {  //wait for button to be pressed
                                    M5.update();
                                    if (multi->isPressed()) {
                                        M5.Lcd.clear(WHITE);
                                        delete multi;
                                        snow = 1;
                                    }
                                }


                                int hit = 0;
                                players[ii] = recordingdata(diffmode);  //record hit 
                                M5.Lcd.clear(WHITE);
                                M5.Lcd.setCursor(10, 20);
                                M5.Lcd.print("Total hit:");
                                M5.Lcd.println(players[ii]);
                                M5.Lcd.setCursor(10, 40);
                                M5.Lcd.println("Please click the button to continue.");
                                //continue will take players to the next step (next player or display winner)
                                multi = new Button(150, 100, 120, 100, false, "Continue");
                                multi->draw({ WHITE, BLACK, BLUE });
                                delay(500);
                                int modenew = 0;
                                while (modenew == 0) {  //wait for button to be pressed
                                    M5.update();
                                    if ((multi->pressedFor(1000, 200) || multi->wasReleased())) {
                                        M5.Lcd.clear(WHITE);
                                        delete multi;
                                        modenew = 1;
                                    }
                                }
                            }

                            //get the highest hit and lowest hit
                            int getmax(-1), getmaxi(0);
                            int getmin(11), getmini(0);
                            for (int ii = 0; ii < 3; ii++) {
                                if (players[ii] > getmax) { getmax = players[ii]; getmaxi = ii + 1; }
                                if (players[ii] < getmin) { getmin = players[ii]; getmini = ii + 1; }
                            }

                            //different situations for hits of each player
                            if (getmini == getmaxi) {  //Players draw
                                M5.Lcd.clear(WHITE);
                                M5.Lcd.setCursor(10, 20);
                                M5.Lcd.print("Players draw with  ");
                                M5.Lcd.println(players[1]);
                                delay(1000);
                            }
                            else if (players[0] == players[1]) {  
                                if (1 == getmaxi) {  //Player 1 and 2 wins player 3
                                    M5.Lcd.clear(WHITE);
                                    M5.Lcd.setCursor(10, 20);
                                    M5.Lcd.println("Player 1 and 2 wins player 3 with ");
                                    M5.Lcd.setCursor(10, 40);
                                    M5.Lcd.print(players[1]);
                                    M5.Lcd.print("  to  ");
                                    M5.Lcd.println(players[2]);
                                    delay(1000);
                                }
                                else {
                                    //Player 3 wins player 1 and 2
                                    M5.Lcd.clear(WHITE);
                                    M5.Lcd.setCursor(10, 20);
                                    M5.Lcd.println("Player 3 wins player 1 and 2 with ");
                                    M5.Lcd.setCursor(10, 40);
                                    M5.Lcd.print(players[2]);
                                    M5.Lcd.print("  to  ");
                                    M5.Lcd.println(players[1]);
                                    delay(1000);


                                }




                            }
                            else if (players[0] == players[2]) {
                                if (1 == getmaxi) {//Player 1 and 3 wins player 2
                                    M5.Lcd.clear(WHITE);
                                    M5.Lcd.setCursor(10, 20);
                                    M5.Lcd.println("Player 1 and 3 wins player 2 with ");
                                    M5.Lcd.setCursor(10, 40);
                                    M5.Lcd.print(players[0]);
                                    M5.Lcd.print("  to  ");
                                    M5.Lcd.println(players[1]);
                                    delay(1000);
                                }
                                else {
                                    //Player 2 wins player 1 and 3
                                    M5.Lcd.clear(WHITE);
                                    M5.Lcd.setCursor(10, 20);
                                    M5.Lcd.println("Player 2 wins player 1 and 3 with ");
                                    M5.Lcd.setCursor(10, 40);
                                    M5.Lcd.print(players[1]);
                                    M5.Lcd.print("  to  ");
                                    M5.Lcd.println(players[0]);
                                    delay(1000);


                                }
                            }
                            else if (players[2] == players[1]) {
                                if (2 == getmaxi) {//Player 3 and 2 wins player 1
                                    M5.Lcd.clear(WHITE);
                                    M5.Lcd.setCursor(10, 20);
                                    M5.Lcd.println("Player 3 and 2 wins player 1 with ");
                                    M5.Lcd.setCursor(10, 40);
                                    M5.Lcd.print(players[1]);
                                    M5.Lcd.print("  to  ");
                                    M5.Lcd.println(players[0]);
                                    delay(1000);
                                }
                                else {
                                    //Player 1 wins player 3 and 2
                                    M5.Lcd.clear(WHITE);
                                    M5.Lcd.setCursor(10, 20);
                                    M5.Lcd.println("Player 1 wins player 3 and 2 with ");
                                    M5.Lcd.setCursor(10, 40);
                                    M5.Lcd.print(players[0]);
                                    M5.Lcd.print("  to  ");
                                    M5.Lcd.println(players[1]);
                                    delay(1000);


                                }
                            }
                            else {//each hit differently
                                M5.Lcd.setCursor(10, 20);
                                M5.Lcd.print("Rank 1 is player  ");
                                M5.Lcd.print(getmaxi);
                                M5.Lcd.print("  Hit: ");
                                M5.Lcd.println(getmax);
                                M5.Lcd.setCursor(10, 40);
                                M5.Lcd.print("Rank 2 is player  ");
                                M5.Lcd.print(6 - getmaxi - getmini);
                                M5.Lcd.print("  Hit: ");
                                M5.Lcd.println(players[6 - getmaxi - getmini - 1]);
                                M5.Lcd.setCursor(10, 60);
                                M5.Lcd.print("Rank 3 is player  ");
                                M5.Lcd.print(getmini);
                                M5.Lcd.print("  Hit: ");
                                M5.Lcd.println(getmin);
                                delay(1000);



                            }

                            M5.update();
                            
                            //ask for user to return
                            M5.Lcd.println("Please click the button to return.");
                            multi = new Button(150, 100, 120, 100, false, "Return");
                            multi->draw({ WHITE, BLACK, BLUE });
                            delay(500);
                            int modenow = 0;
                            while (modenow == 0) {
                                M5.update();
                                if ((multi->pressedFor(1000, 200) || multi->wasReleased())) {
                                    M5.Lcd.clear(WHITE);
                                    delete multi;
                                    modenow = 1;
                                }
                            }
                        }
                    }


                }


                if (M5.BtnA.pressedFor(1000, 200) || M5.BtnA.wasReleased()) {
                    //check if the player want to exit, set different values to exit the loop
                    Pressing1 = 1; 
                    Pressing2 = 3;
                    checktl = 0;
                }

            }




        }

    }
    M5.Lcd.clear(WHITE);
    M5.Lcd.setCursor(10, 20);
    M5.Lcd.println("Thank you for playing!");
    delay(10000); //keep showing "Thank you for playing!"

}

int gamestart() {
    //choosing the difficulty level
    M5.Lcd.clear(WHITE);
    M5.Lcd.setCursor(100, 100);
    M5.Lcd.println("Please chose the difficulty level: hard, middle, easy. Double click on dots below the screen. The dots (from left to right) stands for hard, middle and easy respectively.)");
    delay(1000);

    while (mode == 0) {//wait for player to hit the button
        M5.update();
        if (M5.BtnA.pressedFor(1000, 200) || M5.BtnA.wasReleased()) {
            mode = 3;
            M5.Lcd.clear(WHITE);
            break;
        }
        else if (M5.BtnB.pressedFor(1000, 200) || M5.BtnB.wasReleased()) {
            mode = 2;
            M5.Lcd.clear(WHITE);
            break;
        }
        else if (M5.BtnC.pressedFor(1000, 200) || M5.BtnC.wasReleased()) {
            mode = 1;
            M5.Lcd.clear(WHITE);
            break;
        }




    }

    return mode;


}

int recordingdata(int diffuculty) {
    //this functions records accleration data and returns how many balls are hit
    double disa(0), disc(0); //displacement in the x and z direction
    double velocitya, velocityc; //velocity in the x and z direction
    delay(1000);
    int i = 0;
    int statusbutton = 0;
    M5.Lcd.clear(WHITE);
    M5.Lcd.setCursor(20, 50);
    M5.Lcd.println("Starting now. Please press hold till you want to release.");
    delay(100);
    M5.update();
    bhold = new Button(120, 100, 100, 100, false, "HOLD", on_clrs);
    bhold->draw(on_clrs);  //let button shown on screen 
    while (i == 0) { //waiting for player to press hold
        M5.update();
        if (bhold->pressedFor(1000, 200)) { //button is pressed
            statusbutton = 1;
            while (statusbutton == 1) {//loop for recording the data
                M5.update();
                i = i + 1;
                //obtain data from IMU
                M5.IMU.getAccelData(&accX, &accY, &accZ);
                //velocityaluea[i] and velocityaluec[i] is used to record the accleration data in x and z direction
                velocityaluea[i] = accX * 6; //6 is emperical value, accX*6 is close to real value
                velocityaluec[i] = accZ * 6; //6 is emperical value, accZ*6 is close to real value
                if (bhold->isReleased()) { statusbutton = 0; } //stop recording to exit the loop
                if (i>999) {statusbutton = 0;} //stop recording after 1s to exit the loop
                delay(2); //record data every 2ms
            }

            M5.Axp.SetLDOEnable(3, true); //send vibration to user that the ball has released 
            delay(100); //vibrate 100ms
            M5.Axp.SetLDOEnable(3, false); //stop the vibration
            delay(50);
        }
    }
    delete bhold;  //release dynamic memory
    velocitya = integralofv(velocityaluea, i, disa); //caculate velocity and displacement
    velocityc = integralofv(velocityaluec, i, disc); //caculate velocity and displacement

    return (calculationnum(disc, disa, velocityc, velocitya, diffuculty));
    //calculationnum(): calculate how many balls are hit

}


double integralofv(double arr[], int n, double& displacement) {
    //caculate the velocity and displacement 
    double sum;
    sum = 0;
    displacement = 0;


    for (int i = 0; i < n; i++) {
        if (abs(arr[i]) > 0.2) {
            sum = sum + arr[i] * 0.002;   //calculate the velocity (numerical integration)
            displacement = displacement + sum * 0.002;   //calculate the displacement (numerical integration)

        }

    }

    return sum;
}



int calculationnum(double x0, double y0, double vxi, double vyi, int diffuculty) { 
  //the part that calculates how many sticks are hit

    double acc = -0.05 * diffuculty;//define accelaration
    double sticks = 10;//define total pin num according to real life

    double ki = vyi / vxi;
    double ci = 0 - ki * x0;
    double kleft, cleft;//the left side of pin triangle, see explaination grah for more
    calcline(0, 15, -5, 18, kleft, cleft);
    double kright, cright;//the right side
    calcline(0, 15, 5, 18, kright, cright);
    double kideal, cideal;
    calcline(x0, 0, 0, 18, kideal, cideal);
    double x1, y1, x2, y2;
    calcintersect(ki, ci, kleft, cleft, x1, y1);
    calcintersect(ki, ci, kright, cright, x2, y2);
    double xtrue, ytrue, xideal, yideal;
    double ratio1, ratio2;
    double xdistance;
    if (x1 > 0) { // determine whether left or right side is hit
        xtrue = x2;
        ytrue = y2;
        calcintersect(kright, cright, kideal, cideal, xideal, yideal);
    }
    else {
        xtrue = x1;
        ytrue = y1;
        calcintersect(kleft, cleft, kideal, cideal, xideal, yideal);
    }
    if (abs(xtrue) > 5) { // the first ratio: precision calculation
        ratio1 = 0;
    }
    else {
        xdistance = xtrue - xideal;
        ratio1 = calcprecision(xdistance, xideal, diffuculty);
    }

    double vi = pow(pow(vxi, 2) + pow(vyi, 2), 0.5);
    double displacement = pow((pow((xtrue - x0), 2) + pow((ytrue - y0), 2)), 0.5);
    ratio2 = calcspeed(vi, acc, displacement, diffuculty); 
    // the ratio for determining whether the speed is good enough and how good it is

    double RESULT = int(sticks * ratio1 * ratio2) + 1;
    if (RESULT == 11) {
        RESULT = 10;
    }
    return RESULT;
}



void calcline(double x1, double y1, double x2, double y2, double& k, double& c) { 
  // calculate the line's k and c
    k = (y2 - y1) / (x2 - x1);
    c = y1 - x1 * k;
}
void calcintersect(double k1, double c1, double k2, double c2, double& x, double& y) { 
  // calculate intersection between two lines
    x = (c2 - c1) / (k1 - k2);
    y = k1 * x + c1;
}
double calcprecision(double xdistance, double xideal, double diffuculty) {
  //ratio1=calcprecision(xdistance, xideal, diffuculty); xdistance = xtrue-xideal;
    double ratio;
    if (xdistance > 0) { //left
        ratio = -1 / (5 - xideal) * xdistance + 1;
    }
    else {
        ratio = 1 / (xideal + 5) * xdistance + 1;
    }
    ratio = ratio / diffuculty;
    if (ratio > 1)
        ratio = 1;
    return ratio;
}
double calcspeed(double vi, double a, double d, double diffuculty) {
    double vf;
    double ratio;
    if ((2 * a * d + pow(vi, 2)) < 0) {
        ratio = 0;
    }
    else {
        vf = pow((2 * a * d + pow(vi, 2)), 0.5);
        ratio = vf / (diffuculty * 3);
    }
    if (ratio > 1)
        ratio = 1;
    return ratio;
}
