//
// Copyright 2020 Autodesk
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
#include "UsdRotateUndoableCommand.h"

#include "private/Utils.h"

MAYAUSD_NS_DEF {
namespace ufe {

UsdRotateUndoableCommand::UsdRotateUndoableCommand(const UsdSceneItem::Ptr& item,
    GfVec3f rotation)
	: Ufe::RotateUndoableCommand(item)
    , UsdTRSUndoableCommandBase(item, std::move(rotation), UsdGeomXformOp::Type::TypeRotateXYZ)
{}

UsdRotateUndoableCommand::~UsdRotateUndoableCommand()
{}

/*static*/
UsdRotateUndoableCommand::Ptr UsdRotateUndoableCommand::create(
    const UsdSceneItem::Ptr& item, GfVec3f rotation)
{
    auto cmd = std::make_shared<MakeSharedEnabler<UsdRotateUndoableCommand>>(
        item, std::move(rotation));
    cmd->initialize();
    return cmd;
}

void UsdRotateUndoableCommand::undo()
{
    UsdTRSUndoableCommandBase::undo();
}

void UsdRotateUndoableCommand::redo()
{
    UsdTRSUndoableCommandBase::redo();
}

//------------------------------------------------------------------------------
// Ufe::RotateUndoableCommand overrides
//------------------------------------------------------------------------------

bool UsdRotateUndoableCommand::rotate(double x, double y, double z)
{
	perform(x, y, z);
	return true;
}

} // namespace ufe
} // namespace MayaUsd
