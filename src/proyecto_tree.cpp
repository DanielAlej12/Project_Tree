#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

struct Hechizo {
    string nombre;
    Hechizo* siguiente;
    Hechizo(const string& n) : nombre(n), siguiente(nullptr) {}
};

struct Mago {
    int id;
    string name;
    string last_name;
    char gender; // 'H' o 'M'
    int age;
    int id_father;
    bool is_dead;
    string type_magic; // elemental, unique, mixed, no_magic
    bool is_owner;
    Mago* left;  // primer discípulo
    Mago* right; // segundo discípulo
    Mago* father; // apuntador al padre para facilitar búsquedas
    Hechizo* listaHechizos;

    Mago(int _id, string _name, string _last_name, char _gender, int _age, int _id_father,
         bool _is_dead, string _type_magic, bool _is_owner)
        : id(_id), name(_name), last_name(_last_name), gender(_gender), age(_age),
          id_father(_id_father), is_dead(_is_dead), type_magic(_type_magic), is_owner(_is_owner),
          left(nullptr), right(nullptr), father(nullptr), listaHechizos(nullptr) {}
};

class ArbolMagos {
private:
    Mago* root;

    // Buscar mago en el árbol por ID
    Mago* buscarMago(Mago* nodo, int id) {
        if (!nodo) return nullptr;
        if (nodo->id == id) return nodo;
        Mago* encontrado = buscarMago(nodo->left, id);
        if (encontrado) return encontrado;
        return buscarMago(nodo->right, id);
    }

    // Insertar discípulo izquierdo o derecho del padre
    void insertarDiscipulo(Mago* padre, Mago* discipulo) {
        if (padre->left == nullptr) {
            padre->left = discipulo;
        } else if (padre->right == nullptr) {
            padre->right = discipulo;
        } else {
            cout << "El mago con id " << padre->id << " ya tiene dos discipulos." << endl;
        }
        discipulo->father = padre;
    }

    // Mostrar línea de sucesión según reglas (solo vivos)
    void mostrarLineaSucesionSegunReglas(Mago* mago) {
        if (!mago) return;
        if (!mago->is_dead) {
            cout << mago->name << " " << mago->last_name << " (Edad:" << mago->age 
                 << ") ID:" << mago->id << " Magia:" << mago->type_magic
                 << (mago->is_owner ? " [Dueno]" : "") << "\n";
        }

        Mago* siguiente = buscarDiscipuloCondicion(mago);
        if (siguiente && siguiente != mago) {
            mostrarLineaSucesionSegunReglas(siguiente);
        }
    }

    // Buscar el mago dueño actual (is_owner == true)
    Mago* buscarDuenio(Mago* nodo) {
        if (!nodo) return nullptr;
        if (nodo->is_owner) return nodo;
        Mago* l = buscarDuenio(nodo->left);
        if (l) return l;
        return buscarDuenio(nodo->right);
    }

    // Buscar discípulo vivo con tipo de magia específica entre un arreglo de opciones
    Mago* buscarDiscipuloConMagia(Mago* nodo, const string magias[], int n) {
        if (!nodo) return nullptr;
        if (!nodo->is_dead) {
            for (int i = 0; i < n; i++) {
                if (nodo->type_magic == magias[i]) return nodo;
            }
        }
        Mago* res = buscarDiscipuloConMagia(nodo->left, magias, n);
        if (res) return res;
        return buscarDiscipuloConMagia(nodo->right, magias, n);
    }

    // Buscar primer hombre vivo en subárbol
    Mago* buscarPrimerHombreVivo(Mago* nodo) {
        if (!nodo) return nullptr;
        if (!nodo->is_dead && nodo->gender == 'H') return nodo;
        Mago* res = buscarPrimerHombreVivo(nodo->left);
        if (res) return res;
        return buscarPrimerHombreVivo(nodo->right);
    }

    // Buscar compañero discípulo (hermano)
    Mago* buscarCompanero(Mago* mago) {
        if (!mago || !mago->father) return nullptr;
        Mago* padre = mago->father;
        if (padre->left && padre->left != mago) return padre->left;
        if (padre->right && padre->right != mago) return padre->right;
        return nullptr;
    }

    // Verifica si dos magos comparten la misma magia
    bool compartenMagia(Mago* a, Mago* b) {
        if (!a || !b) return false;
        return a->type_magic == b->type_magic;
    }

    // Buscar discípulo vivo con la prioridad de magias según reglas
    Mago* buscarDiscipuloCondicion(Mago* nodo) {
        if (!nodo) return nullptr;
        const string magiasPrimarias[] = {"elemental", "unique"};
        Mago* res = buscarDiscipuloConMagia(nodo->left, magiasPrimarias, 2);
        if (res) return res;
        res = buscarDiscipuloConMagia(nodo->right, magiasPrimarias, 2);
        if (res) return res;
        const string magiasMixed[] = {"mixed"};
        res = buscarDiscipuloConMagia(nodo->left, magiasMixed, 1);
        if (res) return res;
        res = buscarDiscipuloConMagia(nodo->right, magiasMixed, 1);
        if (res) return res;
        res = buscarPrimerHombreVivo(nodo->left);
        if (res) return res;
        return buscarPrimerHombreVivo(nodo->right);
    }

    // Buscar dentro del árbol los discípulos del compañero que cumplan la condición inicial
    Mago* buscarEnArbolCompanero(Mago* companero) {
        if (!companero) return nullptr;
        Mago* nuevo = buscarDiscipuloCondicion(companero);
        if (nuevo) return nuevo;
        return nullptr;
    }

    // Obtener el compañero del maestro (abuelo)
    Mago* companeroDeMaestro(Mago* mago) {
        if (!mago || !mago->father || !mago->father->father) return nullptr;
        Mago* maestro = mago->father;
        Mago* abuelo = maestro->father;
        if (abuelo->left && abuelo->left != maestro) return abuelo->left;
        if (abuelo->right && abuelo->right != maestro) return abuelo->right;
        return nullptr;
    }

    // Buscar mujer más joven con discípulos cuyo maestro fue dueño y tiene magia mixed
    void buscarMujerJovenConDiscipulosYNecesaria(Mago* nodo, Mago*& candidata, int& edadCandidata) {
        if (!nodo) return;
        if (nodo->gender == 'M' && !nodo->is_dead && (nodo->left || nodo->right)) {
            Mago* maestro = nodo->father;
            if (maestro && maestro->is_owner && maestro->type_magic == "mixed") {
                if (nodo->age < edadCandidata) {
                    candidata = nodo;
                    edadCandidata = nodo->age;
                }
            }
        }
        buscarMujerJovenConDiscipulosYNecesaria(nodo->left, candidata, edadCandidata);
        buscarMujerJovenConDiscipulosYNecesaria(nodo->right, candidata, edadCandidata);
    }

    // Buscar mujer más joven viva (sin más condiciones)
    void buscarMujerJoven(Mago* nodo, Mago*& candidata, int& edadCandidata) {
        if (!nodo) return;
        if (nodo->gender == 'M' && !nodo->is_dead) {
            if (nodo->age < edadCandidata) {
                candidata = nodo;
                edadCandidata = nodo->age;
            }
        }
        buscarMujerJoven(nodo->left, candidata, edadCandidata);
        buscarMujerJoven(nodo->right, candidata, edadCandidata);
    }

    // Buscar discípulo con la misma magia que el mago
    Mago* buscarDiscipuloMagia(Mago* nodo, const string& magia) {
        if (!nodo) return nullptr;
        if (!nodo->is_dead && nodo->type_magic == magia) return nodo;
        Mago* res = buscarDiscipuloMagia(nodo->left, magia);
        if (res) return res;
        return buscarDiscipuloMagia(nodo->right, magia);
    }

    // Buscar mago vivo más viejo en el subárbol
    void buscarMasViejo(Mago* nodo, Mago*& masViejo, int& edadMax) {
        if (!nodo) return;
        if (!nodo->is_dead && nodo->age > edadMax) {
            masViejo = nodo;
            edadMax = nodo->age;
        }
        buscarMasViejo(nodo->left, masViejo, edadMax);
        buscarMasViejo(nodo->right, masViejo, edadMax);
    }

    // Transferir lista de hechizos del viejo dueño al nuevo dueño
    void transferirHechizos(Mago* viejoDuenio, Mago* nuevoDuenio) {
        if (!viejoDuenio || !nuevoDuenio || viejoDuenio == nuevoDuenio) return;
        if (viejoDuenio->listaHechizos == nullptr) return; // nada que transferir

        if (nuevoDuenio->listaHechizos == nullptr) {
            nuevoDuenio->listaHechizos = viejoDuenio->listaHechizos;
        } else {
            Hechizo* actual = viejoDuenio->listaHechizos;
            while (actual->siguiente != nullptr) {
                actual = actual->siguiente;
            }
            actual->siguiente = nuevoDuenio->listaHechizos;
            nuevoDuenio->listaHechizos = viejoDuenio->listaHechizos;
        }
        viejoDuenio->listaHechizos = nullptr;
    }

public:
    ArbolMagos() : root(nullptr) {}

    // 1. Cargar datos desde CSV para construir árbol binario
    void cargarDesdeCSV(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "No se pudo abrir el archivo " << filename << endl;
            return;
        }

        string line;
        getline(file, line); // Encabezado

        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            int id, age, id_father;
            string name, last_name, type_magic;
            char gender;
            int is_dead_int, is_owner_int;

            getline(ss, token, ','); id = stoi(token);
            getline(ss, name, ',');
            getline(ss, last_name, ',');
            getline(ss, token, ','); gender = token[0];
            getline(ss, token, ','); age = stoi(token);
            getline(ss, token, ','); id_father = stoi(token);
            getline(ss, token, ','); is_dead_int = stoi(token);
            getline(ss, type_magic, ',');
            getline(ss, token, ',');
            if (!token.empty() && token.back() == '\r') token.pop_back();
            is_owner_int = token.empty() ? 0 : stoi(token);

            Mago* nuevo = new Mago(id, name, last_name, gender, age, id_father, is_dead_int == 1, type_magic, is_owner_int == 1);

            if (id_father == 0) {
                root = nuevo;
            } else {
                Mago* padre = buscarMago(root, id_father);
                if (padre) {
                    insertarDiscipulo(padre, nuevo);
                } else {
                    cout << "No se encontro padre con id " << id_father << " para el mago " << id << endl;
                }
            }
        }
        file.close();
    }

    // Agrega hechizo al mago (a la cabeza de la lista)
    void agregarHechizo(Mago* mago, const string& nombreHechizo) {
        Hechizo* nuevo = new Hechizo(nombreHechizo);
        nuevo->siguiente = mago->listaHechizos;
        mago->listaHechizos = nuevo;
    }

    // Cargar hechizos desde CSV asignándolos a magos
    void cargarHechizosDesdeCSV(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "No se pudo abrir el archivo de hechizos " << filename << endl;
            return;
        }

        string line;
        getline(file, line); // cabecera

        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            int id_mago;
            string nombre_hechizo;

            getline(ss, token, ',');
            id_mago = stoi(token);
            getline(ss, nombre_hechizo);
            if (!nombre_hechizo.empty() && nombre_hechizo.back() == '\r')
                nombre_hechizo.pop_back();

            Mago* mago = buscarMago(root, id_mago);
            if (mago) {
                agregarHechizo(mago, nombre_hechizo);
            } else {
                cout << "No se encontró mago con id " << id_mago << " para asignar hechizo " << nombre_hechizo << endl;
            }
        }
        file.close();
    }

    // 2. Mostrar la linea de sucesión actual (solo vivos)
    void mostrarLineaSucesion() {
        cout << "Linea de sucesion actual:\n";
        Mago* duenio = buscarDuenio(root);

        if (!duenio) {
            cout << "No se encontro dueno actual\n";
            return;
        }
        mostrarLineaSucesionSegunReglas(duenio);
    }

void asignarNuevoDuenio() {
        Mago* duenio = buscarDuenio(root);
        if (!duenio) {
            cout << "No hay dueno actual.\n";
            return;
        }
        if (!duenio->is_dead) {
            cout << "El dueno actual esta vivo: " << duenio->name << " " << duenio->last_name << endl;
            return;
        }

        cout << "El dueno actual " << duenio->name << " " << duenio->last_name << " ha fallecido. Buscando nuevo dueno...\n";
        duenio->is_owner = false;

        if (duenio->left || duenio->right) {
            Mago* n = buscarDiscipuloCondicion(duenio);
            if (n) {
                n->is_owner = true;
                transferirHechizos(duenio, n);
                cout << "Nuevo dueno es discipulo con condicion: " << n->name << " " << n->last_name << endl;
                return;
            }
        }

        Mago* companero = buscarCompanero(duenio);
        if (companero) {
            if (!companero->is_dead && compartenMagia(duenio, companero)) {
                companero->is_owner = true;
                transferirHechizos(duenio, companero);
                cout << "Nuevo dueno es companero vivo y comparten magia: " << companero->name << " " << companero->last_name << endl;
                return;
            }
            Mago* enArbolComp = buscarEnArbolCompanero(companero);
            if (enArbolComp) {
                enArbolComp->is_owner = true;
                transferirHechizos(duenio, enArbolComp);
                cout << "Nuevo dueno es discipulo vivo del arbol del companero: " << enArbolComp->name << " " << enArbolComp->last_name << endl;
                return;
            }
        }

        Mago* companeroMaestro = companeroDeMaestro(duenio);
        if (companeroMaestro && !companeroMaestro->is_dead) {
            companeroMaestro->is_owner = true;
            transferirHechizos(duenio, companeroMaestro);
            cout << "Nuevo dueno es companero del maestro: " << companeroMaestro->name << " " << companeroMaestro->last_name << endl;
            return;
        }

        if (duenio->father && duenio->father->is_dead) {
            Mago* nuevoDuenio = nullptr;
            const string magias_primarias[] = {"elemental", "unique"};
            nuevoDuenio = buscarDiscipuloConMagia(root, magias_primarias, 2);
            if (!nuevoDuenio) {
                const string magias_mixed[] = {"mixed"};
                nuevoDuenio = buscarDiscipuloConMagia(root, magias_mixed, 1);
            }
            if (!nuevoDuenio) nuevoDuenio = buscarPrimerHombreVivo(root);

            if (nuevoDuenio) {
                nuevoDuenio->is_owner = true;
                transferirHechizos(duenio, nuevoDuenio);
                cout << "Por la condicion del maestro muerto, nuevo dueno: " << nuevoDuenio->name << " " << nuevoDuenio->last_name << endl;
                return;
            }
        }

        Mago* candidata = nullptr;
        int edadCandidata = 1000;
        buscarMujerJovenConDiscipulosYNecesaria(root, candidata, edadCandidata);
        if (candidata) {
            candidata->is_owner = true;
            transferirHechizos(duenio, candidata);
            cout << "Nuevo dueno mujer joven con discipulos y maestro con magia mixed: " << candidata->name << " " << candidata->last_name << endl;
            return;
        }

        candidata = nullptr;
        edadCandidata = 1000;
        buscarMujerJoven(root, candidata, edadCandidata);
        if (candidata) {
            candidata->is_owner = true;
            transferirHechizos(duenio, candidata);
            cout << "Nuevo dueno mujer mas joven: " << candidata->name << " " << candidata->last_name << endl;
            return;
        }

        if (duenio->age > 70) {
            Mago* discipulo = buscarDiscipuloMagia(duenio, duenio->type_magic);
            if (discipulo) {
                discipulo->is_owner = true;
                transferirHechizos(duenio, discipulo);
                cout << "De acuerdo a la regla de edad > 70, nuevo dueno asignado: " << discipulo->name << " " << discipulo->last_name << endl;
                return;
            } else {
                Mago* masViejo = nullptr;
                int edadMax = -1;
                buscarMasViejo(root, masViejo, edadMax);
                if (masViejo) {
                    masViejo->is_owner = true;
                    transferirHechizos(duenio, masViejo);
                    cout << "De acuerdo a la regla de edad > 70, nuevo dueno asignado (mas viejo): " << masViejo->name << " " << masViejo->last_name << endl;
                    return;
                }
            }
        }

        cout << "No se encontro mago para asignar como nuevo dueno." << endl;
    }

void modificarDatos(int id) {
        Mago* mago = buscarMago(root, id);
        if (!mago) {
            cout << "No se encontro mago con id " << id << endl;
            return;
        }
        cout << "Modificando datos de " << mago->name << " " << mago->last_name << endl;

        cout << "Nuevo nombre: ";
        string n; cin >> n; mago->name = n;

        cout << "Nuevo apellido: ";
        cin >> n; mago->last_name = n;

        cout << "Nuevo genero (H/M): ";
        char c; cin >> c;
        if (c == 'H' || c == 'M') mago->gender = c;

        cout << "Nueva edad: ";
        int e; cin >> e; mago->age = e;

        cout << "Esta muerto? (0 = No, 1 = Si): ";
        int m; cin >> m; mago->is_dead = (m == 1);

        cout << "Nuevo tipo de magia (elemental, unique, mixed, no_magic): ";
        string t; cin >> t; mago->type_magic = t;

        cout << "Desea agregar un hechizo? (s/n): ";
        char resp;
        cin >> resp;
        while (resp == 's' || resp == 'S') {
            cout << "Nombre del hechizo: ";
            string nuevoHechizo;
            cin.ignore(); // limpiar buffer para getline
            getline(cin, nuevoHechizo);
            agregarHechizo(mago, nuevoHechizo);
            cout << "Agregar otro hechizo? (s/n): ";
            cin >> resp;
        }

        cout << "Desea eliminar un hechizo? (s/n): ";
        cin >> resp;
        if (resp == 's' || resp == 'S') {
            cout << "Nombre del hechizo a eliminar: ";
            string eliminarHechizo;
            cin.ignore();
            getline(cin, eliminarHechizo);

            Hechizo* actual = mago->listaHechizos;
            Hechizo* prev = nullptr;
            while (actual && actual->nombre != eliminarHechizo) {
               prev = actual;
               actual = actual->siguiente;
            }
            if (!actual) {
                cout << "Hechizo no encontrado.\n";
            } else {
                if (!prev) {
                    mago->listaHechizos = actual->siguiente; // Eliminar cabeza
                } else {
                    prev->siguiente = actual->siguiente;
                }
                delete actual;
            }
        }

        cout << "Es dueno del hechizo? (0 = No, 1 = Si): ";
        int d; cin >> d; mago->is_owner = (d == 1);

        cout << "Datos modificados correctamente.\n";
    }
