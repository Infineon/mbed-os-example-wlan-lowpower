/*******************************************************************************
* File Name: cycfg.h
*
* Description:
* Simple wrapper header containing all generated files.
* This file was automatically generated and should not be modified.
* Tools Package 2.1.0.1266
* psoc6pdl 1.5.1.2655
* personalities_2.0 2.0.0.0
* udd 1.2.0.136
*
********************************************************************************
* Copyright 2020 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
********************************************************************************/

#if !defined(CYCFG_H)
#define CYCFG_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "cycfg_notices.h"
#include "cycfg_system.h"
#include "cycfg_connectivity_wifi.h"
#include "cycfg_clocks.h"
#include "cycfg_routing.h"
#include "cycfg_peripherals.h"
#include "cycfg_pins.h"

void init_cycfg_all(void);


#if defined(__cplusplus)
}
#endif


#endif /* CYCFG_H */