/*
 *  Copyright 2013 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_BASE_IFADDRS_SWITCH_H_
#define WEBRTC_BASE_IFADDRS_SWITCH_H_

#if defined(WEBRTC_SWITCH)
#include <stdio.h>
#include <libPosix/posix_nx.h>

 // Implementation of getifaddrs for Switch.
 // Fills out a list of ifaddr structs (see below) which contain information
 // about every network interface available on the host.
 // See 'man getifaddrs' on Linux or OS X (nb: it is not a POSIX function).

namespace rtc {

	int getifaddrs(struct ifaddrs** result);
	void freeifaddrs(struct ifaddrs* addrs);

}  // namespace rtc

#endif  // WEBRTC_SWITCH

#endif  // WEBRTC_BASE_IFADDRS_SWITCH_H_