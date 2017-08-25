////////////////////////////////////////////////////////////////////////////////
//
// Francisco Javier Caracuel Beltrán
//
// 3º A - TSI(CCIA) - 2017
//
// explorer.h
//

#ifndef WANDER_BOT_SRC_EXPLORER_H_
#define WANDER_BOT_SRC_EXPLORER_H_

#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Odometry.h"
#include <tf/transform_datatypes.h>
#include <math.h>
#include <vector>

using namespace std;

class Explorer{
    
public:
    
    // Tunable parameters
    const static double MIN_SCAN_ANGLE = -30.0 / 180 * M_PI;
    const static double MAX_SCAN_ANGLE = +30.0 / 180 * M_PI;
    
    // Tamaño del vector donde se guardan la posiciones
    const static unsigned int SIZE_LIST_LAST_ACTIONS = 30;
    
    // Número de posiciones que comprueba para detectar si se ha quedado en una
    // esquina ciclando
    const static unsigned int SIZE_LIST_LAST_ACTIONS_LOOP = 10;
    
    // Constantes usadas para la asignación de la última acción
    const static int NA = -1;
    const static int FORWARD = 0;
    const static int LEFT = 1;
    const static int RIGHT = 2;

    Explorer();
    void startMoving();

private:
    
    ros::NodeHandle node;
    
    // Publisher to the robot's velocity command topic
    ros::Publisher commandPub; 
    
    // Subscriber to the robot's laser scan topic
    ros::Subscriber laserSub; 
    
    ros::Subscriber velSub; 
    
    ros::Subscriber poseSub;
    
    // Indicates whether the robot should continue moving
    bool keepMoving; 
    
    // Indica si está girando
    bool isTurning;
    
    // Indica si ha ciclado y está dando girando más de lo habitual
    bool turnAround;
        
    // Indica si hay obstáculos por la derecha o por la izquierda
    bool isObstacleInRight;
    bool isObstacleInLeft;
    
    // Distancia mínima a un obstáculo
    float minDistance;
    
    // Velocidad del robot
    float forwardSpeed;
    
    // Velocidad de giro
    float turnSpeed;
        
    // Vector con las últimas acciones del robot
    // -1 -> inicialización
    //  0 -> recto
    //  1 -> izquierda
    //  2 -> derecha
    vector<int> listLastActions;
    
    // Se guarda la última acción realizada
    int lastAction;
    
    void moveForward();
    void turn();
    void scanCallback(const sensor_msgs::LaserScan::ConstPtr& scan);
    void velCallback(const geometry_msgs::Twist::ConstPtr& msg);
    void poseCallback(const nav_msgs::OdometryConstPtr& msg);
    
    ////////////////////////////////////////////////////////////////////////////
    // Utils
    //
    
    // Añade en el vector de acciones, la acción realizada
    void setLastAction(int action);
    
    // Procesa las acciones que hay en el vector.
    // Busca si hay movimientos solo hacia la derecha o a la izquierda. En este
    // caso devuelve true.
    // Si hay alternados giros a izquierda o derecha con movimientos hacia 
    // delante, devuelve false.
    // En caso de devolver true, significaría que se ha encontrado en una
    // esquina y debe salir de ahí.
    bool processActions(bool init);
    
    // Cuando escanea la lista de últimas acciones y detecta que se ha quedado
    // ciclando, se deben quitar todos los 0 de esta lista para que termine 
    // correctamente el giro. Los 0 se cambiarán por la última acción realizada
    void prepareListLastActions();
    
};

#endif /* WANDER_BOT_SRC_EXPLORER_H_ */
