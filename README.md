# Árbol de Magos

Este proyecto implementa un árbol binario que representa una jerarquía de magos con reglas específicas para la asignación de dueño de un hechizo, gestión de hechizos asociados y modificaciones de datos.

---

## Descripción

- Cada mago (nodo) contiene información personal, estado (vivo o muerto), tipo de magia, lista de hechizos y si es dueño del hechizo.
- El dueño actual del hechizo se determina por reglas específicas según el estado de los magos y su tipo de magia.
- Se permite cargar datos de magos y hechizos desde archivos CSV.
- El programa incluye un menú interactivo para consultar información, modificar datos y reasignar dueño.

---

## Funcionalidades principales

- Carga de magos y hechizos desde CSV.
- Visualización de la línea de sucesión del dueño del hechizo (solo magos vivos).
- Asignación automática de nuevo dueño del hechizo según reglas de magia, género y edad.
- Modificación interactiva de datos de cualquier mago, incluyendo hechizos.
- Visualización de hechizos asignados a cada mago.

Para más detalles revisar el código fuente.
