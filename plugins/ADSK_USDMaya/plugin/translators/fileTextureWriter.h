//
// Copyright 2018 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef PXRUSDTRANSLATORS_FILE_TEXTURE_WRITER_H
#define PXRUSDTRANSLATORS_FILE_TEXTURE_WRITER_H

/// \file pxrUsdTranslators/fileTextureWriter.h

#include "pxr/pxr.h"
#include <mayaUsdCore/fileio/shaderWriter.h>
#include <mayaUsdCore/fileio/writeJobContext.h>

#include "pxr/base/tf/token.h"
#include "pxr/usd/sdf/path.h"
#include "pxr/usd/usd/timeCode.h"

#include <maya/MFnDependencyNode.h>


PXR_NAMESPACE_OPEN_SCOPE


/// Shader writer for exporting Maya's "file" texture shading node to USD.
class PxrUsdTranslators_FileTextureWriter : public UsdMayaShaderWriter
{
    public:
        PxrUsdTranslators_FileTextureWriter(
                const MFnDependencyNode& depNodeFn,
                const SdfPath& usdPath,
                UsdMayaWriteJobContext& jobCtx);

        void Write(const UsdTimeCode& usdTime) override;

        TfToken GetShadingPropertyNameForMayaAttrName(
                const TfToken& mayaAttrName) override;
};


PXR_NAMESPACE_CLOSE_SCOPE


#endif
