/*
 * Copyright (c) 2004-2006 Voltaire, Inc. All rights reserved.
 * Copyright (c) 2002-2006 Mellanox Technologies LTD. All rights reserved.
 * Copyright (c) 1996-2003 Intel Corporation. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

/*
 * Abstract:
 * 	Declaration of osm_opensm_t.
 *	This object represents the OpenSM super object.
 *	This object is part of the OpenSM family of objects.
 *
 * Environment:
 * 	Linux User Mode
 *
 * $Revision: 1.6 $
 */

#ifndef _OSM_OPENSM_H_
#define _OSM_OPENSM_H_

#include <stdio.h>
#include <complib/cl_dispatcher.h>
#include <complib/cl_passivelock.h>
#include <complib/cl_atomic.h>
#include <complib/cl_nodenamemap.h>
#include <opensm/osm_stats.h>
#include <opensm/osm_log.h>
#include <opensm/osm_sm.h>
#include <opensm/osm_sa.h>
#include <opensm/osm_perfmgr.h>
#include <opensm/osm_event_plugin.h>
#include <opensm/osm_db.h>
#include <opensm/osm_subnet.h>
#include <opensm/osm_mad_pool.h>
#include <opensm/osm_vl15intf.h>

#ifdef __cplusplus
#  define BEGIN_C_DECLS extern "C" {
#  define END_C_DECLS   }
#else				/* !__cplusplus */
#  define BEGIN_C_DECLS
#  define END_C_DECLS
#endif				/* __cplusplus */

BEGIN_C_DECLS
/****h* OpenSM/OpenSM
* NAME
*	OpenSM
*
* DESCRIPTION
*	The OpenSM object encapsulates the information needed by the
*	OpenSM to govern itself.  The OpenSM is one OpenSM object.
*
*	The OpenSM object is thread safe.
*
*	This object should be treated as opaque and should
*	be manipulated only through the provided functions.
*
* AUTHOR
*	Steve King, Intel
*
*********/
/****s* OpenSM: OpenSM/osm_routing_engine
* NAME
*	struct osm_routing_engine
*
* DESCRIPTION
*	OpenSM routing engine module definition.
* NOTES
*	routing engine structure - yet limited by ucast_fdb_assign and
*	ucast_build_fwd_tables (multicast callbacks may be added later)
*/
struct osm_routing_engine {
	const char *name;
	void *context;
	int (*build_lid_matrices) (void *context);
	int (*ucast_build_fwd_tables) (void *context);
	void (*ucast_dump_tables) (void *context);
	void (*delete) (void *context);
};
/*
* FIELDS
*	name
*		The routing engine name (will be used in logs).
*
*	context
*		The routing engine context. Will be passed as parameter
*		to the callback functions.
*
*	build_lid_matrices
*		The callback for lid matrices generation.
*
*	ucast_build_fwd_tables
*		The callback for unicast forwarding table generation.
*
*	ucast_dump_tables
*		The callback for dumping unicast routing tables.
*
*	delete
*		The delete method, may be used for routing engine
*		internals cleanup.
*/

typedef struct _osm_console_t {
	int socket;
	int in_fd;
	int out_fd;
	FILE *in;
	FILE *out;
} osm_console_t;

/****s* OpenSM: OpenSM/osm_opensm_t
* NAME
*	osm_opensm_t
*
* DESCRIPTION
*	OpenSM structure.
*
*	This object should be treated as opaque and should
*	be manipulated only through the provided functions.
*
* SYNOPSIS
*/
typedef struct _osm_opensm_t {
	osm_subn_t subn;
	osm_sm_t sm;
	osm_sa_t sa;
#ifdef ENABLE_OSM_PERF_MGR
	osm_perfmgr_t perfmgr;
#endif				/* ENABLE_OSM_PERF_MGR */
	osm_epi_plugin_t *event_plugin;
	osm_db_t db;
	osm_mad_pool_t mad_pool;
	osm_vendor_t *p_vendor;
	osm_vl15_t vl15;
	osm_log_t log;
	cl_dispatcher_t disp;
	cl_plock_t lock;
	struct osm_routing_engine routing_engine;
	osm_stats_t stats;
	osm_console_t console;
	nn_map_t *node_name_map;
} osm_opensm_t;
/*
* FIELDS
*	subn
*		Subnet object for this subnet.
*
*	sm
*		The Subnet Manager (SM) object for this subnet.
*
*	sa
*		The Subnet Administration (SA) object for this subnet.
*
*	db
*		Persistant storage of some data required between sessions.
*
*	mad_pool
*		Pool of Management Datagram (MAD) objects.
*
*	p_vendor
*		Pointer to the Vendor specific adapter for various
*		transport interfaces, such as UMADT, AL, etc.  The
*		particular interface is set at compile time.
*
*	vl15
*		The VL15 interface.
*
*	log
*		Log facility used by all OpenSM components.
*
*	disp
*		Central dispatcher containing the OpenSM worker threads.
*
*	lock
*		Shared lock guarding most OpenSM structures.
*
*	routing_engine
*		Routing engine; will be initialized then used.
*
*	stats
*		Open SM statistics block
*
* SEE ALSO
*********/

/****f* OpenSM: OpenSM/osm_opensm_construct
* NAME
*	osm_opensm_construct
*
* DESCRIPTION
*	This function constructs an OpenSM object.
*
* SYNOPSIS
*/
void osm_opensm_construct(IN osm_opensm_t * const p_osm);
/*
* PARAMETERS
*	p_osm
*		[in] Pointer to a OpenSM object to construct.
*
* RETURN VALUE
*	This function does not return a value.
*
* NOTES
*	Allows calling osm_opensm_init, osm_opensm_destroy
*
*	Calling osm_opensm_construct is a prerequisite to calling any other
*	method except osm_opensm_init.
*
* SEE ALSO
*	SM object, osm_opensm_init, osm_opensm_destroy
*********/

/****f* OpenSM: OpenSM/osm_opensm_destroy
* NAME
*	osm_opensm_destroy
*
* DESCRIPTION
*	The osm_opensm_destroy function destroys an SM, releasing
*	all resources.
*
* SYNOPSIS
*/
void osm_opensm_destroy(IN osm_opensm_t * const p_osm);
/*
* PARAMETERS
*	p_osm
*		[in] Pointer to a OpenSM object to destroy.
*
* RETURN VALUE
*	This function does not return a value.
*
* NOTES
*	Performs any necessary cleanup of the specified OpenSM object.
*	Further operations should not be attempted on the destroyed object.
*	This function should only be called after a call to osm_opensm_construct or
*	osm_opensm_init.
*
* SEE ALSO
*	SM object, osm_opensm_construct, osm_opensm_init
*********/

/****f* OpenSM: OpenSM/osm_opensm_init
* NAME
*	osm_opensm_init
*
* DESCRIPTION
*	The osm_opensm_init function initializes a OpenSM object for use.
*
* SYNOPSIS
*/
ib_api_status_t
osm_opensm_init(IN osm_opensm_t * const p_osm,
		IN const osm_subn_opt_t * const p_opt);
/*
* PARAMETERS
*	p_osm
*		[in] Pointer to an osm_opensm_t object to initialize.
*
*	p_opt
*		[in] Pointer to the subnet options structure.
*
* RETURN VALUES
*	IB_SUCCESS if the OpenSM object was initialized successfully.
*
* NOTES
*	Allows calling other OpenSM methods.
*
* SEE ALSO
*	SM object, osm_opensm_construct, osm_opensm_destroy
*********/

/****f* OpenSM: OpenSM/osm_opensm_sweep
* NAME
*	osm_opensm_sweep
*
* DESCRIPTION
*	Initiates a subnet sweep.
*
* SYNOPSIS
*/
static inline void osm_opensm_sweep(IN osm_opensm_t * const p_osm)
{
	osm_sm_sweep(&p_osm->sm);
}

/*
* PARAMETERS
*	p_osm
*		[in] Pointer to an osm_opensm_t object on which to
*		initiate a sweep.
*
* RETURN VALUES
*	None
*
* NOTES
*	If the OpenSM object is not bound to a port, this function
*	does nothing.
*
* SEE ALSO
*********/

/****f* OpenSM: OpenSM/osm_opensm_set_log_flags
* NAME
*	osm_opensm_set_log_flags
*
* DESCRIPTION
*	Sets the log level.
*
* SYNOPSIS
*/
static inline void
osm_opensm_set_log_flags(IN osm_opensm_t * const p_osm,
			 IN const osm_log_level_t log_flags)
{
	osm_log_set_level(&p_osm->log, log_flags);
}

/*
* PARAMETERS
*	p_osm
*		[in] Pointer to an osm_opensm_t object.
*
*	log_flags
*		[in] Log level flags to set.
*
* RETURN VALUES
*	None
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: OpenSM/osm_opensm_bind
* NAME
*	osm_opensm_bind
*
* DESCRIPTION
*	Binds the opensm object to a port guid.
*
* SYNOPSIS
*/
ib_api_status_t
osm_opensm_bind(IN osm_opensm_t * const p_osm, IN const ib_net64_t guid);
/*
* PARAMETERS
*	p_osm
*		[in] Pointer to an osm_opensm_t object to bind.
*
*	guid
*		[in] Local port GUID with which to bind.
*
* RETURN VALUES
*	None
*
* NOTES
*	A given opensm object can only be bound to one port at a time.
*
* SEE ALSO
*********/

/****f* OpenSM: OpenSM/osm_opensm_wait_for_subnet_up
* NAME
*	osm_opensm_wait_for_subnet_up
*
* DESCRIPTION
*	Blocks the calling thread until the subnet is up.
*
* SYNOPSIS
*/
static inline cl_status_t
osm_opensm_wait_for_subnet_up(IN osm_opensm_t * const p_osm,
			      IN uint32_t const wait_us,
			      IN boolean_t const interruptible)
{
	return (osm_sm_wait_for_subnet_up(&p_osm->sm, wait_us, interruptible));
}

/*
* PARAMETERS
*	p_osm
*		[in] Pointer to an osm_opensm_t object.
*
*	wait_us
*		[in] Number of microseconds to wait.
*
*	interruptible
*		[in] Indicates whether the wait operation can be interrupted
*		by external signals.
*
* RETURN VALUES
*	CL_SUCCESS if the wait operation succeeded in response to the event
*	being set.
*
*	CL_TIMEOUT if the specified time period elapses.
*
*	CL_NOT_DONE if the wait was interrupted by an external signal.
*
*	CL_ERROR if the wait operation failed.
*
* NOTES
*
* SEE ALSO
*********/

/* dump helpers */
void osm_dump_mcast_routes(osm_opensm_t * osm);
void osm_dump_all(osm_opensm_t * osm);

/****v* OpenSM/osm_exit_flag
*/
extern volatile unsigned int osm_exit_flag;
/*
* DESCRIPTION
*  Set to one to cause all threads to leave
*********/

END_C_DECLS
#endif				/* _OSM_OPENSM_H_ */
