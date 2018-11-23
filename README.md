# watchdog-example

This package shows a way of implementing a watchdog as a daemon process, that remains in touch with the main process, which is arming the watchdog in a regular way.  If the watchdog is not armed when the timer expires, a series of actions can be done.

This is a very simplistic framework, and for sure it will not be suitable for most of your needs.  Just a toy framework for myself.
