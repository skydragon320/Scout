# Scout — Autonomous Hexapod Research Platform

**Scout** is an open source six-legged robot designed as a research platform for autonomous navigation in GPS-denied environments. Built on an ESP32 microcontroller with 18 MG996R servos across 6 fully articulated 3DOF legs, Scout is designed from the ground up to be modular, extensible, and field deployable in harsh conditions.

> 🔗 [github.com/skydragon320/Scout](https://github.com/skydragon320/Scout)

---

## The Problem

Modern autonomous robots rely heavily on GPS for navigation and positioning. In environments where GPS is unavailable or unreliable — underground, in dense urban canyons, in space, or at high latitudes like interior Alaska — most autonomous systems fail entirely.

Legged robots offer a unique advantage in these environments: they can traverse terrain that wheeled or tracked platforms cannot, making them ideal candidates for search and rescue, planetary exploration, and remote sensing in challenging conditions. However, most capable legged platforms are expensive, closed source, and not designed for GPS-denied autonomy.

Scout addresses this gap.

---

## Vision

Scout is designed around a hierarchical intelligence architecture:

- **Local reasoning** — Scout runs a Hierarchical Reasoning Model (HRM) onboard for real-time decision making without requiring a network connection
- **Mesh networking** — multiple Scout units communicate peer-to-peer, sharing map data and sensor readings to build a collective picture of their environment
- **Queen AI** — a base station running high-level mission planning, receiving telemetry from all scouts in the field
- **Modular sensor heads** — hot-swappable sensor payloads allow Scout to be configured for different mission profiles without rebuilding the platform

The long term goal is a swarm of Scout units capable of autonomous area mapping, search operations, and reconnaissance in environments where GPS and human operators are unavailable.

---

## Current Status

Scout mk1 is an active development platform. Current capabilities:

- 18 MG996R servos across 6 legs, 3DOF per leg
- Dual PCA9685 PWM driver boards over I2C
- ESP32 WiFi access point with AsyncWebServer
- WebSocket based web controller, accessible from any browser
- Smooth cosine interpolated motion
- Standup sequence, wave, stomp, and pseudo-walk routines
- 3DOF inverse kinematics (law of cosines)
- 7.4V 5000mAh LiPo with DROK 12A buck converter for servo power

---

## Hardware

| Component | Details |
|---|---|
| Microcontroller | ESP32-WROOM-32 |
| Servo drivers | 2x PCA9685 (0x40, 0x41) |
| Servos | 18x MG996R |
| Power | Spektrum 2S 5000mAh LiPo |
| Buck converter | DROK REES-1822 12A |
| Communication | WiFi (ESP32 SoftAP) |

---

## Software Architecture

```
Scout/
├── Scout.ino       — main sketch, motion functions
├── controller.h    — web controller declarations  
├── controller.cpp  — WiFi, WebSocket, web UI
└── leg.h           — Leg class, IK math, constants
```

The controller and motion systems are intentionally separated so each can be developed and tested independently before integration.

---

## Roadmap

**Near term**
- [ ] Full tripod gait
- [ ] IK integrated with gait
- [ ] Body translation and rotation
- [ ] ESP32-CAM head with live video stream

**Summer**
- [ ] Bezier curve swing phase
- [ ] Camera tracking algorithm
- [ ] Telemetry dashboard
- [ ] Custom PCB carrier board

**Research**
- [ ] HRM local reasoning implementation
- [ ] Mesh network between Scout units
- [ ] GPS-denied navigation via leg odometry and IMU fusion
- [ ] Planetary surface analog testing (Alaska terrain)

---

## License

- Firmware: [LGPL v3](https://www.gnu.org/licenses/lgpl-3.0.en.html)
- Hardware (PCB, STL): [CERN OHL-W v2](https://ohwr.org/cern_ohl_w_v2.txt)
- Documentation: [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/)

Improvements to Scout must be open sourced and contributed back. Projects built using Scout do not need to be open source.

---

## Contributing

Scout is an open platform and contributions are welcome. If you build on Scout, improve it, or deploy it — open a PR or reach out. The goal is a platform that grows with the community.

---

*Built in Fairbanks, Alaska. Designed for everywhere.*
