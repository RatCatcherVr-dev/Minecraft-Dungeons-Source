//------------------------------------------------------------------------------
// <copyright file="UTFAnnotations.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

/** Annotation to ensure return values are not ignored. */
#define UTF_ANNOTATE_HRESULT_METHOD _Check_return_ _Success_(!FAILED(return))
