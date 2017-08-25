////////////////////////////////////////////////////////////////////////////////
//
// Francisco Javier Caracuel Beltrán
//
// Técnicas de los Sistemas Inteligentes. CCIA. GII.
//
// Curso 2016/2017
//
// Fichero: frontier_explorer_lite.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include <limits>
#include <iostream>
#include <fstream>

#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <boost/thread.hpp>

// Gestión de costmaps
#include "tf/transform_listener.h"

#include "frontier_explorer_lite.h"

using namespace std;

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

////////////////////////////////////////////////////////////////////////////////
// Declaraciones de funciones
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Imprime en pantalla el contendio de un mapa
//
void printMapa(std::vector< std::vector<int> > mapa);

////////////////////////////////////////////////////////////////////////////////
// Calcula la distancia (sobrecargada) entre dos nodos de frontera
//
double distancia(nodeOfFrontier &n1, nodeOfFrontier &n2);

////////////////////////////////////////////////////////////////////////////////
// Calcula la distancia (sobrecargada) entre dos nodos reales
//
double distancia(double c1_x, double c1_y, double c2_x, double c2_y);

////////////////////////////////////////////////////////////////////////////////
// Fin de definición de funciones
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// main
//
int main(int argc, char** argv){

    ros::init(argc, argv, "frontier_explorer_node");

    // Inicializa el explorador
    FrontierExplorer explorador;
    
    if(argc == 2){
        explorador.setTimeout(atoi(argv[1]));
        ROS_INFO("Timeout set to %f", atof(argv[1]));
    }

    // Inicializa la visualización de la frontera
    explorador.inicializaMarkers("frontera", 0, 0.0f, 1.0f, 0.0f);

    // Inicializa la visualización del objetivo seleccionado.
    explorador.inicializaMarkerSphere("objetivo", 0, 0.0, 0.0, 1.0);

    // Se gestinan los callbacks hasta que el mapa tenga información
    ROS_INFO("Esperando a que mapa Reinicie y tenga información");

    while (explorador.mapIsEmpty)
        ros::spinOnce();

    //ROS_INFO("YA SÉ QUE EL MAPA NO ESTÁ VACÍO %d filas, %d columnas",
            //explorador.cmGlobal.info.height, explorador.cmGlobal.info.width);

    // Cambiar a 0.2
    ros::Rate frecuencia(1);

    while (ros::ok()){

        //printMapa(explorador.theGlobalCm);

        ROS_INFO("Escribiendo mapa en fichero de texto.");
        explorador.printMapToFile();

        ROS_INFO("Posicion actual del robot (%f %f %f)",
                explorador.nodoPosicionRobot.x,
                explorador.nodoPosicionRobot.y,
                explorador.yaw);

        explorador.processActions();

        ROS_INFO("Esperando a finalizar el bucle");

        frecuencia.sleep();

        ros::spinOnce();

    }

    // shutdown the node and join the thread back before exiting
    ros::shutdown();

    return 0;

}

////////////////////////////////////////////////////////////////////////////////
// Definición de funciones
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Imprime en pantalla el contendio de un mapa
//
void printMapa(std::vector< std::vector<int> > mapa){

    // Lo escribe en la orientación correcta, primero escribe las filas
    // superiores ....
    for (int x = 0; x < mapa[0].size(); x++){
        for (int y = mapa.size() - 1; y >= 0; y--){
            std::cout << (int) mapa[y][x] << " ";
        }
    }

}


////////////////////////////////////////////////////////////////////////////////
// Calcula la distancia (sobrecargada) entre dos nodos de frontera
//
double distancia(nodeOfFrontier &n1, nodeOfFrontier &n2){

    return sqrt((pow(n1.x - n2.x, 2)) + pow(n1.y - n2.y, 2));

}

////////////////////////////////////////////////////////////////////////////////
// Calcula la distancia (sobrecargada) entre dos nodos reales
//
double distancia(double c1_x, double c1_y, double c2_x, double c2_y){

    return sqrt((pow(c1_x - c2_x, 2)) + pow(c1_y - c2_y, 2));


}

////////////////////////////////////////////////////////////////////////////////
// Fin de definición de funciones
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Definición de los métodos de la clase FrontierExplorer
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Constructor
//
FrontierExplorer::FrontierExplorer(){

    // Inicializa el explorador de fronteras
    mapIsEmpty = true;

    // Posición del objetivo
    posGoal.x = posGoal.y = 0;

    // Posición actual
    nodoPosicionRobot.x = nodoPosicionRobot.y = 0;

    // Angulo (en radianes) de orientación del robot
    yaw = 0;

    // Velocidad angular
    v_angular = v_lineal = 0.0;

    // Advertise a new publisher for the simulated robot's velocity command
    // topic
    commandPub = node.advertise<geometry_msgs::Twist>("cmd_vel", 10);

    // Publicador del objetivo en rviz
    marker_pubobjetivo = node.advertise<visualization_msgs::Marker>("visualizacion_objetivo", 1);

    // Publicador de los nodos de frontera para visuoalizarlos en rviz
    marker_pub = node.advertise<visualization_msgs::Marker>("visualization_marker", 1000);

    // Se subscribe a la posición publicada por la odometría
    odomSub = node.subscribe("odom", 100, &FrontierExplorer::odomCallBack, this);

    // Se subscribe al mapa publicado por gmapping
    mapSub = node.subscribe("map", 100, &FrontierExplorer::getmapCallBack, this);

    ////////////////////////////////////////////////////////////////////////////
    // Comienzo de mi código
    //
    
    // Se suscribe a los mensajes que indican la velocidad actual del robot.
    velSub = node.subscribe("cmd_vel", 1, 
            &FrontierExplorer::velCallback, this);
    
    oldYaw = 0;
    
    // Se inicializan los valores por defecto
    resetParam();
    
    // El movimiento con el que comienza el robot es con el 1:
    // Gira 360º 2 veces.
    action = 1;
    
    // Guarda si ha habido un error al enviar al robot al objetivo
    errorToObjetive = false;

    //
    ////////////////////////////////////////////////////////////////////////////

}

////////////////////////////////////////////////////////////////////////////////
// Método para que el robot gire 360º
//
void FrontierExplorer::gira360(){
    
    // Se crea el objeto que le va a enviar la dirección a la que gira
    geometry_msgs::Twist msg;
    
    // ROS::TIME
    ros::Time begin = ros::Time::now();
    ros::Time stop = begin + ros::Duration(SEC_IN_MOV);
    
    while(ros::Time::now()<stop){
        
        // Se gira en el eje Z a una velocidad dada
        msg.angular.z = V_ANGULAR_CTE;
        
        // Se publica el movimiento que debe hacer el robot
        commandPub.publish(msg);
        
    }
        
}

////////////////////////////////////////////////////////////////////////////////
// cell_x representa la abcisa de una celda dada del mapa.
// cell_y representa la ordenada.
// Devuelve true si hay alguna casilla vecina a la celda dada que es desconocida
//
bool FrontierExplorer::someNeighbourIsUnknown(int cell_x, int cell_y){

    bool someNeighbour = false;

    for (int x = -1; x <= 1 && !someNeighbour; x++){

        for (int y = -1; y <= 1  && !someNeighbour; y++){

            if ((cell_x + x >= 0)
                    && (cell_x + x < cmGlobal.info.width)
                    && (cell_y + y >= 0) && (cell_y + y < cmGlobal.info.height)){

                if (theGlobalCm[cell_y + y][cell_x + x] == -1){
                    someNeighbour = true;
                }

            }

        }
    }

    return someNeighbour;

}

////////////////////////////////////////////////////////////////////////////////
// cell_x representa la abcisa de una celda dada del mapa.
// cell_y representa la ordenada.
// Devuelve true si hay alguna casilla vecina a la celda dada que es un obstáculo
//
bool FrontierExplorer::someNeighbourIsObstacle(int cell_x, int cell_y){

    bool someNeighbour = false;

    for (int x = -1; x <= 1 && !someNeighbour; x++){

        for (int y = -1; y <= 1  && !someNeighbour; y++){

            if ((cell_x + x >= 0)
                    && (cell_x + x < cmGlobal.info.width)
                    && (cell_y + y >= 0) && (cell_y + y < cmGlobal.info.height)){

                if (theGlobalCm[cell_y + y][cell_x + x] == 100){
                    someNeighbour = true;
                }

            }

        }
    }

    return someNeighbour;

    return true;

}

////////////////////////////////////////////////////////////////////////////////
// cell_x representa la abcisa de una celda dada del mapa.
// cell_y representa la ordenada.
// Devuelve true si hay alguna casilla vecina a la celda dada que está libre
//
bool FrontierExplorer::someNeighbourIsFree(int cell_x, int cell_y){

    bool someNeighbour = false;

    for (int x = -1; x <= 1 && !someNeighbour; x++){

        for (int y = -1; y <= 1  && !someNeighbour; y++){

            if ((cell_x + x >= 0)
                    && (cell_x + x < cmGlobal.info.width)
                    && (cell_y + y >= 0) && (cell_y + y < cmGlobal.info.height)){

                if (theGlobalCm[cell_y + y][cell_x + x] == 0){
                    someNeighbour = true;
                }

            }

        }
    }

    return someNeighbour;

}

////////////////////////////////////////////////////////////////////////////////
// La frontera es un vector de pares de coordenadas (de tipo TipoFrontera)
// Insertar en frontera los puntos, en coordenadas del mundo, que tienen algún
// vecino desconocido.
//
void FrontierExplorer::labelFrontierNodes(){
    
    frontera.clear();
    
    unsigned int sizeX = theGlobalCm[0].size();
    
    // Se recorre el mapa
    for(int x=0; x<sizeX; x++){

        for(int y=theGlobalCm.size() - 1; y >= 0; y--){
            
            // Se comprueba si existe algún valor -1 alrededor
            if(theGlobalCm[y][x] != -1 && someNeighbourIsUnknown(x, y) && someNeighbourIsFree(x, y)){
                
                // Si existe algún valor -1, se insertan las coordenadas reales
                // en frontera
                nodeOfFrontier node;
    
                // Se calculan las coordenadas del mundo real               
                node.x = x*cmGlobal.info.resolution + cmGlobal.info.origin.position.x;
                node.y = y*cmGlobal.info.resolution + cmGlobal.info.origin.position.y;
                
                //cout << "Coordenadas --> x: " << node.x << " y: " << node.y << endl;

                // Se añade el nodo a la frontera
                frontera.push_back(node);
                
            }
            
        }

    }
    

}

////////////////////////////////////////////////////////////////////////////////
//
void FrontierExplorer::selectNode(nodeOfFrontier &selectedNode){

    if(errorToObjetive){
        
        // Selecciona el nodo más cercano al robot
        selectNearestNode(selectedNode);
        
    } else{
        
        // Selecciona el nodo más lejano al robot
        selectFarthestNode(selectedNode);
        
    }
    
}

////////////////////////////////////////////////////////////////////////////////
// Borra nodos de la frontera a una distancia de dos metros de la posición
// (px,py)
//
void FrontierExplorer::eraseFrontier(double px, double py){

    TipoFrontera::iterator it = frontera.begin();

    while (it != frontera.end()) {

        if (distancia(px, py, it->x, it->y) <= 2)
            frontera.erase(it);
        else
            it++;

    }

}

////////////////////////////////////////////////////////////////////////////////
//
void FrontierExplorer::printMapToFile(){

    // Escribe el mapa en el fichero "grid.txt"
    std::ofstream gridFile;
    gridFile.open("grid.txt");

    for (int x = 0; x < theGlobalCm[0].size(); x++) {

        for (int y = theGlobalCm.size() - 1; y >= 0; y--) {
            gridFile << (int) theGlobalCm[y][x] << " ";
        }

        gridFile << std::endl;

    }

    gridFile.close();

}

////////////////////////////////////////////////////////////////////////////////
//
void FrontierExplorer::rellenaObstaculos(int cell_x, int cell_y){

    // Distancia que se quiere rellenar entre lo que mide cada celda.
    float cellToFill = DISTANCE_FILL_OBSTACLE / 0.05;
    
    // Columna inicial y final
    float initCellX = cell_x - cellToFill/2;
    float lastCellX = cell_x + cellToFill/2;
    
    // Se comprueba si la columna inicial es menor que 0
    initCellX = initCellX<0 ? 0 : initCellX;
    
    // Se comprueba si la columna final es mayor que tamaño tiene
    lastCellX = lastCellX>cmGlobal.info.width ? cmGlobal.info.width : lastCellX;
    
    // Fila inicial y final
    float initCellY = cell_y - cellToFill/2;
    float lastCellY = cell_y + cellToFill/2;
    
    // Se comprueba si la fila inicial es menor que 0
    initCellY = initCellY<0 ? 0 : initCellY;
    
    // Se comprueba si la fila final es mayor que tamaño tiene
    lastCellY = lastCellY>cmGlobal.info.height ? cmGlobal.info.height : lastCellY;
    
    //cout << "La distancia es: " << cellToFill << endl;
    //cout << "X inicial: " << initCellX << " X final: " << lastCellX << ", Y inicial: " << initCellY <<  " Y final: " << lastCellY << endl;
    
    // Se recorren las casillas que tiene alrededor y se rellenan de obstáculos
    for(unsigned int i=initCellX; i<lastCellX; i++){
        
        for(unsigned int j=initCellY; j<lastCellY; j++){
            
            theGlobalCm[j][i] = 100;
            
        }
        
    }

}

////////////////////////////////////////////////////////////////////////////////
// Visualizar Frontera
//

////////////////////////////////////////////////////////////////////////////////
//
void FrontierExplorer::inicializaMarkers(std::string ns, int id, float r,
                                            float g, float b){

    // cmGlobal.header.frame_id.c_str();//getGlobalFrameID().c_str();
    markers_frontera.header.frame_id = "map";
    markers_frontera.header.stamp = ros::Time::now();
    markers_frontera.ns = ns;

    // La otra opción es DELETE
    markers_frontera.action = visualization_msgs::Marker::ADD;
    markers_frontera.pose.orientation.w = 0.0;

    markers_frontera.id = id;

    markers_frontera.type = visualization_msgs::Marker::POINTS;

    // POINTS markers use x and y scale for width/height respectively

    // cmGlobal.info.resolution;
    markers_frontera.scale.x = 0.05;

    // cmGlobal.info.resolution;
    markers_frontera.scale.y = 0.05;

    // Points are green
    markers_frontera.color.g = g;
    markers_frontera.color.r = r;
    markers_frontera.color.b = b;
    markers_frontera.color.a = 1.0;

}

////////////////////////////////////////////////////////////////////////////////
//
void FrontierExplorer::inicializaMarkerSphere(std::string ns, int id, float r,
                                                float g, float b){

    marker_objetivo.header.frame_id = "map";
    marker_objetivo.header.stamp = ros::Time::now();
    marker_objetivo.ns = ns;

    // La otra opcion es DELETE
    marker_objetivo.action = visualization_msgs::Marker::ADD;
    marker_objetivo.pose.orientation.w = 0.0;

    marker_objetivo.id = id;

    marker_objetivo.type = visualization_msgs::Marker::SPHERE;

    // POINTS markers use x and y scale for width/height respectively

    // cmGlobal.info.resolution;
    marker_objetivo.scale.x = 0.2;

    // cmGlobal.info.resolution;
    marker_objetivo.scale.y = 0.2;

    // Points are green
    marker_objetivo.color.g = g;
    marker_objetivo.color.r = r;
    marker_objetivo.color.b = b;
    marker_objetivo.color.a = 1.0;

}

////////////////////////////////////////////////////////////////////////////////
//
void FrontierExplorer::visualizaObjetivo(double x, double y){

    //Se pinta: cpstart.x, cpstart.y, scale == costmap_->getResolution
    limpiaMarkerObjetivo();

    marker_objetivo.pose.position.x = x;
    marker_objetivo.pose.position.y = y;

    // Se publica
    marker_pubobjetivo.publish(marker_objetivo);

    // Se quita el punto de la lista de puntos, se borra con DELETE cuando se
    // saque de abiertos
    //points.points.pop_back();

}

////////////////////////////////////////////////////////////////////////////////
//
void FrontierExplorer::limpiaMarkerObjetivo(){

    marker_objetivo.action = visualization_msgs::Marker::DELETE;
    marker_pubobjetivo.publish(marker_objetivo);
    marker_objetivo.action = visualization_msgs::Marker::ADD;

}

////////////////////////////////////////////////////////////////////////////////
//
void FrontierExplorer::visualizaLista(TipoFrontera lista){

    limpiaMarkers();

    markers_frontera.points.erase(markers_frontera.points.begin(),
            markers_frontera.points.end());

    for (TipoFrontera::iterator it = lista.begin(); it != lista.end(); ++it){

        double wpose_x = it->x;
        double wpose_y = it->y;

        // Se pinta: cpstart.x, cpstart.y, scale == costmap_->getResolution
        geometry_msgs::Point p;
        p.x = wpose_x;
        p.y = wpose_y;

        //¿?
        p.z = 0;

        markers_frontera.points.push_back(p);

    }

    marker_pub.publish(markers_frontera);

    // Quitar neighborCells de points .popback

}

////////////////////////////////////////////////////////////////////////////////
//
void FrontierExplorer::limpiaMarkers(){

    if (!markers_frontera.points.empty()){

        markers_frontera.action = visualization_msgs::Marker::DELETE;
        marker_pub.publish(markers_frontera);
        markers_frontera.action = visualization_msgs::Marker::ADD;

    }

    markers_frontera.points.clear();

}

////////////////////////////////////////////////////////////////////////////////
// Private
//

////////////////////////////////////////////////////////////////////////////////
//
void FrontierExplorer::odomCallBack(const nav_msgs::Odometry::ConstPtr& msg){

    // Se obtiene la posición y se guarda en el dato miembro del objeto
    // FrontierExplorer
    nodoPosicionRobot.x = msg->pose.pose.position.x;
    nodoPosicionRobot.y = msg->pose.pose.position.y;

    // También se guarda en FrontierExplorer::odometria
    odometria.pose.pose.position.x = nodoPosicionRobot.x;
    odometria.pose.pose.position.y = nodoPosicionRobot.y;

    // Get Quaternion anglular information
    double x = msg->pose.pose.orientation.x;
    double y = msg->pose.pose.orientation.y;
    double z = msg->pose.pose.orientation.z;
    double w = msg->pose.pose.orientation.w;

    // Se guarda en la odometría
    odometria.pose.pose.orientation.x = x;
    odometria.pose.pose.orientation.y = y;
    odometria.pose.pose.orientation.z = z;
    odometria.pose.pose.orientation.w = w;
        
    //Se convierte en pitch,roll, yaw
    // Consultado en http://answers.ros.org/question/50113/transform-quaternion/
    // Se guarda el yaw en FrontierExplorer::yaw
    yaw = atan2(2 * (y * x + w * z), w * w + x * x - y * y - z * z);

}


////////////////////////////////////////////////////////////////////////////////
//
void FrontierExplorer::getmapCallBack(const nav_msgs::OccupancyGrid::ConstPtr& msg){

    // Callback para obtener el mapa. Se asume que el mapa está vacío antes de
    // recibirlo
    mapIsEmpty = true;

    // Se guarda en la variable cmGlobal el mensaje recibido para disponer del
    // mensaje en nuestro programa
    cmGlobal = *msg;

    // Redimensiona el mapa segun las dimensiones del mapa recibido
    theGlobalCm.resize(cmGlobal.info.height);

    for (int y = 0; y < cmGlobal.info.height; y++)
        theGlobalCm[y].resize(cmGlobal.info.width);

    // Copia el mapa del mensaje en el mapa del explorador de fronteras e
    // informa en mapEmpty si hay celdas con información.
    int currCell = 0;

    for (int y = 0; y < cmGlobal.info.height; y++){

        for (int x = 0; x < cmGlobal.info.width; x++){
            
            if(theGlobalCm[y][x] != 100){

                theGlobalCm[y][x] = cmGlobal.data[currCell];

                // Si la celda actual tiene un obstáculo, se llama al método que
                // se encarga de gestionarlo
                if(cmGlobal.data[currCell] == 100){
                    rellenaObstaculos(x, y);
                }
                
                if (theGlobalCm[y][x] != -1)
                    mapIsEmpty = false;
                
            }

            currCell++;

        }

    }

    //cmGlobal.header.seq = msg->header.seq;
    //cmGlobal.header.stamp = msg->header.stamp;
    //cmGlobal.header.frame_id = msg->header.frame_id;
    //
    //cmGlobal.info.map_load_time = msg->info.map_load_time ;
    //cmGlobal.info.resolution = msg->info.resolution;
    //cmGlobal.info.width = msg->info.width;
    //cmGlobal.info.height = msg->info.height;
    //
    //cmGlobal.info.origin.position.x = msg->info.origin.position.x;
    //cmGlobal.info.origin.position.y = msg->info.origin.position.y;
    //cmGlobal.info.origin.position.z = msg->info.origin.position.z;
    //
    //cmGlobal.info.origin.orientation.x = msg->info.origin.orientation.x;
    //cmGlobal.info.origin.orientation.y = msg->info.origin.orientation.y;
    //cmGlobal.info.origin.orientation.z = msg->info.origin.orientation.z;
    //cmGlobal.info.origin.orientation.w = msg->info.origin.orientation.w;

}

////////////////////////////////////////////////////////////////////////////////
// Comienzo de mi código
//

////////////////////////////////////////////////////////////////////////////
// Método que procesa las acciones que tiene que realizar en cada momento
//
void FrontierExplorer::processActions(){
    
    // Si el mapa está completamente descubierto, se termina
    if(fullMap()){
        action = 0;
    }
    
    // 0 -> Termina la ejecución
    // 1 -> Gira 360º 2 veces
    // 2 -> Pinta las fronteras
    // 3 -> Selecciona y envía al nodo objetivo
    // 4 -> Gira 360º 1 vez
    switch(action){
                
        case 0:
            
            ROS_INFO("The map is fully inspected");
            
            // shutdown the node and join the thread back before exiting
            ros::shutdown();

            break;

        case 1:
                        
            // Si va a empezar el movimiento, se le indica cuándo debe parar
            if(iAction == 0){
                iActionStop = 2;
            }

            gira360();
            
            iAction++;
            
            if(iAction == iActionStop){
                action = 3;
                iAction = 0;
            }
            
            break;
            
        case 2:
                        
            // Si va a empezar el movimiento, se le indica cuándo debe parar
            if(iAction == 0){
                iActionStop = 1;
            }

            gira360();
            
            iAction++;
            
            if(iAction == iActionStop){
                action = 3;
                iAction = 0;
            }
            
            break;

        case 3:
                        
            labelFrontierNodes();
            
            visualizaLista(frontera);
                                    
            action = 4;

            break;

        case 4:
                        
            // Selecciona el nodo objetivo
            selectNode(nodoObjetivo);
    
            // Pinta el nodo objetivo
            visualizaObjetivo(nodoObjetivo.x, nodoObjetivo.y);
                                    
            // create the action client	
            MoveBaseClient ac("move_base", true);

            // Wait 60 seconds for the action server to become available
            ROS_INFO("Waiting for the move_base action server");
            ac.waitForServer(ros::Duration(timeout));

            ROS_INFO("Connected to move base server");

            // Send a goal to move_base
            move_base_msgs::MoveBaseGoal goal;
            goal.target_pose.header.frame_id = "map";
            goal.target_pose.header.stamp = ros::Time::now();	

            // Se le envía a la posición del nodo objetivo
            goal.target_pose.pose.position.x = nodoObjetivo.x;
            goal.target_pose.pose.position.y = nodoObjetivo.y;

            // Convert the Euler angle to quaternion
            double radians = yaw * (M_PI/180);
            tf::Quaternion quaternion;
            quaternion = tf::createQuaternionFromYaw(radians);

            geometry_msgs::Quaternion qMsg;
            tf::quaternionTFToMsg(quaternion, qMsg);

            goal.target_pose.pose.orientation = qMsg;

            // Send the goal command
            ROS_INFO("Sending robot to: x = %f, y = %f, theta = %f", nodoObjetivo.x, nodoObjetivo.y, yaw);
            ac.sendGoal(goal);

            // Wait for the action to return
            ac.waitForResult();

            // Se consigue llegar al objetivo actualizará la frontera y volverá
            // a establecer un objetivo.
            // Si no consigue llegar al objetivo hace un giro de 360º y cambia
            // al objetivo más cercano
            if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED){
                
                errorToObjetive = false;
                action = 3;
                
            }else{
                
                errorToObjetive = true;
                action = 2;
                
                ROS_INFO("\n\nNo ha podido llegar a su destino. Recalculando...\n\n");
                
            }
            
            break;
            
    }
            
    // Se guarda el valor anterior del yaw
    oldYaw = yaw;
    
}

////////////////////////////////////////////////////////////////////////////
// Asigna el tiempo máximo de espera
//
void FrontierExplorer::setTimeout(float t){
    timeout = t;
}

////////////////////////////////////////////////////////////////////////////
// Callback para obtener la velocidad del robot
//
void FrontierExplorer::velCallback(const geometry_msgs::Twist::ConstPtr& msg){
    
    // Se guarda la información que se obtiene de la velocidad    
    vel.linear.x = msg->linear.x;
    vel.linear.y = msg->linear.y;
    vel.linear.z = msg->linear.z;
    
    vel.angular.x = msg->angular.x;
    vel.angular.y = msg->angular.y;
    vel.angular.z = msg->angular.z;
            
}

////////////////////////////////////////////////////////////////////////////
// Resetea a los valores por defecto los parámetros que se utilizan para
// el movimiento del robot
void FrontierExplorer::resetParam(){
    
    // Movimiento que va a realizar
    //action = 0;
    
    // Iteración del movimiento que va a realizar
    iAction = 0;
    
    // Iteración en la que debe parar el robot de hacer su movimiento
    iActionStop = 0;
    
    // Se indica que no está realizando ningún movimiento
    isAction = false;
    
    // Variable que se usa para almacenar datos intermedios
    aux = 0;
        
}

////////////////////////////////////////////////////////////////////////////////
// Método que selecciona el nodo más cercano de la frontera para establecer 
// el objetivo al que debe navegar el robot
//
void FrontierExplorer::selectNearestNode(nodeOfFrontier &selectedNode){

    // Se parte de que el nodo seleccionado es el primero
    unsigned int posNode = 0;
    
    // Diferencia entre el nodo actual y el primero
    float difference = getDifferenceAbsNodes(nodoPosicionRobot, frontera[posNode]);
    
    // Se recorre todo el vector de nodos desde el segundo y se va guardando
    // la mejor posición
    for(unsigned int i=1; i<frontera.size(); i++){
        
        // Se consigue la diferencia del nodo actual
        float differenceAux = getDifferenceAbsNodes(nodoPosicionRobot, frontera[i]);
           
        // Si la diferencia es menor de un punto que no está rodeado de vacío
        // se actualiza
        if(differenceAux < difference + MIN_DISTANCE_TO_OBJETIVE){
            
            difference = differenceAux;
            posNode = i;
            
        }
        
    }
    
    selectedNode = frontera[posNode];
    
    //cout << "Nodo actual -> X: " << nodoPosicionRobot.x << " Y: " << nodoPosicionRobot.y << endl;
    //cout << "Nodo al que va -> X: " << selectedNode.x << " Y: " << selectedNode.y << endl;

}

////////////////////////////////////////////////////////////////////////////////
// Método que selecciona el nodo más lejano de la frontera para establecer 
// el objetivo al que debe navegar el robot
//
void FrontierExplorer::selectFarthestNode(nodeOfFrontier &selectedNode){

    // Se parte de que el nodo seleccionado es el primero
    unsigned int posNode = 0;
    
    // Diferencia entre el nodo actual y el primero
    float difference = getDifferenceAbsNodes(nodoPosicionRobot, frontera[posNode]);
    
    // Se recorre todo el vector de nodos desde el segundo y se va guardando
    // la mejor posición
    for(unsigned int i=1; i<frontera.size(); i++){
        
        // Se consigue la diferencia del nodo actual
        float differenceAux = getDifferenceAbsNodes(nodoPosicionRobot, frontera[i]);
           
        // Si la diferencia es mayor de un punto que no está rodeado de vacío
        // se actualiza
        if(differenceAux > difference + MIN_DISTANCE_TO_OBJETIVE){
            
            difference = differenceAux;
            posNode = i;
            
        }
        
    }
    
    selectedNode = frontera[posNode];
    
    //cout << "Nodo actual -> X: " << nodoPosicionRobot.x << " Y: " << nodoPosicionRobot.y << endl;
    //cout << "Nodo al que va -> X: " << selectedNode.x << " Y: " << selectedNode.y << endl;

}

////////////////////////////////////////////////////////////////////////////
// Devuelve la suma de las diferencias en valor absoluto de dos nodos
//
unsigned int FrontierExplorer::getDifferenceAbsNodes(nodeOfFrontier node1, 
                                                        nodeOfFrontier node2){
    
    return abs(node1.x - node2.x) + abs(node1.y - node2.y);
    
}

////////////////////////////////////////////////////////////////////////////
// Comprueba si el mapa ha sido descubierto al completo basándose en las
// fronteras disponibles
//
bool FrontierExplorer::fullMap(){
    
    labelFrontierNodes();
    
    return frontera.size() == 0;
    
}

////////////////////////////////////////////////////////////////////////////////
// Fin de definición de los métodos de la clase FrontierExplorer
////////////////////////////////////////////////////////////////////////////////
