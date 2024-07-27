Java Object Graph Size
======================

Uses [IterateOverObjectsReachableFromObject](https://docs.oracle.com/en/java/javase/21/docs/specs/jvmti.html#IterateOverObjectsReachableFromObject) to compute the size of an object and all the objects reachable from it.


> During the execution of this function the state of the heap does not change: no objects are allocated, no objects are garbage collected, and the state of objects (including held values) does not change. As a result, threads executing Java programming language code, threads attempting to resume the execution of Java programming language code, and threads attempting to execute JNI functions are typically stalled.

If JFR is present we additionally generate a JFR event allowing you to assess the overhead.

Usage
-----

The native agent has to be loaded using a JVM option.

```
-agentpath:/path/to/libobject-size-agent.so
```

Then it can be used from Java

```
ObjectSizeCalculator.getInstance().getObjectSize(myObject);
```
