package com.github.marschall.objectsize;

import java.util.Objects;

import jdk.jfr.Category;
import jdk.jfr.Description;
import jdk.jfr.Event;
import jdk.jfr.Label;

/**
 * Adds a JFR event for {@link #getObjectSize(Object)}, helps to assess the cost.
 */
final class JfrObjectSizeCalculator implements ObjectSizeCalculator {

  private final ObjectSizeCalculator delegate;

  JfrObjectSizeCalculator(ObjectSizeCalculator delegate) {
    Objects.requireNonNull(delegate);
    this.delegate = delegate;
  }

  static boolean isSupported() {
    try {
      Class.forName("jdk.jfr.Event");
      return true;
    } catch (ClassNotFoundException e) {
      return false;
    }
  }

  @Override
  public long getObjectSize(Object o) {
    Event event = new GetObjectSizeEvent();
    event.begin();
    try {
      return delegate.getObjectSize(o);
    } finally {
      event.commit();
    }
  }

  @Label("IterateOverObjectsReachableFromObject")
  @Description("JVMTI IterateOverObjectsReachableFromObject was called.")
  @Category("JVMTI")
  static final class GetObjectSizeEvent extends Event {

  }

}
