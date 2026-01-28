# ESP32 mDNS Device Discovery (Same Wi-Fi Network)

This project demonstrates **automatic device discovery** on a local Wi-Fi network using **mDNS (Multicast DNS)**.

An **ESP32** advertises itself as a discoverable service, and a **Python client** finds it without using IP addresses, DNS servers, or router configuration.

This approach works fully **offline** and is ideal for:
- ESP32 ↔ Desktop (Linux / GNOME)
- ESP32 ↔ Android
- Local device control and monitoring

---

## 1. What This Project Does

- ESP32 connects to a Wi-Fi router
- ESP32 starts an HTTP server on port `9078`
- ESP32 advertises an mDNS service: `_myapp._tcp.local`
- Python script searches the LAN for `_myapp._tcp.local`
- Python lists discovered devices and calls `/info`

✔ No manual IP entry  
✔ No backend server  
✔ No router or DNS configuration  

---

## 2. Network Requirement

- **ESP32 and PC must be on the same Wi-Fi network**
- mDNS uses multicast and does not cross routers

---

## 3. Requirements

### ESP32 Side
- ESP32 board
- Arduino IDE
- ESP32 Arduino Core installed
- Libraries (included in ESP32 core):
  - `WiFi`
  - `ESPmDNS`
  - `WebServer`

### PC / Ubuntu Side
- Python 3.8+
- Same Wi-Fi network as ESP32

Install Python dependencies:
```bash
pip install zeroconf requests
