// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file GPUTRDTrack.h
/// \author Ole Schmidt

#ifndef GPUTRDTRACK_H
#define GPUTRDTRACK_H

#include "GPUTRDDef.h"
#include "GPUCommonDef.h"
#include "GPUCommonRtypes.h"

struct GPUTRDTrackDataRecord;
class AliHLTExternalTrackParam;

namespace o2
{
namespace tpc
{
class TrackTPC;
} // namespace tpc
namespace dataformats
{
class TrackTPCITS;
} // namespace dataformats
} // namespace o2

//_____________________________________________________________________________
#if (defined(__CINT__) || defined(__ROOTCINT__)) && !defined(__CLING__)
namespace GPUCA_NAMESPACE
{
namespace gpu
{
template <typename T>
class GPUTRDTrack_t;
} // namespace gpu
} // namespace GPUCA_NAMESPACE
#else
#if (!defined(GPUCA_STANDALONE) && !defined(GPUCA_ALIROOT_LIB)) || defined(HAVE_O2HEADERS)
#include "GPUTRDO2BaseTrack.h"
#include "GPUTRDInterfaceO2Track.h"
#endif

namespace GPUCA_NAMESPACE
{
namespace gpu
{

template <typename T>
class GPUTRDTrack_t : public T
{
 public:
  enum EGPUTRDTrack {
    kNLayers = 6,
    kStopFlag = 7
  };

  GPUd() GPUTRDTrack_t();
  GPUTRDTrack_t(const typename T::baseClass& t) = delete;
  GPUd() GPUTRDTrack_t(const GPUTRDTrack_t& t);
  GPUd() GPUTRDTrack_t(const AliHLTExternalTrackParam& t);
#ifndef GPUCA_GPUCODE
  GPUd() GPUTRDTrack_t(const o2::dataformats::TrackTPCITS& t, float vDrift);
  GPUd() GPUTRDTrack_t(const o2::tpc::TrackTPC& t, float tbWidth, float vDrift, unsigned int iTrk);
#endif
  GPUd() GPUTRDTrack_t(const T& t);
  GPUd() GPUTRDTrack_t& operator=(const GPUTRDTrack_t& t);

  // attach a tracklet to this track; this overwrites the mIsFindable flag to true for this layer
  GPUd() void addTracklet(int iLayer, int idx) { mAttachedTracklets[iLayer] = idx; }

  // getters
  GPUd() int getNlayersFindable() const;
  GPUd() int getTracklet(int iLayer) const { return mAttachedTracklets[iLayer]; }
  GPUd() int getTPCtrackId() const { return mTPCTrackId; }
  GPUd() int getNtracklets() const;
  GPUd() float getChi2() const { return mChi2; }
  GPUd() float getReducedChi2() const { return getNlayersFindable() == 0 ? mChi2 : mChi2 / getNlayersFindable(); }
  GPUd() bool getIsStopped() const { return (mIsFindable >> kStopFlag) & 0x1; }
  GPUd() bool getIsFindable(int iLayer) const { return (mIsFindable >> iLayer) & 0x1; }
  GPUd() int getNmissingConsecLayers(int iLayer) const;
  // for AliRoot compatibility. To be removed once HLT/global/AliHLTGlobalEsdConverterComponent.cxx does not require them anymore
  GPUd() int GetTPCtrackId() const { return getTPCtrackId(); }
  GPUd() bool GetIsStopped() const { return getIsStopped(); }
  GPUd() int GetNtracklets() const { return getNtracklets(); }

  // setters
  GPUd() void setTPCtrackId(int v) { mTPCTrackId = v; }
  GPUd() void setIsFindable(int iLayer) { mIsFindable |= (1U << iLayer); }
  GPUd() void setIsStopped() { mIsFindable |= (1U << kStopFlag); }
  GPUd() void setChi2(float chi2) { mChi2 = chi2; }

  // conversion to / from HLT track structure (only for AliRoot)
  GPUd() void ConvertTo(GPUTRDTrackDataRecord& t) const;
  GPUd() void ConvertFrom(const GPUTRDTrackDataRecord& t);

 protected:
  float mChi2;                      // total chi2
  int mTPCTrackId;                  // corresponding TPC track; for O2 this could be GlobalTrackID type, but for AliRoot that doesn't exist
  int mAttachedTracklets[kNLayers]; // indices of the tracklets attached to this track; -1 means no tracklet in that layer
  unsigned char mIsFindable;        // bitfield; LSB indicates whether track is findable in layer 0; MSB flags whether the track is stopped in the TRD; one bit is currently not used

 private:
  GPUd() void initialize();
#if !defined(GPUCA_STANDALONE) && !defined(GPUCA_ALIROOT_LIB)
  ClassDefNV(GPUTRDTrack_t, 1);
#endif
};
} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif // !((defined(__CINT__) || defined(__ROOTCINT__)) && !defined(__CLING__))

#endif // GPUTRDTRACK_H
