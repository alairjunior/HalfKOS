# HalfKOS: A Minimal Preemptive OS for Ultra-Constrained Microcontrollers

Welcome to **HalfKOS**, an embedded operating system designed specifically for microcontrollers with as little as **512 bytes of RAM**. Offering essential multitasking capabilities with an ultra-low memory footprint, HalfKOS is an ideal solution for the most resource-constrained devices.

---

## Why Choose HalfKOS?

### 🌟 **Optimized for Tiny MCUs**

HalfKOS is built to run on microcontrollers like the **MSP430G2553**, where resources are limited. Unlike traditional RTOS solutions that may be too heavy for such devices, HalfKOS keeps things minimal and efficient.

### 🚀 **Effortless Multitasking**

No more manual state machines or complex interrupt handling. With **HalfKOS**, you can easily run multiple tasks at the same time, including:
- **Sensor polling**
- **LED control**
- **Communication over UART**

HalfKOS provides a preemptive scheduler that manages concurrent tasks, making multitasking simple and reliable.

### 💡 **Extremely Low Resource Consumption**

Designed to work within tight memory constraints, HalfKOS has minimal overhead, allowing your application code to take up the majority of available resources. It’s perfect for projects where **every byte counts**.

### 🎓 **Great for Learning and Experimenting**

HalfKOS is not just for developers building real-world applications — it’s also an excellent platform for:
- **Learning OS concepts** like task scheduling and context switching.
- Experimenting with **multitasking** on constrained MCUs.
- Gaining experience in **low-level embedded systems development**.

### 🔄 **Portability Across MCUs**

Currently, HalfKOS supports the **MSP430G2553**, but it’s designed to be easily ported to other microcontroller families. The **roadmap** includes:
- **AVR** (e.g., ATmega328)
- **PIC**
- **8051**

This allows you to use HalfKOS across a wide variety of projects.

---

## Key Features

- **Preemptive Multitasking**: Simple, efficient multitasking with minimal overhead.
- **Minimal RAM Footprint**: Optimized for MCUs with just 512 bytes of RAM.
- **Portable Architecture**: Easily ported to different microcontroller platforms.
- **Clean and Simple Codebase**: Designed for simplicity and readability.
- **Ideal for Learning**: A great tool for understanding embedded operating system concepts.

---

## Why Choose HalfKOS Over Bare-Metal?

While bare-metal programming is often used for ultra-constrained MCUs, there are several advantages to using HalfKOS:

### ⚡ **Simplified Task Management**
Managing multiple tasks in bare-metal programming can be complex and error-prone. HalfKOS abstracts task scheduling, allowing you to focus on your application's logic.

### 🔄 **Easier Code Maintenance**
With tasks isolated in separate modules, your code becomes more modular, easier to debug, and more scalable. This makes it simpler to manage as your project grows in complexity.

###🕹️ Task Scheduling Without Overhead
While not a true RTOS, HalfKOS allows you to manage concurrent tasks without the complexity of managing timers and interrupts manually. It's a lightweight solution for applications requiring multitasking.

---

## Roadmap

HalfKOS is still in its early stages, but here's what you can expect in the near future:
- **Additional MCU Ports**:
  - AVR (e.g., ATmega328)
  - PIC
  - 8051
- **Expanded Documentation**: More detailed guides to help you get started.
- **Community Contributions**: We welcome feedback and contributions from the embedded systems community.

---

## Who Should Use HalfKOS?

HalfKOS is ideal for:
- **Embedded developers** working with ultra-low-resource microcontrollers.
- **Hobbyists** who need simple multitasking on small devices.
- **Educators and students** looking to explore embedded OS concepts.

---

## Contributing

We welcome contributions! If you'd like to see HalfKOS support more MCUs, or if you find any bugs or have suggestions, feel free to contribute:
- **Fork the repository** and submit pull requests.
- **Open issues** to report bugs or request new features.

Let’s work together to improve HalfKOS and make it even better.

---

**HalfKOS**: Enabling multitasking even in the tightest memory constraints.
