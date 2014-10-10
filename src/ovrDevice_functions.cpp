/*
 *  Copyright 2010-2014 Fabric Software Inc. All rights reserved.
 */

#include <FabricEDK.h>
#include "ovrDevice_functions.h"
#include <OVR.h>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include "conversion.h"

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
  this_->configured = false;
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
  _result.Resolution.w = hmd->Resolution.w;
  _result.Resolution.h = hmd->Resolution.h;
  _result.WindowsPos.x = hmd->WindowsPos.x;
  _result.WindowsPos.y = hmd->WindowsPos.y;
  _result.DefaultEyeFov[0].UpTan = hmd->DefaultEyeFov[0].UpTan;
  _result.DefaultEyeFov[0].DownTan = hmd->DefaultEyeFov[0].DownTan;
  _result.DefaultEyeFov[0].LeftTan = hmd->DefaultEyeFov[0].LeftTan;
  _result.DefaultEyeFov[0].RightTan = hmd->DefaultEyeFov[0].RightTan;
  _result.DefaultEyeFov[1].UpTan = hmd->DefaultEyeFov[1].UpTan;
  _result.DefaultEyeFov[1].DownTan = hmd->DefaultEyeFov[1].DownTan;
  _result.DefaultEyeFov[1].LeftTan = hmd->DefaultEyeFov[1].LeftTan;
  _result.DefaultEyeFov[1].RightTan = hmd->DefaultEyeFov[1].RightTan;
  _result.MaxEyeFov[0].UpTan = hmd->MaxEyeFov[0].UpTan;
  _result.MaxEyeFov[0].DownTan = hmd->MaxEyeFov[0].DownTan;
  _result.MaxEyeFov[0].LeftTan = hmd->MaxEyeFov[0].LeftTan;
  _result.MaxEyeFov[0].RightTan = hmd->MaxEyeFov[0].RightTan;
  _result.MaxEyeFov[1].UpTan = hmd->MaxEyeFov[1].UpTan;
  _result.MaxEyeFov[1].DownTan = hmd->MaxEyeFov[1].DownTan;
  _result.MaxEyeFov[1].LeftTan = hmd->MaxEyeFov[1].LeftTan;
  _result.MaxEyeFov[1].RightTan = hmd->MaxEyeFov[1].RightTan;
  _result.EyeRenderOrder[0] = hmd->EyeRenderOrder[0];
  _result.EyeRenderOrder[1] = hmd->EyeRenderOrder[1];
  _result.DisplayDeviceName = hmd->DisplayDeviceName;
  _result.DisplayId = hmd->DisplayId;
}

// Defined at src\ovrDevice.kl:52:1
FABRIC_EXT_EXPORT KL::Float64 ovrDevice_GetTimeInSeconds(
  KL::Traits< KL::ovrDevice >::INParam this_
) {
  if(numOvrDevices)
    return ovr_GetTimeInSeconds();
  return 0.0;
}

// Defined at src\ovrDevice.kl:55:1
FABRIC_EXT_EXPORT void ovrDevice_WaitTillTime(
  KL::Traits< KL::ovrDevice >::IOParam this_,
  KL::Traits< KL::Float64 >::INParam absTime
) {
  if(numOvrDevices)
    ovr_WaitTillTime(absTime);
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
      this_->configured = true;
      return true;
    }
  }
  return false;
}

// Defined at src\ovrDevice.kl:76:1
FABRIC_EXT_EXPORT void ovrDevice_GetTrackingState(
  KL::Traits< KL::ovrTrackingState >::Result _result,
  KL::Traits< KL::ovrDevice >::INParam this_,
  KL::Traits< KL::Float64 >::INParam absTime
) {
  if(!this_->handle)
    return;
  ovrHmd hmd = (ovrHmd)this_->handle;

  ovrTrackingState ts = ovrHmd_GetTrackingState(hmd, absTime);
  convert(ts, _result);
}

// Defined at src\ovrDevice.kl:100:1
FABRIC_EXT_EXPORT void ovrDevice_RecenterPose(
  KL::Traits< KL::ovrDevice >::IOParam this_
) {
  if(!this_->handle)
    return;
  ovrHmd hmd = (ovrHmd)this_->handle;
  ovrHmd_RecenterPose(hmd);  
}

// Defined at src\ovrDevice.kl:103:1
FABRIC_EXT_EXPORT void ovrDevice_GetLastError(
  KL::Traits< KL::String >::Result _result,
  KL::Traits< KL::ovrDevice >::INParam this_
) {
  const char * error = NULL;
  if(this_->handle) {
    ovrHmd hmd = (ovrHmd)this_->handle;
    error = ovrHmd_GetLastError(hmd);
  }
  if(!error)
    error = ovrHmd_GetLastError(NULL);
  if(error)
    _result = error;
}

// Defined at src\ovrDevice.kl:114:1
FABRIC_EXT_EXPORT KL::Boolean ovrDevice_ConfigureRendering(
  KL::Traits< KL::ovrDevice >::IOParam this_,
  KL::Traits< KL::ovrRenderAPIConfig >::INParam config,
  KL::Traits< KL::SInt32 >::INParam distortionCaps,
  KL::Traits< KL::FixedArray< KL::ovrFovPort, 2 > >::INParam eyeFovIn,
  KL::Traits< KL::FixedArray< KL::ovrEyeRenderDesc, 2 > >::INParam eyeRenderDescOut
) {
  if(!this_->handle)
    return false;
  ovrHmd hmd = (ovrHmd)this_->handle;

  ovrRenderAPIConfig config_;
  ovrFovPort eyeFovIn_[2];
  ovrEyeRenderDesc eyeRenderDescOut_[2];
  convert(config, config_);
  convert(eyeFovIn[0], eyeFovIn_[0]);
  convert(eyeFovIn[1], eyeFovIn_[1]);
  convert(eyeRenderDescOut[0], eyeRenderDescOut_[0]);
  convert(eyeRenderDescOut[1], eyeRenderDescOut_[1]);
  return ovrHmd_ConfigureRendering(hmd, &config_, distortionCaps, eyeFovIn_, eyeRenderDescOut_);
}

// Defined at src\ovrDevice.kl:117:1
FABRIC_EXT_EXPORT KL::Boolean ovrDevice_DisableRendering(
  KL::Traits< KL::ovrDevice >::IOParam this_
) {
  if(!this_->handle)
    return false;
  ovrHmd hmd = (ovrHmd)this_->handle;
  ovrFovPort eyeFovIn_[2];
  ovrEyeRenderDesc eyeRenderDescOut_[2];
  return ovrHmd_ConfigureRendering(hmd, NULL, 0, eyeFovIn_, eyeRenderDescOut_);
}

// Defined at src\ovrDevice.kl:120:1
FABRIC_EXT_EXPORT void ovrDevice_BeginFrame(
  KL::Traits< KL::ovrFrameTiming >::Result _result,
  KL::Traits< KL::ovrDevice >::IOParam this_,
  KL::Traits< KL::UInt32 >::INParam frameIndex
) {
  if(!this_->handle)
    return;
  ovrHmd hmd = (ovrHmd)this_->handle;
  ovrFrameTiming ft = ovrHmd_BeginFrame(hmd, frameIndex);
  convert(ft, _result);
}

// Defined at src\ovrDevice.kl:130:1
FABRIC_EXT_EXPORT void ovrDevice_EndFrame(
  KL::Traits< KL::ovrDevice >::IOParam this_,
  KL::Traits< KL::FixedArray< KL::ovrPose, 2 > >::INParam renderPose,
  KL::Traits< KL::FixedArray< KL::ovrTexture, 2 > >::INParam eyeTexture
) {
  if(!this_->handle)
    return;
  ovrHmd hmd = (ovrHmd)this_->handle;

  ovrPosef renderPose_[2];
  ovrTexture eyeTexture_[2];
  convert(renderPose[0], renderPose_[0]);
  convert(renderPose[1], renderPose_[1]);
  convert(eyeTexture[0], eyeTexture_[0]);
  convert(eyeTexture[1], eyeTexture_[1]);
  ovrHmd_EndFrame(hmd, renderPose_, eyeTexture_);
}

// Defined at src\ovrDevice.kl:135:1
FABRIC_EXT_EXPORT void ovrDevice_GetEyePoses(
  KL::Traits< KL::FixedArray< KL::ovrPose, 2 > >::Result _result,
  KL::Traits< KL::ovrDevice >::INParam this_
) {
  if(!this_->handle)
    return;
  ovrHmd hmd = (ovrHmd)this_->handle;

  ovrPosef pose = ovrHmd_GetEyePose(hmd, ovrEye_Left);
  convert(pose, _result[0]);
  pose = ovrHmd_GetEyePose(hmd, ovrEye_Right);
  convert(pose, _result[1]);
}

// Defined at src\ovrDevice.kl:139:1
FABRIC_EXT_EXPORT void ovrDevice_GetFrameTiming(
  KL::Traits< KL::ovrFrameTiming >::Result _result,
  KL::Traits< KL::ovrDevice >::INParam this_,
  KL::Traits< KL::UInt32 >::INParam frameIndex
) {
  if(!this_->handle)
    return;
  ovrHmd hmd = (ovrHmd)this_->handle;

}

// Defined at src\ovrDevice.kl:144:1
FABRIC_EXT_EXPORT void ovrDevice_BeginFrameTiming(
  KL::Traits< KL::ovrFrameTiming >::Result _result,
  KL::Traits< KL::ovrDevice >::INParam this_,
  KL::Traits< KL::UInt32 >::INParam frameIndex
) {
  if(!this_->handle)
    return;
  ovrHmd hmd = (ovrHmd)this_->handle;
  ovrFrameTiming ft = ovrHmd_BeginFrameTiming(hmd, frameIndex);
  convert(ft, _result);
}

// Defined at src\ovrDevice.kl:149:1
FABRIC_EXT_EXPORT void ovrDevice_EndFrameTiming(
  KL::Traits< KL::ovrDevice >::IOParam this_
) {
  if(!this_->handle)
    return;
  ovrHmd hmd = (ovrHmd)this_->handle;
  ovrHmd_EndFrameTiming(hmd);
}

// Defined at src\ovrDevice.kl:154:1
FABRIC_EXT_EXPORT void ovrDevice_ResetFrameTiming(
  KL::Traits< KL::ovrDevice >::INParam this_,
  KL::Traits< KL::UInt32 >::INParam frameIndex
) {
  if(!this_->handle)
    return;
  ovrHmd hmd = (ovrHmd)this_->handle;
  ovrHmd_ResetFrameTiming(hmd, frameIndex);
}