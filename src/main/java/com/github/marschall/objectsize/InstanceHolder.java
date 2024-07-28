package com.github.marschall.objectsize;

final class InstanceHolder {

  static final ObjectSizeCalculator INSTANCE = getInstance();

  private static ObjectSizeCalculator getInstance() {
    try {
      if (!JvmtiObjectSizeCalculator.isSupported()) {
        return new UnsupportedObjectSizeCalculator();
      }
    } catch (LinkageError e) {
      return new UnsupportedObjectSizeCalculator();
    }
    ObjectSizeCalculator calculator = new JvmtiObjectSizeCalculator();
    if (JfrObjectSizeCalculator.isSupported()) {
      calculator = new JfrObjectSizeCalculator(calculator);
    }
    return calculator;
  }

}