/*
OZCollide - Collision Detection Library
Copyright (C) 2006  Igor Kravtchenko

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Contact the author: igor@tsarevitch.org
*/

#ifndef OZCOLLIDE_AABBTREE_H
#define OZCOLLIDE_AABBTREE_H

#ifndef OZCOLLIDE_PCH
#include <ozcollide/ozcollide.h>
#endif

#include <ozcollide/box.h>

ENTER_NAMESPACE_OZCOLLIDE

class DataOut;
class StringTokenizer;
class Monitor;

class AABBTreeNode {
public:
	Box aabb;
	// if both left and right are NULL, it's a leaf
	const AABBTreeNode *left, *right; 
};

enum AABBTREE_LEAFTYPE {
	AABBTREE_LEAFTYPE_POLYGON,
	AABBTREE_LEAFTYPE_SPHERE,
	AABBTREE_LEAFTYPE_AABB,
};

/*
	Note about the memory's layout.
	All nodes are placed in one big block. The start of this block is the address of the root's node.
	All leafs are also placed in one big block.
	In other words, there are two pointers, one for the nodes array and one for the leafs array.
	That merely optimizes the memory's layout of the tree.
	Note we build the tree by only providing memory pointers that could be a little confusing since
	a given memory pointer is not obviously in the correct memory block. The problem could be
	minimised by using indices but that would slighty slow further read access. Anyway the AABBs
	tree construction is not currently in charge of the client...
*/

class AABBTree {

	AABBTree(AABBTREE_LEAFTYPE _type, int _leafDepth);
	virtual ~AABBTree();

public:

	virtual ERR saveBinary(const char *filename) = 0;
	virtual ERR	 saveBinary(DataOut &_data) = 0;
	virtual void scale(float) = 0;

	OZCOLLIDE_API static ERR loadBinary(const char *filename, AABBTree **tree);

	OZCOLLIDE_API void destroy();

	/** Return the maximum number of primitives we can encount on a single leaf */
	ozinline int getLeafDepth() const { return leafDepth_; }

	ozinline int getNbNodes() const { return nbNodes_; }
	ozinline int getNbLeafs() const { return nbLeafs_; }
	ozinline AABBTREE_LEAFTYPE getLeafType() const { return leafType_; }

	/** Active/unactive temporal coherence
	    It behaves differently depending of the query */
	ozinline bool hasTemporalCoherence() const { return bUseTemporalCoherence_; }
	ozinline void setTemporalCoherence(bool _b) { bUseTemporalCoherence_ = _b; }

	/** Get/Set the coefficient used to inflate the volume in a collision query when temporal coherence is turned on */
	ozinline float getCoefVolumeInflater() const { return coefVolInflater_; }
	ozinline void setCoefVolumeInflater(float _f) { coefVolInflater_ = _f; }

	/** Get/Set an arbitraty value which define the type of the user data we can found at each leaf */
	ozinline int getTypeOfUserData() const { return userDataType_; }
	ozinline void setTypeOfUserData(int type) { userDataType_ = type; }

	ozinline const AABBTreeNode* getRootNode() const { return root_; }

	/** Get the memory consumption of the entire content of the tree in bytes */
	virtual int getMemoryConsumption() const = 0;

protected:

	int						nbNodes_;
	AABBTreeNode			*root_;
	int						nbLeafs_;

	AABBTREE_LEAFTYPE		leafType_;
	int						userDataType_;

	bool					bUseTemporalCoherence_;
	float					coefVolInflater_;

	int						leafDepth_;

	friend class AABBTreePoly;
	friend class AABBTreeSphere;
	friend class AABBTreeAABB;
};

LEAVE_NAMESPACE

#endif
