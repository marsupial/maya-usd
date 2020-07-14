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
#include "ProxyShapeHierarchy.h"

#include <stdexcept>

#include <ufe/log.h>
#include <ufe/pathComponent.h>
#include <ufe/pathSegment.h>
#include <ufe/rtid.h>

#include <pxr/usd/usd/stage.h>

#include <mayaUsd/ufe/Utils.h>
#include <mayaUsd/ufe/Global.h>

#ifdef UFE_V2_FEATURES_AVAILABLE
#if UFE_PREVIEW_VERSION_NUM >= 2013
#include <mayaUsd/ufe/UsdUndoInsertChildCommand.h>
#endif

#if UFE_PREVIEW_VERSION_NUM >= 2017
#include <mayaUsd/ufe/UsdUndoCreateGroupCommand.h>
#endif
#endif

MAYAUSD_NS_DEF {
namespace ufe {

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
extern Ufe::Rtid g_USDRtid;

//------------------------------------------------------------------------------
// ProxyShapeHierarchy
//------------------------------------------------------------------------------

ProxyShapeHierarchy::ProxyShapeHierarchy(
    const Ufe::HierarchyHandler::Ptr& mayaHierarchyHandler
)
	: Ufe::Hierarchy()
	, fMayaHierarchyHandler(mayaHierarchyHandler)
{
}

ProxyShapeHierarchy::~ProxyShapeHierarchy()
{
}

/*static*/
ProxyShapeHierarchy::Ptr ProxyShapeHierarchy::create(const Ufe::HierarchyHandler::Ptr& mayaHierarchyHandler)
{
	return std::make_shared<ProxyShapeHierarchy>(mayaHierarchyHandler);
}

/*static*/
ProxyShapeHierarchy::Ptr ProxyShapeHierarchy::create(
    const Ufe::HierarchyHandler::Ptr& mayaHierarchyHandler,
    const Ufe::SceneItem::Ptr&        item
)
{
    auto hierarchy = create(mayaHierarchyHandler);
    hierarchy->setItem(item);
    return hierarchy;
}

void ProxyShapeHierarchy::setItem(const Ufe::SceneItem::Ptr& item)
{
	// Our USD root prim is from the stage, which is from the item. So if we are
	// changing the item, it's possible that we won't have the same stage (and
	// thus the same root prim). To be safe, clear our stored root prim.
	if (fItem != item)
	{
		fUsdRootPrim = UsdPrim();
	}
	fItem = item;
	fMayaHierarchy = fMayaHierarchyHandler->hierarchy(item);
}

const UsdPrim& ProxyShapeHierarchy::getUsdRootPrim() const
{
	if (!fUsdRootPrim.IsValid())
	{
		// FIXME During AL_usdmaya_ProxyShapeImport, nodes (both Maya
		// and USD) are being added (e.g. the proxy shape itself), but
		// there is no stage yet, and there is no way to detect that a
		// proxy shape import command is under way.  PPT, 28-Sep-2018.
		UsdStageWeakPtr stage = getStage(fItem->path());
		if (stage)
		{
			fUsdRootPrim = stage->GetPseudoRoot();
		}
	}
	return fUsdRootPrim;
}

//------------------------------------------------------------------------------
// Ufe::Hierarchy overrides
//------------------------------------------------------------------------------

Ufe::SceneItem::Ptr ProxyShapeHierarchy::sceneItem() const
{
	return fItem;
}

bool ProxyShapeHierarchy::hasChildren() const
{
	const UsdPrim& rootPrim = getUsdRootPrim();
	if (!rootPrim.IsValid()) {
		UFE_LOG("invalid root prim in ProxyShapeHierarchy::hasChildren()");
		return false;
	}
	return !rootPrim.GetChildren().empty();
}

Ufe::SceneItemList ProxyShapeHierarchy::children() const
{
	// Return children of the USD root.
	const UsdPrim& rootPrim = getUsdRootPrim();
	if (!rootPrim.IsValid())
		return Ufe::SceneItemList();

	auto usdChildren = rootPrim.GetChildren();
	auto parentPath = fItem->path();

	// We must create selection items for our children.  These will have as
	// path the path of the proxy shape, with a single path segment of a
	// single component appended to it.
	Ufe::SceneItemList children;
	for (const auto& child : usdChildren)
	{
		children.emplace_back(UsdSceneItem::create(parentPath + Ufe::PathSegment(
			Ufe::PathComponent(child.GetName().GetString()), g_USDRtid, '/'), child));
	}
	return children;
}

Ufe::SceneItem::Ptr ProxyShapeHierarchy::parent() const
{
	return fMayaHierarchy->parent();
}

#if UFE_PREVIEW_VERSION_NUM < 2018
Ufe::AppendedChild ProxyShapeHierarchy::appendChild(const Ufe::SceneItem::Ptr& child)
{
	throw std::runtime_error("ProxyShapeHierarchy::appendChild() not implemented");
}
#endif

#ifdef UFE_V2_FEATURES_AVAILABLE
#if UFE_PREVIEW_VERSION_NUM >= 2013
Ufe::UndoableCommand::Ptr ProxyShapeHierarchy::insertChildCmd(
    const Ufe::SceneItem::Ptr& child,
    const Ufe::SceneItem::Ptr& pos
)
{
    // UsdUndoInsertChildCommand expects a UsdSceneItem which wraps a prim, so
    // create one using the pseudo-root and our own path.
    auto usdItem = UsdSceneItem::create(sceneItem()->path(), getUsdRootPrim());

    return UsdUndoInsertChildCommand::create(
        usdItem, downcast(child), downcast(pos));
}
#endif

#if UFE_PREVIEW_VERSION_NUM < 2017

Ufe::SceneItem::Ptr ProxyShapeHierarchy::createGroup(const Ufe::PathComponent& name) const
{
	throw std::runtime_error("ProxyShapeHierarchy::createGroup() not implemented");
}

Ufe::Group ProxyShapeHierarchy::createGroupCmd(const Ufe::PathComponent& name) const
{
	throw std::runtime_error("ProxyShapeHierarchy::createGroupCmd not implemented");
}

#else

Ufe::SceneItem::Ptr ProxyShapeHierarchy::createGroup(const Ufe::Selection& selection, const Ufe::PathComponent& name) const
{
	Ufe::SceneItem::Ptr createdItem;

	auto usdItem = UsdSceneItem::create(sceneItem()->path(), getUsdRootPrim());
	UsdUndoCreateGroupCommand::Ptr cmd = UsdUndoCreateGroupCommand::create(usdItem, selection, name.string());
	if (cmd) {
		cmd->execute();
		createdItem = cmd->group();
	}

	return createdItem;
}

Ufe::UndoableCommand::Ptr ProxyShapeHierarchy::createGroupCmd(const Ufe::Selection& selection, const Ufe::PathComponent& name) const
{
	auto usdItem = UsdSceneItem::create(sceneItem()->path(), getUsdRootPrim());

	return UsdUndoCreateGroupCommand::create(usdItem, selection, name.string());
}

#endif // UFE_PREVIEW_VERSION_NUM

#if UFE_PREVIEW_VERSION_NUM >= 2018

Ufe::SceneItem::Ptr ProxyShapeHierarchy::defaultParent() const
{
    // Default parent for Maya nodes is "|world".
    return createItem(Ufe::Path(Ufe::PathSegment(Ufe::PathComponent("world"), 
                                                 getMayaRunTimeId(), '|')));
}

Ufe::SceneItem::Ptr ProxyShapeHierarchy::insertChild(
        const Ufe::SceneItem::Ptr& ,
        const Ufe::SceneItem::Ptr& 
)
{
    // Should be possible to implement trivially when support for returning the
    // result of the parent command (MAYA-105278) is implemented.  For now,
    // Ufe::Hierarchy::insertChildCmd() returns a base class
    // Ufe::UndoableCommand::Ptr object, from which we can't retrieve the added
    // child.  PPT, 13-Jul-2020.
    return nullptr;
}

#endif

#endif // UFE_V2_FEATURES_AVAILABLE

} // namespace ufe
} // namespace MayaUsd
