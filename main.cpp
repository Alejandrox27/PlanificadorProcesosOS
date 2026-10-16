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

// 3. Clase Pila de Historial (LIFO)
class PilaHistorial {
private:
  NodoPila *cima;
  int tamano;

public:
  PilaHistorial() {
    cima = nullptr;
    tamano = 0;
  }

  ~PilaHistorial() {
    while (!esVacia()) {
      pop();
    }
  }

  bool esVacia() { return cima == nullptr; }

  // PUSH: Inserta el proceso finalizado en la cima
  void push(Proceso p) {
    NodoPila *nuevo = new NodoPila(p);
    nuevo->sig = cima;
    cima = nuevo;
    tamano++;
  }

  // POP: Elimina y retorna el proceso en la cima
  Proceso pop() {
    if (esVacia()) {
      cout << "La pila esta vacia.\n";
      return {-1, "NULO", 0, 0};
    }
    NodoPila *aux = cima;
    Proceso p = aux->proceso;
    cima = cima->sig;
    delete aux;
    tamano--;
    return p;
  }

  // TOP / PEEK: Consulta el proceso en la cima sin eliminarlo
  Proceso top() {
    if (!esVacia()) {
      return cima->proceso;
    }
    return {-1, "NULO", 0, 0};
  }

  int obtenerTamano() { return tamano; }
};

class Planificador {
private:
  priority_queue<Proceso, vector<Proceso>, CompararPrioridad>
      colaPrioridad; // Estructura 1: Cola con Prioridad (Alta urgencia)
  queue<Proceso>
      colaCircular; // Estructura 2: Cola (Round-Robin para prioridad normal)
  PilaHistorial
      historial; // Estructura 3: Pila (Historial de procesos finalizados)
  int quantum;   // Tiempo de CPU asignado por turno (ej. 2 segundos)

public:
  Planificador(int q) { quantum = q; }

  // Agregar proceso a la estructura correspondiente
  void agregarProceso(int id, string nombre, int prioridad, int tiempo) {
    Proceso p = {id, nombre, prioridad, tiempo};
    if (prioridad <= 2) {
      colaPrioridad.push(p);
      cout << "[+] Proceso '" << nombre << "' agregado a COLA DE PRIORIDAD.\n";
    } else {
      colaCircular.push(p);
      cout << "[+] Proceso '" << nombre
           << "' agregado a COLA CIRCULAR (Round-Robin).\n";
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

  void mostrar_historial() {}
};

int main() {

};