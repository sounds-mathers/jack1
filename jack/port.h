/*
    Copyright (C) 2001 Paul Davis
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
    
    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software 
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    $Id$
*/

#ifndef __jack_port_h__
#define __jack_port_h__

#include <pthread.h>
#include <jack/types.h>
#include <jack/jslist.h>

#define JACK_PORT_NAME_SIZE 32
#define JACK_PORT_TYPE_SIZE 32

/* these should probably go somewhere else */
#define JACK_CLIENT_NAME_SIZE 32
typedef unsigned long jack_client_id_t;

typedef struct _jack_port_type_info {
    const char type_name[JACK_PORT_TYPE_SIZE];   /* what do you think ? */

    void (*mixdown)(jack_port_t *, jack_nframes_t);   /* function to mixdown multiple inputs to a buffer. can be
						    NULL, indicating that multiple input connections
						    are not legal for this data type.
						 */

    long buffer_scale_factor;                    /* If == 1, then a buffer to handle nframes worth of
						    data is sizeof(jack_default_audio_sample_t) * nframes bytes large.
						    
						    If anything other than 1, the buffer allocated
						    for input mixing will be this value times
						    sizeof (jack_default_audio_sample_t) * nframes bytes in size.

						    Obviously, for non-audio data types, it may have
						    a different value.

						    if < 0, then the value should be ignored, and
						    port->shared->buffer_size should be used.
						 */
} jack_port_type_info_t;

/* This is the data structure allocated in shared memory
   by the engine.
*/

typedef struct _jack_port_shared {
    int                      shm_key;
    size_t                   offset;
    
    unsigned long            flags; 
    unsigned long            buffer_size;
    jack_port_id_t           id;
    char                     name[JACK_CLIENT_NAME_SIZE+JACK_PORT_NAME_SIZE+2];
    jack_port_type_info_t    type_info;
    jack_client_id_t         client_id;

    volatile jack_nframes_t  latency;
    volatile jack_nframes_t  total_latency;
    volatile unsigned char   monitor_requests;

    char                     in_use     : 1;
    char                     locked     : 1;
    struct _jack_port       *tied;

} jack_port_shared_t;

/* This is the data structure allocated by the client
   in local memory. The `shared' pointer points
   the the corresponding structure in shared memory.
*/

struct _jack_port {
    char                     *client_segment_base;
    struct _jack_port_shared *shared;
    pthread_mutex_t           connection_lock;
    JSList                   *connections;
};

/* inline would be cleaner, but it needs to be fast even in non-optimized 
   code.
*/

#define jack_port_buffer(p) ((void *) ((p)->client_segment_base + (p)->shared->offset))

/* this is the structure allocated by the engine in local
   memory.
*/

typedef struct _jack_port_internal {
    struct _jack_port_shared *shared;
    JSList                   *connections;
    void                     *buffer_info;
} jack_port_internal_t;

#endif /* __jack_port_h__ */

