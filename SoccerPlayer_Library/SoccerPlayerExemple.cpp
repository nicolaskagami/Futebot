//------------------------------------------------------------------------------
// Inclusão da biblioteca que implementa a interface com o SoccerMatch.
#include "environm.h"

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <math.h>

void printRobot(environm::soccer::robotBox rob);
float triangle(float alpha, float beta, float gamma, float input);
float trapezium(float alpha, float beta, float gamma, float theta, float input);
int main( int argc, char* argv[] ) {

    float   ballAngle, targetAngle, leftMotor, rightMotor;

    // Declaração do objeto que representa o ambiente.
    environm::soccer::clientEnvironm environment;
    

    if ( argc != 3 ) {
        printf( "\nInvalid parameters. Expecting:" );
        printf( "\nSoccerPlayer SERVER_ADDRESS_STRING SERVER_PORT_NUMBER\n" );
        printf( "\nSoccerPlayer localhost 1024\n" );        
        return 0;
    }

    // Conecta-se ao SoccerMatch. Supõe que SoccerMatch está rodando na máquina
    // local e que um dos robôs esteja na porta 1024. Porta e local podem mudar.
    if ( ! environment.connect( argv[1], atoi( argv[2] ) ) ) {
        printf( "\nFail connecting to the SoccerMatch.\n" );
        return 0;  // Cancela operação se não conseguiu conectar-se.
    }

    // Laço de execução de ações.
    printf( "\nRunning..." );
    printf("Triangulo: %f\n",triangle(25,55,85,84));
    getchar();
    while ( 1 ) {

        // Deve obter os dados desejados do ambiente. Métodos do clientEnvironm.
        // Exemplos de métodos que podem ser utilizados.
        float Width = environment.getWorldWidth();
        float Height = environment.getWorldHeight();

        float spin = environment.getSpin();
        ballAngle = environment.getBallAngle();
        targetAngle = environment.getTargetAngle( environment.getOwnGoal() );
        printf("Target Angle: %f\n",targetAngle);
        printf("Ball Angle: %f\n",ballAngle);
        printf("World: %f x %f\n",Width,Height);


        // A partir dos dados obtidos, deve inferir que ações executar. Neste
        // exemplo as forcas destinadas a cada robo sao guardadas em leftMotor e
        // rightMotor. Esta etapa deve ser substituida pelo controlador fuzzy.
        ballAngle = ballAngle - targetAngle;
        if ( ballAngle < -M_PI ) {
            ballAngle += 2 * M_PI;
        }
        if ( ballAngle > M_PI ) {
            ballAngle -= 2 * M_PI;
        }
        if ( ballAngle < ( -M_PI / 2 ) ) {
            ballAngle = -M_PI / 2;
        }
        if ( ballAngle > ( M_PI / 2 ) ) {
            ballAngle = M_PI / 2;
        }
        float distance = environment.getDistance();
        leftMotor  = (cos( ballAngle ) - sin( ballAngle ));
        rightMotor = (cos( ballAngle ) + sin( ballAngle ));
        float goalLength = environment.getGoalLength();
        environm::soccer::robotBox OwnRobot = environment.getOwnRobot(); 
        printf("Goal Length: %f \n",goalLength);
        printf("Distance: %f\n",distance);
        printf("Sin: %f\n",spin);
        printRobot(OwnRobot);
        /*switch(c)
        {
            case 'w': 
                    leftMotor = 1;
                    rightMotor = 1;
                    break;
            case 'a': 
                    leftMotor = -1;
                    rightMotor = 1;
                    break;
            case 's': 
                    leftMotor = -1;
                    rightMotor = -1;
                    break;
            case 'd': 
                    leftMotor = 1;
                    rightMotor = -1;
                    break;
        }
        *///leftMotor = 20000;
        //rightMotor = 20000;

        // Transmite ação do robô ao ambiente. Fica bloqueado até que todos os
        // robôs joguem. Se erro, retorna false (neste exemplo, sai do laco).
        if ( ! environment.act( leftMotor, rightMotor ) ) {
            break; // Termina a execução se falha ao agir.
        }
    }

    return 0;
}
//------------------------------------------------------------------------------
void printRobot(environm::soccer::robotBox rob)
{
    printf("Robot:\n");
    printf("Position: %f %f\n",rob.pos.x,rob.pos.y);
    printf("Old Position: %f %f\n",rob.oldPos.x,rob.oldPos.y);
    printf("Angle: %f\n",rob.angle);
    printf("Old Angle: %f\n",rob.oldAngle);
    printf("Difference: %f\n",rob.angle - rob.oldAngle);
    printf("Forces: %f %f\n",rob.force[0],rob.force[1]);
    printf("Action: %d\n",rob.action);
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
float getAreaX(float alpha, float beta, float gamma, float theta, float cap)
{
    newbeta = cap*(beta-alpha)+alpha;
    newgamma = (1-cap)*(theta-gamma)+gamma;
    upSlopeSize = (newbeta-alpha);
    midFlatSize = (newgamma-newbeta);
    downSlupeSize = (theta-newgamma);

    return (((upSlopeSize/2 + alpha)*upSlopeSize*cap)/2)+((midSlopeSize/2 + newbeta)*midSlopeSize*cap)+(((downSlopeSize/2 + newgamma)*downSlopeSize*cap)/2);
}
float getArea(float alpha, float beta, float gamma, float theta, float cap)
{
    newbeta = cap*(beta-alpha)+alpha;
    newgamma = (1-cap)*(theta-gamma)+gamma;
    upSlopeSize = (newbeta-alpha);
    midFlatSize = (newgamma-newbeta);
    downSlupeSize = (theta-newgamma);

    return ((upSlopeSize*cap)/2)+(midSlopeSize*cap)+((downSlopeSize*cap)/2);
}
