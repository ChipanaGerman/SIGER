# Guía de Contribución — SIGER

## Convenciones de Nombres

- **Clases:** `PascalCase` → `Vehiculo`, `GestorPedidos`
- **Métodos/funciones:** `camelCase` → `calcularCosto()`, `asignarPedido()`
- **Variables miembro:** prefijo `m_` → `m_peso`, `m_velocidad`
- **Variables locales:** `camelCase` → `totalEntregas`
- **Constantes:** `UPPER_SNAKE_CASE` → `MAX_CAPACIDAD`
- **Archivos:** `PascalCase` para clases → `Vehiculo.h`, `GestorPedidos.cpp`

## Estructura de Commits

```
<tipo>(<módulo>): <descripción corta en imperativo>

Tipos: feat | fix | docs | test | refactor | style | chore
```

**Ejemplos:**
```
feat(dominio): agregar clase abstracta Vehiculo con calcularCosto virtual pura
feat(generico): implementar template Cola<T> con priority_queue
feat(threading): agregar SimuladorEntregas con mutex y condition_variable
feat(mvc): implementar patrón Observer en GestorPedidos
feat(gui): crear DashboardView con tabla de pedidos en tiempo real
fix(threading): corregir race condition en cola compartida
docs: actualizar diagrama UML de clases
test: agregar tests de polimorfismo en jerarquía Paquete
```

## Ramas

```
main          ← producción estable
develop       ← integración
feature/lbacac-dominio-oop
feature/gmejia-generico-stl
feature/rponcel-threading
feature/gchipana-mvc-gui
```

## Indentación

- 4 espacios (no tabs)
- Llaves en la misma línea para métodos cortos, línea siguiente para definiciones de clase

## Estándar de Código

- C++17 mínimo
- Usar `auto` donde mejore la legibilidad
- Smart pointers obligatorios (`std::shared_ptr`, `std::unique_ptr`)
- Sin `using namespace std;` en archivos `.h`
