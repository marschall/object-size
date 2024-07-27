package com.github.marschall.objectsize;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Collections;

import org.junit.jupiter.api.Test;

class ObjectSizeCalculatorTests {

  private ObjectSizeCalculator calculator;

  void setUp() {
    this.calculator = ObjectSizeCalculator.getInstance();
  }

  @Test
  void testNull() {
    assertEquals(0L, this.calculator.getObjectSize(null));
  }

  @Test
  void testObject() {
    assertTrue(this.calculator.getObjectSize(new Object()) > 0L);
  }

  @Test
  void testClass() {
    assertTrue(this.calculator.getObjectSize(Object.class) > 0L);
  }

  @Test
  void testArray() {
    assertTrue(this.calculator.getObjectSize(new Object[] {1}) > 0L);
  }

  @Test
  void testCollection() {
    assertTrue(this.calculator.getObjectSize(Collections.singletonList(1)) > 0L);
  }

}
