package com.github.marschall.objectsize;

final class JvmtiObjectSizeCalculator implements ObjectSizeCalculator {

  @Override
  public long getObjectSize(Object o) {
    if (o == null) {
      return 0L;
    }
    long size = getObjectSize0(o, RuntimeException.class);
    if (size == -1L) {
      // we should not reach here, JNI code should have thrown an exception
      throw new RuntimeException("calculation failed");
    }
    return size;
  }

  private static native long getObjectSize0(Object o, Class<? extends RuntimeException> exceptionClass);

  static boolean isSupported() {
    return isSupported0();
  }

  private static native boolean isSupported0();

}
