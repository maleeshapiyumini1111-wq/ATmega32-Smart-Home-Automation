# ATmega32-Smart-Home-Automation
Smart Home Automation and Safety System using ATmega32 and Embedded C
# ATmega32 Smart Home Automation & Core Safety System

An industrial-grade Smart Home Automation and Safety response network designed around the **ATmega32 (8-bit AVR architecture)**. This system implements an asynchronous multi-sensor fusion matrix coupled with a strict conditional priority execution hierarchy written in **Bare-Metal Embedded C**.

## 🛠️ Technical Specifications & Architecture

- **Microcontroller:** ATmega32 (Clock Speed: 1MHz)
- **Firmware Environment:** Microchip Studio (Embedded C)
- **Simulation Platform:** Labcenter Proteus 8.13
- **Display Interfacing:** 16x2 LCD customized via a 4-bit parallel data masking layer to optimize GPIO overhead.

---

## 🧠 System Execution & Priority Hierarchy

The core loop processes multiple analog and digital peripherals concurrently, handling critical safety overrides according to the following deterministic priority matrix:

1. **🥇 Micro-Climate Control (Temp > 35°C):** Monitored via LM35. Instantly energizes a dedicated Room Cooling Fan Relay (`PB4`) smoothly without triggering safety alarms.
2. **🥈 Motion Intrusion:** Triggered via PIR Sensor (`PD0`). Instantly fires the Master Alarm Buzzer (`PB0`).
3. **🥉 Structural Breach (Door Open):** Triggered via Door Limit Switch (`PD1`). Fires the Master Alarm Buzzer (`PB0`).
4. **🏅 Perimeter Interruption (Beam Broken):** Triggered via Laser Photodiode Network (`PD2`). Fires the Master Alarm Buzzer (`PB0`).
5. **🔥 High-Hazard Containment (Gas Leak / Fire):** Monitored via MQ2/Fire Switch (`PD3`). Activates both the Master Alarm (`PB0`) and an isolated electromechanical High-Velocity Smoke Exhaust Fan (`PB1`) simultaneously.
6. **💧 Utility Automation (Water Low < 30%):** Monitored via Water Level Core. Engages the Water Pump Relay (`PB2`). Yields instantly if a higher-priority threat emerges.
7. **🟢 Standby State (System Secure):** Executes when all vectors are nominal. Runs standard telemetry diagnostics on the LCD and illuminates the Safe Green Status LED (`PB3`).

### ☀️ Background Automation (Independent Thread)
- **Smart Night-Light (`PB5`):** Tracks ambient luminosity via an LDR photocell. If light density drops below **30%**, it autonomously toggles the night-light LED using bitwise masking, keeping it fully isolated from the security loop states.

---

## ⚙️ Hardware Peripheral Mapping (PORT B)

| Pin | Device / Actuator | Function |
|---|---|---|
| `PB0` | Master Alarm Buzzer | Intruder & Critical Fire Audio Alert |
| `PB1` | High-Velocity Exhaust Fan | Emergency Gas/Smoke Ventilation |
| `PB2` | Water Pump Relay | Automated Water Tank Replenishment |
| `PB3` | Safe Status LED (Green) | System Standby Nominal Indicator |
| `PB4` | Room Cooling Fan Relay | Temperature Override Dissipation |
| `PB5` | Smart Night-Light LED | Ambient Darkness Automation |

---

## 🚀 Key Firmware Engineering Highlights
- **Bitwise Registers Manipulation:** Developed purely on register configurations without heavy abstract wrappers (e.g., Arduino API) to minimize compiled bin footprint.
- **Software State Tracking:** Engineered a logic gate (`last_state` debouncing) ensuring LCD commands execute only during state transitions, eliminating redundant processing cycles and screen flickering.
- **Hardware Isolation Design:** Implemented relay logic frameworks to completely isolate the $5\text{V}$ logic processing core from high-current mechanical inductive loads.
