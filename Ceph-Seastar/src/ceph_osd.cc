// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2006 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/scoped_ptr.hpp>

#include <iostream>
#include <string>

#include "osd/OSD.h"
#include "os/ObjectStore.h"
#include "mon/MonClient.h"
#include "include/ceph_features.h"

#include "common/config.h"

#include "mon/MonMap.h"

#include "msg/Messenger.h"

#include "common/Throttle.h"
#include "common/Timer.h"
#include "common/TracepointProvider.h"
#include "common/ceph_argparse.h"

#include "global/global_init.h"
#include "global/signal_handler.h"

#include "include/color.h"
#include "common/errno.h"
#include "common/pick_address.h"

//#include "perfglue/heap_profiler.h"

#include "include/assert.h"

#include "common/Preforker.h"

#define dout_context g_ceph_context
#define dout_subsys ceph_subsys_osd

//#include "core/reactor.hh"
//#include "core/app-template.hh"
//#include "core/temporary_buffer.hh"
//#include "core/future-util.hh"
//#include "core/distributed.hh"
//#include "core/sleep.hh"
//#include <vector>

char hostname[150];
namespace {

    TracepointProvider::Traits osd_tracepoint_traits("libosd_tp.so",
                                                     "osd_tracing");
    TracepointProvider::Traits os_tracepoint_traits("libos_tp.so",
                                                    "osd_objectstore_tracing");
#ifdef WITH_OSD_INSTRUMENT_FUNCTIONS
    TracepointProvider::Traits cyg_profile_traits("libcyg_profile_tp.so",
                                                 "osd_function_tracing");
#endif

} // anonymous namespace

OSD *osd = nullptr;

void handle_osd_signal(int signum)
{
  if (osd)
    osd->handle_signal(signum);
}


//namespace bpo = boost::program_options;
//using namespace seastar;


int main(int argc, const char **argv)
{
  std::cout << hostname << "::FRISK SEASTAR-OSD START" << std::endl;
  std::cout << "FRISK CUSTOMIZED OSD END" << std::endl;
  return 0;
}
