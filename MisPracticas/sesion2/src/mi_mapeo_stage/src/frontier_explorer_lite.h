////////////////////////////////////////////////////////////////////////////////
//
// Francisco Javier Caracuel Beltrán
//
// Técnicas de los Sistemas Inteligentes. CCIA. GII.
//
// Curso 2016/2017
//
// Clase FrontierExplorer
// 
// Fichero: frontier_explorer_lite.h
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// catkin_make
// roslaunch mi_mapeo_stage mi_gmapping.launch
// rosrun mi_mapeo_stage mi_mapeo_stage
////////////////////////////////////////////////////////////////////////////////

#ifndef FRONTIEREXPLORER_H
#define FRONTIEREXPLORER_H

#include <vector>

#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "nav_msgs/Odometry.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/OccupancyGrid.h"
#include <move_base_msgs/MoveBaseAction.h>

#include "costmap_2d/costmap_2d_ros.h"
#include <costmap_2d/costmap_2d.h>

// Para dibujar en rviz
#include <visualization_msgs/Marker.h> 

using namespace std;

//Tipo de los nodos que forman parte de una frontera
struct nodeOfFrontier {
    
    // Coordenada x de un punto de la frontera
    double x; 
    
    // Coordenada y de un punto de la frontera
    double y; 
    
};

//La frontera es un vector de nodos de la forma (x,y).
typedef std::vector<nodeOfFrontier> TipoFrontera;

class FrontierExplorer {
    
public:
    
    // Indica si el mapa está vacío, es decir,  no tiene elementos != -1
    bool mapIsEmpty;

    // Guarda la posicion del robot capturada en la callback de odometría
    nodeOfFrontier nodoPosicionRobot; 
    
    // Posición del objetivo del robot (calculada en la función selectNode.
    nodeOfFrontier nodoObjetivo;
    
    // Lista de las posiciones de los puntos de la frontera detectada. 
    // Calculada en la función labelFrontierNodes
    TipoFrontera frontera;

    // Guarda el último mensaje de odometría recibido desde la callback de 
    // odometría
    nav_msgs::Odometry odometria; 
    
    // Guarda el último mensaje de mapa recibido desde gmapping
    nav_msgs::OccupancyGrid cmGlobal; 

    // Matriz que alamacena el mapa, todas las operaciones sobre el mapa van 
    // referidas a esta matriz
    std::vector<std::vector<int> > theGlobalCm; 

    // Posición del objetivo
    nodeOfFrontier posGoal; 
    
    // Ángulo (en radianes) de orientación del robot
    double yaw; 
    
    // Velocidad angular para enviar al robot
    double v_angular; 
    
    // Velocidad lineal para enviar al robot
    double v_lineal; 
    
    ////////////////////////////////////////////////////////////////////////////
    // Comienzo de mi código
    //
    
    // Tiempo en el que hará el movimiento
    static const unsigned int SEC_IN_MOV = 8;

    // Valor de la velocidad angular constante.
    const static double V_ANGULAR_CTE = 2 * M_PI / SEC_IN_MOV;

    ////////////////////////////////////////////////////////////////////////////
    // Constructores
    //
    FrontierExplorer(); 
    
    ////////////////////////////////////////////////////////////////////////////
    // Utils
    //
    
    ////////////////////////////////////////////////////////////////////////////
    // Método para que el robot gire 360º
    //
    void gira360(); 
    
    ////////////////////////////////////////////////////////////////////////////
    // Devuelve true si algún vecino de la celda (x,y), en coordenadas 
    // cartesianas, es desconocido
    //
    bool someNeighbourIsUnknown(int cell_x, int cell_y);
    
    ////////////////////////////////////////////////////////////////////////////
    // Devuelve true si algún vecino de la celda (x,y), en coordenadas 
    // cartesianas, es un obstáculo
    //
    bool someNeighbourIsObstacle(int cell_x, int cell_y);

    ////////////////////////////////////////////////////////////////////////////
    // Método que detecta nodos (celdas) frontera y las almacena en la lista 
    // frontera
    //
    void labelFrontierNodes(); 
    
    ////////////////////////////////////////////////////////////////////////////
    // Método que selecciona un nodo de la frontera para establecer el objetivo 
    // al que debe navegar el robot
    //
    void selectNode(nodeOfFrontier &selectedNode); 
    
    ////////////////////////////////////////////////////////////////////////////
    // Borra nodos de la frontera que se encuentren a una distancia (programada
    // en la función) del punto real del mapa (x,y)
    //
    void eraseFrontier(double x, double y); 
    
    ////////////////////////////////////////////////////////////////////////////
    // Imprime el mapa en un fichero.
    //
    void printMapToFile();

    ////////////////////////////////////////////////////////////////////////////
    // Método que modifica el mapa rellenando obstáculos alrededor de la celda 
    // (x,y), en coordenadas cartesianas
    //
    void rellenaObstaculos(int cell_x, int cell_y); 

    ////////////////////////////////////////////////////////////////////////////
    // Visualizar Frontera
    //

    ////////////////////////////////////////////////////////////////////////////
    // Publicador de  celdas de la frontera en rviz
    //
    ros::Publisher marker_pub; 
    
    ////////////////////////////////////////////////////////////////////////////
    // Publicador de objetivo en rviz
    //
    ros::Publisher marker_pubobjetivo; 

    ////////////////////////////////////////////////////////////////////////////
    // Contiene la lista de celdas a pintar en rviz. 
    //
    visualization_msgs::Marker markers_frontera; 
    
    ////////////////////////////////////////////////////////////////////////////
    // Contiene el objetivo a dibujar en rviz.
    //
    visualization_msgs::Marker marker_objetivo; 

    ////////////////////////////////////////////////////////////////////////////
    // Inicializa la lista de celdas frontera a pintar en rviz
    //
    void inicializaMarkers(std::string ns, int id, float r, float g, float b); 
    
    ////////////////////////////////////////////////////////////////////////////
    // Inicializa el objetivo a dibujar en rviz.
    //
    void inicializaMarkerSphere(std::string ns, int id, float r, float g, 
                                    float b); 

    ////////////////////////////////////////////////////////////////////////////
    // Visualiza el objetivo (como una esfera azul) en la posición real del mapa 
    // x,y
    //
    void visualizaObjetivo(double x, double y); 

    ////////////////////////////////////////////////////////////////////////////
    // Quita el objetivo de la visualización.
    //
    void limpiaMarkerObjetivo(); 

    ////////////////////////////////////////////////////////////////////////////
    // Visualiza la lista de celdas de la frontera (en verde)
    //
    void visualizaLista(TipoFrontera lista); 
    
    ////////////////////////////////////////////////////////////////////////////
    // Elimina de la visualización las celdas actualmente mostradas.
    //
    void limpiaMarkers(); 
    
    ////////////////////////////////////////////////////////////////////////////
    // Comienzo de mi código
    //
    
    ////////////////////////////////////////////////////////////////////////////
    // Método que procesa las acciones que tiene que realizar en cada momento
    //
    void processActions();
    
    //
    ////////////////////////////////////////////////////////////////////////////

protected:
    
private:
        
    // Manejador de nodo.
    ros::NodeHandle node; 
    
    // Publicador de velocidades
    ros::Publisher commandPub; 

    // Suscriptor del scan láser
    ros::Subscriber laserSub; 
    
    // Suscriptor de la odometría.b
    ros::Subscriber odomSub; 
    
    // Suscriptor del mapa global
    ros::Subscriber mapSub; 
    
    //void scanCallBack(const sensor_msgs::LaserScan::ConstPtr& scan);
    
    // Callback para obtener la pose estimada publicada en el topic /odom
    void odomCallBack(const nav_msgs::Odometry::ConstPtr& msg); 
    
    // Callback para obtener el mapa publicado por gmapping en el topic /map
    void getmapCallBack(const nav_msgs::OccupancyGrid::ConstPtr &msg); 
    
    ////////////////////////////////////////////////////////////////////////////
    // Comienzo de mi código
    //
    
    // Tiempo en el que hará el movimiento
    //static const unsigned int SEC_IN_MOV = 8;
    
    // Constante que indica la cantidad de espacio que debe rellenar de
    // obstáculos al ver uno. 1 metro.
    static const float DISTANCE_FILL_OBSTACLE = 1;
    
    // Suscriptor de la velocidad
    ros::Subscriber velSub; 
    
    // Contiene todo lo referente a la velocidad del robot
    geometry_msgs::Twist vel;
            
    // Acción que se está ejecutando en este momento. Si actúa normal es 0.
    unsigned int action;
    
    // Contador que se va a utilizar para llevar la cuenta mientras se esté 
    // ejecutando alguna acción
    // 1 -> Gira 360º 2 veces
    unsigned int iAction;
    
    // Cuando necesite saber si debe parar o no se le añade el tope
    unsigned int iActionStop;
    
    // Se usa para saber si está haciendo algún movimiento
    bool isAction;
    
    // Variable auxiliar para guardar datos intermedios
    double aux;
    
    // Se guarda el valor anterior de yaw para hacer comparaciones
    double oldYaw;
    
    // Método que se ejecutará con el evento de la velocidad
    void velCallback(const geometry_msgs::Twist::ConstPtr& msg);
    
    // Resetea a los valores por defecto los parámetros que se utilizan para
    // el movimiento del robot
    void resetParam();
    
    ////////////////////////////////////////////////////////////////////////////
    // Método que selecciona el nodo más cercano de la frontera para establecer 
    // el objetivo al que debe navegar el robot
    //
    void selectNearestNode(nodeOfFrontier &selectedNode); 
    
    ////////////////////////////////////////////////////////////////////////////
    // Devuelve la suma de las diferencias en valor absoluto de dos nodos
    //
    unsigned int getDifferenceAbsNodes(nodeOfFrontier node1, nodeOfFrontier node2);
            
    //
    ////////////////////////////////////////////////////////////////////////////

};

// FRONTIEREXPLORER_H
#endif 
