////////////////////////////////////////////////////////////////////////////////
//
// Francisco Javier Caracuel Beltrán
//
// 3º A - TSI(CCIA) - 2017
//
// explorer.cpp
//

#include "explorer.h"

Explorer::Explorer() {
    
    // Comienza siguiendo recto
    keepMoving = true;
    
    // No está girando
    isTurning = false;
    
    // No está girando saliendo de una esquina
    turnAround = false;
    
    // No hay un obstáculo en la derecha
    isObstacleInRight = false;
    
    // No hay un obstáculo en la izquierda
    isObstacleInLeft = false;
            
    // Se inicializa el vector de las últimas acciones del robot con -1
    // -1 -> inicialización
    //  0 -> recto
    //  1 -> izquierda
    //  2 -> derecha
    listLastActions = vector<int>(SIZE_LIST_LAST_ACTIONS, -1);
    
    // No existe ninguna acción
    lastAction = NA;

    // Advertise a new publisher for the robot's velocity command topic
    commandPub = node.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop", 10);

    // Subscribe to the simulated robot's laser scan topic
    laserSub = node.subscribe("scan", 1, &Explorer::scanCallback, this);
    
    // Se suscribe a los mensajes que indican la velocidad actual del robot.
    velSub = node.subscribe("/cmd_vel_mux/input/teleop", 1, &Explorer::velCallback, this);
    
    // Se suscribe a los mensajes que indica la odometría del robot
    poseSub = node.subscribe("/odom", 1, &Explorer::poseCallback, this);
    
    // Se guarda la distancia mínima a la que detectará los obstáculos
    node.getParam("explorer/min_distance", minDistance);
    
    // Se guarda la velocidad del robot
    node.getParam("explorer/forward_speed", forwardSpeed);
    
    // Se guarda la velocidad de giro del robot
    node.getParam("explorer/turn_speed", turnSpeed);
        
}

// Indica que el robot siga recto
void Explorer::moveForward() {
    
    // Se inserta la última acción que ha realizado
    setLastAction(FORWARD);
    
    // The default constructor will set all commands to 0
    geometry_msgs::Twist msg; 
    
    msg.linear.x = forwardSpeed;
    
    commandPub.publish(msg);
    
}

// Indica que el robot debe girar
void Explorer::turn() {
        
    geometry_msgs::Twist msg; 
        
    // Si no está girando saliendo de una esquina, es decir, el avance normal
    // del robot.
    // Si está moviéndose normal y se procesan las últimas acciones (al poner
    // true como parámetro solo se comprueban las SIZE_LIST_LAST_ACTIONS_LOOP
    // últimas acciones. Si detecta que ha hecho un giro a un lado y otro giro
    // al otro sin moverse hacia delante, es que está en una esquina
    if(!turnAround && processActions(true)){
        
        // Si está en la esquina se genera aleatoriamente hacia dónde se debe
        // mover        
        float r = (rand() % 2) - 0.5;
        
        if(r > 0){
                        
            // Se asigna la acción que va a realizar
            setLastAction(LEFT);
            
            // Se envia el movimiento
            msg.angular.z = turnSpeed;
            
            // Teóricamente tiene un obstáculo a la derecha y libre a la
            // izquierda
            isObstacleInRight = true;
            isObstacleInLeft = false;
            
        } else{
            
            // Se asigna la acción que va a realizar
            setLastAction(RIGHT);
            
            // Se envia el movimiento
            msg.angular.z = -turnSpeed;
            
            // Teóricamente tiene un obstáculo a la izquierda y libre a la
            // derecha
            isObstacleInRight = false;
            isObstacleInLeft = true;
            
        }
        
        // Comienza a girar
        isTurning = true;
        
        // El giro es especial y es salir de una esquina
        turnAround = true;
        
        // No sigue recto
        keepMoving = false;
        
        // Para que detecte bien cuando salir de la esquina, se deben quitar
        // todos los FORWARD del vector de últimas acciones
        prepareListLastActions();
        
    }
    
    // Si está saliendo de una esquina
    if(turnAround){
        
        // Si todas las últimas acciones son hacia el mismo lado ya puede
        // seguir su proceso normal
        if(!processActions(false)){
            
            turnAround = false;
            
        // Si aún quedan giros a ambos lados se comprueba cual es el último
        // movimiento que ha tenido y sigue girando en ese sentido
        } else{
            
            switch(lastAction){
                
                case 1:
                    setLastAction(LEFT);
                    msg.angular.z = turnSpeed;
                    break;
                    
                case 2:
                    setLastAction(RIGHT);
                    msg.angular.z = -turnSpeed;
                    break;
                
            }
            
        }
        
    // Si va a girar pero no está en una esquina
    } else if(isObstacleInRight && isObstacleInLeft){
                
        // Si está en una esquina pero aún no la ha detectado o se encuentra
        // justo mirando de frente a un objeto y se detectan objetos en ambos
        // lados, se genera aleatoriamente el giro
        float r = (rand() % 2) - 0.5;
        
        if(r > 0){
                        
            // Se asigna la acción que va a realizar
            setLastAction(LEFT);
            
            // Se envia el movimiento
            msg.angular.z = turnSpeed;
            
            // Teóricamente tiene un obstáculo a la derecha y libre a la
            // izquierda
            isObstacleInRight = true;
            isObstacleInLeft = false;
            
        } else{
            
            // Se asigna la acción que va a realizar
            setLastAction(RIGHT);
            
            // Se envia el movimiento
            msg.angular.z = -turnSpeed;
            
            // Teóricamente tiene un obstáculo a la izquierda y libre a la
            // derecha
            isObstacleInRight = false;
            isObstacleInLeft = true;
            
        }
            
    // Si solo hay obstáculos a la derecha, girará a la izquierda
    } else if(isObstacleInRight){
        
        setLastAction(LEFT);        
        msg.angular.z = turnSpeed;
        
    // Si solo hay obstáculos a la izquierda, girará a la derecha
    } else if(isObstacleInLeft){
        
        setLastAction(RIGHT);
        msg.angular.z = -turnSpeed;
        
    }
  
    // Se publica el movimiento que debe hacer el robot
    commandPub.publish(msg);
    
}

// Process the incoming laser scan message
void Explorer::scanCallback(const sensor_msgs::LaserScan::ConstPtr& scan) {
        
    // Se necesita saber si hay un obstáculo, porque si está girando, seguirá
    // girando como lo hacía
    bool isObstacle = false;

    // Find the closest range between the defined minimum and maximum angles
    int minIndex = ceil((MIN_SCAN_ANGLE - scan->angle_min) / scan->angle_increment);
    int maxIndex = floor((MAX_SCAN_ANGLE - scan->angle_min) / scan->angle_increment);

    // Se calcula la mitad de los sensores
    int halfIndex = maxIndex - (maxIndex-minIndex)/2;

    // Se comprueba el escáner desde ambos lados a la vez, hasta llegar al centro.
    // Cuando se encuentre un obstáculo primero en uno de los lados, girará hacia
    // el otro lado hasta que desaparezca el obstáculo.
    // Si coincide el obstáculo en ambos lados se girará aleatoriamente.
    for (int rightIndex = minIndex + 1, leftIndex = maxIndex; 
            rightIndex <= halfIndex && leftIndex>= halfIndex && !isObstacleInRight && !isObstacleInLeft; 
            rightIndex++, leftIndex--){

        // Escanea por la izquierda
        if (scan->ranges[rightIndex] < minDistance){

            isObstacle = true;
            
            // Si está girando, no modificará si hay obstáculo hasta que
            // no compruebe que no hay ninguno en todo el lado derecho
            if(!isTurning){
                isObstacleInRight = true;
            }

        }

        // Escanea por la derecha
        if (scan->ranges[leftIndex] < minDistance){

            isObstacle = true;
            
            // Si está girando, no modificará si hay obstáculo hasta que
            // no compruebe que no hay ninguno en todo el lado izquierdo
            if(!isTurning){
                isObstacleInLeft = true;
            }

        }

    }   

    // Si ha encontrado un obstáculo y está girando, sigue girando como lo
    // hacía.
    // Si no ha encontrado un obstáculo y está girando, termina de girar y 
    // sigue andando normalmente
    if(isTurning){

        if(!isObstacle){

            isTurning = false;
            isObstacleInRight = false;
            isObstacleInLeft = false;

        }

    // No está girando
    } else{

        // Si no está girando y se encuentra un obstáculo, para y comienza a
        // girar
        if(isObstacle){

            isTurning = true;
            keepMoving = false;


        }else{
            
            // Si no está girando y no hay obstáculo sigue recto
            keepMoving = true;

        }

    }
    
}

// Callback para obtener la velocidad del robot
void Explorer::velCallback(const geometry_msgs::Twist::ConstPtr& msg) {
            
}

// Callback para obtener la odometría
void Explorer::poseCallback(const nav_msgs::OdometryConstPtr& msg) {

   //double quatx= msg->pose.pose.orientation.x;
   //double quaty= msg->pose.pose.orientation.y;
   //double quatz= msg->pose.pose.orientation.z;
   //double quatw= msg->pose.pose.orientation.w;

    //tf::Quaternion q(quatx, quaty, quatz, quatw);
    //tf::Matrix3x3 m(q);
    
    //double roll, pitch, yaw;
    //m.getRPY(roll, pitch, yaw);

    //ROS_INFO("Roll: [%f],Pitch: [%f],Yaw: [%f]",roll,pitch,yaw);
        
}

// Ciclo principal del robot
void Explorer::startMoving() {
    
    // Frecuencia de 10Hz
    ros::Rate rate(10);

    // Keep spinning loop until user presses Ctrl+C or the robot got too close to an obstacle
    while (ros::ok()){

        // Si se ha calculado que no hay obstáculos, sigue recto
        if (keepMoving)
            moveForward();
        // Si debe girar, gira
        else if(isTurning)
            turn();
                
        // Need to call this function often to allow ROS to process incoming messages
        ros::spinOnce(); 
        rate.sleep();
        
    }
    
}

////////////////////////////////////////////////////////////////////////////////
// Utils
//

// Añade en el vector de acciones, la acción realizada
void Explorer::setLastAction(int action){
    
    // Se actualiza la última opción que va a realizar
    lastAction = action;
    
    // Se elimina la primera acción que hubiera
    listLastActions.erase(listLastActions.begin());
    
    // Se añade la acción actual al final
    listLastActions.push_back(action);
    
}

// Procesa las acciones que hay en el vector.
// Funciona de dos formas:
// 1. init = true -> solo busca en las SIZE_LIST_LAST_ACTIONS_LOOP últimas
//           acciones que ha hecho.
// 2. init = false -> busca en todas las acciones guardadas
bool Explorer::processActions(bool init){
    
    // Guarda si se encuentra en una esquina ciclando
    bool loop = true;
    
    // Guarda si ha realizado algún giro a izquierda o derecha
    bool turnLeft = false;
    bool turnRight = false;
    
    // Si busca en todas las acciones que hay guardadas, empieza en 0
    int initLastActions = 0;
    
    // Si solo debe buscar en las últimas SIZE_LIST_LAST_ACTIONS_LOOP acciones,
    // se calcula el valor por el que debe empezar
    if(init){
        initLastActions = SIZE_LIST_LAST_ACTIONS - SIZE_LIST_LAST_ACTIONS_LOOP;
    }
    
    // Se comprueban todas las acciones que ha realizado anteriormente
    for(unsigned int i=initLastActions; i<SIZE_LIST_LAST_ACTIONS && listLastActions[i]!=-1 && loop; i++){
                
        switch(listLastActions[i]){
            
            case FORWARD:
                loop = false;
                break;
                
            case LEFT:
                turnLeft = true;
                break;
                
            case RIGHT:
                turnRight = true;
                break;
            
        }
        
    }
    
    // Si no ha habido al menos un giro a la izquierda y un giro a la derecha,
    // no está en una esquina y no está ciclando
    if(!turnLeft || !turnRight){
        loop = false;
    }
        
    return loop;
    
}

void Explorer::prepareListLastActions(){
    
    // Se recorre todo el vector de acciones. Si encuentra alguna acción de
    // continuar recto, la cambia por la última acción realizada
    for(unsigned int i=0; i<SIZE_LIST_LAST_ACTIONS; i++){
        
        if(listLastActions[i] == FORWARD){
            listLastActions[i] = lastAction;
        }
        
    }
    
}
