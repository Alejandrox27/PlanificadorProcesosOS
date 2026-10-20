#include <iostream> // libreria usada para entrada/salida
#include <string>   // libreria usada para cadenas de texto

// Simulador de Planificador de Procesos de un sistema operativo
/*imita el comportamiento del sistema operativo (como Linux o Windows)
 cuando debe decidir qué programa o tarea tiene acceso al procesador (CPU) en
 cada instante de tiempo. */

// Estructuras de datos usadas:
// Pilas (Stack)
// Colas con prioridad (Priority Queue)
// Colas ciculares (Queue)

using namespace std; // simplifica el codigo al no tener que escribir std::

// Estructura del proceso
struct Proceso {
  int id;              // identificador unico del proceso
  string nombre;       // nombre del proceso
  int prioridad;       // 1-2: Alta prioridad, 3 o mas: Prioridad normal
  int tiempo_restante; // tiempo restante para que el proceso termine
  int tiempo_inicial;  // tiempo inicial del proceso
};

// Operador para comparar dos procesos y determinar cual tiene mayor prioridad
// la funcion operador() devuelve true si el primer parametro (a) debe tener
// menor prioridad que el segundo (b)
struct CompararPrioridad {
  bool operator()(const Proceso &a, const Proceso &b) const {
    return a.prioridad > b.prioridad; // Menor número = mayor prioridad
  }
};

// Nodo de la Pila que almacena un Proceso
class Nodo {
public:
  Proceso proceso;
  Nodo *sig;

  Nodo(Proceso p) {
    proceso = p;
    sig = nullptr;
  }
};

// Pila de Historial (LIFO: Last In First Out)
// Historial de procesos finalizados, se usa para estadisticas o para deshacer
// operaciones.
class PilaHistorial {
private: // variables privadas
  Nodo *cabeza;
  int tamano;

public: // funciones publicas
  PilaHistorial() {
    cabeza = nullptr;
    tamano = 0;
  }

  // Destructor para limpiar memoria al finalizar el programa
  ~PilaHistorial() {
    while (!esVacia()) {
      pop();
    }
  }

  // Verifica si la pila esta vacia
  bool esVacia() { return cabeza == nullptr; }

  // PUSH: Inserta el proceso finalizado en la cima
  void push(Proceso p) {
    Nodo *nuevo = new Nodo(p);
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
    Nodo *aux = cabeza;
    Proceso p = aux->proceso;
    cabeza = cabeza->sig;
    delete aux;
    tamano--;
    return p;
  }

  // TOP: Consulta el proceso en la cima sin eliminarlo
  Proceso top() {
    if (!esVacia()) {
      return cabeza->proceso;
    }
    return {-1, "NULO", 0, 0};
  }

  // Obtiene el tamaño de la pila
  int obtenerTamano() { return tamano; }

  // Muestra los procesos en la pila
  void mostrar() {
    // Condicion para verificar si la pila esta vacia
    if (esVacia()) {
      cout << "   (Pila vacia)\n";
      return;
    }
    // Se recorre la pila de arriba hacia abajo
    Nodo *aux = cabeza;
    while (aux != nullptr) {
      cout << "  -> PID: " << aux->proceso.id << " | " << aux->proceso.nombre
           << " | Prio: " << aux->proceso.prioridad
           << " | T. Restante: " << aux->proceso.tiempo_restante << "s\n";
      aux = aux->sig;
    }
  }
};

// COLA CIRCULAR / SIMPLE (FIFO: First In First Out)
// Se utiliza para agrupar procesos con la misma prioridad, o procesos que no
// tienen prioridad.
class ColaSimple {
private:
  Nodo *cabeza;
  Nodo *ultimo;
  int tamano;

public:
  // Constructor
  ColaSimple() : cabeza(nullptr), ultimo(nullptr), tamano(0) {}

  // Destructor para limpiar memoria al finalizar el programa
  ~ColaSimple() {
    while (!esVacia()) {
      pop();
    }
  }

  // Verifica si la cola esta vacia
  bool esVacia() { return cabeza == nullptr; }

  // PUSH: Inserta siempre al FINAL
  void push(Proceso p) {
    Nodo *nuevo = new Nodo(p);
    if (esVacia()) {
      cabeza = nuevo;
      ultimo = nuevo;
    } else {
      ultimo->sig = nuevo;
      ultimo = nuevo;
    }
    tamano++;
  }

  // Extrae siempre del INICIO
  Proceso pop() {
    if (esVacia())
      return {-1, "NULO", 0, 0, 0};
    Nodo *aux = cabeza;
    Proceso p = aux->proceso;
    cabeza = cabeza->sig;
    if (cabeza == nullptr)
      ultimo = nullptr;
    delete aux;
    tamano--;
    return p;
  }

  // TOP: Consulta el proceso en la cima sin eliminarlo
  Proceso top() {
    if (!esVacia()) {
      return cabeza->proceso;
    }
    return {-1, "NULO", 0, 0, 0};
  }

  // Obtiene el tamaño de la cola
  int obtenerTamano() { return tamano; }

  // Muestra los procesos en la cola
  void mostrar() {
    if (esVacia()) {
      cout << "   (Cola vacia)\n";
      return;
    }
    Nodo *aux = cabeza;
    while (aux != nullptr) {
      cout << "  -> PID: " << aux->proceso.id << " | " << aux->proceso.nombre
           << " | Prio: " << aux->proceso.prioridad
           << " | T. Restante: " << aux->proceso.tiempo_restante << "s\n";
      aux = aux->sig;
    }
  }
};

// COLA CON PRIORIDAD (A partir de Lista Enlazada)
class ColaPrioridad {
private: // variables privadas
  Nodo *cabeza;
  int tamano;

public: // funciones publicas
  // Constructor
  ColaPrioridad() : cabeza(nullptr), tamano(0) {}

  // Destructor para limpiar memoria al finalizar el programa
  ~ColaPrioridad() {
    while (!esVacia()) {
      pop();
    }
  }

  // Verifica si la cola esta vacia
  bool esVacia() { return cabeza == nullptr; }

  // PUSH: Inserta en ORDEN de prioridad (Menor número = mayor urgencia al
  // frente)
  void push(Proceso p) {
    Nodo *nuevo = new Nodo(p);

    // Caso 1: La lista está vacía o el nuevo proceso tiene mayor prioridad que
    // la cabeza
    if (esVacia() || p.prioridad < cabeza->proceso.prioridad) {
      nuevo->sig = cabeza;
      cabeza = nuevo;
    }
    // Caso 2: Buscar la posición correcta dentro de la lista
    else {
      Nodo *aux = cabeza;
      while (aux->sig != nullptr &&
             aux->sig->proceso.prioridad <= p.prioridad) {
        aux = aux->sig;
      }
      nuevo->sig = aux->sig;
      aux->sig = nuevo;
    }
    tamano++;
  }

  // Extrae el elemento de mayor prioridad (CABEZA)
  Proceso pop() {
    if (esVacia())
      return {-1, "NULO", 0, 0, 0};
    Nodo *aux = cabeza;
    Proceso p = aux->proceso;
    cabeza = cabeza->sig;
    delete aux;
    tamano--;
    return p;
  }

  // TOP: Consulta el proceso en la cima sin eliminarlo
  Proceso top() {
    if (!esVacia()) {
      return cabeza->proceso;
    }
    return {-1, "NULO", 0, 0, 0};
  }

  // Obtiene el tamaño de la cola
  int obtenerTamano() { return tamano; }

  // Muestra los procesos en la cola
  void mostrar() {
    if (esVacia()) {
      cout << "   (Cola de Prioridad vacia)\n";
      return;
    }
    Nodo *aux = cabeza;
    while (aux != nullptr) {
      cout << "  -> PID: " << aux->proceso.id << " | " << aux->proceso.nombre
           << " | Prio: " << aux->proceso.prioridad
           << " | T. Restante: " << aux->proceso.tiempo_restante << "s\n";
      aux = aux->sig;
    }
  }
};

// Clase que controla todo el proceso de planificacion de tareas
class Planificador {
private: // variables privadas
  // Estructura 1: Cola con Prioridad (Alta urgencia)
  ColaPrioridad colaPrioridad;
  // Estructura 2: Cola (Prioridad normal)
  ColaSimple colaCircular;
  // Estructura 3: Pila (Historial)
  PilaHistorial historial;
  // Tiempo de CPU asignado por turno (ej. 2 segundos)
  int quantum;
  // Contador para generar IDs únicos
  int contadorPID;

public: // funciones publicas
  // Constructor
  Planificador(int q) {
    quantum = q;
    contadorPID = 100;
  }

  // Funcion para obtener el menor de dos numeros
  int min(int a, int b) {
    if (a < b) {
      return a;
    } else if (b < a) {
      return b;
    }
    return b;
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
           << ") agregado a COLA CIRCULAR.\n";
    }
  }

  // Ejecuta un ciclo de CPU (Paso a paso)
  void ejecutar_paso() {
    // Si hay procesos de alta prioridad, se ejecuta uno
    if (!colaPrioridad.esVacia()) {
      Proceso actual = colaPrioridad.top();
      colaPrioridad.pop();

      // Calcula el tiempo que se va a ejecutar
      int tiempo_ejecutado = min(quantum, actual.tiempo_restante);
      // Resta el tiempo ejecutado al tiempo restante
      actual.tiempo_restante -= tiempo_ejecutado;

      // Imprime el proceso que se está ejecutando
      cout << "\n[CPU -> EJECUTANDO (ALTA PRIORIDAD)] PID: " << actual.id
           << " | Nombre: " << actual.nombre
           << " | Tiempo Restante: " << actual.tiempo_restante << "s\n";

      // Verifica si el proceso terminó
      if (actual.tiempo_restante > 0) {
        // Si no terminó, se vuelve a agregar a la cola de prioridad
        colaPrioridad.push(actual);
        cout << "[*] Proceso '" << actual.nombre
             << "' vuelve a cola de prioridad.\n";

      } else {
        // en caso de haber terminado el proceso, se agrega a la PILA
        // HISTORIAL
        historial.push(actual);
        cout << "[+] Proceso '" << actual.nombre << "' FINALIZADO.\n";
      }

    } else if (!colaCircular.esVacia()) {
      // Si no hay procesos de alta prioridad, se ejecuta uno
      Proceso actual = colaCircular.top();
      colaCircular.pop();

      // Calcula el tiempo que se va a ejecutar
      int tiempo_ejecutado = min(quantum, actual.tiempo_restante);
      // Resta el tiempo ejecutado al tiempo restante
      actual.tiempo_restante -= tiempo_ejecutado;

      // Imprime el proceso que se está ejecutando
      cout << "\n[CPU -> EJECUTANDO (PRIORIDAD NORMAL)] PID: " << actual.id
           << " | Nombre: " << actual.nombre
           << " | Tiempo Restante: " << actual.tiempo_restante << "s\n";

      // Verifica si el proceso terminó
      if (actual.tiempo_restante > 0) {
        // Si no terminó, se vuelve a agregar a la cola de prioridad
        cout << "   -> Quantum agotado. Volviendo al final de la Cola "
                "Circular...\n";
        colaCircular.push(actual);
      } else {
        // en caso de haber terminado el proceso, se agrega a la PILA
        // HISTORIAL
        cout << "   -> ¡Proceso FINALIZADO! Apilando en el Historial...\n";
        historial.push(actual);
      }
    } else {
      cout << "\n[FIN] No hay procesos esperando. CPU en reposo.\n";
    }
  }

  // Ciclo de ejecucion para todos los procesos
  void ejecutar_todo() {
    while (!colaPrioridad.esVacia() || !colaCircular.esVacia()) {
      ejecutar_paso();
    }
  }

  // Deshace el ultimo proceso finalizado
  void deshacer_ultimo() {
    if (historial.esVacia()) {
      cout << "\n[DESHACER] El historial esta vacio. No hay procesos para "
              "desapilar.\n";
      return;
    }
    Proceso p = historial.pop();
    cout << "\n[DESHACER] Se ha sacado de la pila el proceso PID: " << p.id
         << " (" << p.nombre << "). Reinsertando a la CPU...\n";

    // Restablece el tiempo restante al tiempo inicial
    p.tiempo_restante = p.tiempo_inicial;
    // Inserta el proceso en la cola correspondiente
    if (p.prioridad <= 2) {
      colaPrioridad.push(p);
    } else {
      colaCircular.push(p);
    }
  }

  // Muestra el estado actual de todas las estructuras
  void ver_estado_sistema() {
    cout << "\n================ ESTADO ACTUAL DEL SISTEMA ================\n";
    cout << "[COLA DE PRIORIDAD] Elementos: " << colaPrioridad.obtenerTamano()
         << "\n";
    colaPrioridad.mostrar();

    cout << "\n[COLA CIRCULAR] Elementos: " << colaCircular.obtenerTamano()
         << "\n";
    colaCircular.mostrar();

    cout << "\n[PILA DE HISTORIAL] Procesos Finalizados: "
         << historial.obtenerTamano() << "\n";
    if (!historial.esVacia()) {
      Proceso topP = historial.top();
      cout << "  -> Cima actual (Ultimo terminado): PID " << topP.id << " ("
           << topP.nombre << ")\n";
    }
    cout << "===========================================================\n";
  }

  // Muestra el historial de procesos finalizados
  void mostrar_historial() {
    cout << "\n======== HISTORIAL DE PROCESOS FINALIZADOS (PILA) ========\n";
    historial.mostrar();
    cout << "===========================================================\n";
  }
};

// Función main con el menú interactivo para el usuario.
int main() {
  Planificador planificador(2); // Quantum = 2 segundos
  int opcion;

  // Carga inicial de prueba para verificar el funcionamiento.
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
      cout << "\nCerrando la simulacion.\n";
      break;
    default:
      cout << "\nOpcion invalida. Intente de nuevo.\n";
    }
  } while (opcion != 0);

  return 0;
}