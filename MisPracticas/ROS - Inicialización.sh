#!/bin/bash

if ! [ $# -eq 2 ]
	then
	  	echo ""
	  	echo "Error: nombreScript.sh nombreDelWorkSpace nombreDelPaquete"
	  	echo ""
	  	exit
fi

echo ""
read -p "Situa el script donde quieras crear el workspace..."
echo ""

mkdir -p $1/src/
cd $1/src/
catkin_init_workspace

echo ""
echo "Creado el workspace"
echo ""

cd ..
catkin_make

echo ""
echo "Recompilando..."
echo ""

cd src/
catkin_create_pkg $2 std_msgs rospy roscpp

echo ""
echo "Nodo creado"
echo ""

echo "Añade los archivos .cpp que quieras utilizar en $1/src/$2/src"
echo ""

read -p "Pulsa enter para continuar"

echo ""
echo "Modifica el archivo \"$1/src/$2/cMakeLists.txt\" añadiendo por cada nodo:"
echo "add_executable(nombreDelNodo src/fichero.cpp)"
echo "target_link_libraries(nombreDelNodo \${catkin_LIBRARIES})"
echo ""

read -p "Pulsa enter para continuar"
echo ""

cd ..
catkin_make

echo ""
echo "Recompilando..."
echo ""

echo "Ejecutando \"devel/setup.sh\""
echo ""

source devel/setup.sh

echo "Añade al final en el fichero \"~/.bashrc\":"
echo "source /home/fran/Escritorio/Universidad/TSI/MisPracticas/$1/devel/setup.bash"
echo ""

read -p "Pulsa enter para continuar"
echo ""

mkdir src/$2/launch

echo "Añade en \"$1/src/$2/launch\" un launch por cada nodo"
echo ""

read -p "Pulsa enter para continuar"
echo ""

echo "Se puede lanzar el nodo con:"
echo "roslaunch nombreDelNodo nombreDelLaunch.launch"
echo ""
