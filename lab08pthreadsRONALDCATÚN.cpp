

#include <iostream>
#include <pthread.h>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
using namespace std;


struct MatrixThreadData {
    vector<vector<int>>* matriz;  
    int totalFilas;               
    int totalColumnas;            
    int inicio;                   
    int fin;                      
    int id;                       
};

void* ordenarFilasDescendente(void* arg) {
    MatrixThreadData* data = (MatrixThreadData*)arg;

    cout << "[Hilo " << data->id << "] Procesando filas "
         << data->inicio << " a " << (data->fin - 1) << endl;

    for (int i = data->inicio; i < data->fin; i++) {
        sort((*data->matriz)[i].begin(), (*data->matriz)[i].end(), greater<int>());

        int maximo = (*data->matriz)[i][0];
        cout << "[Hilo " << data->id << "] Fila " << i
             << " -> Maximo: " << maximo << endl;
    }

    return NULL;
}

void ejercicio1() {
    cout << "  EJERCICIO I: Ordenamiento de matrices con hilos" << endl;

    const int FILAS    = 400;
    const int COLUMNAS = 600;
    const int NUM_HILOS = 4;

    srand((unsigned int)time(0));

    vector<vector<int>> matriz(FILAS, vector<int>(COLUMNAS));
    for (int i = 0; i < FILAS; i++)
        for (int j = 0; j < COLUMNAS; j++)
            matriz[i][j] = rand() % 1000;

    pthread_t hilos[NUM_HILOS];
    MatrixThreadData datos[NUM_HILOS];

    int filasPorHilo = FILAS / NUM_HILOS;

    for (int i = 0; i < NUM_HILOS; i++) {
        datos[i].matriz        = &matriz;
        datos[i].totalFilas    = FILAS;
        datos[i].totalColumnas = COLUMNAS;
        datos[i].inicio        = i * filasPorHilo;
        datos[i].fin           = (i == NUM_HILOS - 1) ? FILAS : (i + 1) * filasPorHilo;
        datos[i].id            = i;

        pthread_create(&hilos[i], NULL, ordenarFilasDescendente, &datos[i]);
    }

    for (int i = 0; i < NUM_HILOS; i++)
        pthread_join(hilos[i], NULL);

    cout << "\n[Main] Verificacion - Primeras 3 filas (primeros 10 elementos):" << endl;
    for (int i = 0; i < 3; i++) {
        cout << "  Fila " << i << ": ";
        for (int j = 0; j < 10; j++)
            cout << matriz[i][j] << " ";
        cout << "..." << endl;
    }

    cout << "\n[Ejercicio I] Completado con " << NUM_HILOS << " hilos." << endl;
}


struct SumaThreadData {
    vector<int>* arr;     
    int inicio;           
    int fin;              
    int id;               
    long long sumaParcial;
    int cantPares;        
};

void* calcularSegmento(void* arg) {
    SumaThreadData* data = (SumaThreadData*)arg;
    data->sumaParcial = 0;
    data->cantPares   = 0;

    for (int i = data->inicio; i < data->fin; i++) {
        int val = (*(data->arr))[i];
        data->sumaParcial += val;
        if (val % 2 == 0)
            data->cantPares++;
    }

    cout << "[Hilo " << data->id << "] Segmento [" << data->inicio
         << ", " << (data->fin - 1) << "] -> Suma: " << data->sumaParcial
         << "  Pares: " << data->cantPares << endl;

    return NULL;
}

void ejercicio2() {
    cout << "  EJERCICIO II: Calculo Paralelo (suma y pares)" << endl;

    int n, numHilos;

    do {
        cout << "Ingrese el tamanio del arreglo: ";
        cin >> n;
        cout << "Ingrese la cantidad de hilos: ";
        cin >> numHilos;

        if (n < numHilos)
            cout << "[Error] El numero de elementos debe ser >= al numero de hilos. Intente de nuevo.\n" << endl;
    } while (n < numHilos);

    srand((unsigned int)time(0));
    vector<int> arr(n);
    for (int i = 0; i < n; i++)
        arr[i] = rand() % 100;

    if (n <= 40) {
        cout << "\nArreglo generado: ";
        for (int v : arr) cout << v << " ";
        cout << endl;
    }

    vector<pthread_t>       hilos(numHilos);
    vector<SumaThreadData>  datos(numHilos);

    int segmento = n / numHilos;

    cout << "\n";
    for (int i = 0; i < numHilos; i++) {
        datos[i].arr    = &arr;
        datos[i].inicio = i * segmento;
        datos[i].fin    = (i == numHilos - 1) ? n : (i + 1) * segmento;
        datos[i].id     = i;

        pthread_create(&hilos[i], NULL, calcularSegmento, &datos[i]);
    }

    long long sumaTotal  = 0;
    int       paresTotal = 0;

    for (int i = 0; i < numHilos; i++) {
        pthread_join(hilos[i], NULL);
        sumaTotal  += datos[i].sumaParcial;
        paresTotal += datos[i].cantPares;
    }

    cout << "\n[Main] Suma total del arreglo: " << sumaTotal << endl;
    cout << "[Main] Cantidad total de numeros pares: " << paresTotal << endl;
    cout << "\n[Ejercicio II] Completado con " << numHilos << " hilos." << endl;
}


struct DecoderThreadData {
    vector<string>* instrucciones; 
    int inicio;                    
    int fin;                       
    int id;                        
    int validas;                   
    int invalidas;                 
};


bool validarParidad(const string& instruccion) {
    if (instruccion.size() != 8) return false;
    int unos = 0;
    for (char c : instruccion)
        if (c == '1') unos++;
    return (unos % 2 == 0);
}

void* procesarInstrucciones(void* arg) {
    DecoderThreadData* data = (DecoderThreadData*)arg;
    data->validas   = 0;
    data->invalidas = 0;

    cout << "[Hilo " << data->id << "] Procesando instrucciones ["
         << data->inicio << ", " << (data->fin - 1) << "]:" << endl;

    for (int i = data->inicio; i < data->fin; i++) {
        const string& instr = (*(data->instrucciones))[i];
        bool esValida = validarParidad(instr);

        if (esValida) {
            data->validas++;
            cout << "  [Hilo " << data->id << "] Instr[" << i << "] = "
                 << instr << " -> VALIDA (paridad correcta)" << endl;
        } else {
            data->invalidas++;
            cout << "  [Hilo " << data->id << "] Instr[" << i << "] = "
                 << instr << " -> INVALIDA (paridad incorrecta)" << endl;
        }
    }

    cout << "[Hilo " << data->id << "] Resumen: "
         << data->validas << " validas, "
         << data->invalidas << " invalidas." << endl;

    return NULL;
}

void ejercicio3() {
    cout << "  EJERCICIO III: Decodificador de instrucciones binarias" << endl;
    vector<string> instrucciones = {
        "10110001",  
        "11001100",  
        "10101010", 
        "11110000",  
        "10000001",  
        "11111111",  
        "10110011",  
        "11000001",  
        "10011100",  
        "11100001",  
        "10001111",  
        "11010101"   
    };

    int numInstrucciones = (int)instrucciones.size();

    int numHilos;
    cout << "Instrucciones cargadas: " << numInstrucciones << endl;
    cout << "Ingrese la cantidad de hilos (1-" << numInstrucciones << "): ";
    cin >> numHilos;

    if (numHilos < 1) numHilos = 1;
    if (numHilos > numInstrucciones) numHilos = numInstrucciones;

    cout << "\nUsando " << numHilos << " hilos para " << numInstrucciones
         << " instrucciones.\n" << endl;

    vector<pthread_t>          hilos(numHilos);
    vector<DecoderThreadData>  datos(numHilos);

    int bloque = numInstrucciones / numHilos;

    for (int i = 0; i < numHilos; i++) {
        datos[i].instrucciones = &instrucciones;
        datos[i].inicio        = i * bloque;
        datos[i].fin           = (i == numHilos - 1) ? numInstrucciones : (i + 1) * bloque;
        datos[i].id            = i;

        pthread_create(&hilos[i], NULL, procesarInstrucciones, &datos[i]);
    }

    int totalValidas   = 0;
    int totalInvalidas = 0;

    for (int i = 0; i < numHilos; i++) {
        pthread_join(hilos[i], NULL);
        totalValidas   += datos[i].validas;
        totalInvalidas += datos[i].invalidas;
    }

    cout << "\n[Main] RESUMEN GLOBAL:" << endl;
    cout << "  Total instrucciones procesadas: " << numInstrucciones << endl;
    cout << "  Total VALIDAS  (paridad par OK): " << totalValidas << endl;
    cout << "  Total INVALIDAS (paridad errada): " << totalInvalidas << endl;
    cout << "\n[Ejercicio III] Completado con " << numHilos << " hilos." << endl;
}


int main() {
    cout << "  CC3086 - Lab 08: Introduccion a Pthreads" << endl;
    cout << "  Universidad del Valle de Guatemala" << endl;

    ejercicio1();

    ejercicio2();

    ejercicio3();

    cout << "  Laboratorio 08 finalizado." << endl;
    return 0;
}
