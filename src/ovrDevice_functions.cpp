/*
 *  Copyright 2010-2014 Fabric Software Inc. All rights reserved.
 */

#include <FabricEDK.h>
#include "ovrDevice_functions.h"
#include <OVR.h>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

 using namespace Fabric::EDK;

typedef boost::shared_mutex Lock;
typedef boost::unique_lock< Lock >  WriteLock;
typedef boost::shared_lock< Lock >  ReadLock;
Lock gLock;

unsigned int numOvrDevices = 0;

void ContextCallback(bool opening, void const *contextPtr)
{
  if ( !opening )
  {
    WriteLock w_lock(gLock);
    if(numOvrDevices > 0)
    {
      ovr_Shutdown();
      report("Oculus Rift finalized.");
    }
    numOvrDevices = 0;
  }
}
IMPLEMENT_FABRIC_EDK_ENTRIES_WITH_CONTEXT_CALLBACK( Oculus, &ContextCallback )

// Defined at src\ovrDevice.kl:21:1
FABRIC_EXT_EXPORT void ovrDevice_Construct(
  KL::Traits< KL::ovrDevice >::IOParam this_,
  KL::Traits< KL::UInt32 >::INParam index
) {

  WriteLock w_lock(gLock);

  if(numOvrDevices == 0)
  {
    ovr_Initialize();
    report("Oculus Rift initialized.");
  }
  numOvrDevices++;

  this_->handle = (void*)ovrHmd_Create(index);
  if(!this_->handle)
  {
    report("Using debug device of type ovrHmd_DK2.");
    this_->handle = (void*)ovrHmd_CreateDebug(ovrHmd_DK2);
  }
}

// Defined at src\ovrDevice.kl:28:1
FABRIC_EXT_EXPORT void ovrDevice_Destruct(
  KL::Traits< KL::ovrDevice >::IOParam this_
) {

  WriteLock w_lock(gLock);

  if(this_->handle)
    ovrHmd_Destroy((ovrHmd)this_->handle);

  if(numOvrDevices == 1)
  {
    ovr_Shutdown();
    report("Oculus Rift finalized.");
  }
  numOvrDevices--;
}

// Defined at src\ovrDevice.kl:30:1
FABRIC_EXT_EXPORT KL::UInt32 ovrDevice_NumDevices(
  KL::Traits< KL::ovrDevice >::INParam this_
) {
  return ovrHmd_Detect();
}

// Defined at src\ovrDevice.kl:32:1
FABRIC_EXT_EXPORT void ovrDevice_GetDescription(
  KL::Traits< KL::ovrDescription >::Result _result,
  KL::Traits< KL::ovrDevice >::INParam this_
) {

  if(!this_->handle)
    return;
  ovrHmd hmd = (ovrHmd)this_->handle;

  switch(hmd->Type)
  {
    case ovrHmd_DK1:
    {
      _result.ORType = "DK1";
      break;
    }
    case ovrHmd_DKHD:
    {
      _result.ORType = "DKHD";
      break;
    }
    case ovrHmd_DK2:
    {
      _result.ORType = "DK2";
      break;
    }
  }

  _result.ProductName = hmd->ProductName;  
  _result.Manufacturer = hmd->Manufacturer;  
  _result.VendorId = hmd->VendorId;
  _result.ProductId = hmd->ProductId;
  _result.SerialNumber = hmd->SerialNumber;
  _result.FirmwareMajor = hmd->FirmwareMajor;
  _result.FirmwareMinor = hmd->FirmwareMinor;
  _result.CameraFrustumHFovInRadians = hmd->CameraFrustumHFovInRadians;
  _result.CameraFrustumVFovInRadians = hmd->CameraFrustumVFovInRadians;
  _result.CameraFrustumNearZInMeters = hmd->CameraFrustumNearZInMeters;
  _result.CameraFrustumFarZInMeters = hmd->CameraFrustumFarZInMeters;
  _result.HmdCaps = hmd->HmdCaps;
  _result.TrackingCaps = hmd->TrackingCaps;
  _result.DistortionCaps = hmd->DistortionCaps;
  _result.Resolution[0] = hmd->Resolution.w;
  _result.Resolution[1] = hmd->Resolution.h;
  _result.WindowsPos[0] = hmd->WindowsPos.x;
  _result.WindowsPos[1] = hmd->WindowsPos.y;
  _result.DefaultEyeFov[0].x = hmd->DefaultEyeFov[0].UpTan;
  _result.DefaultEyeFov[0].y = hmd->DefaultEyeFov[0].DownTan;
  _result.DefaultEyeFov[0].z = hmd->DefaultEyeFov[0].LeftTan;
  _result.DefaultEyeFov[0].t = hmd->DefaultEyeFov[0].RightTan;
  _result.DefaultEyeFov[1].x = hmd->DefaultEyeFov[1].UpTan;
  _result.DefaultEyeFov[1].y = hmd->DefaultEyeFov[1].DownTan;
  _result.DefaultEyeFov[1].z = hmd->DefaultEyeFov[1].LeftTan;
  _result.DefaultEyeFov[1].t = hmd->DefaultEyeFov[1].RightTan;
  _result.MaxEyeFov[0].x = hmd->MaxEyeFov[0].UpTan;
  _result.MaxEyeFov[0].y = hmd->MaxEyeFov[0].DownTan;
  _result.MaxEyeFov[0].z = hmd->MaxEyeFov[0].LeftTan;
  _result.MaxEyeFov[0].t = hmd->MaxEyeFov[0].RightTan;
  _result.MaxEyeFov[1].x = hmd->MaxEyeFov[1].UpTan;
  _result.MaxEyeFov[1].y = hmd->MaxEyeFov[1].DownTan;
  _result.MaxEyeFov[1].z = hmd->MaxEyeFov[1].LeftTan;
  _result.MaxEyeFov[1].t = hmd->MaxEyeFov[1].RightTan;
  _result.EyeRenderOrder[0] = hmd->EyeRenderOrder[0];
  _result.EyeRenderOrder[1] = hmd->EyeRenderOrder[1];
  _result.DisplayDeviceName = hmd->DisplayDeviceName;
  _result.DisplayId = hmd->DisplayId;
}

// Defined at src\ovrDevice.kl:61:1
FABRIC_EXT_EXPORT KL::Boolean ovrDevice_ConfigureTracking(
  KL::Traits< KL::ovrDevice >::IOParam this_,
  KL::Traits< KL::SInt32 >::INParam supportedTrackingCaps,
  KL::Traits< KL::SInt32 >::INParam requiredTrackingCaps
) {

  if(this_->handle)
  {
    ovrHmd hmd = (ovrHmd)this_->handle;
    if(ovrHmd_ConfigureTracking(hmd, supportedTrackingCaps, requiredTrackingCaps))
    {
      report("Oculus Rift configured for tracking.");
    }
  }
  return false;
}
