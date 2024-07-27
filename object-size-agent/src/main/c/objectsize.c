#include <jni.h>
#include <jvmti.h>
#include <stdlib.h>
#include <string.h>

#define _GNU_SOURCE
#include <stdio.h>

#include "com_github_marschall_objectsize_JvmtiObjectSizeCalculator.h"

typedef struct {
  jvmtiEnv *jvmti;
  jboolean supported;
} GlobalAgentData;

static GlobalAgentData *gdata;

jvmtiIterationControl JNICALL VisitObjectCallback(jvmtiObjectReferenceKind reference_kind, 
                                                  jlong class_tag, 
                                                  jlong size, 
                                                  jlong* tag_ptr, 
                                                  jlong referrer_tag, 
                                                  jint referrer_index, 
                                                  void* user_data) {
    jlong* current_size;
    switch (reference_kind) {
      case JVMTI_REFERENCE_CLASS:
      case JVMTI_REFERENCE_CLASS_LOADER:
      case JVMTI_REFERENCE_SIGNERS:
      case JVMTI_REFERENCE_PROTECTION_DOMAIN:
      case JVMTI_REFERENCE_INTERFACE:
      case JVMTI_REFERENCE_STATIC_FIELD:
      case JVMTI_REFERENCE_CONSTANT_POOL:
        return JVMTI_ITERATION_IGNORE;
      case JVMTI_REFERENCE_FIELD:
      case JVMTI_REFERENCE_ARRAY_ELEMENT:
        current_size = (jlong*) user_data;
        (*current_size) += size;
        return JVMTI_ITERATION_CONTINUE;
    }
    // unknown reference type
    return JVMTI_ITERATION_CONTINUE;
}

JNIEXPORT jboolean JNICALL Java_com_github_marschall_objectsize_JvmtiObjectSizeCalculator_isSupported0(JNIEnv *env, jclass clazz) {
  return gdata->supported;
}

jint jvmti_call_failed(JNIEnv *env, jvmtiError tiError, char* failedFunctionName, jclass exceptionClass) {
  char* message;
  int printCode;

  printCode = asprintf(&message, "%s failed with code: %d", failedFunctionName, tiError);
  if (printCode != 0) {
   // we could create an exception message
   return (*env)->ThrowNew(env, exceptionClass, message);
   free(message);
  } else {
   // we could not format an exception message
   return (*env)->ThrowNew(env, exceptionClass, "JVMTI call failed");
  }
}

JNIEXPORT jlong JNICALL Java_com_github_marschall_objectsize_JvmtiObjectSizeCalculator_getObjectSize0(JNIEnv *env, jclass clazz, jobject object, jclass exceptionClass) {
  jlong total_size = 0;
  jvmtiEnv *jvmti = gdata->jvmti;
  jvmtiError tiError;

  // initialize the the direct size
  tiError = (*jvmti)->GetObjectSize(jvmti, object, &total_size);
  if (tiError != JVMTI_ERROR_NONE) {
    jvmti_call_failed(env, tiError, "GetObjectSize", exceptionClass);
    return -1;
  }

  // add the sizes of the reachable objects
  tiError = (*jvmti)->IterateOverObjectsReachableFromObject(jvmti, object, &VisitObjectCallback, &total_size);
  if (tiError != JVMTI_ERROR_NONE) {
    jvmti_call_failed(env, tiError, "IterateOverObjectsReachableFromObject", exceptionClass);
    return -1;
  }
  return total_size;
}

int enable_tag_objects(jvmtiEnv* jvmti, jboolean* supported) {
  jvmtiCapabilities capabilities;
  jvmtiError tiError;

  // check if can_tag_objects is already active
  memset(&capabilities, 0, sizeof(capabilities));
  tiError = (*jvmti)->GetCapabilities(jvmti, &capabilities);
  if (tiError != JVMTI_ERROR_NONE) {
    fprintf(stderr, "GetCapabilities (JVMTI) failed with error(%d)\n", tiError);
    (*supported) = JNI_FALSE;
    return tiError;
  }
  if (capabilities.can_tag_objects == 1) {
    (*supported) = JNI_TRUE;
    return 0;
  }

  // can_tag_objects is not active 
  // check if can_tag_objects is supported
  memset(&capabilities, 0, sizeof(capabilities));
  tiError = (*jvmti)->GetPotentialCapabilities(jvmti, &capabilities);
  if (tiError != JVMTI_ERROR_NONE) {
    fprintf(stderr, "GetPotentialCapabilities (JVMTI) failed with error(%d)\n", tiError);
    (*supported) = JNI_FALSE;
    return tiError;
  }
  if (capabilities.can_tag_objects == 0) {
    // can_tag_objects is not supported
    (*supported) = JNI_FALSE;
    return 0;
  }

  // can_tag_objects is not active but supported
  // enable can_tag_object
  memset(&capabilities, 0, sizeof(capabilities));
  capabilities.can_tag_objects = 1;
  tiError = (*jvmti)->AddCapabilities(jvmti, &capabilities);
  if (tiError != JVMTI_ERROR_NONE) {
    fprintf(stderr, "AddCapabilities (JVMTI) failed with error(%d)\n", tiError);
    (*supported) = JNI_FALSE;
    return tiError;
  }
  // successfully added capability
  (*supported) = JNI_TRUE;
  return 0;
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *vm, char *options, void *reserved) {
  jvmtiEnv* jvmti;
  jint niError;
  jboolean supported;
  int returnValue;

  niError = (*vm)->GetEnv(vm, (void**)&jvmti, JVMTI_VERSION_1_0);
  if (niError != JNI_OK) {
    fprintf(stderr, "GetEnv (JVMTI) failed with error(%d)\n", niError);
    return niError;
  }

  supported = JNI_FALSE;
  returnValue = enable_tag_objects(jvmti, &supported);
  gdata = (GlobalAgentData*) malloc(sizeof(GlobalAgentData));

  gdata->jvmti = jvmti;
  gdata->supported = supported;

  return returnValue;
}
