<!-- README.md -->

<h1>Embedded Fingerprint Authentication System with Touch-Screen Interface</h1>

<p>
  This project implements an embedded fingerprint authentication system using an Arduino-compatible microcontroller, a fingerprint sensor, and a touch-screen display. The code is organized into modular components for easy maintenance and scalability.
</p>

<h2>Features</h2>
<ul>
  <li>Fingerprint enrollment, scanning, and deletion functionalities</li>
  <li>User interface with touch-screen support using the LVGL library</li>
  <li>Modular code structure separating hardware and UI components</li>
  <li>User data management with JSON storage on SPIFFS</li>
</ul>

<h2>Hardware Requirements</h2>
<ul>
  <li>Arduino-compatible microcontroller (e.g., ESP32)</li>
  <li>Fingerprint sensor module (e.g., Adafruit Fingerprint Sensor)</li>
  <li>TFT touch-screen display compatible with TFT_eSPI library</li>
  <li>SD card module (if required for additional storage)</li>
</ul>

<h2>Software Requirements</h2>
<ul>
  <li>Arduino IDE or compatible development environment</li>
  <li>Arduino libraries:
    <ul>
      <li><code>ArduinoJson</code></li>
      <li><code>Adafruit_Fingerprint</code></li>
      <li><code>TFT_eSPI</code></li>
      <li><code>LVGL</code></li>
      <li><code>SPIFFS</code></li>
    </ul>
  </li>
</ul>

<h2>Installation</h2>
<ol>
  <li>Clone this repository to your local machine.</li>
  <li>Install the required Arduino libraries via the Library Manager or manually.</li>
  <li>Configure the hardware connections as per the pin definitions in <code>hardware.h</code>.</li>
  <li>Open the project in the Arduino IDE.</li>
  <li>Upload the code to your microcontroller.</li>
</ol>

<h2>Usage</h2>
<p>
  Upon starting the system, you will be presented with a touch-screen menu with options to Enroll, Scan, Delete, or enter a Password. Use the touch interface to navigate and perform fingerprint operations.
</p>

<h2>Code Structure</h2>
<ul>
  <li><code>main.cpp</code>: Entry point of the program; initializes hardware and LVGL.</li>
  <li><code>hardware.h</code> / <code>hardware.cpp</code>: Contains hardware-related functions and definitions.</li>
  <li><code>ui.h</code> / <code>ui.cpp</code>: Contains user interface logic and event handlers.</li>
</ul>