# Field Intelligence Collector

A Portable Device for Identifying and Analyzing IoT Devices via Network Patterns.

*Demo video coming soon*

---

## Project Overview

The Field Intelligence Collector (FIC) is a self-contained, Raspberry Pi CM4-based platform designed for security researchers operating in controlled testing environments. Its primary function is to perform low-level packet capture to identify and log network patterns emitted by nearby IoT devices.

This device allows researchers to gather crucial data on how specific devices communicate, providing the foundation for security analysis and fingerprinting.

---

## Core Architecture and Status

The FIC is built as a modular system across three distinct compontents

| Component | Technology | Primary Role | Status |
|-----------|------------|--------------|--------|
| **Collector** | C++ (libpcap, sqlite3) | Low-level packet capture, filtering, and writing patterns to the database. | Fully Functional |
| **Frontend** | Angular | Provides the web-based user interface and data visualization. | Complete (Awaiting Backend Integration) |
| **Backend** | In Development | Handles data processing, analytics, and serves the database content to the frontend | In Progress |

## Key Features

* **Low-Level Capture:** Utilizes an external Alfa AC1900 WiFi adapter (or compatible) for high-performance, raw packet capture.
* **Persistent Storage:** All data is logged directly to a local SQLite database.
* **Portable Interface:** The Angular interface is served via an onboard WiFi Access Point on the CM4.

## Planned Enhancements

* Integration of GPS data to log collection location
* Implementation of the Backend for on-device analytics
* Adding richer metadata fields to capture logs

---

## Getting Started

To get the FIC running, you'll need the hardware and the compiled C++ application

## Prerequisites

* **Hardware:** A Raspberry Pi CM4 (or any supported embedded Linux board) and an external Wi-Fi Adapter capable of monitor mode (e.g., Alfa AC1900).
* **Libraries:** libpcap and sqlite3 must be installed on your operating system.

## Installation and Build

*To be completed*
