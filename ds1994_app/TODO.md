# TODO: Escalamiento del Proyecto DS1994 App (C++)

## 1. Infraestructura de Build
- [ ] Migrar de Makefile a **CMake** (gestión moderna de dependencias, multi-target)
- [ ] Soporte para **cross-compilation** (ARM64, ARMhf, x86_64)
- [ ] Integrar **Conan** o **vcpkg** para manejo de dependencias externas
- [ ] Agregar **sanitizers** (address, undefined, leak) en builds de debug
- [ ] Perfiles de build: `Debug`, `Release`, `RelWithDebInfo`, `MinSizeRel`

## 2. Testing
- [ ] Framework de testing: **Google Test** o **Catch2**
- [ ] **Unit tests** para cada capa: bus 1-Wire, memoria, RTC, timer, cycle counter
- [ ] **Integration tests** con mock de sysfs 1-Wire
- [ ] **Hardware-in-the-loop tests** con fixture físico
- [ ] **Fuzz testing** para parsing de comandos y entrada de datos
- [ ] Code coverage (gcov/lcov) con reportes automatizados

## 3. Arquitectura y Modularización
- [ ] Separar librería `libds1994` (API pública) del CLI `ds1994_app`
- [ ] Capas bien definidas:
  - **Capa HAL** (Hardware Abstraction Layer) para el bus 1-Wire
  - **Capa Driver** (DS1994 protocolo, registros, RTC, SRAM, timer)
  - **Capa CLI** (procesamiento de comandos)
  - **Capa Server/Service** (futuro: daemon, REST API)
- [ ] Interfaces abstractas (polimorfismo) para soportar múltiples backends de bus
- [ ] Fábrica de dispositivos para detectar y manejar diferentes familias 1-Wire

## 4. Backends de Bus 1-Wire
- [ ] Backend **sysfs** (w1-gpio kernel module) — actual, mantener
- [ ] Backend **w1-therm** directo por GPIO (bit-banging userspace)
- [ ] Backend **DS2482-100** (I2C a 1-Wire bridge)
- [ ] Backend **USB 1-Wire** (adaptadores serie)
- [ ] Backend **emulado/mock** para desarrollo sin hardware
- [ ] Selección automática del backend disponible

## 5. Manejo de Errores y Logging
- [ ] Sistema de **logging jerárquico** (spdlog o similar)
  - Niveles: TRACE, DEBUG, INFO, WARN, ERROR, FATAL
  - Salida: consola, archivo, syslog, journald
  - Rotación de logs
- [ ] Result types idiomáticos: `std::expected` (C++23) o `tl::expected`
- [ ] Excepciones solo para errores no recuperables
- [ ] Códigos de error bien definidos (enum class + string description)

## 6. Modo Servidor / Daemon
- [ ] **Daemon systemd** que corre en background
- [ ] **REST API** (crow, oatpp, o pistache) para consulta y control
  - `GET /device/info`
  - `GET /device/rtc`
  - `PUT /device/rtc`
  - `GET /device/sram/{page}`
  - `PUT /device/sram/{page}`
  - `GET /device/timer`
  - `GET /device/cycles`
- [ ] **WebSocket** para notificaciones en tiempo real (eventos/alarmas)
- [ ] **MQTT** para integración IoT
- [ ] Soporte para prometheus endpoint (`/metrics`)

## 7. Múltiples Dispositivos
- [ ] Soporte nativo para **múltiples DS1994** en el mismo bus
- [ ] Cache de dispositivos detectados con refresh periódico
- [ ] Operaciones por ID específico de dispositivo
- [ ] Broadcast/operaciones en paralelo

## 8. Configuración
- [ ] Archivo de configuración **YAML** o **TOML** (`/etc/ds1994/config.yaml`)
  - Backend de bus
  - Intervalo de refresco
  - Límites de alerta (RTC drift, timer, etc.)
  - Logging level y destino
  - Puerto HTTP, MQTT broker, etc.
- [ ] CLI con override de configuración por flags
- [ ] Hot-reload de configuración (SIGHUP)

## 9. Documentación
- [ ] **Doxygen** para API reference
- [ ] Diagramas de arquitectura (PlantUML / Mermaid)
- [ ] Guía de contribución (`CONTRIBUTING.md`)
- [ ] Changelog automatizado (release-please / standard-version)
- [ ] Documentación de protocolo 1-Wire específico del DS1994

## 10. CI/CD y Automatización
- [ ] **GitHub Actions** / GitLab CI
  - Lint (cppcheck, clang-tidy)
  - Build en múltiples arquitecturas
  - Tests unitarios y de integración
  - Code coverage
  - Static analysis (SonarCloud / CodeQL)
- [ ] Generación automática de **packages** (.deb, .rpm, Docker)
- [ ] **Dockerfile** multi-stage (builder + runtime slim)
- [ ] Publicación en GitHub Releases con artefactos

## 11. Performance y Robustez
- [ ] Benchmark suite (benchmarking con Google Benchmark)
- [ ] Timeouts configurables por operación 1-Wire
- [ ] Retry logic con backoff exponencial
- [ ] Watchdog para detección de dispositivos caídos
- [ ] Memory-mapped I/O donde sea posible

## 12. Internacionalización
- [ ] Soporte **i18n** (gettext) para mensajes CLI
- [ ] Locales: español, inglés (por ahora)

## 13. Distribución
- [ ] **Systemd unit** para run como servicio
- [ ] Debian packaging (`dpkg-buildpackage`)
- [ ] Script de instalación (`make install` / `cmake --install`)
- [ ] Soporte para **containers** (Docker, Podman)
- [ ] Integración con **Raspberry Pi OS** / Buildroot / Yocto

## Prioridades Sugeridas

| Prioridad | Área | Justificación |
|-----------|------|---------------|
| 🔴 Alta | Testing | Sin tests no se puede escalar con confianza |
| 🔴 Alta | Modularización | Separar lib de CLI desacopla responsabilidades |
| 🔴 Alta | Backends de Bus | Soporte para DS2482 y mock habilita testing |
| 🟡 Media | Logging | Necesario para debug en producción |
| 🟡 Media | Configuración | Flexibilidad sin recompilar |
| 🟢 Baja | Daemon/REST API | Feature para uso avanzado |
| 🟢 Baja | CI/CD | Automatización cuando el proyecto madure |
