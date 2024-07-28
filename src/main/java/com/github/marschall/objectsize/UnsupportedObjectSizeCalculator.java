package com.github.marschall.objectsize;

final class UnsupportedObjectSizeCalculator implements ObjectSizeCalculator {

  @Override
  public long getObjectSize(Object o) {
    throw new UnsupportedOperationException();
  }

}
