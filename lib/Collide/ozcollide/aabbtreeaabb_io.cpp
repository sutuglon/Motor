
ENTER_NAMESPACE_OZCOLLIDE

ERR AABBTreeAABB::loadBinary(const char *_fileName, AABBTreeAABB **_tree)
{
	DataIn file;
	ERR err;
	
	if (!file.open(_fileName))
		return ERR_CANNOT_OPEN;

	err  = loadBinary(file, _tree);
	if (err)
		return err;

	file.close();
	return NOERR;
}

ERR AABBTreeAABB::loadBinary(DataIn &_file, AABBTreeAABB **_tree)
{
	char chunk[4];
	int chunkSize;

	_file.read(chunk, 4);
	if (MID(chunk[0], chunk[1],chunk[2], chunk[3]) != MID('A', 'A', 'B', 'B'))
		return ERR_INVALID_FORMAT;

	chunkSize = _file.readDword();

	char leafType = _file.readByte();

	if (leafType != 2)
		return ERR_INVALID_FORMAT;

	char leafDepth = _file.readByte();
	int nbNodes = _file.readDword();
	int nbLeafs = _file.readDword();

	AABBTreeAABB *t;

	t = new AABBTreeAABB(leafDepth);
	t->leafs_ = new AABBTreeAABBLeaf[nbLeafs];
	t->root_ = new AABBTreeNode[nbNodes];

	while(chunkSize > 8) {
		char id[4];
		_file.read(id, 4);
		int sc = MID(id[0], id[1], id[2], id[3]);

		int scSize = _file.readDword();
		int pos0 = _file.tell();

		if (sc == MID('N','O','D','S'))
			t->readNODSchunk(_file, scSize, nbNodes);
		else if (sc == MID('L','E','F','S'))
			t->readLEFSchunk(_file, scSize, nbLeafs);
		else
			_file.advance(scSize);

		int pos1 = _file.tell();
		if (pos1 - pos0 != scSize)
			_file.seek(pos0 + scSize);
		chunkSize -= scSize + 8;
	}

	*_tree = t;

	return NOERR;
}

void AABBTreeAABB::readNODSchunk(DataIn &_file, int _chunkSize, int _nbNodes)
{
	int i;

	for (i = 0; i < _nbNodes; i++) {
		AABBTreeNode &node = root_[i];

		node.aabb.center.x = _file.readFloat();
		node.aabb.center.y = _file.readFloat();
		node.aabb.center.z = _file.readFloat();
		node.aabb.extent.x = _file.readFloat();
		node.aabb.extent.y = _file.readFloat();
		node.aabb.extent.z = _file.readFloat();

		int leftID = _file.readDword();
		int rightID = _file.readDword();

		if (leftID == -1)
			node.left = NULL;
		else {
			int isLeaf = leftID >> 31;
			int index = leftID & 0x7fffffff;
			if (isLeaf)
				node.left = &leafs_[index];
			else
				node.left = &root_[index];
		}

		if (rightID == -1)
			node.right = NULL;
		else {
			int isLeaf = rightID >> 31;
			int index = rightID & 0x7fffffff;
			if (isLeaf)
				node.right = &leafs_[index];
			else
				node.right = &root_[index];
		}
	}
}

void AABBTreeAABB::readLEFSchunk(DataIn &_file, int _chunkSize, int _nbLeafs)
{
	int i, j;

	for (i = 0; i < _nbLeafs; i++) {
		AABBTreeAABBLeaf &leaf = leafs_[i];
	
		leaf.aabb.center.x = _file.readFloat();
		leaf.aabb.center.y = _file.readFloat();
		leaf.aabb.center.z = _file.readFloat();
		leaf.aabb.extent.x = _file.readFloat();
		leaf.aabb.extent.y = _file.readFloat();
		leaf.aabb.extent.z = _file.readFloat();

		leaf.left = NULL;
		leaf.right = NULL;

		int nbBoxes = _file.readDword();
		leaf.nbBoxes = nbBoxes;
		leaf.boxes = new Box[nbBoxes];
		for (j = 0; j < nbBoxes; j++) {
			Box &b = (Box &) leaf.boxes[j];
			b.center.x = _file.readFloat();
			b.center.y = _file.readFloat();
			b.center.z = _file.readFloat();
			b.extent.x = _file.readFloat();
			b.extent.y = _file.readFloat();
			b.extent.z = _file.readFloat();
		}
	}
}

ERR AABBTreeAABB::saveBinary(const char *_fname)
{
	DataOut file;
	ERR err;

	if (!file.open(_fname))
		return ERR_CANNOT_OPEN;

	err = saveBinary(file);
	if (err)
		return err;

	file.close();
	return NOERR;
}

ERR AABBTreeAABB::saveBinary(DataOut &_file)
{
	int i, j;

	_file.writeStr("AABB");
	int posBBT = _file.tell();
	_file.advance(4);

	int size = 0;

	size += _file.writeByte(2);

	nbNodes_ = getNbNodes();
	nbLeafs_ = getNbLeafs();

	size += _file.writeByte(leafDepth_);
	size += _file.writeDword(nbNodes_);
	size += _file.writeDword(nbLeafs_);

	size += _file.writeStr("NODS");
	size += _file.writeDword((24 + 8) * nbNodes_);
	for (i = 0; i < nbNodes_; i++) {
		AABBTreeNode *node = &root_[i];
		size += _file.writeFloat(node->aabb.center.x);
		size += _file.writeFloat(node->aabb.center.y);
		size += _file.writeFloat(node->aabb.center.z);
		size += _file.writeFloat(node->aabb.extent.x);
		size += _file.writeFloat(node->aabb.extent.y);
		size += _file.writeFloat(node->aabb.extent.z);

		const AABBTreeNode *left = node->left;
		const AABBTreeNode *right = node->right;

		if (left) {
			int diff = left - root_;
			if (diff < 0 || diff >= nbNodes_) {
				diff = ((AABBTreeAABBLeaf*) left) - leafs_;
				diff |= 0x80000000;
			}
			size += _file.writeDword(diff);
		}
		else
			size += _file.writeDword(-1);

		if (right) {
			int diff = right - root_;
			if (diff < 0 || diff >= nbNodes_) {
				diff = ((AABBTreeAABBLeaf*) right) - leafs_;
				diff |= 0x80000000;
			}
			size += _file.writeDword(diff);
		}
		else
			size += _file.writeDword(-1);
	}

	size += _file.writeStr("LEFS");
	int posLEFS = _file.tell();
	_file.advance(4);
	int sizeLEFS = 0;
	for (i = 0; i < nbLeafs_; i++) {
		AABBTreeAABBLeaf *leaf = &leafs_[i];

		_file.writeFloat(leaf->aabb.center.x);
		_file.writeFloat(leaf->aabb.center.y);
		_file.writeFloat(leaf->aabb.center.z);
		_file.writeFloat(leaf->aabb.extent.x);
		_file.writeFloat(leaf->aabb.extent.y);
		_file.writeFloat(leaf->aabb.extent.z);
		size += 24;		sizeLEFS += 24;

		_file.writeDword(leaf->nbBoxes);
		size += 4;		sizeLEFS += 4;

		int nbBoxes = leaf->nbBoxes;
		for (j = 0; j < nbBoxes; j++) {
			const Box &b = leaf->boxes[j];
			_file.writeFloat(b.center.x);
			_file.writeFloat(b.center.y);
			_file.writeFloat(b.center.z);
			_file.writeFloat(b.extent.x);
			_file.writeFloat(b.extent.y);
			_file.writeFloat(b.extent.z);
		}
		size += nbBoxes * 24;
		sizeLEFS += nbBoxes * 24;
	}

	_file.seek(posLEFS);
	_file.writeDword(sizeLEFS);

	_file.seek(posBBT);
	_file.writeDword(size);

	return NOERR;
}

LEAVE_NAMESPACE
