//Futebot versao 1.1
//1.1: Otimizador de Spin simples
//Author: Nicolas Silveira Kagami 194636

#include "environm.h"
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <math.h>

class Trapezium
{
    public:
        float alpha;
        float beta;
        float gamma;
        float theta;
        Trapezium(float a, float b, float c, float d)
        { alpha = a; beta=b; gamma=c; theta=d; }
};


float BallAngle_LeftBack;
float BallAngle_Left;
float BallAngle_Front;
float BallAngle_RightBack;
float BallAngle_Right;

float TargetAngle_LeftBack;
float TargetAngle_Left;
float TargetAngle_Front;
float TargetAngle_RightBack;
float TargetAngle_Right;

float LeftMotor_Back;
float LeftMotor_SlightBack;
float LeftMotor_SlightForward;
float LeftMotor_Forward;

float LMSumOfAreaX;
float LMSumOfArea;

float LMForce;

float RightMotor_Back;
float RightMotor_SlightBack;
float RightMotor_SlightForward;
float RightMotor_Forward;

float RMSumOfAreaX;
float RMSumOfArea;

float RMForce;

void printRobot(environm::soccer::robotBox rob);
float triangle(float alpha, float beta, float gamma, float input);
float trapezium(float alpha, float beta, float gamma, float theta, float input);
float getAreax(float alpha, float beta, float gamma, float theta, float cap);
float getArea(float alpha, float beta, float gamma, float theta, float cap);

void fuzzification(float BallAngle, float TargetAngle);
void inference();
void defuzzify();
void spinOptimization(float spin);
void collisionAvoidance();

environm::soccer::clientEnvironm environment;

int main( int argc, char* argv[] )
{
    float   ballAngle, targetAngle, leftMotor, rightMotor;
    if ( argc != 3 )
    {
        printf( "\nInvalid parameters. Expecting:" );
        printf( "\nSoccerPlayer SERVER_ADDRESS_STRING SERVER_PORT_NUMBER\n" );
        printf( "\nSoccerPlayer localhost 1024\n" );        
        return 0;
    }
    if ( ! environment.connect( argv[1], atoi( argv[2] ) ) ) 
    {
        //printf( "\nFail connecting to the SoccerMatch.\n" );
        return 0;  // Cancela operação se não conseguiu conectar-se.
    }
    while ( 1 )
    {
        float Width = environment.getWorldWidth();
        float Height = environment.getWorldHeight();
        float distance = environment.getDistance();
        float goalLength = environment.getGoalLength();
        float spin = environment.getSpin();

        ballAngle = environment.getBallAngle();
        targetAngle = environment.getTargetAngle( environment.getOwnGoal() );
        /*//printf("Target Angle: %f\n",targetAngle);
        //printf("Ball Angle: %f\n",ballAngle);
        //printf("World: %f x %f\n",Width,Height);
        //printf("Goal Length: %f \n",goalLength);
        //printf("Distance: %f\n",distance);
        printRobot(OwnRobot);
        getchar();
           
        //printf("Spin: %f\n",spin);*/
        fuzzification(ballAngle,targetAngle);
        inference();
        defuzzify();
        
        collisionAvoidance();
        spinOptimization(spin);

        if(LMForce > 1)
            LMForce = 1;
        if(LMForce < -1)
            LMForce = -1;
        if(RMForce > 1)
            RMForce = 1;
        if(RMForce < -1)
            RMForce = -1;

        if ( ! environment.act( LMForce, RMForce ) ) 
        {
            break; // Termina a execução se falha ao agir.
        }
    }

    return 0;
}
void printRobot(environm::soccer::robotBox rob)
{
    //printf("Robot:\n");
    //printf("Position: %f %f\n",rob.pos.x,rob.pos.y);
    //printf("Old Position: %f %f\n",rob.oldPos.x,rob.oldPos.y);
    //printf("Angle: %f\n",rob.angle);
    //printf("Old Angle: %f\n",rob.oldAngle);
    //printf("Difference: %f\n",rob.angle - rob.oldAngle);
    //printf("Forces: %f %f\n",rob.force[0],rob.force[1]);
    //printf("Action: %d\n",rob.action);
}
float triangle(float alpha, float beta, float gamma, float input)
{
    if((input>=gamma)||(input<=alpha))
        return 0;
    if(input>beta)
        return 1-((input-beta)/(gamma-beta));
    else
        return (input-alpha)/(beta-alpha);
}
float trapezium(float alpha, float beta, float gamma, float theta, float input)
{
    if((input>=theta)||(input<=alpha))
        return 0;
    if(input>beta)
    {
        if(input<gamma)
            return 1;
        else
            return 1-((input-gamma)/(theta-gamma));
    }
    else
        return (input-alpha)/(beta-alpha);
}
float getAreax(float alpha, float beta, float gamma, float theta, float cap)
{
    float newbeta = cap*(beta-alpha)+alpha;
    float newgamma = (1-cap)*(theta-gamma)+gamma;
    float upSlopeSize = (newbeta-alpha);
    float midFlatSize = (newgamma-newbeta);
    float downSlopeSize = (theta-newgamma);

    return (((upSlopeSize/2 + alpha)*upSlopeSize*cap)/2)+((midFlatSize/2 + newbeta)*midFlatSize*cap)+(((downSlopeSize/2 + newgamma)*downSlopeSize*cap)/2);
}
float getArea(float alpha, float beta, float gamma, float theta, float cap)
{
    float newbeta = cap*(beta-alpha)+alpha;
    float newgamma = (1-cap)*(theta-gamma)+gamma;
    float upSlopeSize = (newbeta-alpha);
    float midFlatSize = (newgamma-newbeta);
    float downSlopeSize = (theta-newgamma);

    return ((upSlopeSize*cap)/2)+(midFlatSize*cap)+((downSlopeSize*cap)/2);
}
void fuzzification(float BallAngle, float TargetAngle)
{
    BallAngle_RightBack = trapezium(-3.2,-3.2,-2.5,-2.2,BallAngle);
    BallAngle_Right = trapezium(-2.4,-2,-1,-0.5,BallAngle);
    BallAngle_Front = trapezium(-0.8,-0.5,0.5,0.8,BallAngle);
    BallAngle_Left = trapezium(0.5,1,2.4,2.5,BallAngle);
    BallAngle_LeftBack = trapezium(2.5,2.5,3.2,3.2,BallAngle);

    TargetAngle_RightBack = trapezium(-3.2,-3.2,-2.5,-2.2,TargetAngle);
    TargetAngle_Right = trapezium(-2.4,-2,-1,-0.5,TargetAngle);
    TargetAngle_Front = trapezium(-0.8,-0.5,0.5,0.8,TargetAngle);
    TargetAngle_Left = trapezium(0.5,1,2,2.4,TargetAngle);
    TargetAngle_LeftBack = trapezium(2.2,2.5,3.2,3.2,TargetAngle);

}
void inferenceInstance(float BallCondition, float TargetCondition, Trapezium LeftTrapezium, Trapezium RightTrapezium)
{
    if(BallCondition && TargetCondition)
    {
        LMSumOfAreaX += getAreax(LeftTrapezium.alpha,LeftTrapezium.beta,LeftTrapezium.gamma,LeftTrapezium.theta,fmin(BallCondition,TargetCondition));
        LMSumOfArea += getArea(LeftTrapezium.alpha,LeftTrapezium.beta,LeftTrapezium.gamma,LeftTrapezium.theta,fmin(BallCondition,TargetCondition));
        RMSumOfAreaX += getAreax(RightTrapezium.alpha,RightTrapezium.beta,RightTrapezium.gamma,RightTrapezium.theta,fmin(BallCondition,TargetCondition));
        RMSumOfArea += getArea(RightTrapezium.alpha,RightTrapezium.beta,RightTrapezium.gamma,RightTrapezium.theta,fmin(BallCondition,TargetCondition));
    }
}
void inference()
{

    //(-1,-1,-0.7,-0.5,) //Back
    //(-0.6,-0.4,-0.1,0.1,) //SlightBack
    //(-0.1,0.1,0.4,0.6,) //SlightForw
    //(0.5,0.7,1,1,) //Forw

    Trapezium Back(-1,-1,-0.7,-0.5);
    Trapezium SlightBack(-0.6,-0.4,-0.1,0.1);
    Trapezium SlightForward(-0.1,0.1,0.4,0.6);
    Trapezium Forward(0.5,0.7,1,1);


    LMSumOfAreaX = 0;
    LMSumOfArea = 0;
    RMSumOfAreaX = 0;
    RMSumOfArea = 0;
    
    inferenceInstance(BallAngle_LeftBack,TargetAngle_LeftBack,SlightBack,Forward);
    inferenceInstance(BallAngle_Left,TargetAngle_LeftBack,SlightForward,Forward);
    inferenceInstance(BallAngle_Front,TargetAngle_LeftBack,Forward,Back);
    inferenceInstance(BallAngle_Right,TargetAngle_LeftBack,Forward,SlightForward);
    inferenceInstance(BallAngle_RightBack,TargetAngle_LeftBack,SlightBack,Forward);

    inferenceInstance(BallAngle_LeftBack,TargetAngle_Left,SlightBack,Forward);
    inferenceInstance(BallAngle_Left,TargetAngle_Left,SlightForward,Forward);
    inferenceInstance(BallAngle_Front,TargetAngle_Left,Forward,Back);
    inferenceInstance(BallAngle_Right,TargetAngle_Left,Forward,Back);
    inferenceInstance(BallAngle_RightBack,TargetAngle_Left,SlightBack,Forward);

    inferenceInstance(BallAngle_LeftBack,TargetAngle_Front,Back,Forward);
    inferenceInstance(BallAngle_Left,TargetAngle_Front,Back,Forward);
    inferenceInstance(BallAngle_Front,TargetAngle_Front,Forward,Forward);
    inferenceInstance(BallAngle_Right,TargetAngle_Front,Forward,Back);
    inferenceInstance(BallAngle_RightBack,TargetAngle_Front,Back,Forward);

    inferenceInstance(BallAngle_LeftBack,TargetAngle_Right,Forward,SlightBack);
    inferenceInstance(BallAngle_Left,TargetAngle_Right,Back,Forward);
    inferenceInstance(BallAngle_Front,TargetAngle_Right,Back,Forward);
    inferenceInstance(BallAngle_Right,TargetAngle_Right,Forward,SlightForward);
    inferenceInstance(BallAngle_RightBack,TargetAngle_Right,Forward,SlightBack);

    inferenceInstance(BallAngle_LeftBack,TargetAngle_RightBack,SlightBack,Forward);
    inferenceInstance(BallAngle_Left,TargetAngle_RightBack,SlightForward,Forward);
    inferenceInstance(BallAngle_Front,TargetAngle_RightBack,Forward,Back);
    inferenceInstance(BallAngle_Right,TargetAngle_RightBack,Forward,SlightForward);
    inferenceInstance(BallAngle_RightBack,TargetAngle_RightBack,SlightBack,Forward);
}
void defuzzify()
{
    LMForce = LMSumOfAreaX/LMSumOfArea;
    RMForce = RMSumOfAreaX/RMSumOfArea;
    //printf("LM: %f\n",LMForce);
    //printf("RM: %f\n",RMForce);
}
void spinOptimization(float spin)
{
        LMForce += 4*spin;
        RMForce -= 4*spin;
}
void collisionAvoidance()
{
    static int releaseValve = 0;
    float colDistance = environment.getCollision();
    float colAngle = environment.getObstacleAngle();
    environm::soccer::robotBox OwnRobot = environment.getOwnRobot(); 
    if((OwnRobot.pos.x < -740)||(OwnRobot.pos.x > 740))
    {
        //printf("x: %f\n",OwnRobot.pos.x);
        if(releaseValve<40)
            releaseValve++;
        else
        {
            LMForce = -1;
            RMForce = -1;
        }
        return;
    }
    if((colDistance < 40)&&(colAngle < 0.8)&&(colAngle >-0.8))
    {
        releaseValve++;
        if(colDistance<5)
            colDistance =5;

        //printf("Close\n");
        //printf("Coll Angle: %f\n",colAngle);
        if(colAngle < 0)
            LMForce -= (10/colDistance); 
        else
            RMForce -= (10/colDistance);

        if((releaseValve >40)&&(colDistance > 10))
        {
            LMForce = -1;
            RMForce = -1;
            releaseValve--;
        }
    }
    else
    if((colDistance < 40)&&(colAngle < 2.0)&&(colAngle >-2.0)&&(releaseValve >40))
    {
        LMForce = 1;
        RMForce = 1;
        releaseValve--;
    }
    else
        releaseValve = 0;
}
