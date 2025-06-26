# Watchdog

**Watchdog** is a software watchdog designed to monitor and automatically revive a user application. It manages a companion process (`watch_dog.out`) and a worker thread that communicate using signals. If one side becomes unresponsive, the other revives it to ensure continued operation.

> **Developed during the Infinity Labs R&D Software Development Program.**

---

## Motivation

This project was created to ensure the stability and continuity of user applications during critical sections of execution.

---

## Introduction

A watchdog ensures that a program stays up and running. There are two types of watchdogs:

- **Software Watchdog** — implemented at the software level (this project).
- **Hardware Watchdog** — implemented at the hardware level.

---

## Prerequisites

- Linux OS
- `cmake` installed

---

## API

### `StartWD`

```c
wd_status_t StartWD(size_t threshold, size_t interval, int argc, char** argv);
```

**Description:**\
Initializes the watchdog mechanism.

**Parameters:**

- `threshold` — Number of consecutive missed signals allowed before revival starts.
- `interval` — Interval (ms) between signals sent between the two processes.
- `argc` — Argument count for the watchdog process.
- `argv` — Argument list for the watchdog process.

**Returns:**

- WD_SUCCESS on success
- WD_FAILURE on failure

---

### `StopWD`

```c
void StopWD(void);
```

**Description:**\
Stops the watchdog cleanly, sends termination signals to both processes, and frees resources.

---

## Setup & Usage

### Build Instructions

```bash
git clone https://github.com/Ofirw12/Watchdog.git
cd Watchdog
mkdir build
cd build
cmake ..
make
```

> The compiled executables will appear in the `build/` directory.

---

### Running the Program

Basic usage:

```bash
./user_app ./watch_dog.out
```

With additional arguments for the user app:

```bash
./user_app ./watch_dog.out <arg1> <arg2> ...
```

---

## How It Works

1. The user app starts the watchdog using `StartWD()`, specifying signal interval, threshold, and args.
2. The app forks and runs `watch_dog.out`, while also starting a worker thread.
3. Both processes send `SIGUSR1` signals to each other at the specified interval.
4. If a process misses `threshold` signals, the other process revives it:
    - The user app restarts the watchdog.
    - The watchdog takes over and restarts the user app.
5. Calling `StopWD()` shuts down both processes and cleans up.

---

## Main Components

- **priority\_queue**\
  A heap-based priority queue that schedules tasks by urgency.

- **scheduler**\
  A task scheduler built around the priority queue. It manages timed execution of tasks and ensures signal exchanges occur at the correct intervals.

- **task**\
  Represents individual units of work (e.g., sending a signal) that are scheduled by the scheduler.

- **uid (unique identifier)**\
  Generates unique IDs for tasks to ensure safe management in a multithreaded environment.

- **watchdog (user API)**\
  Provides the public API (`StartWD`, `StopWD`). Handles creation of the worker thread and forking of the watchdog process.

- **inner\_watchdog**\
  Implements the communication logic using signals (`SIGUSR1`, `SIGUSR2`). Handles signal handlers and revival mechanisms.

- **signal handlers**\
  Functions that respond to signals, manage process health, and trigger revival when necessary.

---

