package com.github.marschall.objectsize;

public interface ObjectSizeCalculator {

  long getObjectSize(Object o);

  static ObjectSizeCalculator getInstance() {
    return InstanceHolder.INSTANCE;
  }

}
