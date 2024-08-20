// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _QtTestingExport_h
#define _QtTestingExport_h

#include "QtTestingConfigure.h"

#if defined(QTTESTING_BUILD_SHARED_LIBS)
#if defined(QtTesting_EXPORTS)
#if defined(_WIN32)
#define QTTESTING_EXPORT __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#define QTTESTING_EXPORT __attribute__((visibility("default")))
#endif
#else
#if defined(_WIN32)
#define QTTESTING_EXPORT __declspec(dllimport)
#endif
#endif
#endif

#ifndef QTTESTING_EXPORT
#define QTTESTING_EXPORT
#endif

#endif // !_QtTestingExport_h
