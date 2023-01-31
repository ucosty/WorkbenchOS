// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

typedef __UINT64_TYPE__ uint64_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT8_TYPE__ uint8_t;
typedef __INT64_TYPE__ int64_t;
typedef __INT32_TYPE__ int32_t;
typedef __INT16_TYPE__ int16_t;
typedef __INT8_TYPE__ int8_t;

typedef __UINT64_TYPE__ UINT64;
typedef __UINT32_TYPE__ UINT32;
typedef __UINT16_TYPE__ UINT16;
typedef __UINT8_TYPE__ UINT8;
typedef __INT64_TYPE__ INT64;
typedef __INT32_TYPE__ INT32;
typedef __INT16_TYPE__ INT16;
typedef __INT8_TYPE__ INT8;

//typedef __INT8_TYPE__ BOOLEAN;


#define ACPI_MUTEX_TYPE             ACPI_OSL_MUTEX
#define ACPI_MUTEX                  void *

#define ACPI_USE_NATIVE_DIVIDE
#define ACPI_USE_NATIVE_MATH64

//#define ACPI_SEMAPHORE              sem_id
#define ACPI_SPINLOCK               void *
//#define ACPI_CPU_FLAGS              cpu_status

#define COMPILER_DEPENDENT_INT64    int64_t
#define COMPILER_DEPENDENT_UINT64   uint64_t

#define ACPI_MACHINE_WIDTH          64
