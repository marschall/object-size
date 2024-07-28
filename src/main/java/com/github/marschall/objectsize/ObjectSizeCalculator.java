package com.github.marschall.objectsize;

/**
 * Functions to get object sizes.
 */
public interface ObjectSizeCalculator {

  /**
   * Computes the size of an object and all objects reachable from it.
   * 
   * @param o the object to start the size calclation
   * @return the size of {@code o} and all objects reachable from it in bytes,
   *         {@code 0} if {@code o} is {@code null}
   */
  long getObjectSize(Object o);

  /**
   * Returns the sole instance of {@link ObjectSizeCalculator}.
   * <p>
   * If computing object sizes is not suported an instance that always throws {@link UnsupportedOperationException}
   * is returned.
   * 
   * @return the sole instance of {@link ObjectSizeCalculator}, never {@code null}
   */
  static ObjectSizeCalculator getInstance() {
    return InstanceHolder.INSTANCE;
  }

}
