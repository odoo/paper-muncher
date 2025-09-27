================
Functional Usage
================
Paper Muncher includes both synchronous and asynchronous functional APIs.

.. literalinclude:: ../examples/sync_example.py
    :language: python
    :linenos:
    :lines: 1-13
    :caption: Synchronous example
    :name: sync_example

**N.B.** The synchronous API is based on a per-OS integration for IO timeouts.

#. For POSIX systems, it relies on `selectors`.
#. For Windows with Python 3.12+, it puts the file in non-blocking mode.
#. For Windows with Python < 3.12, it falls back to a potentially blocking read without timeout.

.. literalinclude:: ../examples/async_example.py
    :language: python
    :linenos:
    :lines: 1-13
    :caption: Asynchronous example
    :name: async_example

In addition to that it also includes a context based approach to automatically
handle synchronous and asynchronous code execution.

.. literalinclude:: ../examples/auto_mode_example.py
    :language: python
    :linenos:
    :lines:  1-22
    :caption: Auto Switching example
    :name: auto_example


=====================
Context Manager Usage
=====================
Paper Muncher includes both synchronous and asynchronous context manager APIs.

.. literalinclude:: ../examples/sync_cm_example.py
    :language: python
    :linenos:
    :lines: 1-14
    :caption: Synchronous Context Manager example
    :name: sync_cm_example

**N.B.** The synchronous API is based on a per-OS integration for IO timeouts.

#. For POSIX systems, it relies on `selectors`.
#. For Windows with Python 3.12+, it puts the file in non-blocking mode.
#. For Windows with Python < 3.12, it falls back to a potentially blocking read without timeout.

.. literalinclude:: ../examples/async_cm_example.py
    :language: python
    :linenos:
    :lines: 1-15
    :caption: Asynchronous Context Manager example
    :name: async_cm_example

In addition to that it also includes a context based approach to automatically
handle synchronous and asynchronous code execution.

.. literalinclude:: ../examples/auto_mode_cm_example.py
    :language: python
    :linenos:
    :lines: 1-26
    :caption: Auto Switching example
    :name: auto_cm_example
