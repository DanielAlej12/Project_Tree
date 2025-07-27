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
