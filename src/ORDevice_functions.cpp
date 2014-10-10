/*
 *  Copyright 2010-2014 Fabric Software Inc. All rights reserved.
 */

#include <FabricEDK.h>
#include "ORDevice_functions.h"
#include <OVR.h>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

typedef boost::shared_mutex Lock;
typedef boost::unique_lock< Lock >  WriteLock;
typedef boost::shared_lock< Lock >  ReadLock;
Lock gLock;

unsigned int nORDevices = 0;

void ContextCallback(bool opening, void const *contextPtr)
{
  if ( !opening )
  {
    WriteLock w_lock(gLock);
    if(nORDevices > 0)
      ovr_Shutdown();
    nORDevices = 0;
  }
}
IMPLEMENT_FABRIC_EDK_ENTRIES_WITH_CONTEXT_CALLBACK( Oculus, &ContextCallback )

// Defined at src\ORDevice.kl:12:1
FABRIC_EXT_EXPORT void ORDevice_Construct(
  Fabric::EDK::KL::Traits< Fabric::EDK::KL::ORDevice >::IOParam this_
) {

  WriteLock w_lock(gLock);

  if(nORDevices == 0)
    ovr_Initialize();
  nORDevices++;

}

// Defined at src\ORDevice.kl:13:1
FABRIC_EXT_EXPORT void ORDevice_Destruct(
  Fabric::EDK::KL::Traits< Fabric::EDK::KL::ORDevice >::IOParam this_
) {

  WriteLock w_lock(gLock);

  if(nORDevices == 1)
    ovr_Shutdown();
  nORDevices--;
}
