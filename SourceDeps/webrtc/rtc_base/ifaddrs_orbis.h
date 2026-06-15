/*
 *  Copyright 2013 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_BASE_IFADDRS_ORBIS_H_
#define WEBRTC_BASE_IFADDRS_ORBIS_H_

#if defined(WEBRTC_ORBIS)
#include <stdio.h>
#include <posix_orbis.h>

 // Implementation of getifaddrs for Orbis.
 // Fills out a list of ifaddr structs (see below) which contain information
 // about every network interface available on the host.
 // See 'man getifaddrs' on Linux or OS X (nb: it is not a POSIX function).

namespace rtc {

	int getifaddrs(struct ifaddrs** result);
	void freeifaddrs(struct ifaddrs* addrs);

}  // namespace rtc

#endif  // WEBRTC_ORBIS

#endif  // WEBRTC_BASE_IFADDRS_PS4_H_