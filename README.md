# Planificador de Procesos de un Sistema Operativo

## Introduccion
Programa simple en c++ que simula el funcionamiento de un sistema operativo cuando
tiene programadas varias tareas que necesita ejecutar.

## Estructuras de Datos usadas
- Pilas
- Colas circulares
- Colas con prioridad
- Listas enlazadas

## Como usar el programa
El programa ofrece un menu con varias opciones:

1. Agregar nuevo proceso
2. Ejecutar un ciclo de CPU (Paso a paso)
3. Ejecutar todos los procesos pendientes
4. Ver estado actual de las Colas
5. Ver Historial de Procesos Finalizados (Pila)
6. Deshacer ultimo proceso finalizado (Re-instanciar)
0. Salir

El usuario puede elegir entre cualquiera de esas opciones para hacer un proceso distinto y ver como funciona el planificador de procesos en un sistema operativo.
Por lo general, el usuario deberia empezar por la opcion 1, luego ejecutar un ciclo de cpu (opcion 2) y repetir 
varias veces hasta ejecutar todos los procesos.

El quantum es el tiempo que se le da a cada proceso para ejecutarse. Si el proceso no termina en ese tiempo, se vuelve a meter en la cola. Este es de dos segundos por defecto en el programa. Para cambiarlo, se debe modificar la variable quantum en la funcion ejecutarCicloCPU.

## Requisitos del Sistema
- **Compilador de C++**: Compatible con el estándar **C++11** o superior
- **Entorno de ejecución**: Cualquier sistema operativo con terminal o línea de comandos
- **Dependencias externas**: Ninguna. El proyecto fue desarrollado implementando las estructuras de datos manualmente y solo requiere las bibliotecas estándar iostream y string.

## Compilacion y Ejecucion

1. Abrir una terminal en la carpeta del proyecto.
2. Compilar el archivo fuente: g++ -std=c++11 main.cpp -o simulador
3. Ejecutar: ./simulador

## Herramientas de IA usadas
En el desarrollo de este proyecto se emplearon herramientas de Inteligencia Artificial (Gemini) en aproximadamente un 55% del código, principalmente para la generación de plantillas de interacción por consola y como guía para la estructuración de la lógica del planificador. El 45% restante se desarrollo manualmente, incluyendo la implementación de las estructuras de datos, la lógica del planificador.

Se buscó que el programa usara sus propias estructuras de datos (Pila, colas con prioridad y cola circular) en lugar de las librerias estandar (stack, priority_queue y queue), para poder aplicar los conocimientos de estructuras de datos y algoritmos, y verificar que, el código de cada una de estas estructuras de datos es similar y facil de implementar.