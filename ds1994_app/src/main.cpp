#include "ds1994.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace std;

string toHex(uint16_t v) {
    ostringstream oss;
    oss << "0x" << hex << setw(4) << setfill('0') << v;
    return oss.str();
}

void printBanner() {
    cout << "\n╔════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║              DS1994-F5 iButton - Driver v2.1                    ║" << endl;
    cout << "║              Maxim 1-Wire RTC + SRAM para Raspberry Pi          ║" << endl;
    cout << "╚════════════════════════════════════════════════════════════════╝" << endl;
}

void printHelp(const char* prog) {
    cout << "\nUso: " << prog << " [COMANDO] [args...]" << endl;
    cout << endl;
    cout << "╔══════════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║                      COMANDOS DISPONIBLES                            ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════════════════╝" << endl;
    cout << endl;
    cout << " ┌───────────────┬───────────────────────────────────────────────────┐" << endl;
    cout << " │ COMANDO       │ DESCRIPCIÓN                                        │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ info          │ Información completa del dispositivo               │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ scan          │ Escanear bus en busca de dispositivos DS1994       │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ status        │ Leer Status (0x0200) y Control (0x0201)            │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ dump          │ Volcado hexadecimal completo (SRAM + timekeeping)  │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ ramdump       │ Volcado solo SRAM (512 bytes, 16 páginas)          │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ rtcdump       │ Volcado registros timekeeping (0x0200 - 0x021D)    │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ rtc           │ Leer RTC (timestamp Unix)                          │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ setrtc        │ Sincronizar RTC con hora del sistema               │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ interval      │ Leer Interval Timer (seg + 1/256 fracción)         │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ cycles        │ Leer Cycle Counter (contador de ciclos)            │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ read <addr>    │ Leer 16 bytes desde dirección hex                    │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ read <i> <f>  │ Leer rango de memoria [i] a [f] (hex)              │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ write <pag>   │ Escribir datos hex en página                       │" << endl;
    cout << " │      <hex>    │ Ejemplo: write 0 48656C6C6F                        │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ osc on/off    │ Encender/apagar oscilador interno                  │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ test          │ Prueba de escritura (0xAA en 0x0000)               │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ wp            │ Mostrar estado de protección de escritura          │" << endl;
    cout << " ├───────────────┼───────────────────────────────────────────────────┤" << endl;
    cout << " │ help          │ Mostrar esta ayuda                                 │" << endl;
    cout << " └───────────────┴───────────────────────────────────────────────────┘" << endl;
    cout << endl;
    cout << " Ejemplos:" << endl;
    cout << "   sudo " << prog << " info" << endl;
    cout << "   sudo " << prog << " read 0x0000" << endl;
    cout << "   sudo " << prog << " read 0x0000 0x00FF" << endl;
    cout << "   sudo " << prog << " write 0 48656C6C6F" << endl;
    cout << "   sudo " << prog << " setrtc" << endl;
    cout << "   sudo " << prog << " dump" << endl;
    cout << endl;
}

int main(int argc, char* argv[]) {
    printBanner();

    // Ruta del bus 1-Wire
    string bus_path = "/sys/devices/w1_bus_master1";
    DS1994 ds1994(nullptr, bus_path);

    if (!ds1994.selectDevice()) {
        cerr << "\n❌ No se encontró ningún dispositivo DS1994 en: " << bus_path << endl;
        cerr << "\nVerifique:" << endl;
        cerr << "  1. ls " << bus_path << "/" << endl;
        cerr << "  2. Conexión física del iButton" << endl;
        cerr << "  3. Resistencia pull-up de 4.7kΩ" << endl;
        cerr << "  4. sudo modprobe w1-gpio w1-therm" << endl;
        return 1;
    }

    cout << "\n📟 Dispositivo: " << ds1994.getCurrentDeviceId() << endl;

    string cmd = (argc > 1) ? argv[1] : "info";

    if (cmd == "info") {
        ds1994.printInfo();
    }
    else if (cmd == "scan") {
        auto devices = ds1994.scanDevices();
        cout << "\n🔍 Dispositivos encontrados: " << devices.size() << endl;
        for (const auto& dev : devices) {
            cout << "   📟 " << dev.id;
            if (dev.active) cout << " [ACTIVO]";
            cout << endl;
        }
    }
    else if (cmd == "status") {
        uint8_t s = ds1994.readStatus();
        uint8_t c = ds1994.readControl();
        cout << "\nStatus:  0x" << hex << setw(2) << setfill('0') << (int)s << dec << endl;
        cout << "Control: 0x" << hex << setw(2) << setfill('0') << (int)c << dec << endl;
        cout << "\nControl Register bits:" << endl;
        cout << "  WPR="  << ((c >> 0) & 1) << "  WPI=" << ((c >> 1) & 1)
             << "  WPC=" << ((c >> 2) & 1) << "  RO="  << ((c >> 3) & 1)
             << "  OSC=" << ((c >> 4) & 1) << "  AUTO=" << ((c >> 5) & 1)
             << "  STP=" << ((c >> 6) & 1) << "  DSEL=" << ((c >> 7) & 1) << endl;
    }
    else if (cmd == "dump") {
        ds1994.dumpMemory(true);
    }
    else if (cmd == "ramdump") {
        ds1994.dumpMemory(false);
    }
    else if (cmd == "rtcdump") {
        auto data = ds1994.readMemory(0x0200, 30);
        DS1994::hexDump(data, "Registros Timekeeping (0x0200 - 0x021D)");
    }
    else if (cmd == "rtc") {
        time_t t = ds1994.readRTC();
        cout << "\nRTC: " << ctime(&t);
    }
    else if (cmd == "setrtc") {
        time_t now = time(nullptr);
        cout << "Escribiendo RTC: " << now << " (" << ctime(&now) << ")" << endl;
        if (ds1994.writeRTC(now)) {
            cout << "✅ RTC actualizado" << endl;
            time_t verify = ds1994.readRTC();
            cout << "Verificación RTC: " << ctime(&verify);
        } else {
            cerr << "❌ Falló escritura RTC (dispositivo protegido?)" << endl;
        }
    }
    else if (cmd == "interval") {
        double val = ds1994.readIntervalTimer();
        cout << "\nInterval Timer: " << fixed << setprecision(3) << val << " seg" << endl;
    }
    else if (cmd == "cycles") {
        cout << "\nCycle Counter: " << ds1994.readCycleCounter() << " ciclos" << endl;
    }
    else if (cmd == "read" && argc >= 3) {
        uint16_t start = stoul(argv[2], nullptr, 16);
        if (argc >= 4) {
            uint16_t end = stoul(argv[3], nullptr, 16);
            if (end < start) swap(start, end);
            size_t len = end - start + 1;
            auto data = ds1994.readMemory(start, len);
            DS1994::hexDump(data, "Memoria " + toHex(start) + " - " + toHex(end));
        } else {
            auto data = ds1994.readMemory(start, 16);
            DS1994::hexDump(data, "Memoria " + toHex(start));
        }
    }
    else if (cmd == "write" && argc > 3) {
        int page = stoi(argv[2]);
        string hex = argv[3];
        vector<uint8_t> data;
        for (size_t i = 0; i + 1 < hex.size(); i += 2) {
            data.push_back(stoi(hex.substr(i, 2), nullptr, 16));
        }
        cout << "Escribiendo " << data.size() << " bytes en página " << page << "..." << endl;
        if (ds1994.writePage(page, data)) {
            cout << "✅ Escritura exitosa" << endl;
            auto verify = ds1994.readPage(page);
            DS1994::hexDump(verify, "Verificación página " + to_string(page));
        } else {
            cerr << "❌ Escritura fallida" << endl;
        }
    }
    else if (cmd == "osc" && argc > 2) {
        bool on = (string(argv[2]) == "on");
        if (ds1994.setOscillator(on)) {
            cout << "\n✅ Oscilador " << (on ? "ENCENDIDO" : "APAGADO") << endl;
        }
    }
    else if (cmd == "test") {
        cout << "\n=== PRUEBA DE ESCRITURA ===" << endl;
        if (ds1994.testWrite()) {
            cout << "✅ PRUEBA EXITOSA: El dispositivo escribe correctamente" << endl;
        } else {
            cout << "❌ PRUEBA FALLIDA: Dispositivo protegido o error" << endl;
        }
    }
    else if (cmd == "wp") {
        uint8_t ctrl = ds1994.readControl();
        bool wp = ds1994.isWriteProtected();
        cout << "\nEstado de protección de escritura:" << endl;
        cout << "  RO  (Read Only - todo): " << ((ctrl & CR_RO)  ? "🔴 SÍ (permanente)" : "🟢 No") << endl;
        cout << "  WPR (RTC):               " << ((ctrl & CR_WPR) ? "🔴 SÍ (permanente)" : "🟢 No") << endl;
        cout << "  WPI (Interval Timer):    " << ((ctrl & CR_WPI) ? "🔴 SÍ (permanente)" : "🟢 No") << endl;
        cout << "  WPC (Cycle Counter):     " << ((ctrl & CR_WPC) ? "🔴 SÍ (permanente)" : "🟢 No") << endl;
        cout << "\n  Estado general: " << (wp ? "🔒 PROTEGIDO" : "🔓 DESBLOQUEADO") << endl;
    }
    else {
        printHelp(argv[0]);
    }

    return 0;
}
