//
// Copyright 2019 Autodesk
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#pragma once

#include <ufe/transform3dUndoableCommands.h>

#include <pxr/usd/usd/attribute.h>

#include <mayaUsd/base/api.h>
#include <mayaUsd/ufe/UsdTRSUndoableCommandBase.h>

PXR_NAMESPACE_USING_DIRECTIVE

MAYAUSD_NS_DEF {
namespace ufe {

//! \brief Absolute translation command of the given prim's rotate pivot.
/*!
	Ability to perform undo to restore the original pivot value.
 */
class MAYAUSD_CORE_PUBLIC UsdRotatePivotTranslateUndoableCommand : public Ufe::TranslateUndoableCommand, public UsdTRSUndoableCommandBase
{
public:
	typedef std::shared_ptr<UsdRotatePivotTranslateUndoableCommand> Ptr;

	// Delete the copy/move constructors assignment operators.
	UsdRotatePivotTranslateUndoableCommand(const UsdRotatePivotTranslateUndoableCommand&) = delete;
	UsdRotatePivotTranslateUndoableCommand& operator=(const UsdRotatePivotTranslateUndoableCommand&) = delete;
	UsdRotatePivotTranslateUndoableCommand(UsdRotatePivotTranslateUndoableCommand&&) = delete;
	UsdRotatePivotTranslateUndoableCommand& operator=(UsdRotatePivotTranslateUndoableCommand&&) = delete;

	UsdRotatePivotTranslateUndoableCommand(const UsdTransform3d& item, const GfVec3f& rotPivot = {0,0,0});
	~UsdRotatePivotTranslateUndoableCommand() override;

	//! Create a UsdRotatePivotTranslateUndoableCommand from a USD prim, UFE path and UFE scene item.
	static UsdRotatePivotTranslateUndoableCommand::Ptr create(const UsdTransform3d& item, const GfVec3f& rotPivot = {0,0,0});

	// Ufe::TranslateUndoableCommand overrides
	void undo() override;
	void redo() override;
	bool translate(double x, double y, double z) override;

}; // UsdRotatePivotTranslateUndoableCommand

} // namespace ufe
} // namespace MayaUsd
