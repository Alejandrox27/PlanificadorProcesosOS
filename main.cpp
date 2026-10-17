#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

// Simulador de Planificador de Procesos de un sistema operativo
/*imita el comportamiento del sistema operativo (como Linux o Windows)
 cuando debe decidir qué programa o tarea tiene acceso al procesador (CPU) en
 cada instante de tiempo. */

// Estructuras de datos usadas:
// Pilas (Stack)
// Colas con prioridad (Priority Queue)
// Colas ciculares (Queue)
// Arreglo dinamico

using namespace std;

// Estructura del proceso
struct Proceso {
  int id;
  string nombre;
  int prioridad; // 1-2: Alta prioridad, 3 o mas: Prioridad normal
  int tiempo_restante;
  int tiempo_inicial;
};

// Operador para comparar dos procesos y determinar cual tiene mayor prioridad
// la funcion operador() devuelve true si el primer parametro (a) debe tener
// menor prioridad que el segundo (b)
struct CompararPrioridad {
  bool operator()(const Proceso &a, const Proceso &b) const {
    return a.prioridad > b.prioridad; // Menor número = mayor prioridad
  }
};

// 2. Nodo de la Pila que almacena un Proceso
class NodoPila {
public:
  Proceso proceso;
  NodoPila *sig;

  NodoPila(Proceso p) {
    proceso = p;
    sig = nullptr;
  }
};

// 3. Clase Pila de Historial (LIFO: Last In First Out)
class PilaHistorial {
private:
  NodoPila *cabeza;
  int tamano;

public:
  PilaHistorial() {
    cabeza = nullptr;
    tamano = 0;
  }

  ~PilaHistorial() {
    while (!esVacia()) {
      pop();
    }
  }

  bool esVacia() { return cabeza == nullptr; }

  // PUSH: Inserta el proceso finalizado en la cima
  void push(Proceso p) {
    NodoPila *nuevo = new NodoPila(p);
    nuevo->sig = cabeza;
    cabeza = nuevo;
    tamano++;
  }

  // POP: Elimina y retorna el proceso en la cima
  Proceso pop() {
    if (esVacia()) {
      cout << "La pila esta vacia.\n";
      return {-1, "NULO", 0, 0};
    }
    NodoPila *aux = cabeza;
    Proceso p = aux->proceso;
    cabeza = cabeza->sig;
    delete aux;
    tamano--;
    return p;
  }

  // TOP / PEEK: Consulta el proceso en la cima sin eliminarlo
  Proceso top() {
    if (!esVacia()) {
      return cabeza->proceso;
    }
    return {-1, "NULO", 0, 0};
  }

  int obtenerTamano() { return tamano; }
};

// 4. Clase que controla todo el proceso de planificacion de tareas
class Planificador {
private:
  priority_queue<Proceso, vector<Proceso>, CompararPrioridad>
      colaPrioridad; // Estructura 1: Cola con Prioridad (Alta urgencia)
  queue<Proceso>
      colaCircular; // Estructura 2: Cola (Round-Robin para prioridad normal)
  PilaHistorial
      historial; // Estructura 3: Pila (Historial de procesos finalizados)
  int quantum;   // Tiempo de CPU asignado por turno (ej. 2 segundos)
  int contadorPID;

public:
  Planificador(int q) {
    quantum = q;
    contadorPID = 100;
  }

  // Agregar proceso a la estructura correspondiente
  void agregarProceso(string nombre, int prioridad, int tiempo) {
    contadorPID++;
    Proceso p = {contadorPID, nombre, prioridad, tiempo, tiempo};
    if (prioridad <= 2) {
      colaPrioridad.push(p);
      cout << "\n[+] Proceso '" << nombre << "' (PID: " << contadorPID
           << ") agregado a COLA DE PRIORIDAD.\n";
    } else {
      colaCircular.push(p);
      cout << "\n[+] Proceso '" << nombre << "' (PID: " << contadorPID
           << ") agregado a COLA CIRCULAR (Round-Robin).\n";
    }
  }

  void ejecutar_paso() {
    if (!colaPrioridad.empty()) {
      Proceso actual = colaPrioridad.top();
      colaPrioridad.pop();

      cout << "\n[CPU -> EJECUTANDO (ALTA PRIORIDAD)] PID: " << actual.id
           << " | Nombre: " << actual.nombre
           << " | Tiempo Restante: " << actual.tiempo_restante << "s\n";

      int tiempo_ejecutado = min(quantum, actual.tiempo_restante);
      actual.tiempo_restante -= tiempo_ejecutado;

      if (actual.tiempo_restante > 0) {
        colaPrioridad.push(actual);
        cout << "[*] Proceso '" << actual.nombre
             << "' vuelve a cola de prioridad.\n";

      } else {
        historial.push(actual);
        cout << "[+] Proceso '" << actual.nombre << "' FINALIZADO.\n";
      }

    } else if (!colaCircular.empty()) {
      Proceso actual = colaCircular.front();
      colaCircular.pop();

      cout << "\n[CPU -> EJECUTANDO (PRIORIDAD NORMAL)] PID: " << actual.id
           << " | Nombre: " << actual.nombre
           << " | Tiempo Restante: " << actual.tiempo_restante << "s\n";

      int tiempo_ejecutado = min(quantum, actual.tiempo_restante);
      actual.tiempo_restante -= tiempo_ejecutado;

      if (actual.tiempo_restante > 0) {
        cout << "   -> Quantum agotado. Volviendo al final de la Cola "
                "Circular...\n";
        colaCircular.push(actual);
      } else {
        cout << "   -> ¡Proceso FINALIZADO! Apilando en el Historial...\n";
        historial.push(actual);
      }
    } else {
      cout << "\n[FIN] No hay procesos esperando. CPU en reposo.\n";
    }
  }

  void ejecutar_todo() {
    while (!colaPrioridad.empty() || !colaCircular.empty()) {
      ejecutar_paso();
    }
  }

  void deshacer_ultimo() {
    if (historial.esVacia()) {
      cout << "\n[DESHACER] El historial esta vacio. No hay procesos para "
              "desapilar.\n";
      return;
    }
    Proceso p = historial.pop();
    cout << "\n[DESHACER] Se ha sacado de la pila el proceso PID: " << p.id
         << " (" << p.nombre << "). Reinsertando a la CPU...\n";

    p.tiempo_restante = p.tiempo_inicial;
    if (p.prioridad <= 2) {
      colaPrioridad.push(p);
    } else {
      colaCircular.push(p);
    }
  }

  void ver_estado_sistema() {
    cout << "\n================ ESTADO ACTUAL DEL SISTEMA ================\n";

    // Inspección de Cola de Prioridad
    cout << "[COLA DE PRIORIDAD (Alta Urgencia)] Elementos: "
         << colaPrioridad.size() << "\n";
    auto auxPrioridad = colaPrioridad;
    while (!auxPrioridad.empty()) {
      Proceso p = auxPrioridad.top();
      auxPrioridad.pop();
      cout << "  -> PID: " << p.id << " | " << p.nombre
           << " | Prio: " << p.prioridad
           << " | T. Restante: " << p.tiempo_restante << "s\n";
    }

    // Inspección de Cola Circular
    cout << "\n[COLA CIRCULAR (Round-Robin)] Elementos: " << colaCircular.size()
         << "\n";
    auto auxCircular = colaCircular;
    while (!auxCircular.empty()) {
      Proceso p = auxCircular.front();
      auxCircular.pop();
      cout << "  -> PID: " << p.id << " | " << p.nombre
           << " | Prio: " << p.prioridad
           << " | T. Restante: " << p.tiempo_restante << "s\n";
    }

    cout << "\n[PILA DE HISTORIAL] Procesos Finalizados: "
         << historial.obtenerTamano() << "\n";
    if (!historial.esVacia()) {
      Proceso topP = historial.top();
      cout << "  -> Cima actual: PID " << topP.id << " (" << topP.nombre
           << ")\n";
    }
    cout << "===========================================================\n";
  }

  void mostrar_historial() {
    cout << "======== Historial de Procesos Finalizados (Pila) ========"
         << endl;

    if (historial.esVacia()) {
      cout << "No hay procesos finalizados.";
    } else {
      // recorrer la pila sin destruirla
      PilaHistorial auxPila;
      while (!historial.esVacia()) {
        Proceso p = historial.pop();
        auxPila.push(p);
      }

      // Mostrar
      cout << "" << endl;
      while (!auxPila.esVacia()) {
        Proceso p = auxPila.top();
        auxPila.pop();

        cout << " - PID: " << p.id << " | Nombre: " << p.nombre
             << " | Prioridad: " << p.prioridad
             << " | Tiempo Final: " << p.tiempo_restante << endl;

        historial.push(p);
      }
    }
    cout << "==========================================================\n"
         << endl;
  }
};

int main() {
  Planificador planificador(2); // Quantum = 2 segundos
  int opcion;

  // Carga inicial de prueba
  planificador.agregarProceso("Sistema Base", 1, 3);
  planificador.agregarProceso("Navegador Web", 3, 5);
  planificador.agregarProceso("Antivirus", 2, 4);

  do {
    cout << "\n========== SIMULADOR PLANIFICADOR DE PROCESOS ==========\n";
    cout << "1. Agregar nuevo proceso\n";
    cout << "2. Ejecutar un ciclo de CPU (Paso a paso)\n";
    cout << "3. Ejecutar todos los procesos pendientes\n";
    cout << "4. Ver estado actual de las Colas\n";
    cout << "5. Ver Historial de Procesos Finalizados (Pila)\n";
    cout << "6. Deshacer ultimo proceso finalizado (Re-instanciar)\n";
    cout << "0. Salir\n";
    cout << "========================================================\n";
    cout << "Seleccione una opcion: ";
    cin >> opcion;

    switch (opcion) {
    case 1: {
      string nombre;
      int prioridad, tiempo;
      cout << "Nombre del proceso: ";
      cin.ignore();
      getline(cin, nombre);
      cout << "Prioridad (1-2: Alta / 3+: Normal): ";
      cin >> prioridad;
      cout << "Tiempo de CPU necesario (segundos): ";
      cin >> tiempo;
      planificador.agregarProceso(nombre, prioridad, tiempo);
      break;
    }
    case 2:
      planificador.ejecutar_paso();
      break;
    case 3:
      planificador.ejecutar_todo();
      break;
    case 4:
      planificador.ver_estado_sistema();
      break;
    case 5:
      planificador.mostrar_historial();
      break;
    case 6:
      planificador.deshacer_ultimo();
      break;
    case 0:
      cout << "\nCerrando la simulacion. ¡Hasta luego!\n";
      break;
    default:
      cout << "\nOpcion invalida. Intente de nuevo.\n";
    }
  } while (opcion != 0);

  return 0;
}