*This project has been created as part of the 42 curriculum by kel-hadd.*

# Codexion

## Description

Codexion is a C concurrency simulation in which coders share a circular set
of USB dongles. A coder needs its two neighboring dongles at the same time to
compile, then debugs, refactors, and tries again.

A coder burns out if it does not start compiling before its deadline. The
simulation also stops when every coder has completed the required number of
compiles.

The allocator supports two policies:

- `fifo`: oldest request first.
- `edf`: earliest burnout deadline first, with FIFO order as the tie-breaker.

Released dongles also remain unavailable for `dongle_cooldown` milliseconds.

## Instructions

### Compilation

```sh
make
make clean
make fclean
make re
```

The project is compiled with:

```sh
cc -Wall -Wextra -Werror -pthread
```

### Running

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

Example:

```sh
./codexion 3 300 60 60 60 1 10 edf
```

All numeric arguments must contain only decimal digits. `number_of_coders`
must be at least 1, and `scheduler` must be exactly `fifo` or `edf`.

## Blocking cases handled

- **Deadlock prevention:** a coder never keeps one dongle while waiting for
  the other. The allocator grants both required dongles atomically while
  holding one allocator mutex. This removes the hold-and-wait condition.
- **Fair arbitration:** active requests are inserted into a binary heap.
  FIFO compares request sequence numbers; EDF compares deadlines and uses the
  sequence number as a deterministic tie-breaker.
- **Starvation prevention:** EDF always considers the earliest deadline first
  among requests that can currently obtain both required dongles.
- **Cooldown handling:** each dongle stores `free_at`. `LONG_MAX` means it is
  currently held; otherwise the value is the first millisecond at which it
  may be granted again.
- **Efficient waiting:** coders use a condition variable while a required
  dongle is held and `pthread_cond_timedwait` when only cooldown time remains.
  This avoids fixed-interval polling of the allocator.
- **Precise burnout detection:** a separate monitor checks coder deadlines
  every millisecond and stops the simulation when a deadline is reached.
- **Serialized output:** a logging mutex prevents two state messages from
  being mixed on the same line.
- **Single-coder case:** the only coder can take the only dongle but cannot
  obtain a second one, so the monitor eventually reports its burnout.

## Thread synchronization mechanisms

- **`alloc_lock`:** protects every dongle state, every pending request, the
  FIFO sequence counter, and heap scheduling operations. Because all resource
  check-and-grant operations happen under this mutex, two coders cannot be
  granted the same dongle.
- **`alloc_cond`:** sleeps coders while resources are unavailable. Releases,
  grants, and shutdown wake waiting threads. Cooldown expiry is handled with
  an absolute timed wait.
- **Per-coder mutex:** protects `deadline` and `compiles_done`, which are read
  by the monitor while coder threads update them.
- **`stop_lock`:** protects the shared simulation stop flag.
- **`log_lock`:** serializes terminal output.

The scheduling heap is allocated once during initialization and reused for
all arbitration passes, so scheduling does not allocate memory while threads
are running.

## Resources

- POSIX threads documentation: `pthread_create`, `pthread_join`,
  `pthread_mutex_*`, `pthread_cond_*`.
- `gettimeofday(2)` and `usleep(3)`.
- Binary heap / priority queue references.
- Earliest Deadline First scheduling references.
- Coffman's deadlock conditions and the dining philosophers problem.
- 42 Norm documentation.

### AI usage

AI was used as a support tool to explain concurrency concepts, review the
implementation, suggest small refactors, and design
additional tests.
an AI assistant was used solely to clarify theoretical
concepts — e.g. mutexes, dining-philosophers-style deadlock, and how POSIX
threads/condition variables work.

## Validation performed

The project was checked with `-Wall -Wextra -Werror -pthread`, repeated
FIFO/EDF runs, single-coder and invalid-input cases, cooldown scenarios,
completion and burnout scenarios, AddressSanitizer, UndefinedBehaviorSanitizer,
and ThreadSanitizer. A second `make` performs no unnecessary relink.
