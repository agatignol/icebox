/* $Id: VBoxVRDP.h $ */
/** @file
 * VBoxVRDP - VRDP notification
 */

/*
 * Copyright (C) 2006-2019 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

#ifndef GA_INCLUDED_SRC_WINNT_VBoxTray_VBoxVRDP_h
#define GA_INCLUDED_SRC_WINNT_VBoxTray_VBoxVRDP_h
#ifndef RT_WITHOUT_PRAGMA_ONCE
# pragma once
#endif

/* The restore service prototypes. */
int                VBoxVRDPInit    (const VBOXSERVICEENV *pEnv, void **ppInstance, bool *pfStartThread);
unsigned __stdcall VBoxVRDPThread  (void *pInstance);
void               VBoxVRDPDestroy (const VBOXSERVICEENV *pEnv, void *pInstance);

#endif /* !GA_INCLUDED_SRC_WINNT_VBoxTray_VBoxVRDP_h */
